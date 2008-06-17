#!/usr/bin/python

"""
tbnflutter.py

Analyze and reconstruct the flutter-tongued trombone tone that was
part of the ICMC 2000 bake-off, called conant-trombone-flutter.aiff.

Last updated: 16 June 2008 by Kelly Fitz
"""

import loris, time, os

orate = 44100

tag = ''

stuff = {}


# ----------------------------------------------------------------------------

"""
Parameters we used then were resolution 150 Hz, freq floor 100 Hz and
window width 600 Hz.

Note boudaries are near .75 s, 1.25 s, and 2.1 s, and the duration is
2.4 s. Some fundamental frequencies are 291 Hz at 1 s, 280.5 Hz at
1.5 s, and 269 Hz at 2.0 s.

Distilling at 2 partials per harmonic eliminates pretty much
all artifacts of distillation.
"""

# Need to specify non-default value for gap time to achieve
# good results (as in Loris 1.4)
Fade = 0.001
Gap = 0.0001

def do_tbnflutter( exportDir = '' ):

	name = 'tbnflutter'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	anal = loris.Analyzer( 150, 600 )
	anal.buildFundamentalEnv( 200, 300 )
	
	p = anal.analyze( f.samples(), f.sampleRate() )
	ref = anal.fundamentalEnv()
	
	# distill at N Partials per harmonic
	print 'distilling %i partials (%s)'%(p.size(), time.ctime(time.time()))
	N = 2
	loris.channelize( p, ref, N )
 	loris.distill( p, Fade, Gap )
	
	# relabel the partials so that they can still
	# be used for morphing, esp. with harmonic sounds
	for part in p:
		if 0 == part.label()%N:
			part.setLabel( part.label() / N )
		else:
			part.setLabel( 0 )
	
	if exportDir:
	
		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )
		
	stuff[ name ] = ( p, ref, anal )
	
	print 'Done. (%s)'%(time.ctime(time.time()))			


# ----------------------------------------------------------------------------

if __name__ == '__main__':
	print __doc__

	print 'Using Loris version %s'%( loris.version() )

	import sys
	odir = os.curdir
	if len( sys.argv ) > 1:
		tag = '.' + sys.argv[1]
		
	do_tbnflutter( odir )
	
# ----------------------------------------------------------------------------


# if 0:
# 	source = 'tbnflutter.aiff'
# 	file = loris.AiffFile( source )
# 	samples = file.samples()
# 	rate = file.sampleRate()
# 
# 	resolutions = ( 100, 150 )
# 	w = 600
# 	ref = loris.createFreqReference( loris.importSdif( 'tbnflutter.fund.sdif' ), 0, 1000 )
# 	for r in resolutions:
# 		a = loris.Analyzer( r, w )
# 		p = a.analyze( samples, rate )
# 		# export raw
# 		ofile = 'tbnflutter.%i.%i'%(r, w)
# 		loris.exportAiff( ofile + '.raw.aiff', loris.synthesize( p, rate ), rate )
# 		p1 = p
# 		p2 = p1.copy()
# 		
# 		# distill one partial per harmonic
# 		loris.channelize( p1, ref, 1 )
# 		loris.distill( p1 )
# 		# export
# 		ofile1 = ofile + '.d1'
# 		loris.exportAiff( ofile1 + '.aiff', loris.synthesize( p1, rate ), rate )
# 		loris.exportSdif( ofile1 + '.sdif', p1 )
# 		loris.exportSpc( ofile1 + '.s.spc', p1, 60, 0 ) 
# 		loris.exportSpc( ofile1 + '.e.spc', p1, 60, 1 ) 
# 				
# 		# distill two partials per harmonic
# 		loris.channelize( p2, ref, 2 )
# 		loris.distill( p2 )
# 		# export
# 		ofile2 = ofile + '.d2'
# 		loris.exportAiff( ofile2 + '.aiff', loris.synthesize( p2, rate ), rate )
# 		loris.exportSdif( ofile2 + '.sdif', p2 )
# 		loris.exportSpc( ofile2 + '.s.spc', p2, 60, 0 ) 
# 		loris.exportSpc( ofile2 + '.e.spc', p2, 60, 1 ) 
# 

#!/usr/bin/python

"""
tuba.py

Analyze and reconstruct a low tuba note C2.

Last updated: 17 Aug 2007 by Kelly Fitz
"""

import loris, time, os

orate = 44100

tag = ''

stuff = {}

# ----------------------------------------------------------------------------

def doTuba( exportDir = '' ):

	name = 'tuba.C2'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	fHz = 65.4
	anal = loris.Analyzer( 62, 120 )
	anal.setFreqDrift( .2*fHz )
	anal.setBwRegionWidth(0)
	anal.buildFundamentalEnv( 60, 70 )
	p = anal.analyze( f.samples(), f.sampleRate() )
	
	print 'distilling %s (%s)'%(name, time.ctime(time.time()))
	ref = anal.fundamentalEnv()
	loris.channelize( p, ref, 1 )
	loris.distill( p )
	
	if exportDir:

		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		samps = loris.synthesize( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile = loris.AiffFile( samps, orate )
		out_sfile.setMidiNoteNumber( 36 )
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( name + tag + '.recon.aiff' )
		

		opath = os.path.join( exportDir, name + tag + '.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )

		
		print 'computing residual signal (%s)'%(time.ctime(time.time()))
		resid = list(f.samples())
		for k in range(len(resid)):
			if k < len(samps):
				resid[k] = resid[k] - samps[k]

		opath = os.path.join( exportDir, name + tag + '.residual.aiff' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile = loris.AiffFile( resid, orate )
		out_sfile.setMidiNoteNumber( 36 )
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( name + tag + '.residual.aiff' )

	stuff[ name ] = ( p, anal )
	
	print 'Done. (%s)'%(time.ctime(time.time()))


# ----------------------------------------------------------------------------

if __name__ == '__main__':
	print __doc__

	print 'Using Loris version %s'%( loris.version() )

	import sys
	odir = os.curdir
	if len( sys.argv ) > 1:
		tag = '.' + sys.argv[1]
		
	doTuba( odir )

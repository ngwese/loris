#!/usr/bin/python

"""
meows.py

Analyze cats' meows (meow1 and meow3), store distillations at one and
two Partials per harmonic.
	
It is no longer really necessary to use more than one partial per harmonic,
a usable sound can be constructed using just one per harmonic. Using a long
narrow window helps smooth out some of the crud introduced by all the 
background hiss and rumble. A slightly more natural sound is produced 
using two partials per harmonic, bandwidth enhancement doesn't seem to help.
This is more important for meow1 than meow3.

Last updated: 4 June 2008 by Kelly Fitz
"""

import loris, time, os

orate = 44100

tag = ''

stuff = {}
# 
# # ----------------------------------------------------------------------------
# # DEBUGGING
# 
# def exportReference( env, fname ):
# 	print 'EXPORTING REFERENCE ENVELOPE', fname
# 	p =loris.Partial()
# 	t = 0
# 	while t < 1.10:
# 		p.insert( t, loris.Breakpoint( env.valueAt( t ), 0.1, 0, 0 ) )
# 		t = t + 0.001
# 	l = loris.PartialList()
# 	l.append( p )
# 	
# 	f = loris.SdifFile( l )
# 	f.write( fname )
# 
# def importReference( fname ):
# 	print 'IMPORTING REFERENCE ENVELOPE', fname
# 	f = loris.SdifFile( fname )
# 	l = f.partials()
# 	p = l.first()
# 	
# 	env = loris.LinearEnvelope()
# 	for bp in p:
# 		env.insert( bp.time(), bp.frequency() )
# 		
# 	return env
# 
# ----------------------------------------------------------------------------

def doMeow1( exportDir = '' ):

	name = 'meow1'
	f = loris.AiffFile( name + '.aiff' )
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	
	# set resolution to capture two partials per harmonic,
	# can also increase resolution if only one partial
	# per harmonic is desired. Other essential analysis
	# parameters are unaffected.
	anal = loris.Analyzer( 150, 500 )
	anal.setBwRegionWidth( 0 ) # disable BW enhancement, doesn't help
	anal.setFreqDrift( 40 )
	anal.setFreqFloor( 210 )
	p = anal.analyze( f.samples(), f.sampleRate() )
	
	# meow1 at 2 Partials per harmonic
	N = 2
	ref = loris.createFreqReference( p, 470, 620 )
	loris.channelize( p, ref, N )
 	
 	# Need to specify non-default value for gap time to achieve
 	# good results (as in Loris 1.4)
 	Fade = 0.005
 	Gap = 0.0001
 	loris.distill( p, Fade, Gap )
 	
 	# problem in older versions of Loris
 	loris.crop( p, 0, 100 )
 	
	loris.scaleBandwidth( p, 0 )
	
	# relabel the partials so that they can still
	# be used for morphing, esp. with harmonic sounds
	for part in p:
		if 0 == part.label()%N:
			part.setLabel( part.label() / N )
		else:
			part.setLabel( 0 )
			
	
	if exportDir:
		print 'synthesizing %i distilled Partials per harmonic (%s)'%(N, time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.%i.recon.aiff'%(N) ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.%i.sdif'%(N) )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )

		print 'synthesizing 1 distilled Partial per harmonic (%s)'%(time.ctime(time.time()))
		loris.removeLabeled( p, 0 )
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.%i.recon.aiff'%(1) ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.%i.sdif'%(1) )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )


 
# ----------------------------------------------------------------------------

def doMeow3( exportDir = '' ):

	name = 'meow3'
	f = loris.AiffFile( name + '.aiff' )
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	
	# set resolution to capture two partials per harmonic,
	# can also set resolution to 380 if only one partial
	# per harmonic is desired. Other essential analysis
	# parameters are unaffected.
	anal = loris.Analyzer( 190, 400 )
	anal.setFreqDrift( 40 )
	anal.setFreqFloor( 210 )
	anal.setBwRegionWidth( 0 ) # disable BW enhancement, doesn't help
	p = anal.analyze( f.samples(), f.sampleRate() )
	
	# meow3 at 2 Partials per harmonic
	N = 2
	ref = loris.createFreqReference( p, 400, 600 )
	loris.channelize( p, ref, N )
	
 	# Need to specify non-default value for gap time to achieve
 	# good results (as in Loris 1.4)
 	Fade = 0.005
 	Gap = 0.0001
 	loris.distill( p, Fade, Gap )
 	
	# problem in older versions of Loris
 	loris.crop( p, 0, 100 )
 	
	loris.scaleBandwidth( p, 0 )
	
	# relabel the partials so that they can still
	# be used for morphing, esp. with harmonic sounds
	for part in p:
		if 0 == part.label()%N:
			part.setLabel( part.label() / N )
		else:
			part.setLabel( 0 )

	if exportDir:
		print 'synthesizing %i distilled Partials per harmonic (%s)'%(N, time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.%i.recon.aiff'%(N) ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.%i.sdif'%(N) )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )

		print 'synthesizing 1 distilled Partial per harmonic (%s)'%(time.ctime(time.time()))
		loris.removeLabeled( p, 0 )
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.%i.recon.aiff'%(1) ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.%i.sdif'%(1) )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )


# ----------------------------------------------------------------------------

if __name__ == '__main__':
	print __doc__

	print 'Using Loris version %s'%( loris.version() )

	import sys
	odir = os.curdir
	if len( sys.argv ) > 1:
		tag = '.' + sys.argv[1]
		
	doMeow1( odir )
	doMeow3( odir )

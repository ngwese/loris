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

Last updated: 10 Aug 2007 by Kelly Fitz
"""

import loris, time, os

orate = 44100

tag = ''

stuff = {}

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
	loris.distill( p )
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
	loris.distill( p )
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

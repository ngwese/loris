#!/usr/bin/python

"""
flutes.py

Analyze and reconstruct the breathy flute tone, D above middle C, around
291 Hz, with an interesting pre-attack whistle, and the 330 Hz
flutter-tongued flute tone.

Used to use much narrower windows for the flute, doesn't seem to improve
things any.

Last updated: 17 Jun 2008 by Kelly Fitz
"""

import loris, time, os

orate = 44100

tag = ''

stuff = {}

# Set these distillation parameters globally to the old values.
Fade = 0.005 	# 5 ms
Gap = 0.0001 	# 1 ms 


# ----------------------------------------------------------------------------

def doFlute( exportDir = '' ):

	name = 'flute291'	
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	anal = loris.Analyzer( 250, 500 )
	anal.buildFundamentalEnv( 291*.9, 291*1.1 )
	p = anal.analyze( f.samples(), f.sampleRate() )

	
	# distilled using one partial per harmonic
	ref =  anal.fundamentalEnv()
	loris.channelize( p, ref, 1 )
	loris.distill( p, Fade, Gap )
	
	if exportDir:
	
		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.setMidiNoteNumber( 62 )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )

	stuff[ name ] = ( p, anal )
	
	print 'Done. (%s)'%(time.ctime(time.time()))

# ----------------------------------------------------------------------------

def doFlutter( exportDir = '' ):

	name = 'flutter'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	anal = loris.Analyzer( 250, 500 )
	anal.buildFundamentalEnv( 330*.9, 330*1.1 )
	p = anal.analyze( f.samples(), f.sampleRate() )
	
	# distilled using one partial per harmonic
	ref =  anal.fundamentalEnv()
	loris.channelize( p, ref, 1 )
	loris.distill( p, Fade, Gap )
	
	if exportDir:

		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMidiNoteNumber( 64 )
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )

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
		
	doFlute( odir )
	doFlutter( odir )


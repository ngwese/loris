#!/usr/bin/python

"""
pianos.py

Analyze and reconstruct piano tones taken from the Iowa samples:

- B flat 3 (B flat below middle C)
- E flat 3 (E flat below middle C)
- C 4 (middle C)

Might need to take advantage of the harmonic stretching capabilities
of the Channelizer to make these work better.

Last updated: 10 Sep 2007 by Kelly Fitz
"""

import loris, time, os

orate = 44100

tag = ''

stuff = {}

Fade = 0.001
Gap = 0.0001

# ----------------------------------------------------------------------------

def do_pianoBb3( exportDir = '' ):

	name = 'piano.Bb3'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	fHz = 233
	anal = loris.Analyzer( .85*fHz, 1.5*fHz )
	# anal.setBwRegionWidth( 0 ) # no BW association
	anal.buildFundamentalEnv( .7*fHz, 1.3*fHz )
	
	p = anal.analyze( f.samples(), f.sampleRate() )
	ref = anal.fundamentalEnv()
	
	# distill at N Partials per harmonic
	print 'distilling %i partials (%s)'%(p.size(), time.ctime(time.time()))
	N = 1
	loris.channelize( p, ref, N )
	loris.distill( p, Fade, Gap )
	
	if exportDir:
	
		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMidiNoteNumber( 58 )
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

def do_pianoEb3( exportDir = '' ):

	name = 'piano.Eb3'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	fHz = 154
	anal = loris.Analyzer( 123, 154 )
	# anal.setBwRegionWidth( 0 ) # no BW association
	anal.buildFundamentalEnv( .7*fHz, 1.3*fHz )
	
	p = anal.analyze( f.samples(), f.sampleRate() )
	ref = anal.fundamentalEnv()
	
	# distill at N Partials per harmonic
	print 'distilling %i partials (%s)'%(p.size(), time.ctime(time.time()))
	N = 1
	loris.channelize( p, ref, N )
	loris.distill( p, Fade, Gap )
	
	if exportDir:
	
		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.setMidiNoteNumber( 51 )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )
		
	stuff[ name ] = ( p, ref, anal )
	
	print 'Done. (%s)'%(time.ctime(time.time()))			

# ----------------------------------------------------------------------------

def do_pianoC4( exportDir = '' ):

	name = 'piano.C4'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	fHz = 262
	anal = loris.Analyzer( .85*fHz, 1.5*fHz )
	# anal.setBwRegionWidth( 0 ) # no BW association
	anal.buildFundamentalEnv( .7*fHz, 1.3*fHz )
	
	p = anal.analyze( f.samples(), f.sampleRate() )
	ref = anal.fundamentalEnv()
	
	# distill at N Partials per harmonic
	print 'distilling %i partials (%s)'%(p.size(), time.ctime(time.time()))
	N = 1
	loris.channelize( p, ref, N )
	loris.distill( p, Fade, Gap )
	
	if exportDir:
	
		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMidiNoteNumber( 60 )
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

def do_pianoDb2( exportDir = '' ):

	name = 'piano.Db2'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	fHz = 69.69
	anal = loris.Analyzer( .85*fHz, 1.5*fHz )
	# anal.setBwRegionWidth( 0 ) # no BW association
	anal.buildFundamentalEnv( .7*fHz, 1.3*fHz )
	
	p = anal.analyze( f.samples(), f.sampleRate() )
	ref = anal.fundamentalEnv()
	
	# distill at N Partials per harmonic
	print 'distilling %i partials (%s)'%(p.size(), time.ctime(time.time()))
	N = 1
	loris.channelize( p, ref, N )
	loris.distill( p, Fade, Gap )
	
	if exportDir:
	
		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMidiNoteNumber( 37 )
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
		
	do_pianoBb3( odir )
	do_pianoEb3( odir )
	do_pianoC4( odir )
	do_pianoDb2( odir )

#!/usr/bin/python

"""
bass.py

Analyze single acoustic bass tones, plucked and 
arco, taken from the Iowa samples. Channelized with
fundamental extracted during analysis.

- plucked open D string (D two octaves below middle C)
- plucked G on the D string (G two octaves below middle C)
- bowed Db on the A string (?) (Db two octaves below middle C)

Last updated: 1 July 2008 by Kelly Fitz
"""

import loris, time, os

orate = 44100

tag = ''

stuff = {}

# ----------------------------------------------------------------------------

def doBassD2( exportDir = '' ):
	name = 'bass.D2'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	fHz = 73.41
	anal = loris.Analyzer( .7*fHz, 1.8*fHz )
	anal.setFreqDrift( .2*fHz )
	anal.setAmpFloor(-75)
	anal.buildFundamentalEnv( .9*fHz, 1.1*fHz )
	p = anal.analyze( f.samples(), f.sampleRate() )
	
	print 'distilling %s (%s)'%(name, time.ctime(time.time()))
	ref = anal.fundamentalEnv()
	loris.channelize( p, ref, 1 )
	#loris.sift( p )
	#loris.removeLabeled( p, 0 )
	loris.distill( p )	
	
	if exportDir:
		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMidiNoteNumber( 38 )
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

def doBassG2( exportDir = '' ):
	name = 'bass.G2'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	fHz = 97
	anal = loris.Analyzer( .7*fHz, 1.8*fHz )
	anal.setFreqDrift( .2*fHz )
	anal.setAmpFloor(-75)
	anal.buildFundamentalEnv( .9*fHz, 1.1*fHz )
	p = anal.analyze( f.samples(), f.sampleRate() )
	
	print 'distilling %s (%s)'%(name, time.ctime(time.time()))
	ref = anal.fundamentalEnv()
	loris.channelize( p, ref, 1 )
	#loris.sift( p )
	#loris.removeLabeled( p, 0 )
	loris.distill( p )
	
	if exportDir:
		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMidiNoteNumber( 43 )
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

def doBassDb2Arco( exportDir = '' ):
	name = 'bass.Db2.arco'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	fHz = 69.3
	anal = loris.Analyzer( .85*fHz, 1.65*fHz )
	anal.setFreqDrift( .2*fHz )
	anal.setBwRegionWidth(0)
	# anal.buildFundamentalEnv( .9*fHz, 1.1*fHz )
	p = anal.analyze( f.samples(), f.sampleRate() )
	
	print 'distilling %s (%s)'%(name, time.ctime(time.time()))
	# ref = anal.fundamentalEnv()
	ref = loris.createF0Estimate( p, .9*fHz, 1.1*fHz, 0.01 )
	loris.channelize( p, ref, 1 )
	
	Fade = 0.01
	Gap = 0.005
	#loris.sift( p )
	#loris.removeLabeled( p, 0 )
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
		
	doBassD2( odir )
	doBassG2( odir )
	doBassDb2Arco( odir )

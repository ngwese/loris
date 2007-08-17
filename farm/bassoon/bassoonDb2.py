#!/usr/bin/python

"""
bassoonDb2.py

Analyze a single  bassoon tone D flat 2 (Db two octaves below 
middle C), taken from the Iowa samples.


Last updated: 10 Aug 2007 by Kelly Fitz
"""

import loris, time, os

orate = 44100

tag = ''

stuff = {}

# ----------------------------------------------------------------------------

def doBassoonDb2( exportDir = '' ):
	name = 'bassoon.Db2'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	fHz = 69.69
	anal = loris.Analyzer( .7*fHz, 1.8*fHz )
	anal.setFreqDrift( 10 )
	anal.buildFundamentalEnv( .9*fHz, 1.1*fHz )
	p = anal.analyze( f.samples(), f.sampleRate() )
	
	print 'distilling %s (%s)'%(name, time.ctime(time.time()))
	ref = anal.fundamentalEnv()
	# ref = loris.createFreqReference( p, .9*fHz, 1.1*fHz )
	loris.channelize( p, ref, 1 )
	Fade = 0.01
	loris.distill( p, Fade )
	
	if exportDir:
		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMidiNoteNumber( 37.11 )
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
		
	doBassoonDb2( odir )


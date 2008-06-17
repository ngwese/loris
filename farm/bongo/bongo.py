#!/usr/bin/python

"""
bongo.py

Analyze the bongo drum roll that was used in my
dissertation work, and was part of the ICMC 2000 bakeoff.

200 and 300 Hz resolutions sound very similar
frequency floor at 200 is no different from floor at 300 hz

Can distill this with fixed-frequency (200 Hz) channels without 
destroying it.

Last updated: 16 June 2008 by Kelly Fitz
"""

import loris, time, os

orate = 44100

tag = ''

stuff = {}

# ----------------------------------------------------------------------------

def doBongo( exportDir = '' ):

	name = 'bongoroll'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	anal = loris.Analyzer( 300, 800 )
	anal.setFreqFloor( 200 )
	anal.setFreqDrift( 50 )
	anal.setBwRegionWidth( 0 )
	p = anal.analyze( f.samples(), f.sampleRate() )

	print 'cropping %s (%s)'%(name, time.ctime(time.time()))
	loris.crop( p, 0, 20 )
	
 	# Need to specify non-default value for gap time to achieve
 	# good results (as in Loris 1.4)
 	Fade = 0.001
 	Gap = 0.0001
 	print 'collating %s (%s)'%(name, time.ctime(time.time()))
	pcollate = loris.PartialList( p )
	loris.collate( pcollate, Fade, Gap )

	print 'distilling %s at 200 Hz (%s)'%(name, time.ctime(time.time()))
	env = loris.LinearEnvelope( 200 )
	loris.channelize( p, env, 1 )
	loris.distill( p, Fade, Gap )

	
	if exportDir:
		print 'synthesizing %i collated partials (%s)'%(pcollate.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( pcollate, orate )
		
		opath = os.path.join( exportDir, name + tag + '.raw.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.raw.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( pcollate )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )
		

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

	stuff[ name ] = ( p, pcollate, anal )
	
	print 'Done. (%s)'%(time.ctime(time.time()))

	


# ----------------------------------------------------------------------------

if __name__ == '__main__':
	print __doc__

	print 'Using Loris version %s'%( loris.version() )

	import sys
	odir = os.curdir
	if len( sys.argv ) > 1:
		tag = '.' + sys.argv[1]
		
	doBongo( odir )



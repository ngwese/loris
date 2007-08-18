#!/usr/bin/python

"""
derbari.py

Analyze and reconstruct the raga singing sample that was included in the 
ICMC 2000 bakeoff and called then "shafqat-derbari."

There is something going on at high-ish frequencies that might best be captured
as noise, but bandwidth enhancement and distilling doesn't get it. Sounds a
little better if the zero-labeled partials are retained.

Last updated: 17 Aug 2007 by Kelly Fitz
"""

import loris, time, os

orate = 44100

tag = ''

stuff = {}

# ----------------------------------------------------------------------------

def doDerbari( exportDir = '' ):
	
	name = 'derbari'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	anal = loris.Analyzer( 140, 280 )
	anal.setFreqFloor( 100 )
	anal.setFreqDrift( 20 )
	anal.setBwRegionWidth( 0 )
	p = anal.analyze( f.samples(), f.sampleRate() )

	ref = loris.createFreqReference( p, 140, 330 )
	
	
	print 'collating %s (%s)'%(name, time.ctime(time.time()))
	pcollate = loris.PartialList( p )
	loris.collate( pcollate )

	print 'sifting %s (%s)'%(name, time.ctime(time.time()))
	psift = loris.PartialList( p )
	loris.channelize( psift, ref, 1 )
	loris.sift( psift )
	# loris.removeLabeled( psift, 0 )
	loris.distill( psift )

	print 'distilling %s at 200 Hz (%s)'%(name, time.ctime(time.time()))
	loris.channelize( p, ref, 1 )
	loris.distill( p )

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

		print 'synthesizing %i sifted partials (%s)'%(psift.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( psift, orate )
		
		opath = os.path.join( exportDir, name + tag + '.sift.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.sift.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( psift )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )
		

		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.distill.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.distill.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )

	stuff[ name ] = ( p, pcollate, psift, anal, ref )
	
	print 'Done. (%s)'%(time.ctime(time.time()))



# ----------------------------------------------------------------------------

if __name__ == '__main__':
	print __doc__

	print 'Using Loris version %s'%( loris.version() )

	import sys
	odir = os.curdir
	if len( sys.argv ) > 1:
		tag = '.' + sys.argv[1]
		
	doDerbari( odir )

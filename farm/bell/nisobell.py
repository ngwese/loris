#!/usr/bin/python

"""
nisobell.py

Analyze a noisy temple bell sample recorded in Niso.
	
95 Hz resolution with a 200 Hz window works well, can be
harmonically distilled at fundamentals up to 150, can also have lots
of little noisy partials pruned out. Prominent partials in this tone
are approximately 105 Hz (1), 271 Hz (2), 398 Hz (4), 541 Hz (5),
689 Hz (6).


Last updated: 15 Aug 2007 by Kelly Fitz
"""
import loris, time, os

orate = 44100

tag = ''

stuff = {}

# ----------------------------------------------------------------------------

def doBell( exportDir = '' ):
	name = 'nisobell'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	anal = loris.Analyzer( 95, 200 )
	anal.setFreqDrift( 30 )
	
	p = anal.analyze( f.samples(), f.sampleRate() )
	
	print 'collating %s (%s)'%(name, time.ctime(time.time()))
	pcollate = loris.PartialList( p )
	loris.collate( pcollate )

	print 'pruning very short partials before .2 and after .5 seconds'
	pruneMe = -1
	for part in p:
		if (part.duration() < .2) and ((part.startTime() > .5) or (part.endTime() < .2)):
			part.setLabel( pruneMe )
	loris.removeLabeled( p, pruneMe )
	
	
	distfreq = 110
	print 'distilling %s at %d Hz (%s)'%(name, distfreq, time.ctime(time.time()))
	env = loris.LinearEnvelope( distfreq )
	loris.channelize( p, env, 1 )
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
		

		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.clean.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.clean.sdif' )
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
		
	doBell( odir )

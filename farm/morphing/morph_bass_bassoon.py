#!/usr/bin/python

"""
morph_bass_bassoon.py

Morph a bassoon tone (D flat 2) with an acoustic
bass tone (D 2). This is the poster child for phase accuracy
in morphing, owing to the sensitivity of the bassoon sound to
accurate phase maintenance.

The only difference between this and my phase accurate morphing
experiments in C++ is that I am not fixing the frequencies here,
but I think that we don't need to anymore, now that the Morpher
has been "improved".

Last updated: 28 Sep 2009 by Kelly Fitz
"""
print __doc__

import loris, time, os

print """
Using Loris version %s
"""%loris.version()

orate = 44100

tag = ''

stuff = {}

# ----------------------------------------------------------------------------

def doBassoonBassMorph( exportDir = '' ):

	#	time points
	tbassbeg = .47
	tbasspeak = .52
	tbassend = 4.835
	
	tsoonbeg = 0.053
	tsoonend = 2.478
	
	morphTimes_old = [ tbasspeak, tbasspeak+.8, tbasspeak+1.6 ]
	morphTimes_new = [ tbasspeak+.1, tbasspeak+.8, tbasspeak+1.6 ]
	morphTimes_third = [ tbasspeak+.1, tbasspeak+.85, tbasspeak+1.6 ]
	morphTimes = morphTimes_new
			
	morphMidpoint = 0.5
	finalMorphVal = 1
	
	from os import path
	bass_dir = path.join( path.pardir, "bass" )
	print "--- importing bass from %s ---"%(bass_dir)
	bass = loris.importSdif( path.join( bass_dir, "bass.D2.sdif" ) )
	
	print "--- cropping and pitch-shifting bass ---"
	loris.crop( bass, tbassbeg, tsoonend )
	loris.shiftPitch( bass, -(50+11) )
		# bassoon is 11 cents sharp, and bass is way off
	
	bassoon_dir = path.join( path.pardir, "bassoon" )
	print "--- importing bassoon from %s ---"%(bassoon_dir)
	bassoon = loris.importSdif( path.join( bassoon_dir, "bassoon.Db2.sdif" ) )
	
	print "--- chopping junk off front of bassoon ---"
	loris.crop( bassoon, tsoonbeg, 2*tsoonend )
	
	print "--- dilating (shifting start time of) bassoon ---"
	loris.dilate( bassoon, [0], [ tbassbeg - tsoonbeg ] )
	
	print "--- morphing ---"
	env = loris.BreakpointEnvelope()
	env.insertBreakpoint( morphTimes[0], 0 )
	env.insertBreakpoint( morphTimes[1], morphMidpoint*finalMorphVal )
	env.insertBreakpoint( morphTimes[2], finalMorphVal )
	
	# morph using reference partials, sounds much worse without this!
	m = loris.morph( bass, bassoon, 1, 1, env, env, env )
	
	print "--- removing noise from morph ---"
	loris.scaleNoiseRatio( m, 0 ) 

	if exportDir:
	
		print "--- rendering ---"
		opath = os.path.join( exportDir, 'bass-oon.morph' + tag + '.aiff' ) 
		
		fout = loris.AiffFile( m, orate )
		fout.setMidiNoteNumber( 37.11 )
		fout.write( opath )
	
		# export the dilated data as SDIF files.
		print "--- exporting Sdif files ---"
		
		opath = os.path.join( exportDir, 'bass.aligned' + tag + '.sdif' ) 
		loris.exportSdif( opath, bass )
		
		opath = os.path.join( exportDir, 'bassoon.aligned' + tag + '.sdif' ) 
		loris.exportSdif( opath, bassoon )

	stuff[ 'bassoon aligned' ] = bassoon
	stuff[ 'bass aligned' ] = bass
	stuff[ 'morph' ] = m
	stuff[ 'times' ] = morphTimes
	
	print 'Done. (%s)'%(time.ctime(time.time()))

# ----------------------------------------------------------------------------

if __name__ == '__main__':
	print __doc__

	print 'Using Loris version %s'%( loris.version() )

	import sys
	odir = os.curdir
	if len( sys.argv ) > 1:
		tag = '.' + sys.argv[1]
		
	doBassoonBassMorph( odir )
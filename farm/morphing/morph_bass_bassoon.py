#!/usr/bin/python

"""
morph_bass_bassoon.py

Python script for testing the morphing capabilities in Loris. 

This script morphs a bassoon tone (D flat 2) with an acoustic
bass tone (D 2). This is the poster child for phase accuracy
in morphing, owing to the sensitivity of the bassoon sound to
accurate phase maintenance.

The only difference between this and my phase accurate morphing
experiments in C++ is that I am not fixing the frequencies here,
but I think that we don't need to anymore, now that the Morpher
has been "improved".

Last updated: 30 July 2005 by Kelly Fitz
"""
print __doc__

import loris, time

print """
Using Loris version %s
"""%loris.version()


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

print "--- rendering ---"
fout = loris.AiffFile( m, 44100 )
fout.write( "bass-oon.morph.aiff" )

# export the dilated data as spc files for Kyma morphing.
print "--- exporting Spc files for Kyma morphing ---"

ofilebase = "bass.aligned"
loris.exportSdif( ofilebase + ".sdif", bass )
loris.exportSpc( ofilebase + '.s.spc', bass, 37.11, 0 ) 
loris.exportSpc( ofilebase + '.e.spc', bass, 37.11, 1 ) 

ofilebase = "bassoon.aligned"
loris.exportSdif( ofilebase + ".sdif", bassoon )
loris.exportSpc( ofilebase + '.s.spc', bassoon, 37.11, 0 ) 
loris.exportSpc( ofilebase + '.e.spc', bassoon, 37.11, 1 ) 


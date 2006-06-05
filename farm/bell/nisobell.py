#!/usr/bin/python

"""
nisobell.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

	
Niso bell: 
	95 Hz resolution with a 200 Hz window works well, can be
	harmonically distilled at fundamentals up to 150, can also have lots
	of little noisy partials pruned out. Prominent partials in this tone
	are approximately 105 Hz (1), 271 Hz (2), 398 Hz (4), 541 Hz (5),
	689 Hz (6).

Last updated: 5 June 2006 by Kelly Fitz
"""
print __doc__

import loris, time

print """
Using Loris version %s
"""%loris.version()

anal = loris.Analyzer( 95, 200 )
orate = 44100

# analyze nisobell
name = 'nisobell'
f = loris.AiffFile( name + '.aiff' )
print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
p = anal.analyze( f.samples(), f.sampleRate() )

# nisobell collated
pcollate = loris.PartialList( p )
loris.collate( pcollate )
print 'synthesizing raw (collated) %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( pcollate, orate )
loris.exportAiff( name + '.raw.aiff', samples, orate )
loris.exportSdif( name + '.raw.sdif', pcollate )


print 'pruning very short partials before .2 and after .5 seconds'
pruneMe = -1
for part in p:
	if (part.duration() < .2) and ((part.startTime() > .5) or (part.endTime() < .2)):
		part.setLabel( pruneMe )
loris.removeLabeled( p, pruneMe )

# nisobell harmonically distilled at 110 Hz
env = loris.LinearEnvelope( 110 )
loris.channelize( p, env, 1 )
loris.distill( p )
print 'synthesizing harmonically distilled (110 Hz) %s (%s)'%(name, time.ctime(time.time()))
loris.exportAiff( name + '.clean.aiff', p, orate )
loris.exportSdif( name + '.clean.sdif', p )

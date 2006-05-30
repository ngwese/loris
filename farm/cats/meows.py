#!/usr/bin/python

"""
meows.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

Cats' meows (meow1 and meow3): 
	Analyze with 75 Hz resolution and 240 Hz window, distill at 5 Partials
	per harmonic for meow1, and 3 Partials per harmonic for meow3.
	
It is no longer really necessary to use more than one partial per harmonic,
a usable sound can be constructed using just one per harmonic. Using a long
narrow window helps smooth out some of the crud introduced by all the 
background hiss and rumble. A slightly more natural sound is produced 
using two partials per harmonic, bandwidth enhancement doesn't seem to help.
This is more important for meow1 than meow3.

Last updated: 30 May 2006 by Kelly Fitz
"""

print __doc__

import loris, time
print """
Using Loris version %s
"""%loris.version()

orate = 44100

##############################################################################################

# analyze meow1
name = 'meow1'
f = loris.AiffFile( name + '.aiff' )
print 'analyzing %s (%s)'%(name, time.ctime(time.time()))

# set resolution to capture two partials per harmonic,
# can also increase resolution if only one partial
# per harmonic is desired. Other essential analysis
# parameters are unaffected.
anal = loris.Analyzer( 150, 500 )
anal.setBwRegionWidth( 0 ) # disable BW enhancement, doesn't help
anal.setFreqDrift( 40 )
anal.setFreqFloor( 210 )
p = anal.analyze( f.samples(), f.sampleRate() )

# meow1 at 2 Partials per harmonic
N = 2
ref = loris.createFreqReference( p, 470, 620 )
loris.channelize( p, ref, N )
loris.distill( p )
print 'exporting %i distilled Partials per harmonic %s (%s)'%(N, name, time.ctime(time.time()))
loris.exportAiff( name + '.%i.recon.aiff'%(N), p, orate )
loris.exportSdif( name + '.%i.sdif'%(N), p )

# meow1 at 1 Partial per harmonic,
# remove in-between harmonic partials
for part in p:
    if 0 == part.label()%N:
        part.setLabel( part.label() / N )
    else:
        part.setLabel( 0 )
loris.removeLabeled( p, 0 )
print 'exporting 1 distilled Partials per harmonic %s (%s)'%(name, time.ctime(time.time()))
loris.exportAiff( name + '.1.recon.aiff', p, orate )
loris.exportSdif( name + '.1.sdif', p )

 
##############################################################################################

# analyze meow3 
name = 'meow3'
f = loris.AiffFile( name + '.aiff' )
print 'analyzing %s (%s)'%(name, time.ctime(time.time()))

# set resolution to capture two partials per harmonic,
# can also set resolution to 380 if only one partial
# per harmonic is desired. Other essential analysis
# parameters are unaffected.
anal = loris.Analyzer( 190, 400 )
anal.setFreqDrift( 40 )
anal.setFreqFloor( 210 )
anal.setBwRegionWidth( 0 ) # disable BW enhancement, doesn't help
p = anal.analyze( f.samples(), f.sampleRate() )

# meow3 at 2 Partials per harmonic
N = 2
ref = loris.createFreqReference( p, 400, 600 )
loris.channelize( p, ref, N )
loris.distill( p )
print 'exporting %i distilled Partials per harmonic %s (%s)'%(N, name, time.ctime(time.time()))
loris.exportAiff( name + '.%i.recon.aiff'%(N), p, orate )
loris.exportSdif( name + '.%i.sdif'%(N), p )

# meow3 at 1 Partial per harmonic,
# remove in-between harmonic partials
for part in p:
    if 0 == part.label()%N:
        part.setLabel( part.label() / N )
    else:
        part.setLabel( 0 )
loris.removeLabeled( p, 0 )
print 'exporting 1 distilled Partials per harmonic %s (%s)'%(name, time.ctime(time.time()))
loris.exportAiff( name + '.1.recon.aiff', p, orate )
loris.exportSdif( name + '.1.sdif', p )

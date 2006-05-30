"""
bongo.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the bongo drum roll that was used in my
dissertation work, and was part of the ICMC 2000 bakeoff. A the 
time of the bakeoff, we liked to use 300 Hz resolution, a 800 Hz
window, and frequency floor at 200 Hz. 

trial 1:
	- 400 Hz resolution just sounds wrong, almost like the wrong pitch
	- 200 and 300 Hz resolutions sound very similar
	- different windows are pretty hard to distinguish too

trial 2: 
	- frequency floor at 200 is no different from floor at 300 hz
	- 500 Hz window isn't quite right either

Can distill this with fixed-frequency (200 Hz) channels without 
destroying it too.

Last updated: 26 May 2006 by Kelly Fitz
"""

print __doc__

import loris, time
print "using Loris version", loris.version()

source = 'bongoroll.aiff'
file = loris.AiffFile( source )
samples = file.samples()
rate = file.sampleRate()

a = loris.Analyzer( 300, 800 )
a.setBwRegionWidth( 0 )
a.setFreqFloor( 200 )
a.setFreqDrift( 50 )

p = a.analyze( samples, rate )
loris.crop( p, 0, 20 )

praw = loris.PartialList( p )
loris.collate( praw )

# export raw
loris.exportAiff( 'bongo.raw.recon.aiff', praw, rate )
loris.exportSdif( 'bongo.raw.sdif', praw  )


ref = loris.LinearEnvelope( 200 )
loris.channelize( p, ref, 1 )
loris.distill( p, 0.001 )

# export distilled
loris.exportAiff( 'bongo.recon.aiff', p, rate )
loris.exportSdif( 'bongo.sdif', p  )

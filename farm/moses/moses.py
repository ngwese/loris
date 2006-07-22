#!/usr/bin/python

"""
moses.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the speech sample contributed to the 
dissertation experiments by Kurt Hebel. It is Kenneth Branaugh
(sp?) speaking in a low, slow half-whisper, so it is pretty noisy
but the noise is pitched. 

The parameters we once liked for this are resolution 90 and
window width 200 Hz.

notes from trial 1 (using 1.0beta8):
	- none of these sounds as present as the original, all are 
	slightly reverberant
	- maybe need a lower freq threshold?
	- 100 Hz windows are no good
	- 100 Hz res is marginal
	- with 200 and 300 windows and res 70,80,or 90 Hz, all sound pretty
	much the same, and very usable

more recent notes, still trial 1, but Loris 1.0.1beta1:
	- 100 Hz window is too narrow, 200 Hz window is better than
	300 Hz.
	- resoultion 90 and 100 are too wide, 70 and 80 are slightly
	different, maybe, but its hard to pick one as better.
	- these notes pertain to a batch run with a modification to the 
	analyzer, such that spectral peaks below the noise floor are 
	_not_ accumulated as noise energy. Verify the above with the
	unmodified Analyzer.
	
notes from trial 2 with Loris 1.2.0beta2:
	- these both sound quite good, and they are difficult to 
	distinguish, and they both need some attention to the noisy
	parts.

Last updated: 22 July 2006 by Kelly Fitz
"""
print __doc__

import loris, time

name = 'moses'

file = loris.AiffFile( name + '.aiff' )
samples = file.samples()
rate = file.sampleRate()

print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
a = loris.Analyzer( 80, 200 )
p = a.analyze( samples, rate )

print 'synthesizing raw (collated) %s (%s)'%(name, time.ctime(time.time()))
loris.collate( p )
samples = loris.synthesize( p, rate )

print 'exporting raw (collated) %s (%s)'%(name + '.raw', time.ctime(time.time()))
loris.exportAiff( name + '.raw.aiff', samples, rate )
loris.exportSdif( name + '.raw.sdif', p )


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

notes from trial 1:
	- none of these sounds as present as the original, all are 
	slightly reverberant
	- maybe need a lower freq threshold?
	- 100 Hz windows are no good
	- 100 Hz res is marginal
	- with 200 and 300 windows and res 70,80,or 90 Hz, all sound pretty
	much the same

Last updated: 4 Feb 2002 by Kelly Fitz
"""
print __doc__

import loris, time
from trials import *

# use this trial counter to skip over
# eariler trials
trial = 1

print "running trial number", trial, time.ctime(time.time())

source = 'moses.aiff'

if trial == 1:
	resolutions = (70,80,90,100)
	widths = ( 100,200,300 )
	for r in resolutions:
		for w in widths:
			p = analyze( source, r, w )
			ofile = 'moses.%i.%i.aiff'%(r, w)
			synthesize( ofile, p )

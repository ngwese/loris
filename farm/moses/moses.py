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

Last updated: 5 Dec 2001 by Kelly Fitz
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

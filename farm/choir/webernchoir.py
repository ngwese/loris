#!/usr/bin/python

"""
webernchoir.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to Webern choir recording that we used 
intermittently for a long time, and for which we finally got 
mediocre results. The polyphony and close harmony and the low
male voices make this one hard, but there aren't any transients.

The parameters we settled on were resolution 15 Hz, floor 50 Hz, 
window 100 Hz, region width 5 kHz.

Last updated: 21 May 2001 by Kelly Fitz
"""

print __doc__

import loris, time
from trials import *

# use this trial counter to skip over
# eariler trials
trial = 1

print "running trial number", trial, time.ctime(time.time())

source = 'webernchoir.aiff'

if trial == 1:
	resolutions = ( 15, 20, 30, 40, 50 )
	widths = ( 50, 70, 90, 110, 150 )
	bws = ( 2000, 5000 )
	for r in resolutions:
		for w in widths:
			for bw in bws:
				p = analyze( source, r, w, bw )
				ofile = 'webern.%i.%i.%ik.aiff'%(r, w, bw/1000)
				synthesize( ofile, p )


#!/usr/bin/python

"""
elephants.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to three elephant sounds that were candidates 
for the Toy Angst animation. None of these came out great in the past.

The parameters we once liked for these were:

elephant1 (formerly oldelephant): resolution 50 Hz, window width 200 Hz,
	reference envelope was created from the longest partial below 1 kHz, 
	and corresponded to the 12th quasi-harmonic channel

elephant2: ("barely usable") resolution 40 Hz, window 200 Hz,
	reference envelope was created from the longest partial below 1 kHz, 
	and corresponded to the 18th quasi-harmonic channel, but was much 
	shorter than the sound iteslf

elephant3 (formerly valentele): resolution 50 Hz, window 150 Hz,
	a flat 20 Hz reference envelope was used as the first quasi-harmonic 
	channel

Last updated: 21 May 2001 by Kelly Fitz
"""

print __doc__

import loris, time
from trials import *

# use this trial counter to skip over
# eariler trials
trial = 1

print "running trial number", trial, time.ctime(time.time())

sources = ( 'elephant1.aiff', 'elephant2.aiff', 'elephant3.aiff' )


if trial == 1:
	for source in sources:
		resolutions = (20, 30, 40, 50, 60, 70, 80 )
		widths = ( 100, 130, 150, 180, 200, 250 )
		bws = (1000, 2000, 4000)
		for r in resolutions:
			for w in widths:
				if w == 0:
					w = r
				for bw in bws:
					p = analyze( source, r, w, bw )
					ofile = '%s.%i.%i.%ik.aiff'%(source[:-5], r, w, bw/1000)
					synthesize( ofile, p )

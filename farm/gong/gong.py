#!/usr/bin/python

"""
gong.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the orchestral gong sample used in the
dissertation experiments. It is taken from the MUMS disks, and
has a very noisy sounding onset, but also some sustained partials
giving the gong its pitch, and a strong thud in the attack that
has been hard to reproduce.

The parameters we once liked for this are resolution 25 and
window width 75 Hz.

notes from trial 1:
	- 50 Hz has artifacts
	- 200 Hz window is too wide
	- 20,25, 30 res with 75 or 100 window are hard to distinguish

Last updated: 4 Feb 2002 by Kelly Fitz
"""
print __doc__

import loris, time
from trials import *

# use this trial counter to skip over
# eariler trials
trial = 1

print "running trial number", trial, time.ctime(time.time())

source = 'gong.aiff'

if trial == 1:
	resolutions = (20,25,30,50)
	widths = ( 75,100,200 )
	funds = ( 50,100,150 )
	for r in resolutions:
		for w in widths:
			p = analyze( source, r, w )
			ofile = 'gong.%i.%i.aiff'%(r, w)
			synthesize( ofile, p )
			for f in funds:
				p2 = p.copy()
				harmonicDistill( p2, f )
				ofile = 'gong.%i.%i.d%i.aiff'%(r, w, f)
				synthesize( ofile, p2 )

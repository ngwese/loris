#!/usr/bin/python

"""
french.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the french speech excerpt used in the
dissertation experiments. It is taken from a radio broadcast
included on the Pierre Schaeffer collection. The speaker has
a very low (male) voice, which was difficult to reproduce
without a slightly reverberant quality.

The parameters we once liked for this are resolution 60 and
window width 200 Hz.

notes from trial 1:
	- 80 Hz is unusable
	- 60,200 is a little noisy, as is 40,200
	- 60,300 sounds a little hoarse, as does 40,300
	- 60,100 is usable

Last updated: 4 Feb 2002 by Kelly Fitz
"""
print __doc__

import loris, time
from trials import *

# use this trial counter to skip over
# eariler trials
trial = 1

print "running trial number", trial, time.ctime(time.time())

source = 'french.aiff'

if trial == 1:
	resolutions = (40,60,80)
	widths = ( 100,200,300 )
	for r in resolutions:
		for w in widths:
			p = analyze( source, r, w )
			ofile = 'french.%i.%i.aiff'%(r, w)
			synthesize( ofile, p )

#!python

"""
saxriff.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the solo alto sax riff of about 17 notes
with some ambient reverb. To do this one correctly, we will need
to come up with a fancy channelizer, and probably need to break 
Partials at note boundaries.

The parameters we once liked for this are resolution 90 and
window width 140 Hz.

First pass notes 4 May 2001:

- windows narrower than 140 Hz are too smooshy
- 200 Hz window give best results
- resolution > 100 Hz is not good
- bw region width 2k and 4k are indistinguishible
- acceptible (and quite good) parameters are 
	res 65, 75, 90 Hz
	window 200 Hz

Last updated: 4 Oct 2001 by Kelly Fitz
"""
print __doc__

import loris, time
from trials import *

# use this trial counter to skip over
# eariler trials
trial = 2

print "running trial number", trial, time.ctime(time.time())

source = 'saxriff.aiff'

if trial == 1:
	resolutions = (65, 75, 90, 100, 110, 125)
	widths = ( 0, 90, 140, 200 )
	bws = (2000, 4000)
	for r in resolutions:
		for w in widths:
			if w == 0:
				w = r
			for bw in bws:
				p = analyze( source, r, w, bw )
				ofile = 'sax.%i.%i.%ik.aiff'%(r, w, bw/1000)
				synthesize( ofile, p )


if trial == 2:
	resolutions = (65, 75, 90)
	widths = ( 140, 170, 200, 250 )
	for r in resolutions:
		for w in widths:
			p = analyze( source, r, w )
			ofile = 'sax.%i.%i.aiff'%(r, w)
			synthesize( ofile, p )

	

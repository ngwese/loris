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

notes from trial 1:
	elephant1:
	- 1k bandwidth association regions aren't wide enough, gets showery
	- windows narrower than 150 Hz are also showery at large resolutions,
		but not so much at lower resolutions, which sound better anyway
	- resolution 80 is bad, 70 is marginal, less than 60 is much better
	- 250 Hz is too wide for the window, sounds a little flangey with 
		small resolutions
	- resolutions 20, 30, 40, 50, 60 are all pretty good for windows
		130, 150, and 180 Hz
		
	elephant2:
	- nothing sounds good
	- resolutions greater than 30 Hz don't work at all, _way_ flangey
	- 20 and 30 Hz resolution with window 100 Hz are the best of the
		batch, maybe need an even narrower window
	- of the ones that sounded at all usable, region width didn't make
		an audible difference
	
	elephant3:
	- resolution greater than 50 Hz doesn't work well, 40 Hz is marginal
	- resolutions 20 and 30 Hz work okay with narrow windows, best is 
		100 Hz window, maybe need even narrower
	- res 20 Hz and window 100 Hz is pretty good, and with those
		conditions, region width doesn't have any effect
	
notes fro trial 2:
	elephant1:	
	- 20 and 40 Hz resolution sound pretty good with all windows, 60 hz
	sounds okay with wider windows (like 200 hz)
	
	elephant2: 
	- sounds a bit metallic with all these parameters settings
	
	elephant3:
	- 80 Hz window sounds pretty good, even 100 is pretty good. 
	
	Next: try 1 and 3 with time dilation, dunno what to do about 2

Last updated: 8 Oct 2001 by Kelly Fitz
"""

print __doc__

import loris, time
from trials import *

# use this trial counter to skip over
# eariler trials
trial = 3

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

if trial == 2:
	# elephant1.aiff
	source = sources[0]  
	resolutions = (20, 40, 60 )
	widths = ( 130, 150, 180, 200 )
	for r in resolutions:
		for w in widths:
			p = analyze( source, r, w )
			ofile = '%s.%i.%i.aiff'%(source[:-5], r, w)
			synthesize( ofile, p )
	# elephant2.aiff and elephant3.aiff
	for source in sources[1:]:
		resolutions = (20, 30 )
		widths = ( 80, 100 )
		for r in resolutions:
			for w in widths:
				p = analyze( source, r, w )
				ofile = '%s.%i.%i.aiff'%(source[:-5], r, w)
				synthesize( ofile, p )

if trial == 3:
	# elephant1.aiff
	source = sources[0]  
	resolutions = ( 20, 40 )
	widths = ( 130, 180 )
	for r in resolutions:
		for w in widths:
			p = analyze( source, r, w )
			p = timescale( p, 2. )
			ofile = '%s.%i.%i.T2aiff'%(source[:-5], r, w)
			synthesize( ofile, p )
	# elephant3.aiff
	source = sources[2]  
	resolutions = ( 20, 30 )
	widths = ( 80, 100, 130 )
	for r in resolutions:
		for w in widths:
			p = analyze( source, r, w )
			p = timescale( p, 2. )
			ofile = '%s.%i.%i.T2aiff'%(source[:-5], r, w)
			synthesize( ofile, p )

#!/usr/bin/python

"""
nisobell.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the temple bell from Niso (Japan?). It
is a noisy field recording, truncatd at the end, with some audible
squeaking of the bell suspension, and lots of hiss. It has an 
interesting attack though, and a nice bell timbre, which is simple
but not harmonic.

The parameters we once liked for this are resolution 60 and
window width 105 Hz, and bandwidth region width 2400 Hz.

first pass notes 4 May 2001:
- the lowest tone in the bell is below 110 Hz, so resolutions
	greater than that loose a big part of the sound, unless the
	freq floor is lowered.
- hiss in the source makes artifacts in reconstruction, less 
	objectionable, maybe, in wider windows (200 Hz)
- windows 140 and 200 Hz are best, 100 and less are unacceptible
- bandwidth region width doesn't have much effect
- with a 200 Hz wide window, resolutions 30, 40, 50, 60, 70, 80, 95
	are hard to distinguish

Second trial attempted distillations.
Third trial added some even wider windows (since the widest,
200 Hz, always sounded best). 

note: distillation doesn't seem to have a deliterious effect

Fourth trial is similar to trial three, but with the new sifting
Distiller, fewer resolutions (since they are almost indistinguishible),
and te standard Bw regions width (2k).

notes from trial 4:
	- the choice of resolution doesn't seem to affect the quality of 
	the bell sound, only the background noise sounds different with
	different resolutions
	- narrower windows increase the noisiness (sounds like a noise burst)
	in the attack, most evident with larger resolutions. 
	- can use 70 or 95 Hz resolution with wider windows and get good results
	- distillation works well for all values tried (up to 120, try larger)

Last updated: 4 Oct 2001 by Kelly Fitz
"""
print __doc__

import loris, time
from trials import *

# use this trial counter to skip over
# eariler trials
trial = 5

print "running trial number", trial, time.ctime(time.time())

source = 'nisobell.aiff'

if trial == 1:
	resolutions = (30, 40, 50, 60, 70, 80, 95, 110)
	widths = ( 0, 85, 105, 140, 200 )
	bws = (1400, 2400, 4000)
	for r in resolutions:
		for w in widths:
			if w == 0:
				w = r
			for bw in bws:
				p = analyze( source, r, w, bw )
				ofile = 'bell.%i.%i.%ik.aiff'%(r, w, bw/1000)
				synthesize( ofile, p )


if trial == 2:
	resolutions = (30, 40, 50, 60, 70, 80, 95)
	widths = ( 140, 200 )
	bws = (2400, )
	funds = (30, 45, 60, 75)
	for r in resolutions:
		for w in widths:
			if w == 0:
				w = r
			for bw in bws:
				for f in funds:
					p = analyze( source, r, w, bw )
					harmonicDistill( p, f )
					ofile = 'bell.%i.%i.%ik.d%i.aiff'%(r, w, bw/1000, f)
					synthesize( ofile, p )

if trial == 3:
	resolutions = (30, 40, 50, 60, 70, 80, 95)
	widths = ( 220, 250, 300 )
	bws = (2400, )
	funds = (30, 45, 60, 75)
	for r in resolutions:
		for w in widths:
			if w == 0:
				w = r
			for bw in bws:
				for f in funds:
					p = analyze( source, r, w, bw )
					ofile = 'bell.%i.%i.%ik.aiff'%(r, w, bw/1000)
					synthesize( ofile, p )
					harmonicDistill( p, f )
					ofile = 'bell.%i.%i.%ik.d%i.aiff'%(r, w, bw/1000, f)
					synthesize( ofile, p )


if trial == 4:
	resolutions = (40, 70, 95)
	widths = ( 100, 200, 300 )
	bw = 2000
	funds = (30, 60, 90, 120)
	for r in resolutions:
		for w in widths:
			p = analyze( source, r, w, bw )
			ofile = 'bell.%i.%i.aiff'%(r, w)
			synthesize( ofile, p )
			for f in funds:
				p2 = p.copy()
				harmonicDistill( p2, f )
				ofile = 'bell.%i.%i.d%i.aiff'%(r, w, f)
				synthesize( ofile, p2 )

if trial == 5:
	resolutions = (70, 95)
	widths = ( 200, 300 )
	funds = ( 90, 120, 150, 180 )
	for r in resolutions:
		for w in widths:
			p = analyze( source, r, w )
			ofile = 'bell.%i.%i.aiff'%(r, w)
			synthesize( ofile, p )
			for f in funds:
				p2 = p.copy()
				harmonicDistill( p2, f )
				ofile = 'bell.%i.%i.d%i.aiff'%(r, w, f)
				synthesize( ofile, p2 )

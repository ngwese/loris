#!python

"""
cellos.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to several cellos that Lippold uses for his 
timbre space, one of which (cello154) we used at the ICMC 2000  
presentations and in Kelly's dissertation.

Typically, we have used resolution and window width slightly less 
than the fundamental (154 Hz) for cello154, and for cello 69, resolution
slightly less than the fundamental (69 Hz) and a window 1.8 times
as wide.

F is for forte, MF is for mezzo-forte.

notes from trial 1:

cello154 
	- very narrow (long) windows smoosh out the attack completely, 
		can't use anything much narrower than 100 Hz
	- some wierd frequency artifacts in the analyses using 146 Hz resolution
	- only the 46 Hz resolution reconstruction matches the abruptness of the
		original attack. Distilling this one still sounds mostly okay, but 
		sifting it at 154 Hz ruins it completely. Other sifts (46 and 77) are
		okay.

cello69.F
	- all sound fairly lousy
	- windows narrower than 60 Hz are bad
	- all sound very showery, except widest windows, which have other problems
	- 65 Hz resolution is too wide
	- 55.138 analysis sounds okay distilled, and maybe a little less showery 
		sifted
	- same could be said for 34.86 analysis, but none really sounds very good
	
cello69.MF
	- all sound bad, showery
	- narrow windows are, as usual, unusable
	- 65 Hz resolution analyses are mostly yucky
	- region width doesn't make any difference
	- sifting and distilling don't seem to make these any worse

Last updated: 29 May 2001 by Kelly Fitz
"""
print __doc__

import loris, time
from trials import *

# use this trial counter to skip over
# eariler trials
trial = 2

print "running trial number", trial, time.ctime(time.time())

sourcedict = { 'cello154.F.aiff':154, 'cello69.F.aiff':69, 'cello69.MF.aiff':69 }

if trial == 1:
	for source in sourcedict.keys():
		fund = sourcedict[source]
		resolutions = ( .3*fund, .5*fund, .8*fund, .95*fund )
		wmults = ( .9, 1., 1.2, 1.5, 1.8, 2.5 )
		bws = (1000, 2000, 4000)
		for r in resolutions:
			for mult in wmults:
				w = r * mult
				for bw in bws:
					p = analyze( source, r, w, bw )
					ofile = '%s.%i.%i.%ik.aiff'%(source[:-5], r, w, bw/1000)
					synthesize( ofile, p )
					for f in (fund, .5*fund,.3*fund):
						p2 = p.copy()
						harmonicDistill( p2, f )
						ofile = '%s.%i.%i.%ik.d%i.aiff'%(source[:-5], r, w, bw/1000, f)
						synthesize( ofile, p2 )
						p2 = p.copy()
						harmonicSift( p2, f )
						ofile = '%s.%i.%i.%ik.s%i.aiff'%(source[:-5], r, w, bw/1000, f)
						synthesize( ofile, p2 )
						
if trial == 2:
	source = 'cello154.F.aiff'
	resolutions = (46, 77)
	winds = (115, 154, 225, 300)
	bw = 2000
	for r in resolutions:
		for w in winds:
			p = analyze( source, r, w, bw )
			ofile = '%s.%i.%i.aiff'%(source[:-5], r, w)
			synthesize( ofile, p )
			for f in (77, 154):
				p2 = p.copy()
				harmonicDistill( p2, f )
				ofile = '%s.%i.%i.d%i.aiff'%(source[:-5], r, w, f)
				synthesize( ofile, p2 )

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

Last updated: 23 May 2001 by Kelly Fitz
"""
print __doc__

import loris, time
from trials import *

# use this trial counter to skip over
# eariler trials
trial = 1

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
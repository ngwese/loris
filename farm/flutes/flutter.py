#!/usr/bin/python

"""
flutter.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to a flutter-tongued flute tone used in 
kelly's Ph. D. thesis work on the reassigned Bandwidth-Enhanced
Additive Sound Model.

The parameters we once liked for this are resolution 300 and
window width 400 Hz. The fundamental used for distillation was
330 Hz.

notes from trial 1:

- most of these sound pretty good raw, but distillation corrupts
them. Of all of them, res 250 width 300 seems to have survived 
distillation in best condition.

- all lack the low frequency thumping present in the original, not
surprisingly since the Analyzer is configured with freq floor equal
to the resolution.

notes from trial 2:
	
- lowering the frequency floor to 30 Hz restored the thumping
to the 250 and 160 resolution analyses with 300 and 450 windows

- 250, 300 still seems to survive distillation the best

notes from trial 3 (using 1.0beta8):
	- distillations have a noise artifact at 2.45 seconds, this might have
	been introduced by dropping the frequency floor (I have an old synthesis
	using these parameters that didn't have this artifact).
	- otherwise, this would sound good

notes from trial 4 (using 1.0beta9):
	- raising the frequency floor to r/2 did not get rid
	of the artifact
	
notes from trial 5: 
	- the artifact has nothing to do with the frequency floor, must
	just be a distiller thing. Look at spc files in Kyma, also
	try sifting, after the sieve is restored.
	- WIERD the artifact doesn't make it into the Spc files...

Last updated: 13 May 2002 by Kelly Fitz
"""
print __doc__

import loris, time
from trials import *

# use this trial counter to skip over
# eariler trials
trial = 5

print "running trial number", trial, time.ctime(time.time())

source = 'flutter.aiff'

if trial == 1:
	resolutions = (160, 250, 300)
	widths = ( 300, 400, 600 )
	funds = ( 330, 165 )
	for r in resolutions:
		for w in widths:
			p = analyze( source, r, w )
			ofile = 'flutter.%i.%i.aiff'%(r, w)
			synthesize( ofile, p )
			for f in funds:
				p2 = p.copy()
				harmonicDistill( p2, f )
				ofile = 'flutter.%i.%i.d%i.aiff'%(r, w, f)
				synthesize( ofile, p2 )

if trial == 2:
	resolutions = (160, 250, 300)
	widths = ( 300, 400, 600 )
	funds = ( 330, 165 )
	for r in resolutions:
		for w in widths:
			a = loris.Analyzer( w )
			a.setFreqResolution( r )
			a.setFreqFloor( 30 )
			# get sample data:
			f = loris.AiffFile( source )
			samples = f.samples()
			rate = f.sampleRate()
			# analyze and return partials
			p = a.analyze( samples, rate )
			ofile = 'flutter.lo.%i.%i.aiff'%(r, w)
			synthesize( ofile, p )
			for f in funds:
				p2 = p.copy()
				harmonicDistill( p2, f )
				ofile = 'flutter.lo.%i.%i.d%i.aiff'%(r, w, f)
				synthesize( ofile, p2 )

if trial == 3:
	resolutions = (250, )
	widths = ( 300, )
	funds = ( 330, 165 )
	for r in resolutions:
		for w in widths:
			a = loris.Analyzer( r, w )
			a.setFreqFloor( 30 )
			# get sample data:
			f = loris.AiffFile( source )
			samples = f.samples()
			rate = f.sampleRate()
			# analyze and return partials
			p = a.analyze( samples, rate )
			ofile = 'flutter.lo.%i.%i.aiff'%(r, w)
			synthesize( ofile, p )
			for f in funds:
				p2 = p.copy()
				harmonicDistill( p2, f )
				ofile = 'flutter.lo.%i.%i.d%i.aiff'%(r, w, f)
				synthesize( ofile, p2 )

if trial == 4:
	resolutions = (250, )
	widths = ( 300, )
	funds = ( 330, 165 )
	for r in resolutions:
		for w in widths:
			a = loris.Analyzer( r, w )
			a.setFreqFloor( r/2 )
			# get sample data:
			f = loris.AiffFile( source )
			samples = f.samples()
			rate = f.sampleRate()
			# analyze and return partials
			p = a.analyze( samples, rate )
			ofile = 'flutter.lo2.%i.%i.aiff'%(r, w)
			synthesize( ofile, p )
			for f in funds:
				p2 = p.copy()
				harmonicDistill( p2, f )
				ofile = 'flutter.lo2.%i.%i.d%i.aiff'%(r, w, f)
				synthesize( ofile, p2 )

if trial == 5:
	r = 250
	w = 300
	a = loris.Analyzer( r, w )
	f = loris.AiffFile( source )
	samples = f.samples()
	rate = f.sampleRate()
	
	funds = ( 330, 165 )
	floors = ( 250, 125, 50 )
	
	for fl in floors:
		a.setFreqFloor( fl )
		p = a.analyze( samples, rate )
		ofile = 'flutter.%i.%i.%i.aiff'%(r, w, fl)
		synthesize( ofile, p )
		for f in funds:
			pd = p.copy()
			harmonicDistill( pd, f )
			ofilebase = 'flutter.%i.%i.%i.d%i'%(r, w, fl, f)
			synthesize( ofilebase + '.aiff', pd )
			loris.exportSpc(ofilebase + 's.spc', pd, 64, 0)			
			loris.exportSpc(ofilebase + 'e.spc', pd, 64, 1)
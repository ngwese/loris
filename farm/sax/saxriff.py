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

notes from trial 2:
	- smaller resolutions leave the reverb intact, 90 makes a
	wierd tremolo effect on the reverb in the latter half of the 
	sound (only really detectable with wide windows)
	- 200 and 250 Hz windows sounds pretty good with 65 and 75 Hz 
	resolutions -- actually 170 is pretty good too
	
notes from trial 3 (using 1.0beta8):
	- these all sound about the same, maybe a little worse with
	the 95 (90?) Hz resolution, none are that pretty, but there aren't
	many really objectionable artifacts.

Try using a fundamental extracted in Kyma to channelize and distill.

note from trial 4:
	- these are all unusable, the harmonic tracking seems to be 
	terrible, even for the fundamental, so even with the good reference
	envelope, distillations (including sifting) are a total loss, 
	with big chunks of even the fundamental partial missing.
	- hypothesis: the notes ring so much (from the reverb) that
	the next note, if shorter, is distilled or sifted away, so 
	that instead of followng the reference channel, partials follow
	the reverb tails?
	

Last updated: 31 May 2002 by Kelly Fitz
"""
print __doc__

import loris, time
from trials import *

# use this trial counter to skip over
# eariler trials
trial = 4

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

	
if trial == 3:
	resolutions = (65, 75, 90)
	widths = ( 170, 200, 250 )
	for r in resolutions:
		for w in widths:
			p = analyze( source, r, w )
			p = timescale( p, 2.0 )
			ofile = 'sax.%i.%i.T2.aiff'%(r, w)
			synthesize( ofile, p )

	
if trial == 4:
	pref = loris.importSpc('saxriff.fund.qharm')
	ref = loris.createFreqReference( pref, 50, 500 )
	resolutions = (65, 75, 90)
	widths = ( 170, 200, 250 )
	for r in resolutions:
		for w in widths:
			p = analyze( source, r, w )
			ofilebase = 'sax.%i.%i'%(r,w)
			loris.exportSdif( ofilebase + '.sdif', p )
			synthesize( ofilebase + '.aiff', p )
			for h in (1,2):
				loris.channelize(p, ref, h)
				# distilled version
				pd = p.copy()
				loris.distill( pd )
				ofilebased = ofilebase + '.d%i'%h
				loris.exportSdif( ofilebased + '.sdif', pd )
				synthesize( ofilebased + '.aiff', pd )
				pruneByLabel( pd, range(1,512) )
				loris.exportSpc( ofilebased + '.s.spc', pd, 70, 0 ) 
				loris.exportSpc( ofilebased + '.e.spc', pd, 70, 1 ) 

				# sifted version
				ps = p.copy()
				loris.sift( ps )
				zeros = loris.extractLabeled( ps, 0 )
				loris.distill( ps )
				ofilebases = ofilebase + '.s%i'%h
				loris.exportSdif( ofilebases + '.sdif', ps )
				synthesize( ofilebases + '.aiff', ps )
				pruneByLabel( ps, range(1,512) )
				loris.exportSpc( ofilebases + '.s.spc', ps, 70, 0 ) 
				loris.exportSpc( ofilebases + '.e.spc', ps, 70, 1 ) 




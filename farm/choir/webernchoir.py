#!/usr/bin/python

"""
webernchoir.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to Webern choir recording that we used 
intermittently for a long time, and for which we finally got 
mediocre results. The polyphony and close harmony and the low
male voices make this one hard, but there aren't any transients.

The parameters we settled on were resolution 15 Hz, floor 50 Hz, 
window 100 Hz, region width 5 kHz.

notes from trial 1:
	- widest resolution, 50 Hz, is too wide
	- narrow wndows, like 50 Hz, sound funny, showery or hissy, 
		except for 15 Hz resolution, which is more forgiving of
		window width than any larger resolution
	- wide windows, like 150 Hz, don't sound right
	- narrow windows sound more reverberant, but I think its just
		smearing out temporal features like vibrato
	- 40 Hz resolution is probably too wide, but 15, 20, and 30 
		sound very similar
	- 5 kHz regions don't sound different from 2 kHz
		
notes from trial 2:
	- 90 Hz window seems about perfect, 70 and 110 don't work well
	- 20 Hz resolution seems best, 15 Hz is no improvement, 30 Hz
	sounds a little noisy

notes from trial 3 (using 1.0beta8):
	- all of these sound crunchy to me today, but otherwise pretty decent
	
	- today, 27 May 2003, using 1.0.3 (not yet relased) they all sound 
	like crap, try turning off BW assoc.

notes from trial 4:
	- turning off BW association gets rid of most of the crunch, but they still
	don't sound very good. 
	- nothing is really any better than 24.90, all are prety similar
	- maybe try changing the noise floor?
	
notes from trial 5: 
	- raising noise floor is bad, makes then all sound much worse, badly
	lowpass filtered

Last updated: 27 May 2003 by Kelly Fitz
"""

print __doc__

import loris, time
# from trials import *

# use this trial counter to skip over
# eariler trials
trial = 6

print "running trial number", trial, time.ctime(time.time())

source = 'webernchoir.aiff'
file = loris.AiffFile( source )
samples = file.samples()
rate = file.sampleRate()

# if trial == 1:
# 	resolutions = ( 15, 20, 30, 40, 50 )
# 	widths = ( 50, 70, 90, 110, 150 )
# 	bws = ( 2000, 5000 )
# 	for r in resolutions:
# 		for w in widths:
# 			for bw in bws:
# 				p = analyze( source, r, w, bw )
# 				ofile = 'webern.%i.%i.%ik.aiff'%(r, w, bw/1000)
# 				synthesize( ofile, p )
# 
# if trial == 2:
# 	resolutions = ( 15, 20, 30 )
# 	widths = ( 70, 90, 110 )
# 	for r in resolutions:
# 		for w in widths:
# 			p = analyze( source, r, w )
# 			ofile = 'webern.%i.%i.aiff'%(r, w)
# 			synthesize( ofile, p )
# 
# if trial == 3:
# 	resolutions = ( 18, 20, 24 )
# 	widths = ( 80, 90, 100 )
# 	for r in resolutions:
# 		for w in widths:
# 			p = analyze( source, r, w )
# 			ofile = 'webern.%i.%i.aiff'%(r, w)
# 			synthesize( ofile, p )

if trial == 4:
	resolutions = ( 20, 24 )
	widths = ( 80, 90, 100 )
	for r in resolutions:
		for w in widths:
			a = loris.Analyzer( r, w )
			# turn off BW association for now
			a.setBwRegionWidth( 0 )
			p = a.analyze( samples, rate )
			ofile = 'choir.%i.%i.raw'%(r, w)
			# collate
			loris.distill( p )
			# export
			loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
			loris.exportSpc( ofile + '.s.spc', p, 60, 0 ) 
			loris.exportSpc( ofile + '.e.spc', p, 60, 1 ) 

if trial == 5:
	r = 24
	widths = ( 80, 90 )
	floors = ( 90, 80, 70 )
	for f in floors:
		for w in widths:
			a = loris.Analyzer( r, w )
			a.setAmpFloor( -f )
			# turn off BW association for now
			a.setBwRegionWidth( 0 )
			p = a.analyze( samples, rate )
			ofile = 'choir.%i.%i.f%i.raw'%(r, w, f)
			# collate
			loris.distill( p )
			# export
			loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
			loris.exportSpc( ofile + '.s.spc', p, 60, 0 ) 
			loris.exportSpc( ofile + '.e.spc', p, 60, 1 ) 

if trial == 6:
	r = 24
	widths = ( 80, 90 )
	for w in widths:
		a = loris.Analyzer( r, w )
		# turn off BW association for now
		a.setBwRegionWidth( 0 )
		p = a.analyze( samples, rate )
		ofile = 'choir.%i.%i.raw'%(r, w)
		# collate
		loris.distill( p )
		# export
		loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
		loris.exportSpc( ofile + '.s.spc', p, 60, 0 ) 
		loris.exportSpc( ofile + '.e.spc', p, 60, 1 ) 

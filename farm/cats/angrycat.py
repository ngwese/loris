#!/usr/bin/python

"""
angrycat.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to tortured, angry cat sound that we used in
the  ICMC 2000 bake-off in Berlin, and was also a candidate sound for 
the Toy Angst animation. We morphed this with a trombone to everyone's 
amusement, but never got really great reconstruction.

The parameters we once liked for this were resolution 30 or 50 Hz, 
window width 200 or 250 Hz, and the reference envelope was
	[ (0,328), (.13, 276), (.37, 273), (.48, 248), (.51, 273), 
	(.97, 238), (.99, 271), (1.56, 231), (2.14, 265), (2.35, 340), 
	(2.75, 388), (3.5, 302), (4, 296), (4.1, 311), (4.7, 253), 
	(4.9, 200), (6, 220) ]
	
Best (usuable) results were obtained by using this envelope for the
second (at least) quasi-harmonic channel.

trial 1:
	70 Hz resolution is too showery
	300 Hz windows don't work very well
	130 Hz windows don't work very well
	none of these sounds great, probably the best are still
	the 30 and 50 Hz resolution analyses with windows of 200 
	or 250 Hz.
	bw region width doesn't make any discernable difference
	
trial 2: 
	30 and 50 Hz resolution sound pretty similar and reasonable (?)
	with 180 and 200 Hz windows
	distillation tends to ruin it, which is not surprising, since I
	used harmonic distillation, dumb
	
trial 3: 
	- seems that even using the right envelope doesn't give great results,
	try sifting
	
trial 4:
	- the raw syntheses are hard to tell apart, 30 Hz seems no better 
	than 50 Hz resolution, and the windows all seem about the same, maybe
	wider is a little better, not much.
	- distillations are utterly useless, but sifted distillations are 
	very much better, though not quite perfect. 
	- with sifting, there doesn't seem to be much difference between 
	channelizing at 3, 5, or 10 reference partial number. Try 1 and 2.
	- Oi! forgot to remove the sifted-out partials (labeled zero). Try again!
	
trial 5:
	- these are all completely unusable. Sifting seems to be important, but
	we cannot throw out the sifted-out partials, have to keep them.
	- I wonder if they are all noise? Or could be? Try that.
	
trial 6: 
	- nope, these are totally unusable, horribly crunchy. Need to save
	those partials, but cannot make them noise either. Sigh. 
	
intermediate conclusion: so far, the best results I can come up with 
(distilled results) use sifting and distillation, but rely heaviliy on
the sifted-out partials.

trial 7:
	the 200 Hz windows sound marginally better, and as long as all the
	sifted partials are saved, it doesn't seem to matter much how we
	channelize what's left, except maybe one partial per harmonic sounds
	a tiny bit worse than more than one. At this point, I am forced to 
	conclude that this sound will always sound ugly, and its only hope
	is clever morphing technique to mask its uglines..
	
Last updated: 21 May 2002 by Kelly Fitz
"""

print __doc__

import loris, time
from trials import *

# use this trial counter to skip over
# eariler trials
trial = 8

print "running trial number", trial, time.ctime(time.time())

source = 'angrycat.aiff'

if trial == 1:
	resolutions = ( 20, 30, 50, 70 )
	widths = ( 130, 180, 200, 250, 300 )
	bws = ( 1000, 2000, 4000 )
	for r in resolutions:
		for w in widths:
			for bw in bws:
				p = analyze( source, r, w, bw )
				ofile = 'angry.%i.%i.%ik.aiff'%(r, w, bw/1000)
				synthesize( ofile, p )

if trial == 2:
	resolutions = ( 30, 50 )
	widths = ( 180, 200, 250 )
	for r in resolutions:
		for w in widths:
			p = analyze( source, r, w )
			ofile = '%s.%i.%i.aiff'%(source[:-5], r, w)
			synthesize( ofile, p )
			for f in (r, 2*r, 3*r):
				p2 = p.copy()
				harmonicDistill( p2, f )
				ofile = '%s.%i.%i.d%i.aiff'%(source[:-5], r, w, f)
				synthesize( ofile, p2 )
				
env = buildEnvelope( [ (0,328), (.13, 276), (.37, 273), (.48, 248), (.51, 273), 
	(.97, 238), (.99, 271), (1.56, 231), (2.14, 265), (2.35, 340), 
	(2.75, 388), (3.5, 302), (4, 296), (4.1, 311), (4.7, 253), 
	(4.9, 200), (6, 220) ] )

if trial == 3:
	resolutions = ( 30, 50 )
	widths = ( 150, 180, 200 )
	for r in resolutions:
		for w in widths:
			p = analyze( source, r, w )
			ofile = 'angry.%i.%i.aiff'%(r, w)
			synthesize( ofile, p )
			for h in (3,5,10):
				p2 = p.copy()
				loris.channelize( p2, env, h )
				loris.distill( p2 )
				ofile = '%s.%i.%i.d%i.aiff'%(source[:-5], r, w, h)
				synthesize( ofile, p2 )

if trial == 4:
	resolutions = ( 30, 50 )
	widths = ( 150, 180, 200 )
	for r in resolutions:
		for w in widths:
			p = analyze( source, r, w )
			ofilebase = 'angry.%i.%i.raw'%(r, w)
			synthesize( ofilebase + '.aiff', p )
			loris.exportSdif( ofilebase + '.sdif', p )
			for h in (3,5,10):
				# channelize:
				p2 = p.copy()
				loris.channelize( p2, env, h )
				siftme = p2
				distillme = p2.copy()
				# just distill one version
				loris.distill( distillme )
				ofilebase = '%s.%i.%i.dist%i'%(source[:-5], r, w, h)
				synthesize( ofilebase + '.aiff', distillme )
				loris.exportSpc( ofilebase + '.s.spc', distillme, 36, 0 )
				loris.exportSpc( ofilebase + '.e.spc', distillme, 36, 1 )
				# sift and distill the other version
				loris.sift( siftme )
				loris.distill( siftme )
				ofilebase = '%s.%i.%i.sift%i'%(source[:-5], r, w, h)
				synthesize( ofilebase + '.aiff', siftme )
				loris.exportSpc( ofilebase + '.s.spc', siftme, 36, 0 )
				loris.exportSpc( ofilebase + '.e.spc', siftme, 36, 1 )

if trial == 5:
	r = 50
	widths = ( 150, 200 )
	for w in widths:
		ofilebase = 'angry.%i.%i.raw'%(r, w)
		p = loris.importSdif( ofilebase + '.sdif' )
		for h in (1,2,3):
			# channelize:
			siftme = p.copy()
			loris.channelize( siftme, env, h )
			loris.sift( siftme )
			zeros = loris.extractLabeled( siftme, 0 )
			loris.distill( siftme )
			ofilebase = 'angry.%i.%i.sift%i'%(r, w, h)
			synthesize( ofilebase + '.aiff', siftme )
			loris.exportSpc( ofilebase + '.s.spc', siftme, 36, 0 )
			loris.exportSpc( ofilebase + '.e.spc', siftme, 36, 1 )

if trial == 6:
	r = 50
	widths = ( 150, 200 )
	for w in widths:
		ofilebase = 'angry.%i.%i.raw'%(r, w)
		p = loris.importSdif( ofilebase + '.sdif' )
		for h in (1,2,3,5):
			# channelize:
			siftme = p.copy()
			loris.channelize( siftme, env, h )
			loris.sift( siftme )
			zeros = loris.extractLabeled( siftme, 0 )
			loris.distill( siftme )
			setAllBandwidth( zeros, 1 )
			siftme.splice( siftme.end(), zeros )
			ofilebase = 'angry.%i.%i.snz%i'%(r, w, h)
			synthesize( ofilebase + '.aiff', siftme )
			loris.exportSpc( ofilebase + '.s.spc', siftme, 36, 0 )
			loris.exportSpc( ofilebase + '.e.spc', siftme, 36, 1 )

if trial == 7:
	r = 50
	widths = ( 150, 200 )
	for w in widths:
		ofilebase = 'angry.%i.%i.raw'%(r, w)
		p = loris.importSdif( ofilebase + '.sdif' )
		for h in (1,2,3,5):
			# channelize:
			siftme = p.copy()
			loris.channelize( siftme, env, h )
			loris.sift( siftme )
			zeros = loris.extractLabeled( siftme, 0 )
			loris.distill( siftme )
			siftme.splice( siftme.end(), zeros )
			ofilebase = 'angry.%i.%i.ssav%i'%(r, w, h)
			synthesize( ofilebase + '.aiff', siftme )
			loris.exportSpc( ofilebase + '.s.spc', siftme, 36, 0 )
			loris.exportSpc( ofilebase + '.e.spc', siftme, 36, 1 )

if trial == 8:	# last trial, pending a great idea
	r = 50
	w = 200
	p = analyze( source, r, w )
	for h in (2,3,5,8):
		# channelize:
		siftme = p.copy()
		loris.channelize( siftme, env, h )
		loris.sift( siftme )
		zeros = loris.extractLabeled( siftme, 0 )
		loris.distill( siftme )
		siftme.splice( siftme.end(), zeros )
		ofilebase = 'angrycat.%i.%i.ssav%i'%(r, w, h)
		synthesize( ofilebase + '.aiff', siftme )
		loris.exportSpc( ofilebase + '.s.spc', siftme, 36, 0 )
		loris.exportSpc( ofilebase + '.e.spc', siftme, 36, 1 )


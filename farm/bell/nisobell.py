#!/usr/bin/python

"""
nisobell.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

	
Niso bell: 
	95 Hz resolution with a 200 Hz window works well, can be
	harmonically distilled at fundamentals up to 150, can also have lots
	of little noisy partials pruned out. Prominent partials in this tone
	are approximately 105 Hz (1), 271 Hz (2), 398 Hz (4), 541 Hz (5),
	689 Hz (6).

Last updated: 11 March 2003 by Kelly Fitz
"""
print __doc__

import loris, time

print """
Using Loris version %s
"""%loris.version()

anal = loris.Analyzer( 95, 200 )
orate = 44100

# analyze nisobell
name = 'nisobell'
f = loris.AiffFile( name + '.aiff' )
print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
p = anal.analyze( f.samples(), f.sampleRate() )

# nisobell collated
pcollate = p.copy()
loris.distill( pcollate )
print 'synthesizing raw (collated) %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( pcollate, orate )
loris.exportAiff( name + '.raw.aiff', samples, orate )
loris.exportSdif( name + '.raw.sdif', pcollate )

# remove any Partials labeled greater than 512
iter = pcollate.begin()
end = pcollate.end()
while not iter.equals(end):
	next = iter.next()
	if iter.partial().label() > 512:
		pcollate.erase(iter)
	iter = next

loris.exportSpc( name + '.raw.s.spc', pcollate, 36, 0 )
loris.exportSpc( name + '.raw.e.spc', pcollate, 36, 1 )

print 'pruning very short partials before .2 and after .5 seconds'
it = p.begin()
end = p.end()
while not it.equals(end):
	nxt = it.next()
	part = it.partial()
	if (part.duration() < .2) and ((part.startTime() > .5) or (part.endTime() < .2)):
		p.erase(it)
	it = nxt

# nisobell harmonically distilled at 110 Hz
env = loris.BreakpointEnvelopeWithValue( 110 )
loris.channelize( p, env, 1 )
loris.distill( p )
print 'synthesizing harmonically distilled (110 Hz) %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( p, orate )
loris.exportAiff( name + '.recon.aiff', samples, orate )
loris.exportSpc( name + '.s.spc', p, 45, 0 )
loris.exportSpc( name + '.e.spc', p, 45, 1 )
loris.exportSdif( name + '.sdif', p )


# This script pertains to the temple bell from Niso (Japan?). It
# is a noisy field recording, truncatd at the end, with some audible
# squeaking of the bell suspension, and lots of hiss. It has an 
# interesting attack though, and a nice bell timbre, which is simple
# but not harmonic.
# 
# The parameters we once liked for this are resolution 60 and
# window width 105 Hz, and bandwidth region width 2400 Hz.
# 
# first pass notes 4 May 2001:
# - the lowest tone in the bell is below 110 Hz, so resolutions
# 	greater than that loose a big part of the sound, unless the
# 	freq floor is lowered.
# - hiss in the source makes artifacts in reconstruction, less 
# 	objectionable, maybe, in wider windows (200 Hz)
# - windows 140 and 200 Hz are best, 100 and less are unacceptible
# - bandwidth region width doesn't have much effect
# - with a 200 Hz wide window, resolutions 30, 40, 50, 60, 70, 80, 95
# 	are hard to distinguish
# 
# Second trial attempted distillations.
# Third trial added some even wider windows (since the widest,
# 200 Hz, always sounded best). 
# 
# note: distillation doesn't seem to have a deliterious effect
# 
# Fourth trial is similar to trial three, but with the new sifting
# Distiller, fewer resolutions (since they are almost indistinguishible),
# and te standard Bw regions width (2k).
# 
# notes from trial 4:
# 	- the choice of resolution doesn't seem to affect the quality of 
# 	the bell sound, only the background noise sounds different with
# 	different resolutions
# 	- narrower windows increase the noisiness (sounds like a noise burst)
# 	in the attack, most evident with larger resolutions. 
# 	- can use 70 or 95 Hz resolution with wider windows and get good results
# 	- distillation works well for all values tried (up to 120, try larger)
# 
# notes from trial 5 (using 1.0beta8):
# 	- 300 Hz windows seem to get a little crunchy after about 3 s, don't use
# 	- 200 Hz windows undistilled reconstructions are pretty good
# 	- distillation at 180 is unusable, total crunch
# 	- 70.200.d120 and 70.200.d150 have noisiness problems
# 	- 70.200.d90 is fine, as are distillations of 95.200
# 	- very high level of background hiss in original makes all of them 
# 	kind of wormy
# 	
# notes from trial 6 (using 1.0beta9):
# 	- confirmed above: disllations of 95.200 are all pretty good, 
# 	as is 70.200.d90, all are wormy.
# 	- need to make some SDIF and Spc files to look at and figure out
# 	what kind of partials are being retained. Maybe there are lots of
# 	short ones that can be converted to noise, or removed. 
# 	
# notes from trial 7 (Spc files in Kyma):
# 	- lots of tiny little partials all over the spectrum, could eliminate
# 	- looks like it might be worth trying to restrict the frequency drift
# 	in these analyses, some of these partials wander quite a lot.
# 	- also short stuff before .2 seconds looks like it could go
# 	- should also try converting all those short things to noise
# 	
# notes from trial 8:
# 	- trimming and restricting frequency drift does not seem
# 	to have adversely affected the quality of the reconstructions,
# 	just removed the background wormy hiss
# 	- this trivial pruning mechanism leaves some pretty big holes, 
# 	but they don't seem to be audible
# 	- spc filenames too long! (fixed, I think)
# 	- partials seem to wander less, though I think that the distillation
# 	process introduces some of that
# 	- these sets of parameters all yield about the same results, these
# 	all sound pretty much identical
# 	- the partials are sufficiently stable, that any of these harmonic
# 	distillations seems to work fine.
# 	
# conclusion: 95 Hz resolution with a 200 Hz window works well, can be 
# harmonically distilled at fundamentals up to 150, can also have lots of
# little noisy partials pruned out. Prominent partials in this tone are 
# approximately 105 Hz (1), 271 Hz (2), 398 Hz (4), 541 Hz (5), 689 Hz (6).
# 
# from trials import *
# 
# # use this trial counter to skip over
# # eariler trials
# trial = 9
# 
# print "running trial number", trial, time.ctime(time.time())
# 
# source = 'nisobell.aiff'
# 
# if trial == 1:
# 	resolutions = (30, 40, 50, 60, 70, 80, 95, 110)
# 	widths = ( 0, 85, 105, 140, 200 )
# 	bws = (1400, 2400, 4000)
# 	for r in resolutions:
# 		for w in widths:
# 			if w == 0:
# 				w = r
# 			for bw in bws:
# 				p = analyze( source, r, w, bw )
# 				ofile = 'bell.%i.%i.%ik.aiff'%(r, w, bw/1000)
# 				synthesize( ofile, p )
# 
# 
# if trial == 2:
# 	resolutions = (30, 40, 50, 60, 70, 80, 95)
# 	widths = ( 140, 200 )
# 	bws = (2400, )
# 	funds = (30, 45, 60, 75)
# 	for r in resolutions:
# 		for w in widths:
# 			if w == 0:
# 				w = r
# 			for bw in bws:
# 				for f in funds:
# 					p = analyze( source, r, w, bw )
# 					harmonicDistill( p, f )
# 					ofile = 'bell.%i.%i.%ik.d%i.aiff'%(r, w, bw/1000, f)
# 					synthesize( ofile, p )
# 
# if trial == 3:
# 	resolutions = (30, 40, 50, 60, 70, 80, 95)
# 	widths = ( 220, 250, 300 )
# 	bws = (2400, )
# 	funds = (30, 45, 60, 75)
# 	for r in resolutions:
# 		for w in widths:
# 			if w == 0:
# 				w = r
# 			for bw in bws:
# 				for f in funds:
# 					p = analyze( source, r, w, bw )
# 					ofile = 'bell.%i.%i.%ik.aiff'%(r, w, bw/1000)
# 					synthesize( ofile, p )
# 					harmonicDistill( p, f )
# 					ofile = 'bell.%i.%i.%ik.d%i.aiff'%(r, w, bw/1000, f)
# 					synthesize( ofile, p )
# 
# 
# if trial == 4:
# 	resolutions = (40, 70, 95)
# 	widths = ( 100, 200, 300 )
# 	bw = 2000
# 	funds = (30, 60, 90, 120)
# 	for r in resolutions:
# 		for w in widths:
# 			p = analyze( source, r, w, bw )
# 			ofile = 'bell.%i.%i.aiff'%(r, w)
# 			synthesize( ofile, p )
# 			for f in funds:
# 				p2 = p.copy()
# 				harmonicDistill( p2, f )
# 				ofile = 'bell.%i.%i.d%i.aiff'%(r, w, f)
# 				synthesize( ofile, p2 )
# 
# if trial == 5:
# 	resolutions = (70, 95)
# 	widths = ( 200, 300 )
# 	funds = ( 90, 120, 150, 180 )
# 	for r in resolutions:
# 		for w in widths:
# 			p = analyze( source, r, w )
# 			ofile = 'bell.%i.%i.aiff'%(r, w)
# 			synthesize( ofile, p )
# 			for f in funds:
# 				p2 = p.copy()
# 				harmonicDistill( p2, f )
# 				ofile = 'bell.%i.%i.d%i.aiff'%(r, w, f)
# 				synthesize( ofile, p2 )
# 
# if trial == 6:
# 	resolutions = (70, 95)
# 	widths = ( 200, )
# 	funds = ( 90, 120, 150 )
# 	for r in resolutions:
# 		for w in widths:
# 			p = analyze( source, r, w )
# 			ofile = 'bell.%i.%i.aiff'%(r, w)
# 			synthesize( ofile, p )
# 			for f in funds:
# 				p2 = p.copy()
# 				harmonicDistill( p2, f )
# 				ofile = 'bell.%i.%i.d%i.aiff'%(r, w, f)
# 				synthesize( ofile, p2 )
# 				
# if trial == 7:
# 	r = 70
# 	w = 200
# 	p = analyze( source, r, w )
# 	ofilebase = 'bell.%i.%i'%(r, w)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	f = 90
# 	harmonicDistill( p, f )
# 	ofilebase = 'bell.%i.%i.d%i'%(r, w, f)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	loris.exportSpc( ofilebase + '.sine.spc', p, 36, 0 ) 
# 	loris.exportSpc( ofilebase + '.bwe.spc', p, 36, 1 ) 
# 	r = 95
# 	w = 200
# 	p = analyze( source, r, w )
# 	p2 = p.copy()
# 	ofilebase = 'bell.%i.%i'%(r, w)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	f = 90
# 	harmonicDistill( p, f )
# 	ofilebase = 'bell.%i.%i.d%i'%(r, w, f)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	loris.exportSpc( ofilebase + '.sine.spc', p, 36, 0 ) 
# 	loris.exportSpc( ofilebase + '.bwe.spc', p, 36, 1 ) 
# 	p = p2
# 	f = 150
# 	harmonicDistill( p, f )
# 	ofilebase = 'bell.%i.%i.d%i'%(r, w, f)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	loris.exportSpc( ofilebase + '.sine.spc', p, 36, 0 ) 
# 	loris.exportSpc( ofilebase + '.bwe.spc', p, 36, 1 ) 
# 	
# ######## trial 8 ############
# 
# def trial8run( sfile, r, w, f ):
# 	print 'analyzing %s (%s)'%(source, time.ctime(time.time()))
# 	a = loris.Analyzer( r, w )
# 	a.setFreqDrift( .2*r )
# 	p = a.analyze( sfile.samples(), sfile.sampleRate() )
# 	psave = p.copy()
# 	harmonicDistill( p, f )
# 	ofilebase = 'bell.fd2.%i.%i.d%i'%(r, w, f)
# 	loris.exportSpc( ofilebase + '.s.spc', p, 36, 0 ) 
# 	loris.exportSpc( ofilebase + '.e.spc', p, 36, 1 ) 
# 	synthesize( ofilebase + '.aiff', p )
# 	
# 	# trim version
# 	print 'trimming very short partials'
# 	p = psave
# 	it = p.begin()
# 	end = p.end()
# 	while not it.equals(end):
# 		nxt = it.next()
# 		part = it.partial()
# 		if (part.duration() < .2) and (part.startTime() > .5):
# 			p.erase(it)
# 		it = nxt
# 	harmonicDistill( p, f )
# 	ofilebase = 'bell.fd2tr.%i.%i.d%i'%(r, w, f)
# 	loris.exportSpc( ofilebase + '.s.spc', p, 36, 0 ) 
# 	loris.exportSpc( ofilebase + '.e.spc', p, 36, 1 ) 
# 	synthesize( ofilebase + '.aiff', p )
# 
# if trial == 8:
# 	filename = source
# 	sfile = loris.AiffFile( filename )
# 	
# 	r = 70
# 	w = 200
# 	f = 90
# 	trial8run( sfile, r, w, f )
# 
# 	r = 95
# 	w = 200
# 	f = 90
# 	trial8run( sfile, r, w, f )
# 	
# 	r = 95
# 	w = 200
# 	f = 150
# 	trial8run( sfile, r, w, f )
# 	
# 	
# 	
# if trial == 9: 	# final trial
# 	r = 95		# resolution
# 	w = 200		# window width
# 	f = 110		# harmonic distillation fundamental, use the lowest partial in the bell sound
# 	sfile = loris.AiffFile( source )
# 	print 'analyzing %s (%s)'%(source, time.ctime(time.time()))
# 	a = loris.Analyzer( r, w )
# 	a.setFreqDrift( .2*r )
# 	p = a.analyze( sfile.samples(), sfile.sampleRate() )
# 	ptrim = p.copy()
# 	
# 	# raw
# 	harmonicDistill( p, f )
# 	ofilebase = 'bell.fd2.%i.%i.d%i'%(r, w, f)
# 	loris.exportSpc( ofilebase + '.s.spc', p, 45, 0 ) 
# 	loris.exportSpc( ofilebase + '.e.spc', p, 45, 1 ) 
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	synthesize( ofilebase + '.aiff', p )
# 	
# 	# pruned
# 	print 'pruning very short partials before .2 and after .5 seconds'
# 	p = ptrim
# 	it = p.begin()
# 	end = p.end()
# 	while not it.equals(end):
# 		nxt = it.next()
# 		part = it.partial()
# 		if (part.duration() < .2) and ((part.startTime() > .5) or (part.endTime() < .2)):
# 			p.erase(it)
# 		it = nxt
# 	harmonicDistill( p, f )
# 	ofilebase = 'bell.fd2tr.%i.%i.d%i'%(r, w, f)
# 	loris.exportSpc( ofilebase + '.s.spc', p, 45, 0 ) 
# 	loris.exportSpc( ofilebase + '.e.spc', p, 45, 1 ) 
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	synthesize( ofilebase + '.aiff', p )

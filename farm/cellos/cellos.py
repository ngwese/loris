#!python

"""
cellos.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

	
Cello 154 Hz forte: 
	154 Hz tone sounds good with 123 Hz resolution and 154 Hz window
	width. Can distill using a flat 154 Hz envelope (although tracking
	the third partial has got to work at least as well, probably better
	for high partials). Distill using one partial per harmonic.

Cello 69 Hz forte:
	The 69 Hz F tone sounds good at 20 Hz resolution with a 69 Hz window
	width. Distill using the third partial as a reference with two
	partials per harmonic to preserve the grunty attack. Sifting seems
	to make this one a little worse.

Cello 69 Hz mezzo-forte:
	The 69 Hz MF tone sounds best, it seems, when analyzed using a
	high-gain version of the sound. Analyses using 34 (or 20) Hz
	resolution and 138 Hz window width work well. Distillations, using
	the third harmonic partial as a reference, are only usable if they
	are sifted first, otherwise they sound like they are under a pillow.
	Distill with two partials per harmonic to preserve the grunty
	attack.

	These are not excellent, and would never pass same-difference tests,
	particularly the low tones, but they are usable.

Last updated: 11 March 2003 by Kelly Fitz
"""
print __doc__

import loris, time

print """
Using Loris version %s
"""%loris.version()

anal = loris.Analyzer( 123, 154 )
orate = 44100

#
# 154.F tone
#
name = 'cello154.F'
f = loris.AiffFile( name + '.aiff' )
print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
p = anal.analyze( f.samples(), f.sampleRate() )

# cello154.F distilled using 3d harmonic
ref = loris.createFreqReference( p, 154*2.5, 154*3.5, 100 )
loris.channelize( p, ref, 3 )
loris.distill( p )
print 'synthesizing distilled %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( p, orate )
loris.exportAiff( name + '.recon.aiff', samples, orate )
loris.exportSdif( name + '.sdif', p )

# remove any Partials labeled greater than 512
iter = p.begin()
end = p.end()
while not iter.equals(end):
	next = iter.next()
	if iter.partial().label() > 512:
		p.erase(iter)
	iter = next

loris.exportSpc( name + '.s.spc', p, 51, 0 )
loris.exportSpc( name + '.e.spc', p, 51, 1 )


#
# 69.F tone	
#
name = 'cello69.F'
anal = loris.Analyzer( 20, 69 )
f = loris.AiffFile( name + '.aiff' )
print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
p = anal.analyze( f.samples(), f.sampleRate() )

# 69.MF tone collated
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

loris.exportSpc( name + '.raw.s.spc', pcollate, 37, 0 )
loris.exportSpc( name + '.raw.e.spc', pcollate, 37, 1 )

# distill using 3d harmonic with two partials per harmonic
ref = loris.createFreqReference( p, 69*2.75, 69*3.25 )
loris.channelize( p, ref, 6 )
loris.distill( p )
print 'synthesizing distilled %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( p, orate )
loris.exportAiff( name + '.recon.aiff', samples, orate )
loris.exportSdif( name + '.sdif', p )

# remove any Partials labeled greater than 512
iter = p.begin()
end = p.end()
while not iter.equals(end):
	next = iter.next()
	if iter.partial().label() > 512:
		p.erase(iter)
	iter = next

loris.exportSpc( name + '.s.spc', p, 37, 0 )
loris.exportSpc( name + '.e.spc', p, 37, 1 )


#
# 69.MF tone	
#
name = 'cello69.MF'
anal = loris.Analyzer( 34, 138 )
f = loris.AiffFile( name + '.loud.aiff' )
print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
p = anal.analyze( f.samples(), f.sampleRate() )

# 69.MF tone collated
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

loris.exportSpc( name + '.raw.s.spc', pcollate, 37, 0 )
loris.exportSpc( name + '.raw.e.spc', pcollate, 37, 1 )

# distill using 3d harmonic with two partials per harmonic
ref = loris.createFreqReference( p, 69*2.75, 69*3.25 )
loris.channelize( p, ref, 6 )
loris.sift( p )
zeros = loris.extractLabeled( p, 0 )
loris.distill( p )
print 'synthesizing distilled %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( p, orate )
loris.exportAiff( name + '.recon.aiff', samples, orate )
loris.exportSdif( name + '.sdif', p )

# remove any Partials labeled greater than 512
iter = p.begin()
end = p.end()
while not iter.equals(end):
	next = iter.next()
	if iter.partial().label() > 512:
		p.erase(iter)
	iter = next

loris.exportSpc( name + '.s.spc', p, 37, 0 )
loris.exportSpc( name + '.e.spc', p, 37, 1 )

# This script pertains to several cellos that Lippold uses for his 
# timbre space, one of which (cello154) we used at the ICMC 2000  
# presentations and in Kelly's dissertation.
# 
# Typically, we have used resolution and window width slightly less 
# than the fundamental (154 Hz) for cello154, and for cello 69, resolution
# slightly less than the fundamental (69 Hz) and a window 1.8 times
# as wide.
# 
# F is for forte, MF is for mezzo-forte.
# 
# notes from trial 1:
# 
# cello154 
# 	- very narrow (long) windows smoosh out the attack completely, 
# 		can't use anything much narrower than 100 Hz
# 	- some wierd frequency artifacts in the analyses using 146 Hz resolution
# 	- only the 46 Hz resolution reconstruction matches the abruptness of the
# 		original attack. Distilling this one still sounds mostly okay, but 
# 		sifting it at 154 Hz ruins it completely. Other sifts (46 and 77) are
# 		okay.
# 
# cello69.F
# 	- all sound fairly lousy
# 	- windows narrower than 60 Hz are bad
# 	- all sound very showery, except widest windows, which have other problems
# 	- 65 Hz resolution is too wide
# 	- 55.138 analysis sounds okay distilled, and maybe a little less showery 
# 		sifted
# 	- same could be said for 34.86 analysis, but none really sounds very good
# 	
# cello69.MF
# 	- all sound bad, showery
# 	- narrow windows are, as usual, unusable
# 	- 65 Hz resolution analyses are mostly yucky
# 	- region width doesn't make any difference
# 	- sifting and distilling don't seem to make these any worse
# 	
# 	
# notes from trial 2:
# cello154
# 	- both resolutions (46, 77 Hz) sound pretty good with all windows tried
# 	- 46 Hz resolution captures some rumble in the original that is absent
# 	at 77 Hz, sounds more like the original, but not necessarily desirable
# 	- distillation seems fine, even at 154 Hz for both resolutions, but buzz 
# 	is less buzzy when distilled at 154, still sounds okay at 77 Hz distillation
# 	or less
# 
# cello69.F
# 	- 20.62, 34.62, and 34.82 are unlistenable, others aren't too bad
# 	- windows are wider than earlier trials, but the sounds are way better
# 	- distillation at 69 Hz seems to make these a little showery or 
# 	hissy, but distillation at 34 Hz seems okay
# 	- 138 Hz window is probably best
# 	
# cello69.MF
# 	- as with F, both resolutions are okay with a 138 Hz window, but
# 	the attack is weak, an the whoe thing sounds kind of whimpy, maybe we
# 	need to lower the noise floor? What would happen if we increased
# 	the level of the source?
# 	- as above, 69 Hz distillation makes hiss out of the buzz, 34 Hz
# 	doesn't do it as much
# 	- overall, buzz is not represented as well as in F, try using a
# 	normalized source
# 	
# notes from trial 3 (using 1.0beta8):
# cello69.MF
# 	- using a gain-normalized source, all of these sound quite passable.
# 	- the tail of all of them sounds gritty
# 	- distillation at 20 and 34 Hz doesn't affect the syntheses adversely
# 	- the differences between all of these are subtle, and none is really
# 	clearly better than the others.
# 
# 
# notes from trial 4 (using 1.0beta9):
# cello69.F:
# 	- of the undistilled versions, 20 Hz resolution seems to yield the 
# 	least artifacts, though its still not quite as grunty as the original. 
# 	In the others, the noise sounds a bit metallic.
# 	- distilling the 20 Hz resolution analyses at two partials per
# 	harmonic doesn't seem to affect them adversely, one partial per harm
# 	has the same noise metallic artifacts introduced in the other resolution 
# 	analyses.
# 	- summary use 20 Hz resolution, diistill at two partials per harmonic (34),
# 	either window seems too work.
# 
# cello69.MF:
# 	- attack is squashed on all of these, need shorter (wider) windows
# 	than 103 Hz I guess
# 
# cello154.F:
# 	- all three resolutions sound about the same, and all distillations are
# 	quite good. They don't quite pass a same/difference test, but all are
# 	quite usable.
# 	- Doesn't seem to be any advantage to distilling at two partials per harm
# 	instead of one.
# 
# notes from trial 5 (Spc files in Kyma):
# 	- 120 partials isn't enough for the low tones
# 	- for the low tones, partial numbering get off by one for
# 	partials above 26 (channelizing is two partials per harmonic),
# 	maybe need to use a real reference instead of n artificial one
# 	- 154 Hz tones look reasonable
# 	- 69.MF.loud names were too long!
# 
# notes from trial 6 (Spc files in Kyma):
# 	- partials in low tones are labeled correctly now
# 	- not clear that 160 partials is enough either!
# 	- attacks have more grunt with the odd partials included,
# 	but sustain is passable with just evens.
# 	- distillations of 69.MF are still WAY muffled. Why?
# 	- NOTE: sifting solves this problem, no longer muffled, but
# 	it does not seem to be a phase problem, since setting all
# 	Breakpoint phases to zero does not cause muffling.
# 	
# conclusion: 154 Hz tone sounds good with 123 Hz resolution and 154 Hz window
# width. Can distill using a flat 154 Hz envelope (although tracking the third
# partial has got to work at least as well, probably better for high partials).
# Distill using one partial per harmonic.
# 
# The 69 Hz F tone sounds good at 20 Hz resolution with a 69 Hz window width.
# Distill using the third partial as a reference with two partials per harmonic
# to preserve the grunty attack. Sifting seems to make this one a little worse.
# 
# The 69 Hz MF tone sounds best, it seems, when analyzed using a high-gain version
# of the sound. Analyses using 34 (or 20) Hz resolution and 138 Hz window width
# work well. Distillations, using the third harmonic partial as a reference,
# are only usable if they are sifted first, otherwise they sound like they are 
# under a pillow. Distill with two partials per harmonic to preserve the grunty
# attack.
# 
# These are not excellent, and would never pass same-difference tests, particularly
# the low tones, but they are usable.


# from trials import *
# 
# # use this trial counter to skip over
# # eariler trials
# trial = 7
# 
# print "running trial number", trial, time.ctime(time.time())
# 
# # this is a dumb way to do this
# sourcedict = { 'cello154.F.aiff':154, 'cello69.F.aiff':69, 'cello69.MF.aiff':69 }
# 
# # this is better:
# sources = ( ('cello154.F.aiff', 154), ('cello69.F.aiff', 69), ('cello69.MF.aiff',69) )
# sources2 = ( ('cello154.F.aiff', 154), ('cello69.F.aiff', 69), ('cello69.MF.loud.aiff',69) )
# 
# if trial == 1:
# 	for source in sourcedict.keys():
# 		fund = sourcedict[source]
# 		resolutions = ( .3*fund, .5*fund, .8*fund, .95*fund )
# 		wmults = ( .9, 1., 1.2, 1.5, 1.8, 2.5 )
# 		bws = (1000, 2000, 4000)
# 		for r in resolutions:
# 			for mult in wmults:
# 				w = r * mult
# 				for bw in bws:
# 					p = analyze( source, r, w, bw )
# 					ofile = '%s.%i.%i.%ik.aiff'%(source[:-5], r, w, bw/1000)
# 					synthesize( ofile, p )
# 					for f in (fund, .5*fund,.3*fund):
# 						p2 = p.copy()
# 						harmonicDistill( p2, f )
# 						ofile = '%s.%i.%i.%ik.d%i.aiff'%(source[:-5], r, w, bw/1000, f)
# 						synthesize( ofile, p2 )
# 						p2 = p.copy()
# 						harmonicSift( p2, f )
# 						ofile = '%s.%i.%i.%ik.s%i.aiff'%(source[:-5], r, w, bw/1000, f)
# 						synthesize( ofile, p2 )
# 						
# if trial == 2:
# 	for (source,fund) in sources:
# 		resolutions = ( .3*fund, .5*fund )
# 		winds = (.9*fund, 1.2*fund, 2.0*fund)
# 		for r in resolutions:
# 			for w in winds:
# 				p = analyze( source, r, w )
# 				ofile = '%s.%i.%i.aiff'%(source[:-5], r, w)
# 				synthesize( ofile, p )
# 				for f in (.3*fund, .5*fund, fund):
# 					p2 = p.copy()
# 					harmonicDistill( p2, f )
# 					ofile = '%s.%i.%i.d%i.aiff'%(source[:-5], r, w, f)
# 					synthesize( ofile, p2 )
# 
# if trial == 3:
# 	(source, fund) = ('cello69.MF.loud.aiff',69)
# 	resolutions = ( .3*fund, .5*fund )
# 	winds = (2.0*fund, 2.2*fund)
# 	for r in resolutions:
# 		for w in winds:
# 			p = analyze( source, r, w )
# 			ofile = '%s.%i.%i.aiff'%(source[:-5], r, w)
# 			synthesize( ofile, p )
# 			for f in (.3*fund, .5*fund):
# 				p2 = p.copy()
# 				harmonicDistill( p2, f )
# 				ofile = '%s.%i.%i.d%i.aiff'%(source[:-5], r, w, f)
# 				synthesize( ofile, p2 )
# 
# if trial == 4:
# 	for (source,fund) in sources2:
# 		resolutions = ( .3*fund, .5*fund, .8*fund )
# 		if fund < 100:
# 			winds = (fund, 1.5*fund )
# 		else:
# 			winds = (fund,)
# 		for r in resolutions:
# 			for w in winds:
# 				p = analyze( source, r, w )
# 				ofile = '%s.%i.%i.aiff'%(source[:-5], r, w)
# 				synthesize( ofile, p )
# 				for f in (.3*fund, .5*fund, fund):
# 					p2 = p.copy()
# 					harmonicDistill( p2, f )
# 					ofile = '%s.%i.%i.d%i.aiff'%(source[:-5], r, w, f)
# 					synthesize( ofile, p2 )
# 
# if trial == 5:
# 	source = 'cello154.F.aiff'
# 	r = 77
# 	w = 154
# 	p = analyze( source, r, w )
# 	ofilebase = 'cello154.F.%i.%i'%(r, w)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	f = 154
# 	harmonicDistill( p, f )
# 	pruneByLabel( p, range(1,120) )
# 	ofilebase = 'cello154.F.%i.%i.d%i'%(r, w, f)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	loris.exportSpc( ofilebase + '.sine.spc', p, 51, 0 ) 
# 	loris.exportSpc( ofilebase + '.bwe.spc', p, 51, 1 ) 
# 	r = 123
# 	w = 154
# 	p = analyze( source, r, w )
# 	ofilebase = 'cello154.F.%i.%i'%(r, w)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	f = 154
# 	harmonicDistill( p, f )
# 	pruneByLabel( p, range(1,120) )
# 	ofilebase = 'cello154.F.%i.%i.d%i'%(r, w, f)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	loris.exportSpc( ofilebase + '.sine.spc', p, 51, 0 ) 
# 	loris.exportSpc( ofilebase + '.bwe.spc', p, 51, 1 ) 
# 	
# 	source = 'cello69.F.aiff'
# 	r = 20
# 	w = 69
# 	p = analyze( source, r, w )
# 	ofilebase = 'cello69.F.%i.%i'%(r, w)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	f = 34
# 	harmonicDistill( p, f )
# 	pruneByLabel( p, range(1,120) )
# 	ofilebase = 'cello69.F.%i.%i.d%i'%(r, w, f)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	loris.exportSpc( ofilebase + '.sine.spc', p, 37, 0 ) 
# 	loris.exportSpc( ofilebase + '.bwe.spc', p, 37, 1 ) 
# 	w = 103
# 	p = analyze( source, r, w )
# 	ofilebase = 'cello69.F.%i.%i'%(r, w)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	f = 34
# 	harmonicDistill( p, f )
# 	pruneByLabel( p, range(1,120) )
# 	ofilebase = 'cello69.F.%i.%i.d%i'%(r, w, f)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	loris.exportSpc( ofilebase + '.sine.spc', p, 37, 0 ) 
# 	loris.exportSpc( ofilebase + '.bwe.spc', p, 37, 1 ) 
# 	
# 	source = 'cello69.MF.loud.aiff'
# 	r = 34
# 	w = 69*2
# 	p = analyze( source, r, w )
# 	ofilebase = 'cello69.MF.loud.%i.%i'%(r, w)
# 	synthesize( ofilebase + '.aiff', p )
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	f = 34
# 	harmonicDistill( p, f )
# 	pruneByLabel( p, range(1,120) )
# 	ofilebase = 'cello69.MF.loud.%i.%i.d%i'%(r, w, f)
# 	synthesize( ofilebase + '.aiff', p )
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	loris.exportSpc( ofilebase + '.sine.spc', p, 37, 0 ) 
# 	loris.exportSpc( ofilebase + '.bwe.spc', p, 37, 1 ) 
# 	w = 69*2.2
# 	p = analyze( source, r, w )
# 	ofilebase = 'cello69.MF.loud.%i.%i'%(r, w)
# 	synthesize( ofilebase + '.aiff', p )
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	f = 34
# 	harmonicDistill( p, f )
# 	pruneByLabel( p, range(1,120) )
# 	ofilebase = 'cello69.MF.loud.%i.%i.d%i'%(r, w, f)
# 	synthesize( ofilebase + '.aiff', p )
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	loris.exportSpc( ofilebase + '.sine.spc', p, 37, 0 ) 
# 	loris.exportSpc( ofilebase + '.bwe.spc', p, 37, 1 ) 
# 
# 	r = 20
# 	w = 69*2
# 	p = analyze( source, r, w )
# 	ofilebase = 'cello69.MF.loud.%i.%i'%(r, w)
# 	synthesize( ofilebase + '.aiff', p )
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	f = 34
# 	harmonicDistill( p, f )
# 	pruneByLabel( p, range(1,120) )
# 	ofilebase = 'cello69.MF.loud.%i.%i.d%i'%(r, w, f)
# 	synthesize( ofilebase + '.aiff', p )
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	loris.exportSpc( ofilebase + '.sine.spc', p, 37, 0 ) 
# 	loris.exportSpc( ofilebase + '.bwe.spc', p, 37, 1 ) 
# 	w = 69*2.2
# 	p = analyze( source, r, w )
# 	ofilebase = 'cello69.MF.loud.%i.%i'%(r, w)
# 	synthesize( ofilebase + '.aiff', p )
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	f = 34
# 	harmonicDistill( p, f )
# 	pruneByLabel( p, range(1,120) )
# 	ofilebase = 'cello69.MF.loud.%i.%i.d%i'%(r, w, f)
# 	synthesize( ofilebase + '.aiff', p )
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	loris.exportSpc( ofilebase + '.sine.spc', p, 37, 0 ) 
# 	loris.exportSpc( ofilebase + '.bwe.spc', p, 37, 1 ) 
# 	
# if trial == 6:
# 	source = 'cello69.F.aiff'
# 	r = 20
# 	w = 69
# 	p = analyze( source, r, w )
# 	ofilebase = 'cello69.F.%i.%i'%(r, w)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	ref = loris.createFreqReference( p, 69*2.75, 69*3.25 )
# 	loris.channelize( p, ref, 6 )
# 	loris.distill( p )
# 	pruneByLabel( p, range(1,160) )
# 	ofilebase = 'cello69.F.%i.%i.d3rd'%(r, w)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	loris.exportSpc( ofilebase + '.s.spc', p, 37, 0 ) 
# 	loris.exportSpc( ofilebase + '.e.spc', p, 37, 1 ) 
# 	w = 103
# 	p = analyze( source, r, w )
# 	ofilebase = 'cello69.F.%i.%i'%(r, w)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	ref = loris.createFreqReference( p, 69*2.75, 69*3.25 )
# 	loris.channelize( p, ref, 6 )
# 	loris.distill( p )
# 	pruneByLabel( p, range(1,160) )
# 	ofilebase = 'cello69.F.%i.%i.d3rd'%(r, w)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	loris.exportSpc( ofilebase + '.s.spc', p, 37, 0 ) 
# 	loris.exportSpc( ofilebase + '.e.spc', p, 37, 1 ) 
# 	
# 	source = 'cello69.MF.loud.aiff'
# 	r = 34
# 	w = 69*2
# 	p = analyze( source, r, w )
# 	ofilebase = 'cello69.MF.%i.%i'%(r, w)
# 	synthesize( ofilebase + '.aiff', p )
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	ref = loris.createFreqReference( p, 69*2.75, 69*3.25 )
# 	loris.channelize( p, ref, 6 )
# 	loris.distill( p )
# 	pruneByLabel( p, range(1,160) )
# 	ofilebase = 'cello69.MF.%i.%i.d3rd'%(r, w)
# 	synthesize( ofilebase + '.aiff', p )
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	loris.exportSpc( ofilebase + '.s.spc', p, 37, 0 ) 
# 	loris.exportSpc( ofilebase + '.e.spc', p, 37, 1 ) 
# 	w = 69*2.2
# 	p = analyze( source, r, w )
# 	ofilebase = 'cello69.MF.%i.%i'%(r, w)
# 	synthesize( ofilebase + '.aiff', p )
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	ref = loris.createFreqReference( p, 69*2.75, 69*3.25 )
# 	loris.channelize( p, ref, 6 )
# 	loris.distill( p )
# 	pruneByLabel( p, range(1,160) )
# 	ofilebase = 'cello69.MF.%i.%i.d3rd'%(r, w)
# 	synthesize( ofilebase + '.aiff', p )
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	loris.exportSpc( ofilebase + '.s.spc', p, 37, 0 ) 
# 	loris.exportSpc( ofilebase + '.e.spc', p, 37, 1 ) 
# 
# 	r = 20
# 	w = 69*2
# 	p = analyze( source, r, w )
# 	ofilebase = 'cello69.MF.%i.%i'%(r, w)
# 	synthesize( ofilebase + '.aiff', p )
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	ref = loris.createFreqReference( p, 69*2.75, 69*3.25 )
# 	loris.channelize( p, ref, 6 )
# 	loris.distill( p )
# 	pruneByLabel( p, range(1,160) )
# 	ofilebase = 'cello69.MF.%i.%i.d3rd'%(r, w)
# 	synthesize( ofilebase + '.aiff', p )
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	loris.exportSpc( ofilebase + '.s.spc', p, 37, 0 ) 
# 	loris.exportSpc( ofilebase + '.e.spc', p, 37, 1 ) 
# 	w = 69*2.2
# 	p = analyze( source, r, w )
# 	ofilebase = 'cello69.MF.%i.%i'%(r, w)
# 	synthesize( ofilebase + '.aiff', p )
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	ref = loris.createFreqReference( p, 69*2.75, 69*3.25 )
# 	loris.channelize( p, ref, 6 )
# 	loris.distill( p )
# 	pruneByLabel( p, range(1,160) )
# 	ofilebase = 'cello69.MF.%i.%i.d3rd'%(r, w)
# 	synthesize( ofilebase + '.aiff', p )
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	loris.exportSpc( ofilebase + '.s.spc', p, 37, 0 ) 
# 	loris.exportSpc( ofilebase + '.e.spc', p, 37, 1 ) 
# 
# if trial == 7:	# final trial?
# 	#
# 	# 154.F tone
# 	#
# 	source = 'cello154.F.aiff'
# 	r = 123
# 	w = 154
# 	p = analyze( source, r, w )
# 	f = 154
# 	ref = loris.createFreqReference( p, 154*2.5, 154*3.5, 100 )
# 	loris.channelize( p, ref, 3 )
# 	loris.distill( p )
# 	ofilebase = 'cello154.F.%i.%i.d3rd'%(r, w)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	synthesize( ofilebase + '.aiff', p )
# 	pruneByLabel( p, range(1,512) )
# 	loris.exportSpc( ofilebase + '.s.spc', p, 51, 0 ) 
# 	loris.exportSpc( ofilebase + '.e.spc', p, 51, 1 ) 
# 	
# 	#
# 	# 69.F tone	
# 	#
# 	source = 'cello69.F.aiff'
# 	r = 20
# 	w = 69
# 	p = analyze( source, r, w )
# 	ref = loris.createFreqReference( p, 69*2.75, 69*3.25 )
# 	
# 	# distill with one partial per harmonic and two partials per harmonic
# 	p1 = p
# 	p2 = p.copy()
# 	loris.channelize( p1, ref, 3 )
# 	loris.channelize( p2, ref, 6 )
# 	
# 	# distill at one partial per harmonic
# 	p = p1
# 	siftme = p.copy()
# 	loris.distill( p )
# 	ofilebase = 'cello69.F.%i.%i.d3rd1'%(r, w)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	synthesize( ofilebase + '.aiff', p )
# 	pruneByLabel( p, range(1,512) )
# 	loris.exportSpc( ofilebase + '.s.spc', p, 37, 0 ) 
# 	loris.exportSpc( ofilebase + '.e.spc', p, 37, 1 ) 
# 	
# 	# try sifting (one partial per harmonic):
# 	p = siftme
# 	loris.sift( p )
# 	zeros = loris.extractLabeled( p, 0 )
# 	loris.distill( p )
# 	ofilebase = 'cello69.F.%i.%i.s3rd1'%(r, w)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	synthesize( ofilebase + '.aiff', p )
# 	pruneByLabel( p, range(1,512) )
# 	loris.exportSpc( ofilebase + '.s.spc', p, 37, 0 ) 
# 	loris.exportSpc( ofilebase + '.e.spc', p, 37, 1 ) 
# 
# 	# distill at two partials per harmonic
# 	p = p2
# 	siftme = p.copy()
# 	loris.distill( p )
# 	ofilebase = 'cello69.F.%i.%i.d3rd2'%(r, w)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	synthesize( ofilebase + '.aiff', p )
# 	pruneByLabel( p, range(1,512) )
# 	loris.exportSpc( ofilebase + '.s.spc', p, 37, 0 ) 
# 	loris.exportSpc( ofilebase + '.e.spc', p, 37, 1 ) 
# 	
# 	# try sifting (two partials per harmonic):
# 	p = siftme
# 	loris.sift( p )
# 	zeros = loris.extractLabeled( p, 0 )
# 	loris.distill( p )
# 	ofilebase = 'cello69.F.%i.%i.s3rd2'%(r, w)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	synthesize( ofilebase + '.aiff', p )
# 	pruneByLabel( p, range(1,512) )
# 	loris.exportSpc( ofilebase + '.s.spc', p, 37, 0 ) 
# 	loris.exportSpc( ofilebase + '.e.spc', p, 37, 1 ) 
# 
# 	#
# 	# 69.MF tone	
# 	#
# 	source = 'cello69.MF.loud.aiff'
# 	r = 34
# 	w = 138
# 	p = analyze( source, r, w )
# 	ref = loris.createFreqReference( p, 69*2.75, 69*3.25 )
# 
# 	# distill with one partial per harmonic and two partials per harmonic
# 	p1 = p
# 	p2 = p.copy()
# 	loris.channelize( p1, ref, 3 )
# 	loris.channelize( p2, ref, 6 )
# 
# 	# distill at one partial per harmonic
# 	p = p1
# 	loris.sift( p )
# 	zeros = loris.extractLabeled( p, 0 )
# 	loris.distill( p )
# 	ofilebase = 'cello69.MF.%i.%i.s3rd1'%(r, w)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	synthesize( ofilebase + '.aiff', p )
# 	pruneByLabel( p, range(1,512) )
# 	loris.exportSpc( ofilebase + '.s.spc', p, 37, 0 ) 
# 	loris.exportSpc( ofilebase + '.e.spc', p, 37, 1 ) 
# 
# 	# distill at two partials per harmonic
# 	p = p2
# 	loris.sift( p )
# 	zeros = loris.extractLabeled( p, 0 )
# 	loris.distill( p )
# 	ofilebase = 'cello69.MF.%i.%i.s3rd2'%(r, w)
# 	loris.exportSdif( ofilebase + '.sdif', p )
# 	synthesize( ofilebase + '.aiff', p )
# 	pruneByLabel( p, range(1,512) )
# 	loris.exportSpc( ofilebase + '.s.spc', p, 37, 0 ) 
# 	loris.exportSpc( ofilebase + '.e.spc', p, 37, 1 ) 

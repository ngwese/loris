#!/usr/bin/python

"""
elephants.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

elephant1:
	Use 40 Hz resolution and 130 Hz window, distill at 40 Hz (can use
	higher fundamental in a pinch).

elephant3:
	Use 30 Hz resolution and 80 Hz window, dift and distill at 30 Hz,
	or 45 Hz if necessary.
	
These sound good individually, but to do the morph, need to distill/sift
them to a common frequency, like 60 Hz.

Last updated: 1 March 2005 by Kelly Fitz
"""

print __doc__

import loris, time

orate = 44100

#
# elephant1
#
name = 'elephant1'
anal = loris.Analyzer( 40, 130 )
f = loris.AiffFile( name + '.aiff' )
print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
p = anal.analyze( f.samples(), f.sampleRate() )

# distilled at 60 Hz
ref = loris.BreakpointEnvelopeWithValue( 60 )
loris.channelize( p, ref, 1 )
loris.distill( p )
print 'synthesizing distilled %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( p, orate )
loris.exportAiff( name + '.recon.aiff', samples, orate )
loris.exportSdif( name + '.sdif', p )

# remove any Partials labeled greater than 256
for partial in p:
	if partial.label() > 256:
		partial.setLabel( 0 )
loris.removeLabeled( p, 0 )

loris.exportSpc( name + '.s.spc', p, 60, 0 )
loris.exportSpc( name + '.e.spc', p, 60, 1 )

#
# elephant3
#
name = 'elephant3'
anal = loris.Analyzer( 30, 80 )
f = loris.AiffFile( name + '.aiff' )
print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
p = anal.analyze( f.samples(), f.sampleRate() )

# sifted at 60 Hz
ref = loris.BreakpointEnvelopeWithValue( 60 )
loris.channelize( p, ref, 1 )
loris.sift( p )
zeros = loris.extractLabeled( p, 0 )
loris.distill( p )
print 'synthesizing sifted %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( p, orate )
loris.exportAiff( name + '.recon.aiff', samples, orate )
loris.exportSdif( name + '.sdif', p )

# remove any Partials labeled greater than 256
for partial in p:
	if partial.label() > 256:
		partial.setLabel( 0 )
loris.removeLabeled( p, 0 )

loris.exportSpc( name + '.s.spc', p, 60, 0 )
loris.exportSpc( name + '.e.spc', p, 60, 1 )


##
##This script pertains to three elephant sounds that were candidates 
##for the Toy Angst animation. None of these came out great in the past.
##
##The parameters we once liked for these were:
##
##elephant1 (formerly oldelephant): resolution 50 Hz, window width 200 Hz,
##	reference envelope was created from the longest partial below 1 kHz, 
##	and corresponded to the 12th quasi-harmonic channel
##
##elephant2: ("barely usable") resolution 40 Hz, window 200 Hz,
##	reference envelope was created from the longest partial below 1 kHz, 
##	and corresponded to the 18th quasi-harmonic channel, but was much 
##	shorter than the sound iteslf
##
##elephant3 (formerly valentele): resolution 50 Hz, window 150 Hz,
##	a flat 20 Hz reference envelope was used as the first quasi-harmonic 
##	channel
##
##notes from trial 1:
##	elephant1:
##	- 1k bandwidth association regions aren't wide enough, gets showery
##	- windows narrower than 150 Hz are also showery at large resolutions,
##		but not so much at lower resolutions, which sound better anyway
##	- resolution 80 is bad, 70 is marginal, less than 60 is much better
##	- 250 Hz is too wide for the window, sounds a little flangey with 
##		small resolutions
##	- resolutions 20, 30, 40, 50, 60 are all pretty good for windows
##		130, 150, and 180 Hz
##		
##	elephant2:
##	- nothing sounds good
##	- resolutions greater than 30 Hz don't work at all, _way_ flangey
##	- 20 and 30 Hz resolution with window 100 Hz are the best of the
##		batch, maybe need an even narrower window
##	- of the ones that sounded at all usable, region width didn't make
##		an audible difference
##	
##	elephant3:
##	- resolution greater than 50 Hz doesn't work well, 40 Hz is marginal
##	- resolutions 20 and 30 Hz work okay with narrow windows, best is 
##		100 Hz window, maybe need even narrower
##	- res 20 Hz and window 100 Hz is pretty good, and with those
##		conditions, region width doesn't have any effect
##	
##notes from trial 2:
##	elephant1:	
##	- 20 and 40 Hz resolution sound pretty good with all windows, 60 hz
##	sounds okay with wider windows (like 200 hz)
##	
##	elephant2: 
##	- sounds a bit metallic with all these parameters settings
##	
##	elephant3:
##	- 80 Hz window sounds pretty good, even 100 is pretty good. 
##	
##	Next: try 1 and 3 with time dilation, dunno what to do about 2
##
##notes from trial 3:
##	elephant 1 stretched: 
##	- all sound pretty good (? not the right word perhaps), I can sometimes
##	imagine that 40.130 is the best of the bunch, least artifacts, and maybe
##	20.180 is the worst, but its hard to tell them apart.
##	
##	elephant 3 stretched:
##	- since this sound was very wet to begin with, the stretched versions 
##	don't sound very good. The slap-echo from hard (cemement presumably) 
##	walls in the original recording stretch to sound like a cheap spring 
##	reverberator. One should try stretching only the middle so that at 
##	least the decay won't be so noticable. 
##	- the 130 window seems to smooth out some of the fast warbling that is
##	present in the original, and evident (stretched) in other reproductions
##	with narrower windows. Seems like it is being captured by retaining 
##	partials thhat are close enough in frequency to give beating.
##	- the reverberation artifacts are more pronounced in the 80 Hz windows
##	than in the 100 Hz windows, esp. 30.80.
##	- not much difference between 20 and 30 Hz resolution in general.
##
##notes from trial 4:
##	Tried making spc files and doing some real time synthesis:
##	- elephant1 is fine at 40.130 harmonically-distilled at 40 Hz. 
##	- elephant3 is best at 30.80 harmonically distilled at 30 Hz, 
##	but there is a little bit of crunch in it.
##	- made a nice Continuum-controlled version of elephant1, noticed
##	that there's nothing much at all going on above Partial 320.
##	- in elephant3, there's nothing going on above Partial 240.
##	- in elephant1, there isn't really a fundamental to track, but
##	the Partial labeled 16 looks as though it could serve as a reference.
##	- in elephant3, there doesn't appear to be anything that could 
##	serve as a reference Partial.
##	
##	Try more distillations. Try sifting elephant3.
##	
##notes from trial 5:
##	- sifting helps elephant3, no more crunch.
##	- distilling at 45 and 60 Hz degrade the reconstruction of elephant3
##	only a little, in fact, they are hard to distinguish from 30 Hz.
##	- distilling elephant1 at 80 Hz (or 60) is almost indistinguishible 
##	from a 40 Hz distillation.
##
##(using new distiller, before version 1.0.3)
##	- elephant1 - 40 Hz distillation crunches less than the others, but the
##	crunch isn't that noticable in this sound.
##	 - elephant 3 still benefits from sifting, s45 is best, or s30
##
##
##	 
##from trials import *
##
### use this trial counter to skip over
### eariler trials
##trial = 5
##
##print "running trial number", trial, time.ctime(time.time())
##
##sources = ( 'elephant1.aiff', 'elephant2.aiff', 'elephant3.aiff' )
##
##
##if trial == 1:
##	for source in sources:
##		resolutions = (20, 30, 40, 50, 60, 70, 80 )
##		widths = ( 100, 130, 150, 180, 200, 250 )
##		bws = (1000, 2000, 4000)
##		for r in resolutions:
##			for w in widths:
##				if w == 0:
##					w = r
##				for bw in bws:
##					p = analyze( source, r, w, bw )
##					ofile = '%s.%i.%i.%ik.aiff'%(source[:-5], r, w, bw/1000)
##					synthesize( ofile, p )
##
##if trial == 2:
##	# elephant1.aiff
##	source = sources[0]  
##	resolutions = (20, 40, 60 )
##	widths = ( 130, 150, 180, 200 )
##	for r in resolutions:
##		for w in widths:
##			p = analyze( source, r, w )
##			ofile = '%s.%i.%i.aiff'%(source[:-5], r, w)
##			synthesize( ofile, p )
##	# elephant2.aiff and elephant3.aiff
##	for source in sources[1:]:
##		resolutions = (20, 30 )
##		widths = ( 80, 100 )
##		for r in resolutions:
##			for w in widths:
##				p = analyze( source, r, w )
##				ofile = '%s.%i.%i.aiff'%(source[:-5], r, w)
##				synthesize( ofile, p )
##
##if trial == 3:
##	# elephant1.aiff
##	source = sources[0]  
##	resolutions = ( 20, 40 )
##	widths = ( 130, 180 )
##	for r in resolutions:
##		for w in widths:
##			p = analyze( source, r, w )
##			p = timescale( p, 2. )
##			ofile = '%s.%i.%i.T2.aiff'%(source[:-5], r, w)
##			synthesize( ofile, p )
##	# elephant3.aiff
##	source = sources[2]  
##	resolutions = ( 20, 30 )
##	widths = ( 80, 100, 130 )
##	for r in resolutions:
##		for w in widths:
##			p = analyze( source, r, w )
##			p = timescale( p, 2. )
##			ofile = '%s.%i.%i.T2.aiff'%(source[:-5], r, w)
##			synthesize( ofile, p )
##			
##			
##if trial == 4:
##	# elephant1.aiff
##	source = sources[0]  
##	r = 40
##	w = 130
##	p = analyze( source, r, w )
##	ofilebase = '%s.%i.%i'%(source[:-5], r, w)
##	synthesize( ofilebase + '.aiff', p )
##	harmonicDistill( p, r )
##	pruneByLabel( p, range(1,511) )
##	ofilebase = ofilebase + '.d%i'%r
##	loris.exportSpc( ofilebase + '.s.spc', p, 60, 0 ) 
##	loris.exportSpc( ofilebase + '.e.spc', p, 60, 1 ) 
##	loris.exportSdif( ofilebase + '.sdif', p )
##	synthesize( ofilebase + '.aiff', p )
##	
##			
##	# elephant3.aiff
##	source = sources[2]  
##	r = 30
##	widths = ( 80, 100 )
##	for w in widths:
##		p = analyze( source, r, w )
##		ofilebase = '%s.%i.%i'%(source[:-5], r, w)
##		synthesize( ofilebase + '.aiff', p )
##		harmonicDistill( p, r )
##		pruneByLabel( p, range(1,511) )
##		ofilebase = ofilebase + '.d%i'%r
##		loris.exportSpc( ofilebase + '.s.spc', p, 60, 0 ) 
##		loris.exportSpc( ofilebase + '.e.spc', p, 60, 1 ) 
##		loris.exportSdif( ofilebase + '.sdif', p )
##		synthesize( ofilebase + '.aiff', p )
##	
##if trial == 5:
##	# elephant1.aiff
##	source = sources[0]  
##	r = 40
##	w = 130
##	p = analyze( source, r, w )
##	ofilebase = '%s.%i.%i'%(source[:-5], r, w)
##	synthesize( ofilebase + '.aiff', p )
##	for f in ( r, r*1.5, r*2 ):
##		pcopy = p.copy()
##		harmonicDistill( pcopy, f )
##		pruneByLabel( pcopy, range(1,511) )
##		ofilebase = '%s.%i.%i.d%i'%(source[:-5], r, w, f)
##		loris.exportSpc( ofilebase + '.s.spc', pcopy, 60, 0 ) 
##		loris.exportSpc( ofilebase + '.e.spc', pcopy, 60, 1 ) 
##		loris.exportSdif( ofilebase + '.sdif', pcopy )
##		synthesize( ofilebase + '.aiff', pcopy )
##	
##			
##	# elephant3.aiff
##	source = sources[2]  
##	r = 30
##	w = 80
##	p = analyze( source, r, w )
##	ofilebase = '%s.%i.%i'%(source[:-5], r, w)
##	synthesize( ofilebase + '.aiff', p )
##	for f in ( r, r*1.5, r*2 ):
##		pcopy = p.copy()
##		harmonicDistill( pcopy, f )
##		pruneByLabel( pcopy, range(1,511) )
##		ofilebase = '%s.%i.%i.d%i'%(source[:-5], r, w, f)
##		loris.exportSpc( ofilebase + '.s.spc', pcopy, 60, 0 ) 
##		loris.exportSpc( ofilebase + '.e.spc', pcopy, 60, 1 ) 
##		loris.exportSdif( ofilebase + '.sdif', pcopy )
##		synthesize( ofilebase + '.aiff', pcopy )
##		
##		pcopy = p.copy()
##		harmonicSift( pcopy, f )
##		pruneByLabel( pcopy, range(1,511) )
##		ofilebase = '%s.%i.%i.s%i'%(source[:-5], r, w, f)
##		loris.exportSpc( ofilebase + '.s.spc', pcopy, 60, 0 ) 
##		loris.exportSpc( ofilebase + '.e.spc', pcopy, 60, 1 ) 
##		loris.exportSdif( ofilebase + '.sdif', pcopy )
##		synthesize( ofilebase + '.aiff', pcopy )
##	

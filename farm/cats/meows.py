#!/usr/bin/python

"""
meows.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the cat meows that I analyzed for the Toy 
Angst sound morphing project. There are two of these, meow1 and meow3,
and they both worked a lot better than the angrycat sound, which has 
never sounded great. 

At the time of Toy Angst, I used 50 Hz resolution and 240 Hz window
for meow1, and 40 Hz resolution and 240 Hz window for meow3. Don't
really know why, but it must've worked out okay. In both cases, a 
reference partial was extracted and used for distilling. The distillations
usd many partials per "harmonic", but this might have been an attempt to
line up the partials with another sound's partials for morphing.

trial 1:
	- 240 Hz window seems best for meow1, 120 is too noisy or hissy, and
	360 sound a little doodly. All resolutions sound about the same, and have
	about the same amount of sdif data.
	- 120 Hz windows are unusable for meow3, same as meow1, 240 Hz and 360 Hz
	windows both seem fine though. All resolutions sound about the same. The
	amount of sdif data for all of these differs dramatically though.
	
trial 2:
	- one partial per harmonic is not enough for meow3
	- for meow3, the 240 windows are a little bit crunchier than the 360
	windows, but the 360 windows have some mild artifacts, esp. at the release
	- for meow3, five partials per harmonic degrades synthesis slightly, three
	partials per harmonic degrades it slightly more.
	- for meow1, the distilling results are somewhat worse, but the trends are 
	the same: fewer partials per harmonic degrade sound more, 360 windows
	add artifacts (really not usable in this case)
	- in neither case did the resolution (50 or 75 Hz) seem to matter
	- maybe sifting would help?
	
trial 3:
	- actually, these all seem quite usable. Sifting doesn't seem to help much,
	but even the pure distillations seem okay at 5 partials per harmonic. Maybe ought
	to try smaller numbers again.
	- in meow3, Partials starting after 3.17 seconds could be elimintated to clean
	up the release.
	
trial 4: 
	- it seems like 5 partials per harmonic might be the lower limit for meow1,
	maybe can use 2 or 3 for meow3.
	
side note: contour boundaries, for morphing with something pitched, are at times
(approximately) 1.5 s, 2.25 s, 3 s, and the total duration is 3.413 s. Some 
fudamental frequencies are 436 Hz at 2 s and 480 Hz at 2.5 s.

Conclusions: 
	Analyze with 75 Hz resolution and 240 Hz window, distill at 5 Partials
	per harmonic for meow1, and 3 Partials per harmonic for meow3.

Last updated: 6 March 2003 by Kelly Fitz
"""

print __doc__

import loris, time
print """
Using Loris version %s
"""%loris.version()

anal = loris.Analyzer( 75, 240 )
orate = 44100

# analyze meow1
name = 'meow1'
f = loris.AiffFile( name + '.aiff' )
print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
p = anal.analyze( f.samples(), f.sampleRate() )

# meow1 collated
pcollate = p.copy()
loris.distill( pcollate )
print 'synthesizing raw (collated) %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( pcollate, orate )
loris.exportAiff( name + '.raw.aiff', samples, orate )

# meow1 at 5 Partials per harmonic
ref = loris.createFreqReference( p, 0, 1000, 100 )
loris.channelize( p, ref, 5 )
loris.distill( p )
print 'synthesizing distilled (5 Partials per harmonic) %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( p, orate )
loris.exportAiff( name + '.d5.aiff', samples, orate )
loris.exportSpc( name + '.d5.s.spc', p, 36, 0 )
loris.exportSpc( name + '.d5.e.spc', p, 36, 1 )
loris.exportSdif( name + '.d5.sdif', p )

# analyze meow3
name = 'meow3'
f = loris.AiffFile( name + '.aiff' )
print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
p = anal.analyze( f.samples(), f.sampleRate() )

# meow3 collated
pcollate = p.copy()
loris.distill( pcollate )
print 'synthesizing raw (collated) %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( pcollate, orate )
loris.exportAiff( name + '.raw.aiff', samples, orate )

# meow3 at 3 Partials per harmonic
ref = loris.createFreqReference( p, 0, 1000, 100 )
loris.channelize( p, ref, 3 )
loris.distill( p )
print 'synthesizing distilled (3 Partials per harmonic) %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( p, orate )
loris.exportAiff( name + '.d3.aiff', samples, orate )
loris.exportSpc( name + '.d3.s.spc', p, 36, 0 )
loris.exportSpc( name + '.d3.e.spc', p, 36, 1 )
loris.exportSdif( name + '.d3.sdif', p )


# from trials import *
# 
# # use this trial counter to skip over
# # eariler trials
# trial = 4
# 
# print "running trial number", trial, time.ctime(time.time())
# 
# sources = ('meow1.aiff', 'meow3.aiff')
# meow1 = sources[0]
# meow3 = sources[1]
# 
# if trial == 1:
# 	resolutions = ( 40, 50, 75 )
# 	widths = ( 120, 240, 360 )
# 	for source in sources:
# 		for r in resolutions:
# 			for w in widths:
# 				p = analyze( source, r, w )
# 				ofilebase = '%s.%i.%i'%(source[:-5], r, w)
# 				synthesize( ofilebase + '.aiff', p )
# 				loris.exportSdif( ofilebase + '.sdif', p )
# 	
# if trial == 2:
# 	resolutions = ( 50, 75 )
# 	widths = ( 240, 360 )
# 	for source in sources:
# 		for r in resolutions:
# 			for w in widths:
# 				p = analyze( source, r, w )
# 				ofilebase = '%s.%i.%i'%(source[:-5], r, w)
# 				synthesize( ofilebase + '.aiff', p )
# 				loris.exportSdif( ofilebase + '.sdif', p )
# 				
# 				# distill at 1, 3, 5 partials per harmonic
# 				ref = loris.createFreqReference( p, 0, 1000, 100 )
# 				psave = p
# 				for n in (1,3,5):
# 					p = psave.copy()
# 					ofilebase = '%s.%i.%i.d%i'%(source[:-5], r, w, n)
# 					loris.channelize( p, ref, n )
# 					loris.distill( p )
# 					synthesize( ofilebase + '.aiff', p )
# 					loris.exportSdif( ofilebase + '.sdif', p )
# 					loris.exportSpc( ofilebase + '.s.spc', p, 36, 0 )
# 					loris.exportSpc( ofilebase + '.e.spc', p, 36, 1 )
# 				
# if trial == 3:
# 	r = 75
# 	w = 240
# 	for source in sources:
# 		p = analyze( source, r, w )
# 		ofilebase = '%s.%i.%i'%(source[:-5], r, w)
# 		
# 		# distill at 5, 8, 11 partials per harmonic
# 		ref = loris.createFreqReference( p, 0, 1000, 100 )
# 		psave = p
# 		for n in (5, 8, 11):
# 			p = psave.copy()
# 			loris.channelize( p, ref, n )
# 			ps = p.copy()
# 			
# 			# distilled
# 			loris.distill( p )
# 			ofilebase = '%s.%i.%i.d%i'%(source[:-5], r, w, n)
# 			synthesize( ofilebase + '.aiff', p )
# 			loris.exportSpc( ofilebase + '.s.spc', p, 36, 0 )
# 			loris.exportSpc( ofilebase + '.e.spc', p, 36, 1 )
# 			
# 			# sifted
# 			p = ps
# 			loris.sift( p )
# 			loris.distill( p )
# 			ofilebase = '%s.%i.%i.s%i'%(source[:-5], r, w, n)
# 			synthesize( ofilebase + '.aiff', p )
# 			loris.exportSpc( ofilebase + '.s.spc', p, 36, 0 )
# 			loris.exportSpc( ofilebase + '.e.spc', p, 36, 1 )
# 			
# 	
# if trial == 4:
# 	r = 75
# 	w = 240
# 	for source in sources:
# 		p = analyze( source, r, w )
# 		ofilebase = '%s.%i.%i'%(source[:-5], r, w)
# 		
# 		# distill at 1,2,3,5 partials per harmonic
# 		ref = loris.createFreqReference( p, 0, 1000, 100 )
# 		psave = p
# 		for n in (1,2,3,5):
# 			p = psave.copy()
# 			loris.channelize( p, ref, n )
# 			ps = p.copy()
# 			
# 			# distilled
# 			loris.distill( p )
# 			ofilebase = '%s.%i.%i.d%i'%(source[:-5], r, w, n)
# 			synthesize( ofilebase + '.aiff', p )
# 			loris.exportSpc( ofilebase + '.s.spc', p, 36, 0 )
# 			loris.exportSpc( ofilebase + '.e.spc', p, 36, 1 )
# 			loris.exportSdif( ofilebase + '.sdif', p )
# 					
# 			# sifted
# 			p = ps
# 			loris.sift( p )
# 			loris.distill( p )
# 			ofilebase = '%s.%i.%i.s%i'%(source[:-5], r, w, n)
# 			synthesize( ofilebase + '.aiff', p )
# 			loris.exportSpc( ofilebase + '.s.spc', p, 36, 0 )
# 			loris.exportSpc( ofilebase + '.e.spc', p, 36, 1 )
# 			loris.exportSdif( ofilebase + '.sdif', p )
# 					
# 	

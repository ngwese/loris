#!/usr/bin/python

"""
flutter.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This 330 Hz flutter-tongued flute tone is is well-represented using 250 Hz
resolution and a 500 Hz window. The reference envelope can be derived from
the fundamental partial (around 330 Hz) and used to distill at one partial
per harmonic. 

Last updated: 16 May 2003 by Kelly Fitz
"""
print __doc__

import loris, time

print """
Using Loris version %s
"""%loris.version()

anal = loris.Analyzer( 250, 500 )
orate = 44100

name = 'flutter'
f = loris.AiffFile( name + '.aiff' )
print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
p = anal.analyze( f.samples(), f.sampleRate() )

# distilled using one partial per harmonic
ref = loris.createFreqReference( p, 330*.75, 330*1.5, 100 )
loris.channelize( p, ref, 1 )
loris.distill( p )
print 'synthesizing distilled %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( p, orate )
loris.exportAiff( name + '.recon.aiff', samples, orate )
loris.exportSdif( name + '.sdif', p )
loris.exportSpc( name + '.s.spc', p, 62, 0 )
loris.exportSpc( name + '.e.spc', p, 62, 1 )

##This script pertains to a flutter-tongued flute tone used in 
##kelly's Ph. D. thesis work on the reassigned Bandwidth-Enhanced
##Additive Sound Model.
##
##The parameters we once liked for this are resolution 300 and
##window width 400 Hz. The fundamental used for distillation was
##330 Hz.
##
##notes from trial 1:
##
##- most of these sound pretty good raw, but distillation corrupts
##them. Of all of them, res 250 width 300 seems to have survived 
##distillation in best condition.
##
##- all lack the low frequency thumping present in the original, not
##surprisingly since the Analyzer is configured with freq floor equal
##to the resolution.
##
##notes from trial 2:
##	
##- lowering the frequency floor to 30 Hz restored the thumping
##to the 250 and 160 resolution analyses with 300 and 450 windows
##
##- 250, 300 still seems to survive distillation the best
##
##notes from trial 3 (using 1.0beta8):
##	- distillations have a noise artifact at 2.45 seconds, this might have
##	been introduced by dropping the frequency floor (I have an old synthesis
##	using these parameters that didn't have this artifact).
##	- otherwise, this would sound good
##
##notes from trial 4 (using 1.0beta9):
##	- raising the frequency floor to r/2 did not get rid
##	of the artifact
##	
##notes from trial 5: 
##	- the artifact has nothing to do with the frequency floor, must
##	just be a distiller thing. Look at spc files in Kyma, also
##	try sifting, after the sieve is restored.
##	- WIERD the artifact doesn't make it into the Spc files...
##	Lip thinks this is definitely a case of having too-fast Partial
##	turnons, and will be ameliorated by lengthening the turnon time
##	to 5 ms (from 1 ms)
##
##notes from trial 6:
##	- Lip may or may not be right about the turnon times, but sifting
##	(forgot to remove the sifted-out partials) definitely eliminated
##	the crunch problem
##	- no apparent advantage (or difference) to using two partials per harmonic
##	- no apparent difference between 125 Hz and 250 Hz freq floor, 50 Hz
##	retains some thumping, but I think that makes it less useful, and doesn't
##	really improve the perceived fidelity.
##	- virtually no difference between sifted version and raw synthesis
##	- try tossing out sifted partials
##	
##notes from trial 7:
##	- seems not to matter whether the sifted out partials are tossed or
##	not (so they might as well be tossed), no perceptible difference.
##	- aside: used csound to render the crunchy (unsifted, distilled)
##	flutter with different fade times, and it seems to have no effect.
##	It is still possible though that the Distiller is using the fast
##	fade time and introducing the artifact. Can't easily test that without
##	changing Loris code.
##
##notes from trial 9:
##	- using the new distillation algorithm (implemented May 2003, before
##	version 1.0.3 release), I do not get any artifact even when I do not
##	sift! Of course, in point of fact, even using loris 1.0.2, I cannot now
##	reproduce the crunchy artifact.
##
##notes from trial 10:
##	- 300, 400, and 500 Hz windows are nearly indistinguishible, even distilled,
##	and all are very good.
##	 - I can convince myself that using a wider window gives a more percussive
##	 reconstruction, so I prefer the 500 Hz window, though I might ust be
##	 imagining the difference. Certainly there's no degredation.
##	- Also, time dilation is better with the long window version, even after
##	distillation. Sounds better than 250,300 raw.
##
##conclusion: This sound is is well-represented using 250 Hz resolution and
##a 500 Hz window. The reference envelope can be derived from the fundamental
##partial (around 330 Hz) and used to distill at one partial per harmonic. 
##

##from trials import *
##
### use this trial counter to skip over
### eariler trials
##trial = 10
##
##print "running trial number", trial, time.ctime(time.time())
##
##source = 'flutter.aiff'
##
##if trial == 1:
##	resolutions = (160, 250, 300)
##	widths = ( 300, 400, 600 )
##	funds = ( 330, 165 )
##	for r in resolutions:
##		for w in widths:
##			p = analyze( source, r, w )
##			ofile = 'flutter.%i.%i.aiff'%(r, w)
##			synthesize( ofile, p )
##			for f in funds:
##				p2 = p.copy()
##				harmonicDistill( p2, f )
##				ofile = 'flutter.%i.%i.d%i.aiff'%(r, w, f)
##				synthesize( ofile, p2 )
##
##if trial == 2:
##	resolutions = (160, 250, 300)
##	widths = ( 300, 400, 600 )
##	funds = ( 330, 165 )
##	for r in resolutions:
##		for w in widths:
##			a = loris.Analyzer( w )
##			a.setFreqResolution( r )
##			a.setFreqFloor( 30 )
##			# get sample data:
##			f = loris.AiffFile( source )
##			samples = f.samples()
##			rate = f.sampleRate()
##			# analyze and return partials
##			p = a.analyze( samples, rate )
##			ofile = 'flutter.lo.%i.%i.aiff'%(r, w)
##			synthesize( ofile, p )
##			for f in funds:
##				p2 = p.copy()
##				harmonicDistill( p2, f )
##				ofile = 'flutter.lo.%i.%i.d%i.aiff'%(r, w, f)
##				synthesize( ofile, p2 )
##
##if trial == 3:
##	resolutions = (250, )
##	widths = ( 300, )
##	funds = ( 330, 165 )
##	for r in resolutions:
##		for w in widths:
##			a = loris.Analyzer( r, w )
##			a.setFreqFloor( 30 )
##			# get sample data:
##			f = loris.AiffFile( source )
##			samples = f.samples()
##			rate = f.sampleRate()
##			# analyze and return partials
##			p = a.analyze( samples, rate )
##			ofile = 'flutter.lo.%i.%i.aiff'%(r, w)
##			synthesize( ofile, p )
##			for f in funds:
##				p2 = p.copy()
##				harmonicDistill( p2, f )
##				ofile = 'flutter.lo.%i.%i.d%i.aiff'%(r, w, f)
##				synthesize( ofile, p2 )
##
##if trial == 4:
##	resolutions = (250, )
##	widths = ( 300, )
##	funds = ( 330, 165 )
##	for r in resolutions:
##		for w in widths:
##			a = loris.Analyzer( r, w )
##			a.setFreqFloor( r/2 )
##			# get sample data:
##			f = loris.AiffFile( source )
##			samples = f.samples()
##			rate = f.sampleRate()
##			# analyze and return partials
##			p = a.analyze( samples, rate )
##			ofile = 'flutter.lo2.%i.%i.aiff'%(r, w)
##			synthesize( ofile, p )
##			for f in funds:
##				p2 = p.copy()
##				harmonicDistill( p2, f )
##				ofile = 'flutter.lo2.%i.%i.d%i.aiff'%(r, w, f)
##				synthesize( ofile, p2 )
##
##if trial == 5:
##	r = 250
##	w = 300
##	a = loris.Analyzer( r, w )
##	f = loris.AiffFile( source )
##	samples = f.samples()
##	rate = f.sampleRate()
##	
##	funds = ( 330, 165 )
##	floors = ( 250, 125, 50 )
##	
##	for fl in floors:
##		a.setFreqFloor( fl )
##		p = a.analyze( samples, rate )
##		ofile = 'flutter.%i.%i.%i.aiff'%(r, w, fl)
##		synthesize( ofile, p )
##		for f in funds:
##			pd = p.copy()
##			harmonicDistill( pd, f )
##			ofilebase = 'flutter.%i.%i.%i.d%i'%(r, w, fl, f)
##			synthesize( ofilebase + '.aiff', pd )
##			loris.exportSpc(ofilebase + 's.spc', pd, 64, 0)			
##			loris.exportSpc(ofilebase + 'e.spc', pd, 64, 1)
##			
##if trial == 6:
##	r = 250
##	w = 300
##	a = loris.Analyzer( r, w )
##	f = loris.AiffFile( source )
##	samples = f.samples()
##	rate = f.sampleRate()
##	
##	refnums = ( 1, 2 )
##	floors = ( 250, 125, 50 )
##	
##	for fl in floors:
##		a.setFreqFloor( fl )
##		p = a.analyze( samples, rate )
##		ofilebase = 'flutter.%i.%i.%i'%(r, w, fl)
##		synthesize( ofilebase + '.aiff', p )
##		loris.exportSdif( ofilebase + '.sdif', p )
##		ref = loris.createFreqReference( p, 330*.75, 330*1.5, 100 )
##		for n in refnums:
##			pd = p.copy()
##			loris.channelize( pd, ref, n )
##			ps = pd.copy()
##
##			# distilled
##			loris.distill( pd )
##			ofilebase = 'flutter.%i.%i.%i.d%i'%(r, w, fl, n)
##			synthesize( ofilebase + '.aiff', pd )
##			loris.exportSpc(ofilebase + 's.spc', pd, 64, 0)			
##			loris.exportSpc(ofilebase + 'e.spc', pd, 64, 1)
##
##			# sifted
##			loris.sift( ps )
##			loris.distill( ps )
##			ofilebase = 'flutter.%i.%i.%i.s%i'%(r, w, fl, n)
##			synthesize( ofilebase + '.aiff', ps )
##			loris.exportSpc(ofilebase + 's.spc', ps, 64, 0)			
##			loris.exportSpc(ofilebase + 'e.spc', ps, 64, 1)
##			
##if trial == 7:
##	r = 250
##	w = 300
##	p = analyze( source, r, w )
##	ofilebase = 'flutter.%i.%i'%(r, w)
##	loris.exportSdif( ofilebase + '.raw.sdif', p )
##	ref = loris.createFreqReference( p, 330*.75, 330*1.5, 100 )
##	loris.channelize( p, ref, 1 )
##	
##	# sifted version
##	loris.sift( p )	
##	loris.distill( p )
##	ofilebase = 'flutter.%i.%i.ssav'%(r, w)
##	synthesize( ofilebase + '.aiff', p )
##	loris.exportSpc( ofilebase + '.s.spc', p, 64, 0 )			
##	loris.exportSpc( ofilebase + '.e.spc', p, 64, 1 )
##	
##	# sifted version with sifted partials removed
##	zeros = loris.extractLabeled( p, 0 )
##	ofilebase = 'flutter.%i.%i.sift'%(r, w)
##	synthesize( ofilebase + '.aiff', p )
##	loris.exportSpc( ofilebase + '.s.spc', p, 64, 0 )			
##	loris.exportSpc( ofilebase + '.e.spc', p, 64, 1 )
##		
##if trial == 8:
##	r = 250
##	w = 300
##	p = analyze( source, r, w )
##	ofilebase = 'flutter.%i.%i'%(r, w)
##	loris.exportSdif( ofilebase + '.raw.sdif', p )
##	ref = loris.createFreqReference( p, 330*.75, 330*1.5, 100 )
##	loris.channelize( p, ref, 1 )
##	
##	# sift
##	loris.sift( p )	
##	loris.distill( p )
##	zeros = loris.extractLabeled( p, 0 )
##	ofilebase = 'flutter.%i.%i.sft'%(r, w)
##	synthesize( ofilebase + '.aiff', p )
##	loris.exportSdif( ofilebase + 'sdif', p )
##	loris.exportSpc( ofilebase + '.s.spc', p, 64, 0 )			
##	loris.exportSpc( ofilebase + '.e.spc', p, 64, 1 )
##		
##						
##if trial == 9:
##	r = 250
##	w = 300
##	p = analyze( source, r, w )
##	ofilebase = 'flutter.%i.%i.raw'%(r, w)
##	praw = p.copy()
##	loris.distill( praw )
##	synthesize( ofilebase + '.aiff', praw )
##	loris.exportSdif( ofilebase + '.sdif', praw )
##	loris.exportSpc( ofilebase + '.s.spc', praw, 64, 0 )			
##	loris.exportSpc( ofilebase + '.e.spc', praw, 64, 1 )
##
##	ref = loris.createFreqReference( p, 330*.75, 330*1.5, 100 )
##	loris.channelize( p, ref, 1 )
##	
##	# distilled version
##	psave = p.copy()
##	loris.distill( p )
##	ofilebase = 'flutter.%i.%i.dist'%(r, w)
##	synthesize( ofilebase + '.aiff', p )
##	loris.exportSpc( ofilebase + '.s.spc', p, 64, 0 )			
##	loris.exportSpc( ofilebase + '.e.spc', p, 64, 1 )
##		
##	# sifted version
##	p = psave
##	loris.sift( p )	
##	loris.distill( p )
##	zeros = loris.extractLabeled( p, 0 )
##	ofilebase = 'flutter.%i.%i.sift'%(r, w)
##	synthesize( ofilebase + '.aiff', p )
##	loris.exportSpc( ofilebase + '.s.spc', p, 64, 0 )			
##	loris.exportSpc( ofilebase + '.e.spc', p, 64, 1 )
##		
##if trial == 10:
##	r = 250
##	windows = (300, 400, 500)
##	for w in windows:
##		p = analyze( source, r, w )
##		ofilebase = 'flutter.%i.%i'%(r, w)
##		synthesize( ofilebase + '.raw.aiff', p )
##		ref = loris.createFreqReference( p, 330*.75, 330*1.5, 100 )
##		loris.channelize( p, ref, 1 )
##		loris.distill( p )
##		synthesize( ofilebase + '.recon.aiff', p )
##


"""
funny.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to two recordings of the words "funny people"
spoken by two different women. The first one (funnyPeeple1.aiff) 
has fundamental frequencies in the range 170-225 Hz, and the other
(funnyPeeple2.aiff) has fundamentals in the range 200-330 Hz. The
goal of this exercise is to morph the two speakers.


notes from trial 1:
	- getting noisy patches, worst for narrower windows. 
	- don't seem to need very narrow resolution.

notes from trial 2:
	- these windows are much better
	- funny1 - with headphones it is possible to detect that the 85 Hz
	resolution has somewhat less artifacts from the breathy sounds
	- funny2 - 500 Hz window has a noise burst at the release of "peeple"
	from the breathiness
	- 100.400 is the best of the funny2's
	
notes from trial 3:
	- funny1 - with the 300 Hz window, distilling or sifting looses the
	fundamental at "ee" in "peeple", 400 Hz window doesn't. None of these 
	is great, but the two-partials-per-harmonic ones are better
	- funny2 - again, not great, but two partials per harmonic is
	better than 1, and sifting is a little better, maybe, than distilling,
	sometimes.
	- looking at these in Kyma, I think that maybe the frequency reference
	was too jittery, and that maybe caused some of the ugly connectivity, 
	and maybe some of the wierd artifacts (like the lost fundamental). Try
	again with smoothed reference envelopes. Also try more parameters, since
	none of these survived that well.

notes from trial 4:
	- the 400 Hz window analyses, sifted with two Partials per harmonic
	seem to be usable. It would be nice if I had Markers working in Loris,
	but I don't, yet, so here are the feature points to use for morphing:

	feature     funny1 time (ms)   funny2 time (ms)
	"f"              202                 238
	"un"             386                 496
	"ny"             598                 697
	"pee"            902                 989
	"puhl"          1173                1241
	(end of "puhl") 1495                1521
	
	next, try without BW association (in Loris 1.0.3, set region width
	to 0)

notes from trial 5:
	- raw syntheses sound very similar, hard to tell them apart. Need to look
	in Kyma to see why they sound so different when I start distilling them.
	- still seems like finer resolution is better, and probably the narrower
	windows are better, but not by much, in the case of raw syntheses
	- even without BW association, distillation introduces some noise, 
	need to remove noise after distilling if I really want no noise.
	- trying this one again with increased gain...
	
	funnyloud1:
	- raw syntheses all sound good
	- distilling with one partial per harmonic introduces too much noise
	- sifting 85.300 looses some fundamental. 
	- distilling with two partials per harmonic is a bit noisy, except 155.400
	sounds pretty good that way.
	- sifting 400 wide windows at two partials per harmonic sound very good.
	- sifting at one partial per harmonic sounds glassy.
	- none of these sound very good in Kyma, using sine-only spc.
	
	- so: 400 Hz windows are best,  sifting at 2 partials per harmonic is safe, 
	distilling at two partials per harmonic is nearly as good with the resolution
	at 155 Hz, but introduces artifacts at 85 Hz resolution.
	
	funnyLoud2:
	- raw syntheses are all quite good
	- sifting with one partial per harmonic sounds funny
	- distilling at one partial per harmonic is way too noisy and crunchy
	- distilling at two partials per harmonic is better, but still not usable
	(if you _had_ to use one, 170.400.d2 is best)
	- sifting with two partials per harmonic is pretty good, best with 400 Hz windows
	
	- so: 400 Hz windows are best, sifting at 2 partials per harmonic is not
	too damaging.
	
	- listening to the loud (gain-normalized) sources, it sounds like some of
	the noisiness in syntheses is similar to the ambient echo in the recording.
	
	- tried a couple morphs and also looked in Kyma, and found that the tracking
	of harmonics is still not very good above the first few. Morphs sound bad 
	because partials that should correspond don't.
	
	- just noticed (6 June) that in trial 5 I was channelizing the same Partials
	twice, first at one partial per harmonic, then at two. Duh.
	
	- used these sounds to develop the tracking analyzer, which helps get
	harmonics connected correctly across phoneme boundaries
	
notes from trial 6:
	- two partial per harmonic sifts are hard to distinguish from raw reconstructions
	- morph is still not great, but its getting better
	
notes from trial 7:
	- halving the hop time (with or without halving the crop time) doesn't
	seem to make any difference
	- updated here to work with Loris 1.3 (beta)

notes from trial 8:
	- try using the fundamental tracker instead of the hand-constructed
	fundamental estimate, seems to be no worse.
	
Last updated: 27 Jan 2006 by Kelly Fitz
"""
print __doc__

import loris, time
# from trials import *
print """
Using Loris version %s
"""%loris.version()


# use this trial counter to skip over
# eariler trials
trial = 8

print "running trial number", trial, time.ctime(time.time())

if trial == 1:
	source = 'funnyPeeple1.aiff'
	resolutions = ( 65, 85, 125, 155 )
	widths = ( 85, 170, 255 )
	for r in resolutions:
		for w in widths:
			sfile = loris.AiffFile( source )
			print 'analyzing %s (%s)'%(source, time.ctime(time.time()))
			a = loris.Analyzer( r, w )
			p = a.analyze( sfile.samples(), sfile.sampleRate() )
			ofilebase = 'funny1.%i.%i'%(r, w)
			synthesize( ofilebase + '.aiff', p )

	source = 'funnyPeeple2.aiff'
	resolutions = ( 70, 100, 170 )
	widths = ( 100, 200, 300 )
	for r in resolutions:
		for w in widths:
			sfile = loris.AiffFile( source )
			print 'analyzing %s (%s)'%(source, time.ctime(time.time()))
			a = loris.Analyzer( r, w )
			p = a.analyze( sfile.samples(), sfile.sampleRate() )
			ofilebase = 'funny2.%i.%i'%(r, w)
			synthesize( ofilebase + '.aiff', p )

if trial == 2:
	source = 'funnyPeeple1.aiff'
	resolutions = ( 85, 155 )
	widths = ( 300, 400 )
	for r in resolutions:
		for w in widths:
			sfile = loris.AiffFile( source )
			print 'analyzing %s (%s)'%(source, time.ctime(time.time()))
			a = loris.Analyzer( r, w )
			p = a.analyze( sfile.samples(), sfile.sampleRate() )
			ofilebase = 'funny1.%i.%i'%(r, w)
			synthesize( ofilebase + '.aiff', p )

	source = 'funnyPeeple2.aiff'
	resolutions = ( 100, 170 )
	widths = ( 400, 500 )
	for r in resolutions:
		for w in widths:
			sfile = loris.AiffFile( source )
			print 'analyzing %s (%s)'%(source, time.ctime(time.time()))
			a = loris.Analyzer( r, w )
			p = a.analyze( sfile.samples(), sfile.sampleRate() )
			ofilebase = 'funny2.%i.%i'%(r, w)
			synthesize( ofilebase + '.aiff', p )


if trial == 3:
	source = 'funnyPeeple1.aiff'
	pref = loris.importSdif('funnyPeeple1.fund.sdif')
	ref = loris.createFreqReference( pref, 50, 500 )
	r = 85
	widths = ( 300, 400 )
	for w in widths:
		sfile = loris.AiffFile( source )
		print 'analyzing %s (%s)'%(source, time.ctime(time.time()))
		a = loris.Analyzer( r, w )
		p = a.analyze( sfile.samples(), sfile.sampleRate() )
		ofilebase = 'funny1.%i.%i'%(r, w)
		for h in (1,2):
			loris.channelize(p, ref, h)
			# distilled version
			pd = p.copy()
			loris.distill( pd )
			ofilebased = ofilebase + '.d%i'%h
			loris.exportSdif( ofilebased + '.sdif', pd )
			synthesize( ofilebased + '.aiff', pd )
			pruneByLabel( pd, range(1,512) )
			loris.exportSpc( ofilebased + '.s.spc', pd, 60, 0 ) 
			loris.exportSpc( ofilebased + '.e.spc', pd, 60, 1 ) 

			# sifted version
			ps = p.copy()
			loris.sift( ps )
			zeros = loris.extractLabeled( ps, 0 )
			loris.distill( ps )
			ofilebases = ofilebase + '.s%i'%h
			loris.exportSdif( ofilebases + '.sdif', ps )
			synthesize( ofilebases + '.aiff', ps )
			pruneByLabel( ps, range(1,512) )
			loris.exportSpc( ofilebases + '.s.spc', ps, 60, 0 ) 
			loris.exportSpc( ofilebases + '.e.spc', ps, 60, 1 ) 


	source = 'funnyPeeple2.aiff'
	pref = loris.importSdif('funnyPeeple2.fund.sdif')
	ref = loris.createFreqReference( pref, 50, 500 )
	r = 100
	w = 400
	sfile = loris.AiffFile( source )
	print 'analyzing %s (%s)'%(source, time.ctime(time.time()))
	a = loris.Analyzer( r, w )
	p = a.analyze( sfile.samples(), sfile.sampleRate() )
	ofilebase = 'funny2.%i.%i'%(r, w)
	for h in (1,2):
		loris.channelize(p, ref, h)
		# distilled version
		pd = p.copy()
		loris.distill( pd )
		ofilebased = ofilebase + '.d%i'%h
		loris.exportSdif( ofilebased + '.sdif', pd )
		synthesize( ofilebased + '.aiff', pd )
		pruneByLabel( pd, range(1,512) )
		loris.exportSpc( ofilebased + '.s.spc', pd, 60, 0 ) 
		loris.exportSpc( ofilebased + '.e.spc', pd, 60, 1 ) 

		# sifted version
		ps = p.copy()
		loris.sift( ps )
		zeros = loris.extractLabeled( ps, 0 )
		loris.distill( ps )
		ofilebases = ofilebase + '.s%i'%h
		loris.exportSdif( ofilebases + '.sdif', ps )
		synthesize( ofilebases + '.aiff', ps )
		pruneByLabel( ps, range(1,512) )
		loris.exportSpc( ofilebases + '.s.spc', ps, 60, 0 ) 
		loris.exportSpc( ofilebases + '.e.spc', ps, 60, 1 ) 


if trial == 4:
	source = 'funnyPeeple1.aiff'
	pref = loris.importSpc('funnyPeeple1.fund.smooth.qharm')
	ref = loris.createFreqReference( pref, 50, 1000 )
	resolutions = ( 85, 155 )
	widths = ( 300, 400 )
	for r in resolutions:
		for w in widths:
			sfile = loris.AiffFile( source )
			print 'analyzing %s (%s)'%(source, time.ctime(time.time()))
			a = loris.Analyzer( r, w )
			p = a.analyze( sfile.samples(), sfile.sampleRate() )
			ofilebase = 'funny1.%i.%i'%(r, w)
			synthesize( ofilebase + '.aiff', p )
			loris.exportSdif( ofilebase + '.sdif', p )
			for h in (1,2):
				loris.channelize(p, ref, h)
				# distilled version
				pd = p.copy()
				loris.distill( pd )
				ofilebased = ofilebase + '.d%i'%h
				loris.exportSdif( ofilebased + '.sdif', pd )
				synthesize( ofilebased + '.aiff', pd )
				pruneByLabel( pd, range(1,512) )
				loris.exportSpc( ofilebased + '.s.spc', pd, 60, 0 ) 
				loris.exportSpc( ofilebased + '.e.spc', pd, 60, 1 ) 
	
				# sifted version
				ps = p.copy()
				loris.sift( ps )
				zeros = loris.extractLabeled( ps, 0 )
				loris.distill( ps )
				ofilebases = ofilebase + '.s%i'%h
				loris.exportSdif( ofilebases + '.sdif', ps )
				synthesize( ofilebases + '.aiff', ps )
				pruneByLabel( ps, range(1,512) )
				loris.exportSpc( ofilebases + '.s.spc', ps, 60, 0 ) 
				loris.exportSpc( ofilebases + '.e.spc', ps, 60, 1 ) 


	source = 'funnyPeeple2.aiff'
	pref = loris.importSpc('funnyPeeple2.fund.smooth.qharm')
	ref = loris.createFreqReference( pref, 50, 1000 )
	resolutions = ( 100, 170 )
	widths = ( 400, 500 )
	for r in resolutions:
		for w in widths:
			sfile = loris.AiffFile( source )
			print 'analyzing %s (%s)'%(source, time.ctime(time.time()))
			a = loris.Analyzer( r, w )
			p = a.analyze( sfile.samples(), sfile.sampleRate() )
			ofilebase = 'funny2.%i.%i'%(r, w)
			synthesize( ofilebase + '.aiff', p )
			loris.exportSdif( ofilebase + '.sdif', p )
			for h in (1,2):
				loris.channelize(p, ref, h)
				# distilled version
				pd = p.copy()
				loris.distill( pd )
				ofilebased = ofilebase + '.d%i'%h
				loris.exportSdif( ofilebased + '.sdif', pd )
				synthesize( ofilebased + '.aiff', pd )
				pruneByLabel( pd, range(1,512) )
				loris.exportSpc( ofilebased + '.s.spc', pd, 60, 0 ) 
				loris.exportSpc( ofilebased + '.e.spc', pd, 60, 1 ) 
		
				# sifted version
				ps = p.copy()
				loris.sift( ps )
				zeros = loris.extractLabeled( ps, 0 )
				loris.distill( ps )
				ofilebases = ofilebase + '.s%i'%h
				loris.exportSdif( ofilebases + '.sdif', ps )
				synthesize( ofilebases + '.aiff', ps )
				pruneByLabel( ps, range(1,512) )
				loris.exportSpc( ofilebases + '.s.spc', ps, 60, 0 ) 
				loris.exportSpc( ofilebases + '.e.spc', ps, 60, 1 ) 

if trial == 5:
	#source = 'funnyPeeple1.aiff'
	source = 'funnyLoud1.aiff'
	pref = loris.importSpc('funnyPeeple1.fund.smooth.qharm')
	ref = loris.createFreqReference( pref, 50, 1000 )
	resolutions = ( 85, 155 )
	widths = ( 300, 400 )
	for r in resolutions:
		for w in widths:
			sfile = loris.AiffFile( source )
			print 'analyzing %s (%s)'%(source, time.ctime(time.time()))
			a = loris.Analyzer( r, w )
			a.setBwRegionWidth( 0 ) # disable BW assoication
			p = a.analyze( sfile.samples(), sfile.sampleRate() )
			ofilebase = 'funny1.%i.%i'%(r, w)
			pcollate = p.copy()
			loris.distill( pcollate )
			synthesize( ofilebase + '.raw.aiff', pcollate )
			loris.exportSdif( ofilebase + '.raw.sdif', pcollate )
			pruneByLabel( pcollate, range(1,512) )
			loris.exportSpc( ofilebase + '.raw.s.spc', pcollate, 60, 0 )
			for h in (1,2):
				loris.channelize(p, ref, h)
				# distilled version
				pd = p.copy()
				loris.distill( pd )
				ofilebased = ofilebase + '.d%i'%h
				loris.exportSdif( ofilebased + '.sdif', pd )
				synthesize( ofilebased + '.aiff', pd )
				pruneByLabel( pd, range(1,512) )
				loris.exportSpc( ofilebased + '.s.spc', pd, 60, 0 ) 
				loris.exportSpc( ofilebased + '.e.spc', pd, 60, 1 ) 
	
				# sifted version
				ps = p.copy()
				loris.sift( ps )
				zeros = loris.extractLabeled( ps, 0 )
				loris.distill( ps )
				ofilebases = ofilebase + '.s%i'%h
				loris.exportSdif( ofilebases + '.sdif', ps )
				synthesize( ofilebases + '.aiff', ps )
				pruneByLabel( ps, range(1,512) )
				loris.exportSpc( ofilebases + '.s.spc', ps, 60, 0 ) 
				loris.exportSpc( ofilebases + '.e.spc', ps, 60, 1 ) 


	#source = 'funnyPeeple2.aiff'
	source = 'funnyLoud2.aiff'
	pref = loris.importSpc('funnyPeeple2.fund.smooth.qharm')
	ref = loris.createFreqReference( pref, 50, 1000 )
	resolutions = ( 100, 170 )
	widths = ( 400, 500 )
	for r in resolutions:
		for w in widths:
			sfile = loris.AiffFile( source )
			print 'analyzing %s (%s)'%(source, time.ctime(time.time()))
			a = loris.Analyzer( r, w )
			a.setBwRegionWidth( 0 ) # disable BW assoication
			p = a.analyze( sfile.samples(), sfile.sampleRate() )
			ofilebase = 'funny2.%i.%i'%(r, w)
			pcollate = p.copy()
			loris.distill( pcollate )
			synthesize( ofilebase + '.raw.aiff', pcollate )
			loris.exportSdif( ofilebase + '.raw.sdif', pcollate )
			pruneByLabel( pcollate, range(1,512) )
			loris.exportSpc( ofilebase + '.raw.s.spc', pcollate, 60, 0 )
			for h in (1,2):
				loris.channelize(p, ref, h)
				# distilled version
				pd = p.copy()
				loris.distill( pd )
				ofilebased = ofilebase + '.d%i'%h
				loris.exportSdif( ofilebased + '.sdif', pd )
				synthesize( ofilebased + '.aiff', pd )
				pruneByLabel( pd, range(1,512) )
				loris.exportSpc( ofilebased + '.s.spc', pd, 60, 0 ) 
				loris.exportSpc( ofilebased + '.e.spc', pd, 60, 1 ) 
		
				# sifted version
				ps = p.copy()
				loris.sift( ps )
				zeros = loris.extractLabeled( ps, 0 )
				loris.distill( ps )
				ofilebases = ofilebase + '.s%i'%h
				loris.exportSdif( ofilebases + '.sdif', ps )
				synthesize( ofilebases + '.aiff', ps )
				pruneByLabel( ps, range(1,512) )
				loris.exportSpc( ofilebases + '.s.spc', ps, 60, 0 ) 
				loris.exportSpc( ofilebases + '.e.spc', ps, 60, 1 ) 
				
if trial == 6:
	source = 'funnyLoud1.aiff'
	pref = loris.importSpc('funnyPeeple1.fund.qharm')
	ref = loris.createFreqReference( pref, 50, 1000 )
	r = 155
	w = 400
	sfile = loris.AiffFile( source )
	print 'analyzing %s (%s)'%(source, time.ctime(time.time()))
	a = loris.Analyzer( r, w )
	a.setBwRegionWidth( 0 ) # disable BW assoication
	p = a.analyze( sfile.samples(), sfile.sampleRate(), ref )
	ofilebase = 'funny1.%i.%i'%(r, w)
	pcollate = p.copy()
	loris.distill( pcollate )
	synthesize( ofilebase + '.raw.aiff', pcollate )
	loris.exportSdif( ofilebase + '.raw.sdif', pcollate )
	pruneByLabel( pcollate, range(1,512) )
	loris.exportSpc( ofilebase + '.raw.s.spc', pcollate, 60, 0 )
	h = 2
	loris.channelize(p, ref, h)
	ps = p.copy()
	loris.sift( ps )
	zeros = loris.extractLabeled( ps, 0 )
	loris.distill( ps )
	ofilebases = ofilebase + '.s%i'%h
	loris.exportSdif( ofilebases + '.sdif', ps )
	synthesize( ofilebases + '.aiff', ps )
	pruneByLabel( ps, range(1,512) )
	loris.exportSpc( ofilebases + '.s.spc', ps, 60, 0 ) 
	loris.exportSpc( ofilebases + '.e.spc', ps, 60, 1 ) 

	source = 'funnyLoud2.aiff'
	pref = loris.importSpc('funnyPeeple2.fund.qharm')
	ref = loris.createFreqReference( pref, 50, 1000 )
	r = 170
	w = 400
	sfile = loris.AiffFile( source )
	print 'analyzing %s (%s)'%(source, time.ctime(time.time()))
	a = loris.Analyzer( r, w )
	a.setBwRegionWidth( 0 ) # disable BW assoication
	p = a.analyze( sfile.samples(), sfile.sampleRate(), ref )
	ofilebase = 'funny2.%i.%i'%(r, w)
	pcollate = p.copy()
	loris.distill( pcollate )
	synthesize( ofilebase + '.raw.aiff', pcollate )
	loris.exportSdif( ofilebase + '.raw.sdif', pcollate )
	pruneByLabel( pcollate, range(1,512) )
	loris.exportSpc( ofilebase + '.raw.s.spc', pcollate, 60, 0 )
	h = 2
	loris.channelize(p, ref, h)
	ps = p.copy()
	loris.sift( ps )
	zeros = loris.extractLabeled( ps, 0 )
	loris.distill( ps )
	ofilebases = ofilebase + '.s%i'%h
	loris.exportSdif( ofilebases + '.sdif', ps )
	synthesize( ofilebases + '.aiff', ps )
	pruneByLabel( ps, range(1,512) )
	loris.exportSpc( ofilebases + '.s.spc', ps, 60, 0 ) 
	loris.exportSpc( ofilebases + '.e.spc', ps, 60, 1 ) 

if trial == 7:
	source = 'funnyLoud1.aiff'
	pref = loris.importSpc('funnyPeeple1.fund.qharm')
	ref = loris.createFreqReference( pref, 50, 1000 )
	r = 155
	w = 400
	sfile = loris.AiffFile( source )
	print 'analyzing %s (%s)'%(source, time.ctime(time.time()))
	a = loris.Analyzer( r, w )
	a.setBwRegionWidth( 0 ) # disable BW assoication
	a.setHopTime( a.hopTime() * 0.5 )
	# a.setCropTime( a.cropTime() * 0.5 )
	p = a.analyze( sfile.samples(), sfile.sampleRate(), ref )
	ofilebase = 'funny1.hires.%i.%i'%(r, w)
	pcollate = loris.PartialList( p ) 
	loris.collate( pcollate )
	loris.exportAiff( ofilebase + '.raw.aiff', loris.synthesize( pcollate ) )
	loris.exportSdif( ofilebase + '.raw.sdif', pcollate )
# 	pruneByLabel( pcollate, range(1,512) )
# 	loris.exportSpc( ofilebase + '.raw.s.spc', pcollate, 60, 0 )
	h = 2
	loris.channelize(p, ref, h)
	ps = loris.PartialList( p )
	loris.sift( ps )
	loris.removeLabeled( ps, 0 )
	loris.distill( ps )
	ofilebases = ofilebase + '.s%i'%h
	loris.exportSdif( ofilebases + '.sdif', ps )
	loris.exportAiff( ofilebases + '.aiff', loris.synthesize( ps ) )
#	pruneByLabel( ps, range(1,512) )
	loris.exportSpc( ofilebases + '.s.spc', ps, 60, 0 ) 
	loris.exportSpc( ofilebases + '.e.spc', ps, 60, 1 ) 

	source = 'funnyLoud2.aiff'
	pref = loris.importSpc('funnyPeeple2.fund.qharm')
	ref = loris.createFreqReference( pref, 50, 1000 )
	r = 170
	w = 400
	sfile = loris.AiffFile( source )
	print 'analyzing %s (%s)'%(source, time.ctime(time.time()))
	a = loris.Analyzer( r, w )
	a.setBwRegionWidth( 0 ) # disable BW assoication
	a.setHopTime( a.hopTime() * 0.5 )
	#a.setCropTime( a.cropTime() * 0.5 )
	p = a.analyze( sfile.samples(), sfile.sampleRate(), ref )
	ofilebase = 'funny2.hires.%i.%i'%(r, w)
	pcollate = loris.PartialList( p )
	loris.collate( pcollate )
	loris.exportAiff( ofilebase + '.raw.aiff', loris.synthesize( pcollate ) )
	loris.exportSdif( ofilebase + '.raw.sdif', pcollate )
# 	pruneByLabel( pcollate, range(1,512) )
# 	loris.exportSpc( ofilebase + '.raw.s.spc', pcollate, 60, 0 )
	h = 2
	loris.channelize(p, ref, h)
	ps = loris.PartialList( p )
	loris.sift( ps )
	loris.removeLabeled( ps, 0 )
	loris.distill( ps )
	ofilebases = ofilebase + '.s%i'%h
	loris.exportSdif( ofilebases + '.sdif', ps )
	loris.exportAiff( ofilebases + '.aiff', loris.synthesize( ps ) )
#	pruneByLabel( ps, range(1,512) )
	loris.exportSpc( ofilebases + '.s.spc', ps, 60, 0 ) 
	loris.exportSpc( ofilebases + '.e.spc', ps, 60, 1 ) 


if trial == 8:
	source = 'funnyLoud1.aiff'
	r = 155
	w = 400
	sfile = loris.AiffFile( source )
	print 'analyzing %s (%s)'%(source, time.ctime(time.time()))
	a = loris.Analyzer( r, w )
	a.setBwRegionWidth( 0 ) # disable BW assoication
	p = a.analyze( sfile.samples(), sfile.sampleRate() )
	ref = loris.createF0Estimate( p, 50, 1000, 0.01 )
	print 'analyzing %s with tracking (%s)'%(source, time.ctime(time.time()))
	p = a.analyze( sfile.samples(), sfile.sampleRate(), ref )
	ofilebase = 'funny1.%i.%i'%(r, w)
	pcollate = loris.PartialList( p ) 
	loris.collate( pcollate )
	loris.exportAiff( ofilebase + '.raw.aiff', loris.synthesize( pcollate ) )
	loris.exportSdif( ofilebase + '.raw.sdif', pcollate )
	h = 2
	loris.channelize(p, ref, h)
	loris.sift( p )
	loris.removeLabeled( p, 0 )
	loris.distill( p )
	ofilebases = ofilebase + '.s%i'%h
	loris.exportSdif( ofilebases + '.sdif', p )
	loris.exportAiff( ofilebases + '.aiff', loris.synthesize( p ) )
	loris.exportSpc( ofilebases + '.s.spc', p, 60, 0 ) 
	loris.exportSpc( ofilebases + '.e.spc', p, 60, 1 ) 

	source = 'funnyLoud2.aiff'
	r = 170
	w = 400
	sfile = loris.AiffFile( source )
	print 'analyzing %s (%s)'%(source, time.ctime(time.time()))
	a = loris.Analyzer( r, w )
	a.setBwRegionWidth( 0 ) # disable BW assoication
	p = a.analyze( sfile.samples(), sfile.sampleRate() )
	ref = loris.createF0Estimate( p, 50, 1000, 0.01 )
	print 'analyzing %s with tracking (%s)'%(source, time.ctime(time.time()))
	p = a.analyze( sfile.samples(), sfile.sampleRate(), ref )
	ofilebase = 'funny2.%i.%i'%(r, w)
	pcollate = loris.PartialList( p )
	loris.collate( pcollate )
	loris.exportAiff( ofilebase + '.raw.aiff', loris.synthesize( pcollate ) )
	loris.exportSdif( ofilebase + '.raw.sdif', pcollate )
	h = 2
	loris.channelize(p, ref, h)
	loris.sift( p )
	loris.removeLabeled( p, 0 )
	loris.distill( p )
	ofilebases = ofilebase + '.s%i'%h
	loris.exportSdif( ofilebases + '.sdif', p )
	loris.exportAiff( ofilebases + '.aiff', loris.synthesize( p ) )
	loris.exportSpc( ofilebases + '.s.spc', p, 60, 0 ) 
	loris.exportSpc( ofilebases + '.e.spc', p, 60, 1 ) 


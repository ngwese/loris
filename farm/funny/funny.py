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

Last updated: 10 June 2002 by Kelly Fitz
"""
print __doc__

import loris, time
from trials import *

# use this trial counter to skip over
# eariler trials
trial = 4

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
"""
shaku.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the shakuhachi sample that was included in the 
ICMC 2000 bakeoff. It is a 13 second phrase that sounds like it was
recorded in a steam tunnel.

The parameters we liked once were 80 Hz resolution and 300 Hz
analysis window.

trial1:
	- sounds okay, as before, the articulation is exaggerated for some reason
	- why should this need 80 Hz resolution? The fundamental is over 400 Hz!
	
trial 2:
	- widest windows (600 Hz) seem to give the most natural-sounding noise (the
	extermely high noise floor in this sound adds a lot of noisiness to the 
	analysis and affects the perception of naturalness).
	- the noisiness also affects the quality of the distillations, particularly
	the big breath at the end
	- the narrow resolution distillation (80 Hz) sounds better than wider ones
	(240 Hz) but maybe not closer to the horrible original, which has some 
	pretty severe noise problems
	- should try with the tracking analyzer, no compelling reason to go with
	2 partials per harmonic though.
	
trial 3:
	- bandwidth enhancement is too crunchy if resolution is big, in fact, its 
	always pretty crunchy, try sifting.
	
trial 4:
	- breath at end still sounds bad when sifted or distilled
	- sifting deinitely improves the reconstructions
	- 160 resolution has a clunk near at the first dynamic peak, others are OK
	- wider window (600) is still a bit smoother than narrower (450)

Last updated: 13 May 2004 by Kelly Fitz
"""
print __doc__

import loris, time
print "using Loris version", loris.version()


# use this trial counter to skip over
# eariler trials
trial = 4
print "running trial number", trial, time.ctime(time.time())


src = 'shaku'
f = loris.AiffFile(src+'.aiff')
samples = f.samples()
rate = f.sampleRate()

if trial == 1:
	res = 80
	mlw = 300
	a = loris.Analyzer( res, mlw )
	p = a.analyze( samples, rate )
	ofile = 'shaku.%i.%i.raw'%(res, mlw)
	# collate
	loris.distill( p )
	# export
	loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
	loris.exportSpc( ofile + '.s.spc', p, 60, 0 ) 
	loris.exportSpc( ofile + '.e.spc', p, 60, 1 ) 

if trial == 2:
	resolutions = ( 80, 160, 240, 320 )
	pref = loris.importSpc('shaku.fund.qharm')
	ref = loris.createFreqReference( pref, 50, 1000 )
	for res in resolutions:
		for mlw in ( 300, 450, 600 ):
			a = loris.Analyzer( res, mlw )
			p = a.analyze( samples, rate )
			d1 = p.copy()
			d2 = p.copy()
			
			# export raw
			loris.distill( p )
			ofile = 'shaku.%i.%i.raw'%(res, mlw)
			loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
			# prune before Spc export
			iter = p.begin()
			while not iter.equals( p.end() ):
				if iter.partial().label() > 511:
					next = iter.next()
					p.erase( iter )
					iter = next
				else:
					iter = iter.next()
			loris.exportSpc( ofile + '.s.spc', p, 68, 0 ) 
			loris.exportSpc( ofile + '.e.spc', p, 68, 1 ) 
		
			# distill one partial per harmonic
			loris.channelize( d1, ref, 1 )
			loris.distill( d1 )
			# export
			ofile = 'shaku.%i.%i.d1'%(res, mlw)
			loris.exportAiff( ofile + '.aiff', loris.synthesize( d1, rate ), rate, 1, 16 )
			# prune before Spc export
			iter = d1.begin()
			while not iter.equals( d1.end() ):
				if iter.partial().label() > 511:
					next = iter.next()
					d1.erase( iter )
					iter = next
				else:
					iter = iter.next()
			loris.exportSpc( ofile + '.s.spc', d1, 68, 0 ) 
			loris.exportSpc( ofile + '.e.spc', d1, 68, 1 ) 
		
			# distill two partials per harmonic
			loris.channelize( d2, ref, 2 )
			loris.distill( d2 )
			# export
			ofile = 'shaku.%i.%i.d2'%(res, mlw)
			loris.exportAiff( ofile + '.aiff', loris.synthesize( d2, rate ), rate, 1, 16 )
			# prune before Spc export
			iter = d2.begin()
			while not iter.equals( d2.end() ):
				if iter.partial().label() > 511:
					next = iter.next()
					d2.erase( iter )
					iter = next
				else:
					iter = iter.next()
			loris.exportSpc( ofile + '.s.spc', d2, 68, 0 ) 
			loris.exportSpc( ofile + '.e.spc', d2, 68, 1 ) 
		
if trial == 3:
	resolutions = ( 160, 240, 320 )
	pref = loris.importSpc('shaku.fund.qharm')
	ref = loris.createFreqReference( pref, 50, 1000 )
	for res in resolutions:
		for mlw in ( 450, 600 ):
			a = loris.Analyzer( res, mlw )
			p = a.analyze( samples, rate, ref )
			d1 = p.copy()
			
			# export raw
			loris.distill( p )
			ofile = 'shaku.%i.%i.raw'%(res, mlw)
			fout = loris.AiffFile( p, rate )
			fout.write( ofile + '.aiff' )
			# prune before Spc export
			for partial in p:
				if partial.label() > 511:
					partial.setLabel( -1 )
			dump = loris.extractLabeled( p, -1 )
			loris.exportSpc( ofile + '.s.spc', p, 68, 0 ) 
			loris.exportSpc( ofile + '.e.spc', p, 68, 1 ) 
		
			# distill one partial per harmonic
			p = d1
			loris.channelize( p, ref, 1 )
			loris.distill( p )
			# export
			ofile = 'shaku.%i.%i.d1'%(res, mlw)
			fout = loris.AiffFile( p, rate )
			fout.write( ofile + '.aiff' )
			# prune before Spc export
			for partial in p:
				if partial.label() > 511:
					partial.setLabel( -1 )
			dump = loris.extractLabeled( p, -1 )
			loris.exportSpc( ofile + '.s.spc', p, 68, 0 ) 
			loris.exportSpc( ofile + '.e.spc', p, 68, 1 ) 
		
if trial == 4:
	resolutions = ( 160, 240, 320 )
	pref = loris.importSpc('shaku.fund.qharm')
	ref = loris.createFreqReference( pref, 50, 1000 )
	for res in resolutions:
		for mlw in ( 450, 600 ):
			a = loris.Analyzer( res, mlw )
			p = a.analyze( samples, rate, ref )
			d1 = p.copy()
			
			# export raw
			loris.distill( p )
			ofile = 'shaku.%i.%i.raw'%(res, mlw)
			fout = loris.AiffFile( p, rate )
			fout.write( ofile + '.aiff' )
			# prune before Spc export
			for partial in p:
				if partial.label() > 511:
					partial.setLabel( -1 )
			dump = loris.extractLabeled( p, -1 )
			loris.exportSpc( ofile + '.s.spc', p, 68, 0 ) 
			loris.exportSpc( ofile + '.e.spc', p, 68, 1 ) 
		
			# distill one partial per harmonic
			p = d1
			loris.channelize( p, ref, 1 )
			loris.sift( p )
			loris.distill( p )
			# export
			ofile = 'shaku.%i.%i.s1'%(res, mlw)
			fout = loris.AiffFile( p, rate )
			fout.write( ofile + '.aiff' )
			# prune before Spc export
			for partial in p:
				if partial.label() > 511:
					partial.setLabel( -1 )
			dump = loris.extractLabeled( p, -1 )
			loris.exportSpc( ofile + '.s.spc', p, 68, 0 ) 
			loris.exportSpc( ofile + '.e.spc', p, 68, 1 ) 
		

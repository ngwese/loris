"""
tbnflutter.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the flutter-tongued trombone tone that was
part of the ICMC 2000 bake-off, called conant-trombone-flutter.aiff.
Parameters we used then were resolution 150 Hz, freq floor 100 Hzm and
window width 600 Hz.

A fundamental frequency contour extracted in Kyma is in
tbnflutter.fund.sdif.

Note boudaries are near .75 s, 1.25 s, and 2.1 s, and the duration is
2.4 s. Some fundamental frequencies are 291 Hz at 1 s, 280.5 Hz at
1.5 s, and 269 Hz at 2.0 s.

notes from trial 1:
	- cannot use 150 Hz window at all, and 300 Hz is clearly worse
	than 600 Hz window.
	- cannot discern any difference between 100 and 150 Hz resolution,
	with or without distilling
	- distillation works well
	
	- the 300 Hz windows smooshes out the flutter, and then the distillations
	are noisy
	- 100 Hz is maybe a little better in the first note
	- both are a little weak on the last blast
	
notes from trial 2:
	- flutter seems to be best-preerved by 600 Hz window
	- 200 resolution is pretty hard to distinguish from 100 or 150
	- 200 Hz has a crunch in distillation, probably due to mistracking
	of the third harmonic at the very beginning of the sound
	- distillations are clearly not as good as raw reconstructions, but 
	100 and 150 are still pretty good.
	
notes from trial 3:
	- distilling at 2 partials per harmonic eliminates pretty much
	all the artifacts
	- 150 is slightly preferable

Last updated: 27 May 2003 by Kelly Fitz
"""

print __doc__

import loris, time
#from trials import *

# use this trial counter to skip over
# eariler trials
trial = 4

print "running trial number", trial, time.ctime(time.time())

source = 'tbnflutter.aiff'
file = loris.AiffFile( source )
samples = file.samples()
rate = file.sampleRate()

# if trial == 1:
# 	resolutions = ( 100, 150 )
# 	widths = ( 150, 300, 600 )
# 	ref = loris.createFreqReference( loris.importSdif( 'tbnflutter.fund.sdif' ), 0, 1000 )
# 	for r in resolutions:
# 		for w in widths:
# 			p = analyze( source, r, w )
# 			ofilebase = '%s.%i.%i'%(source[:-5], r, w)
# 			synthesize( ofilebase + '.aiff', p )
# 			loris.exportSdif( ofilebase + '.sdif', p )
# 			loris.channelize( p, ref, 1 )
# 			loris.distill( p )
# 			ofilebase = '%s.%i.%i.d'%(source[:-5], r, w)
# 			synthesize( ofilebase + '.aiff', p )
# 			loris.exportSdif( ofilebase + '.sdif', p )
# 			loris.exportSpc( ofilebase + '.s.spc', p, 62, 0 )
# 			loris.exportSpc( ofilebase + '.e.spc', p, 62, 1 )
# 

if trial == 2:
	resolutions = ( 100, 150, 200 )
	widths = ( 450, 600 )
	ref = loris.createFreqReference( loris.importSdif( 'tbnflutter.fund.sdif' ), 0, 1000 )
	for r in resolutions:
		for w in widths:
			a = loris.Analyzer( r, w )
			# turn off BW association for now
			# a.setBwRegionWidth( 0 )
			p = a.analyze( samples, rate )
			# export raw
			ofile = 'tbnflutter.%i.%i'%(r, w)
			loris.exportAiff( ofile + '.raw.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
			# distill
			loris.channelize( p, ref, 1 )
			loris.distill( p )
			# export
			loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
			loris.exportSpc( ofile + '.s.spc', p, 60, 0 ) 
			loris.exportSpc( ofile + '.e.spc', p, 60, 1 ) 

if trial == 3:
	resolutions = ( 100, 150, 200 )
	w = 600
	ref = loris.createFreqReference( loris.importSdif( 'tbnflutter.fund.sdif' ), 0, 1000 )
	for r in resolutions:
		a = loris.Analyzer( r, w )
		# turn off BW association for now
		# a.setBwRegionWidth( 0 )
		p = a.analyze( samples, rate )
		# export raw
		ofile = 'tbnflutter.%i.%i'%(r, w)
		loris.exportAiff( ofile + '.raw.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
		p1 = p
		p2 = p1.copy()
		
		# distill one partial per harmonic
		loris.channelize( p1, ref, 1 )
		loris.distill( p1 )
		# export
		ofile1 = ofile + '.d1'
		loris.exportAiff( ofile1 + '.aiff', loris.synthesize( p1, rate ), rate, 1, 16 )
		loris.exportSpc( ofile1 + '.s.spc', p1, 60, 0 ) 
		loris.exportSpc( ofile1 + '.e.spc', p1, 60, 1 ) 
				
		# distill two partials per harmonic
		loris.channelize( p2, ref, 2 )
		loris.distill( p2 )
		# export
		ofile2 = ofile + '.d2'
		loris.exportAiff( ofile2 + '.aiff', loris.synthesize( p2, rate ), rate, 1, 16 )
		loris.exportSpc( ofile2 + '.s.spc', p2, 60, 0 ) 
		loris.exportSpc( ofile2 + '.e.spc', p2, 60, 1 ) 

if trial == 4:
	resolutions = ( 100, 150 )
	w = 600
	ref = loris.createFreqReference( loris.importSdif( 'tbnflutter.fund.sdif' ), 0, 1000 )
	for r in resolutions:
		a = loris.Analyzer( r, w )
		p = a.analyze( samples, rate )
		# export raw
		ofile = 'tbnflutter.%i.%i'%(r, w)
		loris.exportAiff( ofile + '.raw.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
		p1 = p
		p2 = p1.copy()
		
		# distill one partial per harmonic
		loris.channelize( p1, ref, 1 )
		loris.distill( p1 )
		# export
		ofile1 = ofile + '.d1'
		loris.exportAiff( ofile1 + '.aiff', loris.synthesize( p1, rate ), rate, 1, 16 )
		loris.exportSpc( ofile1 + '.s.spc', p1, 60, 0 ) 
		loris.exportSpc( ofile1 + '.e.spc', p1, 60, 1 ) 
				
		# distill two partials per harmonic
		loris.channelize( p2, ref, 2 )
		loris.distill( p2 )
		# export
		ofile2 = ofile + '.d2'
		loris.exportAiff( ofile2 + '.aiff', loris.synthesize( p2, rate ), rate, 1, 16 )
		loris.exportSpc( ofile2 + '.s.spc', p2, 60, 0 ) 
		loris.exportSpc( ofile2 + '.e.spc', p2, 60, 1 ) 


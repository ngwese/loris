"""
derbari.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the raga singing sample that was included in the 
ICMC 2000 bakeoff and called then "shafqat-derbari."

More recently, I have used this sound in my demo at ICMC 2002, adn the 
parameters I liked then were 180 Hz resolution and 350 Hz window. I tried
sifting and distilling, and I think sifting (removing zeros) worked better.

notes from trial 1:
	- none of these sounds particularly realistic, same-difference tests fail
	- turning off BW association makes sift and distill sound identical
	- sounds like a lot of low-frequency oomph is lost from the onset
	
notes from trial 2: 
	- got back some of that oomph, sounds much better, but still not like the
	original

Last updated: 27 May 2003 by Kelly Fitz
"""
print __doc__

import loris, time
print "using Loris version", loris.version()


# use this trial counter to skip over
# eariler trials
trial = 2
print "running trial number", trial, time.ctime(time.time())


src = 'derbari'
f = loris.AiffFile(src+'.aiff')
samples = f.samples()
rate = f.sampleRate()

if trial == 1:
	res = 180
	mlw = 350
	a = loris.Analyzer( res, mlw )
	# turn off BW association for now
	a.setBwRegionWidth( 0 )
	p = a.analyze( samples, rate )
	praw = p
	ref = loris.createFreqReference( p, 150, 300, 100 )
	pdist = p.copy()
	psift = p.copy()
	loris.channelize( pdist, ref, 1 )
	loris.channelize( psift, ref, 1 )
	
	# collate
	loris.distill( praw )
	ofile = 'derbari.%i.%i.raw'%(res, mlw)
	loris.exportAiff( ofile + '.aiff', loris.synthesize( praw, rate ), rate, 1, 16 )
	loris.exportSpc( ofile + '.s.spc', praw, 58, 0 ) 
	loris.exportSpc( ofile + '.e.spc', praw, 58, 1 ) 

	# distill
	loris.distill( pdist )
	ofile = 'derbari.%i.%i.d1'%(res, mlw)
	loris.exportAiff( ofile + '.aiff', loris.synthesize( pdist, rate ), rate, 1, 16 )
	loris.exportSpc( ofile + '.s.spc', pdist, 58, 0 ) 
	loris.exportSpc( ofile + '.e.spc', pdist, 58, 1 ) 

	# sift
	loris.sift( psift )
	zeros = loris.extractLabeled( psift, 0 )
	loris.distill( psift )
	ofile = 'derbari.%i.%i.s1'%(res, mlw)
	loris.exportAiff( ofile + '.aiff', loris.synthesize( psift, rate ), rate, 1, 16 )
	loris.exportSpc( ofile + '.s.spc', psift, 58, 0 ) 
	loris.exportSpc( ofile + '.e.spc', psift, 58, 1 ) 

if trial == 2:
	res = 180
	mlw = 350
	a = loris.Analyzer( res, mlw )
	# turn off BW association for now
	a.setBwRegionWidth( 0 )
	a.setFreqFloor( 50 )
	p = a.analyze( samples, rate )
	praw = p
	ref = loris.createFreqReference( p, 150, 300, 100 )
	pdist = p.copy()
	psift = p.copy()
	loris.channelize( pdist, ref, 1 )
	loris.channelize( psift, ref, 1 )
	
	# collate
	loris.distill( praw )
	ofile = 'derbari.%i.%i.lo.raw'%(res, mlw)
	loris.exportAiff( ofile + '.aiff', loris.synthesize( praw, rate ), rate, 1, 16 )
	loris.exportSpc( ofile + '.s.spc', praw, 58, 0 ) 
	loris.exportSpc( ofile + '.e.spc', praw, 58, 1 ) 

	# distill
	loris.distill( pdist )
	ofile = 'derbari.%i.%i.lo.d1'%(res, mlw)
	loris.exportAiff( ofile + '.aiff', loris.synthesize( pdist, rate ), rate, 1, 16 )
	loris.exportSpc( ofile + '.s.spc', pdist, 58, 0 ) 
	loris.exportSpc( ofile + '.e.spc', pdist, 58, 1 ) 

	# sift
	loris.sift( psift )
	zeros = loris.extractLabeled( psift, 0 )
	loris.distill( psift )
	ofile = 'derbari.%i.%i.lo.s1'%(res, mlw)
	loris.exportAiff( ofile + '.aiff', loris.synthesize( psift, rate ), rate, 1, 16 )
	loris.exportSpc( ofile + '.s.spc', psift, 58, 0 ) 
	loris.exportSpc( ofile + '.e.spc', psift, 58, 1 ) 


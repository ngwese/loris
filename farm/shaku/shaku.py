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
	- sounds about the same as the best we did in CU, maybe even a little 
	better?


Last updated: 27 May 2003 by Kelly Fitz
"""
print __doc__

import loris, time
print "using Loris version", loris.version()


# use this trial counter to skip over
# eariler trials
trial = 1
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

"""
french.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the french speech excerpt used in the
dissertation experiments. It is taken from a radio broadcast
included on the Pierre Schaeffer collection. The speaker has
a very low (male) voice, which was difficult to reproduce
without a slightly reverberant quality.

The parameters we once liked for this are resolution 60 and
window width 200 Hz.

notes from trial 1:
	- 80 Hz is unusable
	- 60,200 is a little noisy, as is 40,200
	- 60,300 sounds a little hoarse, as does 40,300
	- 60,100 is unusable
	
notes from trial 2:
	- turned off BW association
	- all of these sound kind of hoarse, 250 are also a little crunchy (?),
	150 windows are better.

Last updated: 27 May 2003 by Kelly Fitz
"""
print __doc__

import loris, time
#from trials import *

# use this trial counter to skip over
# eariler trials
trial = 2

print "running trial number", trial, time.ctime(time.time())

source = 'french.aiff'
file = loris.AiffFile( source )
samples = file.samples()
rate = file.sampleRate()


# if trial == 1:
# 	resolutions = (40,60,80)
# 	widths = ( 100,200,300 )
# 	for r in resolutions:
# 		for w in widths:
# 			p = analyze( source, r, w )
# 			ofile = 'french.%i.%i.aiff'%(r, w)
# 			synthesize( ofile, p )
# 
if trial == 2:
	resolutions = ( 40,60 )
	widths = ( 150,250 )
	for r in resolutions:
		for w in widths:
			a = loris.Analyzer( r, w )
			# turn off BW association for now
			a.setBwRegionWidth( 0 )
			p = a.analyze( samples, rate )
			ofile = 'french.%i.%i.raw'%(r, w)
			# collate
			loris.distill( p )
			# export
			loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
			loris.exportSpc( ofile + '.s.spc', p, 60, 0 ) 
			loris.exportSpc( ofile + '.e.spc', p, 60, 1 ) 

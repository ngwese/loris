"""
density.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the very reverberant flute solo excerpt 
from Denisty 21.5 (Varese). There are eight notes, some long,
some short, all very noisy and reverberant. This sound was used
to test the frequency tracking analysis developed between releases
1.0.2 and 1.0.3, but because of all the reverberation, frequency
tracking is never going to give very good results, because it will
always chop off the reverb decays. We once liked to use 150 Hz
resolution, and a 200 Hz window, but more recently, it seems that
maybe 100 and 300 were better parameters.

A fundamental frequency contour extracted in Kyma is in
density.fund.qharm (an spc file).

Last updated: 26 July 05 by Kelly Fitz
"""

print __doc__

import loris, time
print "using Loris version", loris.version()

# use this trial counter to skip over
# eariler trials
trial = 1

print "running trial number", trial, time.ctime(time.time())

source = 'density.aiff'
file = loris.AiffFile( source )
samples = file.samples()
rate = file.sampleRate()

if trial == 1:
	resolutions = ( 100, 150 )
	widths = ( 200, 300 )
	for r in resolutions:
		for w in widths:
			a = loris.Analyzer( r, w )
			# turn off BW association for now
			# a.setBwRegionWidth( 0 )
			p = a.analyze( samples, rate )
			# export raw
			ofile = 'density.%i.%i.raw'%(r, w)
			loris.collate( p )
			# export
			fout = loris.AiffFile( p, rate )
			fout.write( ofile + '.aiff' )
			# remove any Partials labeled greater than 512
			pruneMe = -1
			for part in p:
				if part.label() > 512:
					part.setLabel( pruneMe )
			loris.removeLabeled( p, pruneMe )
			loris.exportSpc( ofile + '.s.spc', p, 60, 0 ) 
			loris.exportSpc( ofile + '.e.spc', p, 60, 1 ) 

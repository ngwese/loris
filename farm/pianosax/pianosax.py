"""
pianosax.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the piano/soprano sax duet that I used in 
my dissertation work. I also used this sound to test the frequency 
tracking analysis, developed after release 1.0.2, but didn't work long 
enough at it to make it sound good. We once liked to configure the
analyzer with resolution and window width at 300Hz, but then needed
to lower the frequency floor to about 90 Hz. For testing the frequency
tracking analysis, I used resolution 150 Hz (or mayebe 90?), and 
window width 300 Hz.

A fundamental frequency envelope traced in Kyma is in 
pianosax.fund.qharm.

Last updated: 4 June 2003 by Kelly Fitz
"""

print __doc__

import loris, time
print "using Loris version", loris.version()

# use this trial counter to skip over
# eariler trials
trial = 1

print "running trial number", trial, time.ctime(time.time())

source = 'pianosax.aiff'
file = loris.AiffFile( source )
samples = file.samples()
rate = file.sampleRate()

if trial == 1:
	resolutions = ( 90, 150 )
	widths = ( 300, )
	for r in resolutions:
		for w in widths:
			a = loris.Analyzer( r, w )
			# turn off BW association for now
			# a.setBwRegionWidth( 0 )
			p = a.analyze( samples, rate )
			# export raw
			ofile = 'pianosax.%i.%i.raw'%(r, w)
			loris.distill( p )
			# export
			loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 24 )
			# prune before Spc export
			iter = p.begin()
			while not iter.equals( p.end() ):
				next = iter.next()
				if iter.partial().label() > 511:					
					p.erase( iter )
				iter = next
			loris.exportSpc( ofile + '.s.spc', p, 60, 0 ) 
			loris.exportSpc( ofile + '.e.spc', p, 60, 1 ) 

"""
bongo.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the bongo drum roll that was used in my
dissertation work, and was part of the ICMC 2000 bakeoff. A the 
time of the bakeoff, we liked to use 300 Hz resolution, a 800 Hz
window, and frequency floor at 200 Hz. 

trial 1:
	- 400 Hz resolution just sounds wrong, almost like the wrong pitch
	- 200 and 300 Hz resolutions sound very similar
	- different windows are pretty hard to distinguish too

trial 2: 
	- frequency floor at 200 is no different from floor at 300 hz
	- 500 Hz window isn't quite right either

Last updated: 26 july 2005 by Kelly Fitz
"""

print __doc__

import loris, time
print "using Loris version", loris.version()

# use this trial counter to skip over
# eariler trials
trial = 2

print "running trial number", trial, time.ctime(time.time())

source = 'bongoroll.aiff'
file = loris.AiffFile( source )
samples = file.samples()
rate = file.sampleRate()

if trial == 1:
	resolutions = ( 200, 300, 400 )
	widths = ( 500, 650, 800 )
	for r in resolutions:
		for w in widths:
			a = loris.Analyzer( r, w )
			# turn off BW association for now
			# a.setBwRegionWidth( 0 )
			a.setFreqFloor( 200 )
			p = a.analyze( samples, rate )
			# export raw
			ofile = 'bongo.%i.%i.raw'%(r, w)
			loris.collate( p )
			# export
			loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 24 )

if trial == 2:
	resolutions = ( 200, 300 )
	widths = ( 650, 800 )
	for r in resolutions:
		for w in widths:
			a = loris.Analyzer( r, w )
			# turn off BW association for now
			# a.setBwRegionWidth( 0 )
			# a.setFreqFloor( 200 )
			p = a.analyze( samples, rate )
			# export raw
			ofile = 'bongo.%i.%i.raw'%(r, w)
			loris.collate( p )
			# export
			loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 16 )
			loris.exportSdif( ofile + '.sdif', p  )
			# prune before Spc export
			for part in p:
				if part.label() > 511:					
					part.setLabel( -1 )
			loris.removeLabeled( p, -1 )
			loris.exportSpc( ofile + '.s.spc', p, 60, 0 ) 
			loris.exportSpc( ofile + '.e.spc', p, 60, 1 ) 

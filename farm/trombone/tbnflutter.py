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

Last updated: 28 June 2002 by Kelly Fitz
"""

print __doc__

import loris, time
from trials import *

# use this trial counter to skip over
# eariler trials
trial = 1

print "running trial number", trial, time.ctime(time.time())

source = 'tbnflutter.aiff'

if trial == 1:
	resolutions = ( 100, 150 )
	widths = ( 150, 300, 600 )
	ref = loris.createFreqReference( loris.importSdif( 'tbnflutter.fund.sdif' ), 0, 1000 )
	for r in resolutions:
		for w in widths:
			p = analyze( source, r, w )
			ofilebase = '%s.%i.%i'%(source[:-5], r, w)
			synthesize( ofilebase + '.aiff', p )
			loris.exportSdif( ofilebase + '.sdif', p )
			loris.channelize( p, ref, 1 )
			loris.distill( p )
			ofilebase = '%s.%i.%i.d'%(source[:-5], r, w)
			synthesize( ofilebase + '.aiff', p )
			loris.exportSdif( ofilebase + '.sdif', p )
			loris.exportSpc( ofilebase + '.s.spc', p, 62, 0 )
			loris.exportSpc( ofilebase + '.e.spc', p, 62, 1 )

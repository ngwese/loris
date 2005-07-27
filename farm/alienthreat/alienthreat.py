"""
alienthreat.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the "alien threat" sample of Kurt Hebel's
voice Lip and I used to compare our analysis/synthesis with the 
analysis/synthesis tools in Kyma. This was the subject of my visit
to CU in March 2003.

The parameters we arrived at were those decribed by trial 1. Well,
actually, they are close, but I seem not to have made a note of the
exact parameters. Sigh.

trial1:
	- sounds about the same as the best we did in CU, maybe even a little 
	better?


Last updated: 8 Jun 2005 by Kelly Fitz
"""
print __doc__

import loris, time
print "using Loris version", loris.version()


# use this trial counter to skip over
# eariler trials
trial = 1
print "running trial number", trial, time.ctime(time.time())


src = 'alienthreat'
f = loris.AiffFile(src+'.aiff')
samples = f.samples()
rate = f.sampleRate()

if trial == 1:
	res = 60
	floor = -80
	mlw = 160
	a = loris.Analyzer( res, mlw )
	a.setAmpFloor( floor )
	# turn off BW association
	a.setBwRegionWidth( 0 )
	p = a.analyze( samples, rate )
	ofile = 'threat.%i.%i.raw'%(res, mlw)
	# collate
	loris.collate( p, 1 )
	# export
	fsamps = loris.AiffFile( p, rate )
	fsamps.write( ofile + '.aiff' )
	fpartials = loris.AiffFile( p )
	fpartials.write( ofile + '.sdif' )
	fspc = loris.SpcFile( p )
	fspc.write( ofile + '.e.spc' )
	fspc.write( ofile + '.s.spc', 0 )
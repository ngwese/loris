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

trial 2: 
	- checked various window widths, 160 Hz seems to be the best out of
	110, 120, 140, 160


There is important information between harmonics, but not that much of it.
Sifting and retaining the unlabeled Partials give a good set of harmonic
Partials and still sounds good in reconstruction. The harmonic Partials
alone are usable but the reconstruction is slightly un-natural sounding.

Last updated: 5 June 2006 by Kelly Fitz
"""
print __doc__

import loris, time
print "using Loris version", loris.version()

src = 'alienthreat'
f = loris.AiffFile(src+'.aiff')
samples = f.samples()
rate = f.sampleRate()

a = loris.Analyzer( 60, 160 )
a.setAmpFloor( -80 )
# turn off BW association
a.setBwRegionWidth( 0 )
a.buildFundamentalEnv( 70, 140 )
p = a.analyze( samples, rate )
ref = a.fundamentalEnv()

# sift and distill
print 'sifting %i Partials (%s)'%(p.size(), time.ctime(time.time()))
# ref = loris.createFreqReference( p, 70, 140 )
loris.channelize( p, ref, 1 )
loris.sift( p )
Fade = 0.001
loris.distill( p, Fade )

# export sifted
print 'exporting %i sifted partials (%s)'%(p.size(), time.ctime(time.time()))
ofile = 'threat'
fpartials = loris.SdifFile( p )
fpartials.write( ofile + '.sdif' )

print 'rendering sifted partials (%s)'%(time.ctime(time.time()))
fsamps = loris.AiffFile( p, rate )
print 'writing %s (%s)'%(ofile + '.recon.aiff', time.ctime(time.time()))
fsamps.write( ofile + '.recon.aiff' )

# export harmonics only
print 'isolating harmonic partials (%s)'%(time.ctime(time.time()))
junk = loris.extractLabeled( p, 0 )
ofile = 'threat.harms'
print 'exporting %i harmonic partials (%s)'%(p.size(), time.ctime(time.time()))
fpartials = loris.SdifFile( p )
fpartials.write( ofile + '.sdif' )

print 'rendering harmonic partials (%s)'%(time.ctime(time.time()))
fsamps = loris.AiffFile( p, rate )
print 'writing %s (%s)'%(ofile + '.recon.aiff', time.ctime(time.time()))
fsamps.write( ofile + '.recon.aiff' )

# export non-harmonic junk only
ofile = 'threat.junk'
print 'exporting %i non-harmonic junk partials (%s)'%(junk.size(), time.ctime(time.time()))
loris.setBandwidth( junk, 1 )
fpartials = loris.SdifFile( junk )
fpartials.write( ofile + '.sdif' )

print 'rendering non-harmonic junk partials (%s)'%(time.ctime(time.time()))
fsamps = loris.AiffFile( junk, rate )
print 'writing %s (%s)'%(ofile + '.recon.aiff', time.ctime(time.time()))
fsamps.write( ofile + '.recon.aiff' )


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
	- sounds okay, as before, the articulation is exaggerated for some reason
	- why should this need 80 Hz resolution? The fundamental is over 400 Hz!
	
trial 2:
	- widest windows (600 Hz) seem to give the most natural-sounding noise (the
	extermely high noise floor in this sound adds a lot of noisiness to the 
	analysis and affects the perception of naturalness).
	- the noisiness also affects the quality of the distillations, particularly
	the big breath at the end
	- the narrow resolution distillation (80 Hz) sounds better than wider ones
	(240 Hz) but maybe not closer to the horrible original, which has some 
	pretty severe noise problems
	- should try with the tracking analyzer, no compelling reason to go with
	2 partials per harmonic though.
	
trial 3:
	- bandwidth enhancement is too crunchy if resolution is big, in fact, its 
	always pretty crunchy, try sifting.
	
trial 4:
	- breath at end still sounds bad when sifted or distilled
	- sifting deinitely improves the reconstructions
	- 160 resolution has a clunk near at the first dynamic peak, others are OK
	- wider window (600) is still a bit smoother than narrower (450)


Breath at the end isn't so good, and the hum is lost, but otherwise sounds
pretty good. Even though this recording is extremely noisy, the non-harmonic
partials don't add much. Only the breath at the end is really objectionable.
Retaining only the harmonic partials, 400 and 600 Hz wide windows are 
indistinguishible.

Last updated: 5 June 2006 by Kelly Fitz

"""
print __doc__

import loris, time
print "using Loris version", loris.version()


src = 'shaku'
f = loris.AiffFile(src+'.aiff')
samples = f.samples()
rate = f.sampleRate()
		    
res = 340
mlw = 600
a = loris.Analyzer( res, mlw )
a.buildFundamentalEnv( 350, 550 )
print 'analyzing (%s)'%(time.ctime(time.time()))
# p = a.analyze( samples, rate, ref )
p = a.analyze( samples, rate )
ref = a.fundamentalEnv()

# sift and distill, one partial per harmonic
print 'sifting %i partials (%s)'%(p.size(), time.ctime(time.time()))
loris.channelize( p, ref, 1 )
loris.sift( p )
loris.distill( p )

# export sifted
# print 'exporting %i sifted partials (%s)'%(p.size(), time.ctime(time.time()))
# ofile = 'shaku'
# fpartials = loris.SdifFile( p )
# fpartials.write( ofile + '.sdif' )
# 
# print 'rendering sifted partials (%s)'%(time.ctime(time.time()))
# fsamps = loris.AiffFile( p, rate )
# print 'writing %s (%s)'%(ofile + '.recon.aiff', time.ctime(time.time()))
# fsamps.write( ofile + '.recon.aiff' )

# export harmonics only
print 'isolating harmonic partials (%s)'%(time.ctime(time.time()))
junk = loris.extractLabeled( p, 0 )
ofile = 'shaku.harms'
print 'exporting %i harmonic partials (%s)'%(p.size(), time.ctime(time.time()))
fpartials = loris.SdifFile( p )
fpartials.write( ofile + '.sdif' )

print 'rendering harmonic partials (%s)'%(time.ctime(time.time()))
fsamps = loris.AiffFile( p, rate )
print 'writing %s (%s)'%(ofile + '.recon.aiff', time.ctime(time.time()))
fsamps.write( ofile + '.recon.aiff' )

# export non-harmonic junk only
# ofile = 'shaku.junk'
# print 'exporting %i non-harmonic junk partials (%s)'%(junk.size(), time.ctime(time.time()))
# loris.setBandwidth( junk, 1 )
# fpartials = loris.SdifFile( junk )
# fpartials.write( ofile + '.sdif' )
# 
# print 'rendering non-harmonic junk partials (%s)'%(time.ctime(time.time()))
# fsamps = loris.AiffFile( junk, rate )
# print 'writing %s (%s)'%(ofile + '.recon.aiff', time.ctime(time.time()))
# fsamps.write( ofile + '.recon.aiff' )


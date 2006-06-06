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
tracking analysis, I used resolution 150 Hz (or maybe 90?), and 
window width 300 Hz.

A fundamental frequency envelope traced in Kyma is in 
pianosax.fund.qharm.

trial 1:
	- must have been using 90 Hz resolution (more importantly, 90 Hz
	frequency floor) to get the thuds in the piano notes.
	
trial 2:
	- setting frequency floor to 90 Hz is adequate, don't need resolution
	that low
	- tracking analyses are crunchy, try turning off BW association

trial 3:
	- sifting but leaving the zeros in still sounds crunchy, removing
	the zeros sounds anemic and bubbly
	- two partials per harmonic restores some of the sound, but also leaves
	in some crunch or whoosh, can't figure out where it comes from.
	- the crunch isn't actually noise, apparently, because it is still there
	even if the noise energy in scaled to zero!


Can now get a decent analysis and fundamental tracking. Definitely need 
some non-harmonic low frequency partials to reconstruct the piano thunks.
There's still a little cruch, but not to much.

Last updated: 6 June 2006 by Kelly Fitz
"""

print __doc__

import loris, time
print "using Loris version", loris.version()

name = 'pianosax.aiff'
file = loris.AiffFile( name )
samples = file.samples()
rate = file.sampleRate()

print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
res = 250
width = 600
a = loris.Analyzer( res, width )
a.setFreqFloor( 90 )
a.setFreqDrift( 20 )
a.setBwRegionWidth( 0 ) # no BW association
a.buildFundamentalEnv( 300, 720 )

p = a.analyze( samples, rate )

print 'sifting and distilling %i partials (%s)'%(p.size(), time.ctime(time.time()))
ref = a.fundamentalEnv()
loris.channelize(p, ref, 1)
loris.sift( p )
loris.distill( p )

# export
ofile = 'pianosax'
print 'writing %s (%s)'%(ofile + '.recon.aiff', time.ctime(time.time()))
loris.exportAiff( ofile + '.recon.aiff', p, rate )

print 'writing %s (%s)'%(ofile + '.sdif', time.ctime(time.time()))
loris.exportSdif( ofile + '.sdif', p ) 


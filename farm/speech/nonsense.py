"""
nonsense.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to a fried vocal sample sent by Ben Gillett.

This sound definitely gives better harmonic tracking when reanalyzed
using the tracking analysis and the fundamental extracted in the first
analysis. But two partials per harmonic are always necessary for decent
sound, and the sound doesn't improve with the improved tracking.

Tracking the low-pitched aside is difficult, and degrades with the
improved harmonic tracking.


Last updated: 8 June 2006 by Kelly Fitz
"""
print __doc__

import loris, time
print "using Loris version", loris.version()

name = 'nonsense'
file = loris.AiffFile( name + '.aiff' )
samples = file.samples()
rate = file.sampleRate()

print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
res = 50
width = 180
a = loris.Analyzer( res, width )
a.setFreqFloor( 50 )
a.setFreqDrift( 40 )
a.setBwRegionWidth( 0 ) # no BW association
a.buildFundamentalEnv( 40, 155 )

p = a.analyze( samples, rate )
ref = a.fundamentalEnv()

# distill at N Partials per harmonic
print 'sifting and distilling %i partials (%s)'%(p.size(), time.ctime(time.time()))
N = 2
loris.channelize( p, ref, N )
loris.sift( p )
loris.distill( p )

# export N Partials per harmonic,
print 'exporting %i distilled Partials per harmonic (%s)'%(N, time.ctime(time.time()))
loris.exportAiff( name + '.%i.recon.aiff'%(N), p, rate )
loris.exportSdif( name + '.%i.sdif'%(N), p )

# export 1 Partial per harmonic,
# remove in-between harmonic partials
for part in p:
    if 0 == part.label()%N:
        part.setLabel( part.label() / N )
    else:
        part.setLabel( 0 )
loris.removeLabeled( p, 0 )
print 'exporting 1 distilled Partials per harmonic (%s)'%(time.ctime(time.time()))
loris.exportAiff( name + '.1.recon.aiff', p, rate )
loris.exportSdif( name + '.1.sdif', p )

# try reanalyzing, using tracking
a.buildFundamentalEnv( 0 )
print 're-analyzing with fundamental track %s (%s)'%(name, time.ctime(time.time()))
p = a.analyze( samples, rate, ref )

# distill at N Partials per harmonic
print 'sifting and distilling %i partials (%s)'%(p.size(), time.ctime(time.time()))
loris.channelize( p, ref, N )
loris.sift( p )
loris.distill( p )

# export N Partials per harmonic
name = name + '.tk'
print 'exporting %i distilled Partials per harmonic (%s)'%(N, time.ctime(time.time()))
loris.exportAiff( name + '.%i.recon.aiff'%(N), p, rate )
loris.exportSdif( name + '.%i.sdif'%(N), p )

# export 1 Partial per harmonic,
# remove in-between harmonic partials
for part in p:
    if 0 == part.label()%N:
        part.setLabel( part.label() / N )
    else:
        part.setLabel( 0 )
loris.removeLabeled( p, 0 )
print 'exporting 1 distilled Partials per harmonic (%s)'%(time.ctime(time.time()))
loris.exportAiff( name + '.1.recon.aiff', p, rate )
loris.exportSdif( name + '.1.sdif', p )


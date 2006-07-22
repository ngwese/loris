#!/usr/bin/python

"""
tuba.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to a difficult low tuba note C2.


Last updated: 22 July 2006 by Kelly Fitz
"""
print __doc__

import loris, time

print """
Using Loris version %s
"""%loris.version()

orate = 44100

tag = ''

name = 'tuba.C2'
f = loris.AiffFile( name + '.aiff' )

print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
fHz = 65.4
anal = loris.Analyzer( 62, 120 )
anal.setFreqDrift( .2*fHz )
anal.setBwRegionWidth(0)
anal.buildFundamentalEnv( 60, 70 )
p = anal.analyze( f.samples(), f.sampleRate() )

print 'distilling %s (%s)'%(name, time.ctime(time.time()))
ref = anal.fundamentalEnv()
loris.channelize( p, ref, 1 )
#loris.sift( p )
#loris.removeLabeled( p, 0 )
loris.distill( p )

print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
samps = loris.synthesize( p, orate )

print 'writing %s (%s)'%(name + tag + '.recon.aiff', time.ctime(time.time()))
out_sfile = loris.AiffFile( samps, orate )
out_sfile.setMidiNoteNumber( 36 )
out_sfile.setMarkers( f.markers() )
out_sfile.write( name + tag + '.recon.aiff' )


print 'writing %s (%s)'%(name + tag + '.sdif', time.ctime(time.time()))
out_pfile = loris.SdifFile( p )
out_pfile.setMarkers( f.markers() )
out_pfile.write( name + tag + '.sdif' )

print 'computing residual signal (%s)'%(time.ctime(time.time()))
resid = list(f.samples())
for k in range(len(resid)):
    if k < len(samps):
        resid[k] = resid[k] - samps[k]
        
print 'writing %s (%s)'%(name + tag + '.residual.aiff', time.ctime(time.time()))
out_sfile.setMidiNoteNumber( 36 )
out_sfile.setMarkers( f.markers() )
out_sfile.write( name + tag + '.residual.aiff' )

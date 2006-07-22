#!/usr/bin/python

"""
bass.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the double bass open D string
(D two octaves below middle C), taken from the Iowa samples.
I have used this bass extensively in morphing with a bassoon
sound.

Extended to do the G2 note as well.

Last updated: 5 June 2006 by Kelly Fitz
"""
print __doc__

import loris, time

print """
Using Loris version %s
"""%loris.version()

orate = 44100

tag = ''

name = 'bass.D2'
f = loris.AiffFile( name + '.aiff' )

print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
fHz = 73.41
anal = loris.Analyzer( .7*fHz, 1.8*fHz )
anal.setFreqDrift( .2*fHz )
anal.setAmpFloor(-75)
anal.buildFundamentalEnv( .9*fHz, 1.1*fHz )
p = anal.analyze( f.samples(), f.sampleRate() )

print 'distilling %s (%s)'%(name, time.ctime(time.time()))
ref = anal.fundamentalEnv()
loris.channelize( p, ref, 1 )
#loris.sift( p )
#loris.removeLabeled( p, 0 )
loris.distill( p )

print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
out_sfile = loris.AiffFile( p, orate )

print 'writing %s (%s)'%(name + tag + '.recon.aiff', time.ctime(time.time()))
out_sfile.setMidiNoteNumber( 38 )
out_sfile.setMarkers( f.markers() )
out_sfile.write( name + tag + '.recon.aiff' )

print 'writing %s (%s)'%(name + tag + '.sdif', time.ctime(time.time()))
out_pfile = loris.SdifFile( p )
out_pfile.setMarkers( f.markers() )
out_pfile.write( name + tag + '.sdif' )


name = 'bass.G2'
f = loris.AiffFile( name + '.aiff' )

print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
fHz = 97
anal = loris.Analyzer( .7*fHz, 1.8*fHz )
anal.setFreqDrift( .2*fHz )
anal.setAmpFloor(-75)
anal.buildFundamentalEnv( .9*fHz, 1.1*fHz )
p = anal.analyze( f.samples(), f.sampleRate() )

print 'distilling %s (%s)'%(name, time.ctime(time.time()))
ref = anal.fundamentalEnv()
loris.channelize( p, ref, 1 )
#loris.sift( p )
#loris.removeLabeled( p, 0 )
loris.distill( p )

print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
out_sfile = loris.AiffFile( p, orate )

print 'writing %s (%s)'%(name + tag + '.recon.aiff', time.ctime(time.time()))
out_sfile.setMidiNoteNumber( 43 )
out_sfile.setMarkers( f.markers() )
out_sfile.write( name + tag + '.recon.aiff' )

print 'writing %s (%s)'%(name + tag + '.sdif', time.ctime(time.time()))
out_pfile = loris.SdifFile( p )
out_pfile.setMarkers( f.markers() )
out_pfile.write( name + tag + '.sdif' )


name = 'bass.Db2.arco'
f = loris.AiffFile( name + '.aiff' )

print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
fHz = 69.3
anal = loris.Analyzer( .85*fHz, 1.65*fHz )
anal.setFreqDrift( .2*fHz )
anal.setBwRegionWidth( 0 )
anal.buildFundamentalEnv( .9*fHz, 1.1*fHz )
p = anal.analyze( f.samples(), f.sampleRate() )

print 'distilling %s (%s)'%(name, time.ctime(time.time()))
ref = anal.fundamentalEnv()
loris.channelize( p, ref, 1 )
#loris.sift( p )
#loris.removeLabeled( p, 0 )
loris.distill( p, 0.01 )

print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
out_sfile = loris.AiffFile( p, orate )

print 'writing %s (%s)'%(name + tag + '.recon.aiff', time.ctime(time.time()))
out_sfile.setMidiNoteNumber( 37 )
out_sfile.setMarkers( f.markers() )
out_sfile.write( name + tag + '.recon.aiff' )

print 'writing %s (%s)'%(name + tag + '.sdif', time.ctime(time.time()))
out_pfile = loris.SdifFile( p )
out_pfile.setMarkers( f.markers() )
out_pfile.write( name + tag + '.sdif' )


print 'Done. (%s)'%(time.ctime(time.time()))

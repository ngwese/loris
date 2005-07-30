#!/usr/bin/python

"""
bassoonDb2.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the bassoon tone D flat 2
(Db two octaves below middle C), taken from the Iowa samples.
I have used this bassoon extensively in morphing with a bass
sound.


Last updated: 30 July 2005 by Kelly Fitz
"""
print __doc__

import loris, time

print """
Using Loris version %s
"""%loris.version()


orate = 44100

name = 'bassoon.Db2'
f = loris.AiffFile( name + '.aiff' )

print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
fHz = 69.69
anal = loris.Analyzer( .7*fHz, 1.8*fHz )
anal.setFreqDrift( .2*fHz )
p = anal.analyze( f.samples(), f.sampleRate() )

print 'distilling %s (%s)'%(name, time.ctime(time.time()))
ref = loris.createFreqReference( p, .7*fHz, 1.3*fHz )
loris.channelize( p, ref, 1 )
#loris.sift( p )
#loris.removeLabeled( p, 0 )
loris.distill( p )

print 'synthesizing sifted %s (%s)'%(name, time.ctime(time.time()))
out_sfile = loris.AiffFile( p, orate )

print 'writing %s (%s)'%(name + '.recon.aiff', time.ctime(time.time()))
out_sfile.setMidiNoteNumber( 37.11 )
out_sfile.setMarkers( f.markers() )
out_sfile.write( name + '.recon.aiff' )

print 'writing %s (%s)'%(name + '.sdif', time.ctime(time.time()))
out_pfile = loris.SdifFile( p )
out_pfile.setMarkers( f.markers() )
out_pfile.write( name + '.sdif' )

print 'writing %s (%s)'%(name + '.s.spc', time.ctime(time.time()))
out_sinefile = loris.SpcFile( p )
out_sinefile.setMidiNoteNumber( 37.11 )
out_sinefile.write( name + '.s.spc', 0 )

print 'Done. (%s)'%(time.ctime(time.time()))

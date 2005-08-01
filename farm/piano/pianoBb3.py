#!/usr/bin/python

"""
pianoBb3.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the piano B flat 3
(B flat below middle C), taken from the Iowa samples.
I have used this piano in morphing with a cello
sound.

Last updated: 31 July 2005 by Kelly Fitz
"""
print __doc__

import loris, time

print """
Using Loris version %s
"""%loris.version()

orate = 44100

tag = ''

name = 'piano.Bb3'
f = loris.AiffFile( name + '.aiff' )

print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
fHz = 233
anal = loris.Analyzer( .85*fHz, 1.5*fHz )
p = anal.analyze( f.samples(), f.sampleRate() )

print 'distilling %s (%s)'%(name, time.ctime(time.time()))
ref = loris.createFreqReference( p, .7*fHz, 1.3*fHz )
loris.channelize( p, ref, 1 )
loris.distill( p )

print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
out_sfile = loris.AiffFile( p, orate )

print 'writing %s (%s)'%(name + tag + '.recon.aiff', time.ctime(time.time()))
out_sfile.setMidiNoteNumber( 58 )
out_sfile.setMarkers( f.markers() )
out_sfile.write( name + tag + '.recon.aiff' )

print 'writing %s (%s)'%(name + tag + '.sdif', time.ctime(time.time()))
out_pfile = loris.SdifFile( p )
out_pfile.setMarkers( f.markers() )
out_pfile.write( name + tag + '.sdif' )

print 'writing %s (%s)'%(name + tag + '.s.spc', time.ctime(time.time()))
out_sinefile = loris.SpcFile( p )
out_sinefile.setMarkers( f.markers() )
out_sinefile.setMidiNoteNumber( 58 )
out_sinefile.write( name + tag + '.s.spc', 0 )

print 'Done. (%s)'%(time.ctime(time.time()))

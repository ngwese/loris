#!/usr/bin/python

"""
flutter.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This 330 Hz flutter-tongued flute tone is is well-represented using 250 Hz
resolution and a 500 Hz window. The reference envelope can be derived from
the fundamental partial (around 330 Hz) and used to distill at one partial
per harmonic. 

Now using analysis-timem fundamental tracking.

Last updated: 22 July 2006 by Kelly Fitz
"""
print __doc__

import loris, time

print """
Using Loris version %s
"""%loris.version()

anal = loris.Analyzer( 250, 500 )
orate = 44100

tag = ''

name = 'flutter'
f = loris.AiffFile( name + '.aiff' )
print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
anal.buildFundamentalEnv( 330*.9, 330*1.1 )
p = anal.analyze( f.samples(), f.sampleRate() )

# distilled using one partial per harmonic
ref =  anal.fundamentalEnv()
loris.channelize( p, ref, 1 )
loris.distill( p )

print 'synthesizing distilled %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( p, orate )

print 'writing %s (%s)'%(name + tag + '.recon.aiff', time.ctime(time.time()))
out_sfile = loris.AiffFile( samples, orate )
out_sfile.setMidiNoteNumber( 64 )
out_sfile.setMarkers( f.markers() )
out_sfile.write( name + tag + '.recon.aiff' )

print 'writing %s (%s)'%(name + tag + '.sdif', time.ctime(time.time()))
out_pfile = loris.SdifFile( p )
out_pfile.setMarkers( f.markers() )
out_pfile.write( name + tag + '.sdif' )



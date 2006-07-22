#!python

"""
flute.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the breathy flute tone that represented our
first successful bandwidth-enhanced synthesis, using ad-hoc bandwidth
association. This is one of our poster-child sounds, we use it in
many demos and for testing of basic morphing functionality (usually
with the clarinet). The pitch is D above middle C, around 291 Hz, 
and there's an interesting pre-attack whistle. 

Sounds good with resolution of 240 Hz and a window as wide as the fundamental
frequency (291 Hz), or thereabouts, and distill at one partial per harmonic.

Now using analysis-timem fundamental tracking.

Last updated: 22 July 2006 by Kelly Fitz
"""
print __doc__

import loris, time

print """
Using Loris version %s
"""%loris.version()

tag = ''

anal = loris.Analyzer( 240, 300 )
orate = 44100

#
# 291 Hz flute tone
#
name = 'flute291'
f = loris.AiffFile( name + '.aiff' )
print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
anal.buildFundamentalEnv( 291*.9, 291*1.1 )
p = anal.analyze( f.samples(), f.sampleRate() )

# distilled using one partial per harmonic
ref =  anal.fundamentalEnv()
loris.channelize( p, ref, 1 )
loris.distill( p )

print 'synthesizing distilled %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( p, orate )

print 'writing %s (%s)'%(name + tag + '.recon.aiff', time.ctime(time.time()))
out_sfile = loris.AiffFile( samples, orate )
out_sfile.setMidiNoteNumber( 62 )
out_sfile.setMarkers( f.markers() )
out_sfile.write( name + tag + '.recon.aiff' )

print 'writing %s (%s)'%(name + tag + '.sdif', time.ctime(time.time()))
out_pfile = loris.SdifFile( p )
out_pfile.setMarkers( f.markers() )
out_pfile.write( name + tag + '.sdif' )




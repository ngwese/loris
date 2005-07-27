#!/usr/bin/python

"""
bassD2.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the double bass open D string
(D two octaves below middle C), taken from the Iowa samples.
I have used this bass extensively in morphing with a bassoon
sound.


Last updated: 27 July 2005 by Kelly Fitz
"""
print __doc__

import loris, time

print """
Using Loris version %s
"""%loris.version()

# use this trial counter to skip over
# eariler trials
# trial = 1
# 
# print "running trial number", trial, time.ctime(time.time())

source = 'bass.D2.aiff'

orate = 44100

tag = ''

name = 'bass.D2'
f = loris.AiffFile( name + '.aiff' )

print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
fHz = 73.41
anal = loris.Analyzer( .7*fHz, 1.8*fHz )
anal.setFreqDrift( .2*fHz )
anal.setAmpFloor(-75)
p = anal.analyze( f.samples(), f.sampleRate() )

print 'distilling %s (%s)'%(name, time.ctime(time.time()))
ref = loris.createFreqReference( p, .7*fHz, 1.3*fHz )
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

print 'writing %s (%s)'%(name + tag + '.s.spc', time.ctime(time.time()))
out_sinefile = loris.SpcFile( p )
out_sinefile.setMarkers( f.markers() )
out_sinefile.setMidiNoteNumber( 38 )
out_sinefile.write( name + tag + '.s.spc', 0 )

print 'Done. (%s)'%(time.ctime(time.time()))

#!python
#
#   This is the Loris C++ Class Library, implementing analysis, 
#   manipulation, and synthesis of digitized sounds using the Reassigned 
#   Bandwidth-Enhanced Additive Sound Model.
#   
#   Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
#  
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#  
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY, without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#   GNU General Public License for more details.
#  
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#  
#  
#	morphdemo.py
#
#	Loris instrument tone morphing demonstration.
#
#   Kelly Fitz, 28 Sept 1999
#   loris@cerlsoundgroup.org
#  
#   http://www.cerlsoundgroup.org/Loris/
#  
"""
Welcome to the Loris morphing demo!
Kelly Fitz 2000

Generates several morphs between a clarinet, 
a flute, and a cello. The results can be compared
to those in the morphdemo.out directory.
"""
print __doc__

import loris, os, time
print '(in %s)' % os.getcwd()

#
#	analyze flute tone
#
# The analysis process is as follows:
# - configure the analyzer (the flute and clarinet use the
# same analyzer configuration)
# - analyze, yielding a collection of partials
# - extract a reference envelope and distill partials
# (this reduces the number of partials considerably by 
# connecting and condensing related partials; for example, 
# in quasi-harmonic sounds, the distillation process yields
# one partial per harmonic)
# - a test synthesis of the distilled partials is performed,
# just as a sanity check, and to verify the suitability of the
# analysis configuration and distillation parameters
#
print 'analyzing flute 3D (%s)' % time.ctime(time.time())
a = loris.Analyzer(270)
a.setFreqFloor(250)		# eliminate some subharmonic garbage
v = loris.importAiff( 'flute.aiff' )
(n, samplerate, nchans) = loris.infoAiff( 'flute.aiff' )
flut = a.analyze( v, samplerate )
print 'using fundamental as reference'
flut_env = loris.createFreqReference( flut, 20, 250, 500 )
loris.channelize( flut, flut_env, 1 )
loris.distill( flut )
fname = 'fluteOK.aiff'
print 'synthesizing', fname, '(%s)' % time.ctime(time.time())
loris.exportAiff( fname, loris.synthesize( flut, samplerate ), samplerate, 1, 16 )
fname = 'flute.sdif'
print 'exporting sdif file:', fname, '(%s)' % time.ctime(time.time())
loris.exportSdif( fname, flut )

#
#	analyze clarinet tone
#
print 'analyzing clarinet 3G# (%s)' % time.ctime(time.time())
a.configure(390)
v = loris.importAiff( 'clarinet.aiff' )
(n, samplerate, nchans) = loris.infoAiff( 'clarinet.aiff' )
clar = a.analyze( v, samplerate )
print 'using fundamental as reference'
env = loris.createFreqReference( clar, 20, 350, 500 )
loris.channelize( clar, env, 1 )
loris.distill( clar )
print 'shifting clarinet pitch down by six half steps'
loris.shiftPitch( clar, loris.BreakpointEnvelopeWithValue( -600 ) )
print 'doubling amplitude'
loris.scaleAmp( clar, loris.BreakpointEnvelopeWithValue( 2 ) )
fname = 'clarOK.aiff'
print 'synthesizing', fname, '(%s)' % time.ctime(time.time())
loris.exportAiff( fname, loris.synthesize( clar, samplerate ), samplerate, 1, 16 )
fname = 'clarinet.sdif'
print 'exporting sdif file:', fname, '(%s)' % time.ctime(time.time())
loris.exportSdif( fname, clar )

#
#	analyze cello tone
#
print 'analyzing cello 2D# (%s)' % time.ctime(time.time())
a.configure(135)
# a.setFreqFloor(100) not necessary, right?		# eliminate some subharmonic garbage
v = loris.importAiff( 'cello.aiff' )
(n, samplerate, nchans) = loris.infoAiff( 'cello.aiff' )
cel = a.analyze( v, samplerate )
print 'using third harmonic as reference'
third = loris.createFreqReference( cel, 20, 400, 500 )
loris.channelize( cel, third, 3 )
loris.distill( cel )
fname = 'cellOK.aiff'
print 'synthesizing', fname, '(%s)' % time.ctime(time.time())
loris.exportAiff( fname, loris.synthesize( cel, samplerate ), samplerate, 1, 16 )
fname = 'cello.sdif'
print 'exporting sdif file:', fname, '(%s)' % time.ctime(time.time())
loris.exportSdif( fname, cel )

#
#	perform temporal dilation
#
# Times are the beginning and end times 
# of the attack and the release. To change 
# the duration of the morph, change the 
# target times (tgt_times), as well as the
# morphing function, mf, defined below.
# 
flute_times = [0.175, 0.4, 2.15, 2.31]
clar_times = [0., 0.185, 1.9, 2.15]
cel_times = [0., 0.13, 2.55, 3.9]
tgt_times = [0., 0.19, 3., 3.25]

print 'dilating sounds to match', tgt_times, '(%s)' % time.ctime(time.time())
print 'flute times:', flute_times
loris.dilate( flut, str(flute_times), str(tgt_times) )
print 'clarinet times:', clar_times
loris.dilate( clar, str(clar_times), str(tgt_times) )
print 'cello times:', cel_times
loris.dilate( cel, str(cel_times), str(tgt_times) )

#
#	perform morphs
#
# Morphs are from the first sound to the 
# second over the time 0.6 to 1.6 seconds.
#
mf = loris.BreakpointEnvelope()
mf.insertBreakpoint( 0.6, 0 )
mf.insertBreakpoint( 1.6, 1 )

samplerate = 44100.

print 'morphing flute and clarinet (%s)' % time.ctime(time.time())
loris.exportAiff( 'clariflute.aiff', 
				  loris.synthesize( loris.morph( clar, flut, mf, mf, mf ), samplerate ), 
				  samplerate, 1, 16 )
loris.exportAiff( 'flutinet.aiff', 
				  loris.synthesize( loris.morph( flut, clar, mf, mf, mf ), samplerate ), 
				  samplerate, 1, 16 )

print 'morphing flute and cello (%s)' % time.ctime(time.time())
print 'shifting flute pitch down by eleven half steps'
flut_low = flut.copy()
loris.shiftPitch( flut_low, loris.BreakpointEnvelopeWithValue( -1100 ) )
loris.exportAiff( 'cellute.aiff', 
				  loris.synthesize( loris.morph( cel, flut_low, mf, mf, mf ), samplerate ), 
				  samplerate, 1, 16 )
loris.exportAiff( 'flutello.aiff', 
				  loris.synthesize( loris.morph( flut_low, cel, mf, mf, mf ), samplerate ), 
				  samplerate, 1, 16 )

print 'morphing flute and cello again (%s)' % time.ctime(time.time())
print 'shifting flute pitch up by one half step'
loris.shiftPitch( flut, loris.BreakpointEnvelopeWithValue( 100 ) )
# perform channelization again to relabel the partials such that the
# fundamental is labeled 2, the second harmonic 4, the third 6, etc.
loris.channelize( flut, flut_env, 2 )
loris.distill( flut )	# shouldn't need to re-distill, right?

loris.exportAiff( 'cellute2.aiff', 
				  loris.synthesize( loris.morph( cel, flut, mf, mf, mf ), samplerate ), 
				  samplerate, 1, 16 )
loris.exportAiff( 'flutello2.aiff', 
				  loris.synthesize( loris.morph( flut, cel, mf, mf, mf ), samplerate ), 
				  samplerate, 1, 16 )

# 	all done
print 'hey, I\'m spent. (%s)' % time.ctime(time.time())


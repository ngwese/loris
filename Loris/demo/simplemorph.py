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
#	simplemorph.py
#
#	Very simple Loris instrument tone morphing demonstration.
#
#   Kelly Fitz, 28 Sept 1999
#   loris@cerlsoundgroup.org
#  
#   http://www.cerlsoundgroup.org/Loris/
#
"""
Welcome to the very simple Loris morphing demo!
Kelly Fitz 2000

Generates a simple linear morph between a 
clarinet and a flute.
"""
print __doc__

import loris, os, time
print '(in %s)' % os.getcwd()

#
#	analyze clarinet tone
#
print 'analyzing clarinet 3G# (%s)' % time.ctime(time.time())
a = loris.new_Analyzer(270)
clar = a.analyze('clarinet.aiff')
loris.distill(1, loris.reference(20, clar, 1000), clar )
loris.pitch(-600, clar)

#
#	analyze flute tone
#
print 'analyzing flute 3D (%s)' % time.ctime(time.time())
# reuse: a = loris.new_Analyzer(270)
flut = a.analyze('flute.aiff')
loris.distill(1, loris.reference(20, flut, 1000), flut )

#
#	perform temporal dilation
#
flute_times = [0.4, 1.]
clar_times = [0.2, 1.]
tgt_times = [0.3, 1.2]

print 'dilating sounds to match', tgt_times, '(%s)' % time.ctime(time.time())
print 'flute times:', flute_times
loris.dilate( flute_times, tgt_times, flut )
print 'clarinet times:', clar_times
loris.dilate( clar_times, tgt_times, clar )

#
#	perform morphs
#
print 'morphing flute and clarinet (%s)' % time.ctime(time.time())
mf = [(0.6, 0), (2, 1)]
m = loris.morph(clar, flut, mf, mf, mf )
loris.synthesize('simple_morph.aiff', m)

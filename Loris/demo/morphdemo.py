#	morphdemo.py
#
#	Loris instrument tone morphing demonstration.
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
a = loris.new_Analyzer(270)
a.setFreqFloor(250)		# eliminate some subharmonic garbage
flut = a.analyze('flute.aiff')
print 'using fundamental as reference'
flut_env = loris.reference(20, flut, 1000)
loris.distill(1, flut_env, flut )
fname = 'fluteOK.aiff'
print 'synthesizing', fname, '(%s)' % time.ctime(time.time())
loris.synthesize( fname, flut )

#
#	analyze clarinet tone
#
print 'analyzing clarinet 3G# (%s)' % time.ctime(time.time())
clar = a.analyze('clarinet.aiff')
print 'using fundamental as reference'
env = loris.reference(20, clar, 1000)
loris.distill(1, env, clar )
print 'shifting clarinet pitch down by six half steps'
loris.pitch(-600, clar)
print 'doubling amplitude'
loris.amp(2., clar)
fname = 'clarOK.aiff'
print 'synthesizing', fname, '(%s)' % time.ctime(time.time())
loris.synthesize( fname, clar )

#
#	analyze cello tone
#
print 'analyzing cello 2D# (%s)' % time.ctime(time.time())
a.configure(135)
a.setFreqFloor(100)		# eliminate some subharmonic garbage
cel = a.analyze('cello.aiff')
print 'using third harmonic as reference'
loris.distill(3, loris.reference(20, cel, 200, 1000), cel )
fname = 'cellOK.aiff'
print 'synthesizing', fname, '(%s)' % time.ctime(time.time())
loris.synthesize( fname, cel )

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
loris.dilate( flute_times, tgt_times, flut )
print 'clarinet times:', clar_times
loris.dilate( clar_times, tgt_times, clar )
print 'cello times:', cel_times
loris.dilate( cel_times, tgt_times, cel )

#
#	perform morphs
#
# Morphs are from the first sound to the 
# second over the time 0.6 to 1.6 seconds.
#
mf = [(0.6, 0), (1.6, 1)]
print 'morphing flute and clarinet (%s)' % time.ctime(time.time())
loris.synthesize('clariflute.aiff', loris.morph(clar, flut, mf, mf, mf ))
loris.synthesize('flutinet.aiff', loris.morph(flut, clar, mf, mf, mf ))

print 'morphing flute and cello (%s)' % time.ctime(time.time())
print 'shifting flute pitch down by eleven half steps'
flut_low = flut.copy()
loris.pitch(-1100, flut_low)
loris.synthesize('cellute.aiff', loris.morph(cel, flut_low, mf, mf, mf ))
loris.synthesize('flutello.aiff', loris.morph(flut_low, cel, mf, mf, mf ))

print 'morphing flute and cello again (%s)' % time.ctime(time.time())
print 'shifting flute pitch up by one half step'
loris.pitch(100, flut)
# perform distillation again to relabel the partials such that the
# fundamental is labeled 2, the second harmonic 4, the third 6, etc.
loris.distill(2, flut_env, flut)
loris.synthesize('cellute2.aiff', loris.morph(cel, flut, mf, mf, mf ))
loris.synthesize('flutello2.aiff', loris.morph(flut, cel, mf, mf, mf ))

# 	all done
print 'hey, I\'m spent. (%s)' % time.ctime(time.time())

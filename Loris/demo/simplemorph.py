#	simplemorph.py
#
#	Very simple Loris instrument tone morphing demonstration.
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
# save_flut = flut.copy()
print 'flute times:', flute_times
loris.dilate( flute_times, tgt_times, flut )
# save_clar = clar.copy()
print 'clarinet times:', clar_times
loris.dilate( clar_times, tgt_times, clar )

#
#	perform morphs
#
print 'morphing flute and clarinet (%s)' % time.ctime(time.time())
mf = [(0.6, 0), (2, 1)]
m = loris.morph(clar, flut, mf, mf, mf )
loris.synthesize('simple_morph.aiff', m)

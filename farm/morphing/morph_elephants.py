"""
morph_elephants.py

Morphs two elephant sounds.
The elephants have to be distilled with a common frequency.

Last updated: 10 Sep 2007 by Kelly Fitz
"""
print __doc__

import loris, time

print """
Using Loris version %s
"""%loris.version()

from os import path
elephant_dir = path.join( path.pardir, "elephants" )
print "--- importing elephants from %s ---"%(elephant_dir)

e1 = loris.importSdif( path.join( elephant_dir, "elephant1.sdif" ) )
e3 = loris.importSdif( path.join( elephant_dir, "elephant3.sdif" ) )

print "--- dilating ---"
# times correspond to markers in source files:
# onset, break, release, end
#
e1times = ( 0.029, 2.21, 2.709, 2.789, 2.79 )
e3times = ( 0.183, 1.885, 3.030, 3.272, 3.606 )

tgttimes = ( 0.003, 2.25, 3.5, 3.75, 3.753 )

loris.dilate( e1, e1times, tgttimes )
loris.dilate( e3, e3times, tgttimes )

print "--- morphing ---"
loris.scaleNoiseRatio( e1, 0 ) # ???
env = loris.BreakpointEnvelope()
env.insertBreakpoint( 2.25, 0 )
env.insertBreakpoint( 3.5, 1 )

# use old-style linear amplitude morphing
loris.setAmplitudeMorphShape( loris.LORIS_LINEAR_AMPMORPHSHAPE )
m = loris.morph( e1, e3, env, env, env )
fout = loris.AiffFile( m, 44100 )
fout.write( "elephants.morph.aiff" )

# export the dilated data as spc files for Kyma morphing.
print "--- exporting Spc files for Kyma morphing ---"

ofilebase = "elephant1.aligned"
loris.exportSdif( ofilebase + ".sdif", e1 )
fout = loris.AiffFile( e1 )
fout.write( ofilebase + ".aiff" )

ofilebase = "elephant3.aligned"
loris.exportSdif( ofilebase + ".sdif", e3 )
fout = loris.AiffFile( e3 )
fout.write( ofilebase + ".aiff" )





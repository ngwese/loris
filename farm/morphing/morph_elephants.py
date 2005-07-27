#!python
#
# morph_elephants.py
#
# Elephants have to be distilled with a common frequency!

from loris import *


from os import path
elephant_dir = path.join( path.pardir, "elephants" )
print "--- importing elephants from %s ---"%(elephant_dir)

e1 = importSdif( path.join( elephant_dir, "elephant1.sdif" ) )
e3 = importSdif( path.join( elephant_dir, "elephant3.sdif" ) )

print "--- dilating ---"
# times correspond to markers in source files:
# onset, break, release, end
#
e1times = ( 0.029, 2.21, 2.709, 2.789, 2.79 )
e3times = ( 0.183, 1.885, 3.030, 3.272, 3.606 )

tgttimes = ( 0.003, 2.25, 3.5, 3.75, 3.753 )

dilate( e1, e1times, tgttimes )
dilate( e3, e3times, tgttimes )

print "--- morphing ---"
scaleNoiseRatio( e1, 0 ) # ???
env = BreakpointEnvelope()
env.insertBreakpoint( 2.25, 0 )
env.insertBreakpoint( 3.5, 1 )
m = morph( e1, e3, env, env, env )
fout = AiffFile( m, 44100 )
fout.write( "elephants.morph.aiff" )

# export the dilated data as spc files for Kyma morphing.
# remove any Partials labeled greater than 256
print "--- exporting Spc files for Kyma morphing ---"
for partial in e1:
	if partial.label() > 256:
		partial.setLabel( 0 )
removeLabeled( e1, 0 )

for partial in e3:
	if partial.label() > 256:
		partial.setLabel( 0 )
removeLabeled( e3, 0 )

ofilebase = "elephant1.aligned"
exportSdif( ofilebase + ".sdif", e1 )
fout = AiffFile( e1 )
fout.write( ofilebase + ".aiff" )
exportSpc( ofilebase + '.s.spc', e1, 60, 0 ) 
exportSpc( ofilebase + '.e.spc', e1, 60, 1 ) 


ofilebase = "elephant3.aligned"
exportSdif( ofilebase + ".sdif", e3 )
fout = AiffFile( e3 )
fout.write( ofilebase + ".aiff" )
exportSpc( ofilebase + '.s.spc', e3, 60, 0 ) 
exportSpc( ofilebase + '.e.spc', e3, 60, 1 ) 





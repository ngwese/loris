#!/usr/bin/python

"""
morph_bass_bassoon.py

Python script for testing the morphing capabilities in Loris. 

This script morphs a raga singer with a flutter-tongued
trombone sequence. I have used this in many demos (e.g.
ICMC 2002).

Last updated: 30 July 2005 by Kelly Fitz
"""
print __doc__

import loris, time

print """
Using Loris version %s
"""%loris.version()

from os import path
dir = path.join( path.pardir, "trombone" )
print "--- importing trombone from %s ---"%(dir)
tbn = loris.importSdif( path.join( dir, "tbnflutter.150.600.d1.sdif" ) )

dir = path.join( path.pardir, "derbari" )
print "--- importing raga singer from %s ---"%(dir)
raga = loris.importSdif( path.join( dir, "derbari.d1.sdif" ) )

print "--- dilating raga singer ---"
tbntimes = ( 0, 2.39 )
ragatimes = ( .124, 1.634 )
loris.dilate( raga, ragatimes, tbntimes )

print "--- exporting dilated raga singer ---"
orate = 44100
loris.exportAiff( 'raga.dilated.aiff', loris.synthesize( raga, orate ), orate )

print "--- preforming sequence of morphs ---"
outvec = []
space = [0] * int(.25*orate)
for w in (.2, .5, .7 ):
	print "--- morphing with weight %.2f ---"%(w)
	m = loris.morph( tbn, raga, w, w, w )
	v = loris.synthesize( m, orate )
	outvec.extend( v )
	outvec.extend( space )
	
print "--- exporting morph sequence ---"	
loris.exportAiff( "raga-trombone-seq.aiff", outvec, orate )
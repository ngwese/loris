#!/usr/bin/python

"""
morph_raga_tbn.py

Morph a raga singer with a flutter-tongued trombone.
Makes a progressive sequence of three morphs.

Last updated: 17 Jan 2010 by Kelly Fitz
"""

print __doc__

import loris, time, os

print """
Using Loris version %s
"""%loris.version()

orate = 44100

tag = ''

stuff = {}

# ----------------------------------------------------------------------------

def doRagaTbnMorph( exportDir = '' ):

	from os import path
	dir = path.join( path.pardir, "trombone" )
	print "--- importing trombone from %s ---"%(dir)
	tbn = loris.importSdif( path.join( dir, "tbnflutter.sdif" ) )
	
	dir = path.join( path.pardir, "derbari" )
	print "--- importing raga singer from %s ---"%(dir)
	raga = loris.importSdif( path.join( dir, "derbari.distill.sdif" ) )
	
	print "--- dilating raga singer ---"
	tbntimes = ( 0, 2.39 )
	ragatimes = ( .124, 1.634 )
	loris.dilate( raga, ragatimes, tbntimes )
	
	print "--- preforming sequence of morphs ---"
	morphs = {}
	morph_weights = (.2, .5, .7 )
	for w in morph_weights:
		print "--- morphing with weight %.2f ---"%(w)
		m = loris.morph( tbn, raga, w, w, w )
		morphs[w] = m;
		
	if exportDir:

		print "--- rendering morph sequence ---"
		opath = os.path.join( exportDir, 'raga-trombone-seq' + tag + '.aiff' ) 

		outvec = []
		space = [0] * int(.25*orate)
		for w in morph_weights:
			v = loris.synthesize( morphs[w], orate )
			outvec.extend( v )
			outvec.extend( space )
			
		print "--- exporting morph sequence ---"	
		loris.exportAiff( opath, outvec, orate )
		
		print "--- exporting dilated raga singer ---"
		opath = os.path.join( exportDir, 'raga.dilated' + tag + '.sdif' ) 
		loris.exportSdif( opath, raga )

		opath = os.path.join( exportDir, 'raga.dilated' + tag + '.aiff' ) 
		loris.exportAiff( opath, raga, orate )

		
	stuff[ 'morphs' ] = morphs
	stuff[ 'morph_weights' ] = morph_weights
	stuff[ 'raga dilated' ] = raga
	stuff[ 'raga times' ] = ragatimes
	stuff[ 'ttbn times' ] = tbntimes
	
	
	print 'Done. (%s)'%(time.ctime(time.time()))


# ----------------------------------------------------------------------------

if __name__ == '__main__':
	print __doc__

	print 'Using Loris version %s'%( loris.version() )

	import sys
	odir = os.curdir
	if len( sys.argv ) > 1:
		tag = '.' + sys.argv[1]
		
	doRagaTbnMorph( odir )
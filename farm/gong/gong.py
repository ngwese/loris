#!/usr/bin/python
"""
gong.py

Analyze and reconstruct the orchestral gong sample used in the
dissertation experiments. It is taken from the MUMS disks, and
has a very noisy sounding onset, but also some sustained partials
giving the gong its pitch, and a strong thud in the attack that
is hard to reproduce.

25 and 30 Hz resolution work well enough with window width 75 Hz.

Distillation doesn't sound so great.

Last updated: 16 Aug 2007 by Kelly Fitz
"""

import loris, time, os

orate = 44100

tag = ''

stuff = {}

# ----------------------------------------------------------------------------

def doGong( res, exportDir = '' ):

	name = 'gong'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	print 'using resolution %4f Hz'%(res)
	anal = loris.Analyzer( res, 75 )
	anal.setBwRegionWidth( 0 ) 	# or anal.storeNoBandwidth()
	p = anal.analyze( f.samples(), f.sampleRate() )

	name = '%s%d'%( name, res )
	if exportDir:
	
		print 'synthesizing %i raw partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )

	stuff[ name ] = ( p, anal )
		
		
# ----------------------------------------------------------------------------

if __name__ == '__main__':
	print __doc__

	print 'Using Loris version %s'%( loris.version() )

	import sys
	odir = os.curdir
	if len( sys.argv ) > 1:
		tag = '.' + sys.argv[1]
		
	doGong( 20, odir )
	doGong( 30, odir )
		
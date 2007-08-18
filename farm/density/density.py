#!/usr/bin/python

"""
density.py

Analyze and reconstruct a very reverberant flute solo excerpt from
Denisty 21.5 (Varese). There are eight notes, some long, some short, all
very noisy and reverberant. Because of all the reverberation, frequency
tracking and channelization is never going to give very good results,
because it will always chop off the reverb decays. 

Last updated: 18 Aug 07 by Kelly Fitz
"""

import loris, time, os

orate = 44100

tag = ''

stuff = {}

# ----------------------------------------------------------------------------

def doDensity( exportDir = '' ):

	name = 'density'
	f = loris.AiffFile( name + '.aiff' )

	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	
	# also 150,300 works pretty well, changing the 
	# window to 200 Hz starts to smear away the attacks
	anal = loris.Analyzer( 100, 300 )
	
	# I wonder how well this fundamental tracker
	# works on a tough sound like this one
	anal.buildFundamentalEnv( 250, 850 )
	
	p = anal.analyze( f.samples(), f.sampleRate() )

	if exportDir:

		print 'synthesizing %i raw partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.raw.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.raw.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )

	stuff[ name ] = ( p, anal )

	print 'Done. (%s)'%(time.ctime(time.time()))
	
	
	
# ----------------------------------------------------------------------------

if __name__ == '__main__':
	print __doc__

	print 'Using Loris version %s'%( loris.version() )

	import sys
	odir = os.curdir
	if len( sys.argv ) > 1:
		tag = '.' + sys.argv[1]
		
	doDensity( odir )	
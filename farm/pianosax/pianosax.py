#!/usr/bin/python
"""
pianosax.py


Analyze and reconstruct the piano/soprano sax duet that I used in my
dissertation work. Sifting but leaving the zeros in sounds crunchy,
removing the zeros sounds anemic and bubbly. The crunch isn't actually
noise, apparently, because it is still there even if the noise energy in
scaled to zero.

Two partials per harmonic restores some of the sound, but also leaves in
some crunch or whoosh, can't figure out where it comes from.

Can get a decent analysis and fundamental tracking. Definitely need
some non-harmonic low frequency partials to reconstruct the piano
thunks. There's still a little crunch, but not too much.

The fade time used for sifting and distilling is important in this sound. 
Using the (Distiller) default fade time of 5 ms for sifting causes the
lowest partial to fail to track the pitch changes. Need to use a shorter
fade time, at least for sifting. 1 ms for both sifting and distilling seems 
to work well. 

Last updated: 28 Dec 2009 by Kelly Fitz
"""

import loris, time, os

orate = 44100

tag = ''

stuff = {}

# ----------------------------------------------------------------------------

def doPianoSax( exportDir = '' ):

	name = 'pianosax'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	anal = loris.Analyzer( 250, 600 )
	anal.setFreqFloor( 90 )
	anal.setFreqDrift( 20 )
	anal.setBwRegionWidth( 0 )
	anal.buildFundamentalEnv( 300, 720 )
	p = anal.analyze( f.samples(), f.sampleRate() )

	Fade = 0.001
	Gap = 0.0001
	
	print 'sifting and distilling %i partials at %i ms (%s)'%(p.size(), Fade*1000, time.ctime(time.time()))
	ref = anal.fundamentalEnv()
	loris.channelize( p, ref, 1 )
	loris.sift( p, Fade )
	loris.distill( p, Fade, Gap )
	
	psinonly = loris.PartialList( p )
	MinDurSinusoid = 0.003
	print 'adding bandwidth to short partials (< %i ms) (%s)'%(MinDurSinusoid*1000., time.ctime(time.time()))
	ctr = 0
	for part in p:
		if part.duration() < MinDurSinusoid:
			loris.setBandwidth( part, 1.0 )
			ctr = ctr + 1
	print '(added bw to %i partials)'%(ctr)
			
	
	if exportDir:
	
		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		opath = os.path.join( exportDir, name + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile = loris.AiffFile( psinonly, orate )		
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.bwe.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )		
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
		
	doPianoSax( odir )


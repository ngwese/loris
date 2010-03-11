#!/usr/bin/python

"""
shaku.py


Analyze and reconstruct the shakuhachi sample that was included in the 
ICMC 2000 bakeoff. It is a 13 second phrase that sounds like it was
recorded in a steam tunnel.

Breath at the end isn't so good, and the hum is lost, but otherwise sounds
pretty good. Even though this recording is extremely noisy, the non-harmonic
partials don't add much. Only the breath at the end is really objectionable.
Retaining only the harmonic partials, 400 and 600 Hz wide windows are 
indistinguishible.

Last updated: 12 Nov 2008 by Kelly Fitz

"""

# The parameters we liked once were 80 Hz resolution and 300 Hz
# analysis window.
# 
# trial1:
# 	- sounds okay, as before, the articulation is exaggerated for some reason
# 	- why should this need 80 Hz resolution? The fundamental is over 400 Hz!
# 	
# trial 2:
# 	- widest windows (600 Hz) seem to give the most natural-sounding noise (the
# 	extermely high noise floor in this sound adds a lot of noisiness to the 
# 	analysis and affects the perception of naturalness).
# 	- the noisiness also affects the quality of the distillations, particularly
# 	the big breath at the end
# 	- the narrow resolution distillation (80 Hz) sounds better than wider ones
# 	(240 Hz) but maybe not closer to the horrible original, which has some 
# 	pretty severe noise problems
# 	- should try with the tracking analyzer, no compelling reason to go with
# 	2 partials per harmonic though.
# 	
# trial 3:
# 	- bandwidth enhancement is too crunchy if resolution is big, in fact, its 
# 	always pretty crunchy, try sifting.
# 	
# trial 4:
# 	- breath at end still sounds bad when sifted or distilled
# 	- sifting deinitely improves the reconstructions
# 	- 160 resolution has a clunk near at the first dynamic peak, others are OK
# 	- wider window (600) is still a bit smoother than narrower (450)
# 
# 


import loris, time, os

orate = 44100

tag = ''

stuff = {}

# ----------------------------------------------------------------------------

def doShaku( exportDir = '' ):


	name = 'shaku'
	f = loris.AiffFile( name + '.aiff' )

	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	anal = loris.Analyzer( 340, 600 )
	anal.buildFundamentalEnv( 350, 550 )
	p = anal.analyze( f.samples(), f.sampleRate() )

	
	# sift and distill, one partial per harmonic
	print 'sifting %i partials (%s)'%(p.size(), time.ctime(time.time()))
	ref = anal.fundamentalEnv()
	
	# distill at 1 Partial per harmonic
	# need to use short fade/gap times to keep harmonics from breaking apart
	# and pieces showing up in the "junk"
 	Fade = 0.001
 	Gap = 0.0001
 	print 'sifting and distilling %i partials (%s)'%(p.size(), time.ctime(time.time()))
	loris.channelize( p, ref, 1 )
	loris.sift( p, Fade )
	loris.distill( p, Fade, Gap )
	
	pall = loris.PartialList( p )
	junk = loris.extractLabeled( p, 0 )
	pharms = p
	
	if exportDir:

		print 'synthesizing %i sifted partials (%s)'%(pall.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( pall, orate )
		
		opath = os.path.join( exportDir, name + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( pall )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )


		print 'synthesizing %i harmonic partials (%s)'%(pharms.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( pharms, orate )
		
		opath = os.path.join( exportDir, name + tag + '.harms.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.harms.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( pharms )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )


		print 'synthesizing %i non-harmonic junk partials (%s)'%(junk.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( junk, orate )
		
		opath = os.path.join( exportDir, name + tag + '.junk.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.junk.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( junk )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )

	stuff[ name ] = ( pharms, junk, pall, anal )

	print 'Done. (%s)'%(time.ctime(time.time()))

# ----------------------------------------------------------------------------

if __name__ == '__main__':
	print __doc__

	print 'Using Loris version %s'%( loris.version() )

	import sys
	odir = os.curdir
	if len( sys.argv ) > 1:
		tag = '.' + sys.argv[1]
		
	doShaku( odir )
#!python

"""
cellos.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

	
Cello 154 Hz forte: 
	154 Hz tone sounds good with 123 Hz resolution and 154 Hz window
	width. Can distill using a flat 154 Hz envelope (although tracking
	the third partial has got to work at least as well, probably better
	for high partials). Distill using one partial per harmonic.

Cello 69 Hz forte:
	The 69 Hz F tone sounds good at 20 Hz resolution with a 69 Hz window
	width. Distill using the third partial as a reference with two
	partials per harmonic to preserve the grunty attack. Sifting seems
	to make this one a little worse.

Cello 69 Hz mezzo-forte:
	The 69 Hz MF tone sounds best, it seems, when analyzed using a
	high-gain version of the sound. Analyses using 34 (or 20) Hz
	resolution and 138 Hz window width work well. Distillations, using
	the third harmonic partial as a reference, are only usable if they
	are sifted first, otherwise they sound like they are under a pillow.
	Distill with two partials per harmonic to preserve the grunty
	attack.

	These are not excellent, and would never pass same-difference tests,
	particularly the low tones, but they are usable.
	
	Added four new arco cellos May 2006.
	
	The low cello notes don't sound so bad anymore. They need some 
	cleaning up in the decay, but overall I think they are better than 
	they used to be, and it no longer seems necessary to use the gain
	normalized MF tone, the original works OK too. The attack could use 
	some help, but otherwise OK.
	
	Fixed lousy Ab2 by realizing that it was actually Gb2! Also 
	checked tuning of the other notes. Better now.
	
	Added a new analysis function at two partials per harmonic, keeping 
	in-between partials around to improve the attacks, makes a huge difference
	on sounds like sulD.Bb3!
	
	
Last updated: 5 June 2008 by Kelly Fitz

"""
import loris, time, os

orate = 44100

tag = ''

stuff = {}

# Set these distillation parameters globally for all notes.
Fade = 0.005 	# 5 ms
Gap = 0.001 	# 1 ms 

# The old cellos sound much better with these old values.
SmallGap = 0.0001 # 0.1 ms, old gap time
SmallFade = 0.001 # 1 ms	


# ----------------------------------------------------------------------------
#
# 154.F tone
#
def doCello154F( exportDir = '' ):
	name = 'cello154.F'
	anal = loris.Analyzer( 123, 154 )

	f = loris.AiffFile( name + '.aiff' )
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	p = anal.analyze( f.samples(), f.sampleRate() )
	
	# cello154.F distilled using 3d harmonic
	ref = loris.createFreqReference( p, 154*0.9, 1541.1 )
	loris.channelize( p, ref, 1 )
	loris.distill( p, SmallFade, SmallGap )
	print 'synthesizing distilled %s (%s)'%(name, time.ctime(time.time()))

	if exportDir:
		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )

	stuff[ name ] = ( p, ref, anal )
	
	
	
# ----------------------------------------------------------------------------
#
# 69.F tone	
#
def doCello69F( exportDir = '' ):
	name = 'cello69.F'
	anal = loris.Analyzer( 20, 69 )
	anal.setFreqFloor( 34 )

	f = loris.AiffFile( name + '.aiff' )
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	p = anal.analyze( f.samples(), f.sampleRate() )
	
	# 69.F tone collated
	print 'collating %s (%s)'%(name, time.ctime(time.time()))
	pcollate = loris.PartialList( p )
	loris.collate( pcollate, SmallFade, SmallGap )	
	
	# distill using two partials per harmonic
	print 'distilling %s at two partials per harmonic (%s)'%(name, time.ctime(time.time()))
	ref = loris.createFreqReference( p, 69*0.9, 69*1.1 )
	loris.channelize( p, ref, 2 )	
	loris.distill( p, SmallFade, SmallGap )

	# relabel the partials so that they can still
	# be used for morphing, esp. with harmonic sounds
	for part in p:
		if 0 == part.label()%2:
			part.setLabel( part.label() / 2 )
		else:
			part.setLabel( 0 )
			
	if exportDir:
		print 'synthesizing %i collated partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( pcollate, orate )
		
		opath = os.path.join( exportDir, name + tag + '.raw.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.raw.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( pcollate )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )
		
		
		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )

	stuff[ name ] = ( p, pcollate, ref, anal )


# ----------------------------------------------------------------------------
#
# 69.MF tone	
#
def doCello69MF( exportDir = '' ):
	name = 'cello69.MF'
	anal = loris.Analyzer( 34, 138 )

	f = loris.AiffFile( name + '.aiff' )
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	p = anal.analyze( f.samples(), f.sampleRate() )
	
	# 69.MF tone collated
	print 'collating %s (%s)'%(name, time.ctime(time.time()))
	pcollate = loris.PartialList( p )
	loris.collate( pcollate, SmallFade, SmallGap )
	
	
	# distill using two partials per harmonic
	print 'distilling %s at two partials per harmonic (%s)'%(name, time.ctime(time.time()))
	ref = loris.createFreqReference( p, 69*0.9, 69*1.1 )
	loris.channelize( p, ref, 2 )
	loris.sift( p, SmallFade )
	loris.removeLabeled( p, 0 )
	loris.distill( p, SmallFade, SmallGap )

	# relabel the partials so that they can still
	# be used for morphing, esp. with harmonic sounds
	for part in p:
		if 0 == part.label()%2:
			part.setLabel( part.label() / 2 )
		else:
			part.setLabel( 0 )
			
	if exportDir:
		print 'synthesizing %i collated partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( pcollate, orate )
		
		opath = os.path.join( exportDir, name + tag + '.raw.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.raw.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( pcollate )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )
		
		
		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )

	stuff[ name ] = ( p, pcollate, ref, anal )
	

# ----------------------------------------------------------------------------
#
# arco cellos (new May 2006)
#

def analyze_arco( name, midiNN, exportDir = ''  ):
	import math
	
	# compute note fundamental frequency in Hz
	fHz = 440.0 * math.pow( 2.0, ((midiNN-69)/12.0) )
	
	ArcoFade = 0.01
	
	print "analyzing", name + ".aiff", "at fundamental", fHz, "Hz", time.ctime(time.time())
	f = loris.AiffFile( name + ".aiff" )
	
	anal = loris.Analyzer( .85*fHz, 1.65*fHz )
	anal.setBwRegionWidth( 0 )
	
	p = anal.analyze( f.samples(), f.sampleRate() )

	print "distilling", name, ",", p.size()," partials, fade time ", ArcoFade * 1000, "ms"
	ref = loris.createFreqReference( p, .9*fHz, 1.1*fHz );
	loris.channelize( p, ref, 1 )
	loris.distill( p, ArcoFade, Gap )
	
	# chop off any times before 0
	loris.crop( p, 0, 20 )
	
	if exportDir:
		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )
	
	stuff[ name ] = ( p, ref, anal )

# ----------------------------------------------------------------------------
#
# analysis at two partials per harmonic, with in-betweens
#

def analyze_at2( name, midiNN, exportDir = ''  ):
	import math
	
	# compute note fundamental frequency in Hz
	fHz = 440.0 * math.pow( 2.0, ((midiNN-69)/12.0) )
	
	
	print "analyzing", name + ".aiff", "at fundamental", fHz, "Hz", time.ctime(time.time())
	f = loris.AiffFile( name + ".aiff" )
	
	anal = loris.Analyzer( .45*fHz, 1.65*fHz )
	anal.setBwRegionWidth( 0 )
	
	p = anal.analyze( f.samples(), f.sampleRate() )

	print "distilling", name, ",", p.size()," partials, fade time ", Fade * 1000, "ms"
	ref = loris.createFreqReference( p, .9*fHz, 1.1*fHz );
	loris.channelize( p, ref, 2 )
	loris.distill( p, Fade, Gap )
	
	
 	# problem in older versions of Loris
 	loris.crop( p, 0, 20 )
 	
	loris.scaleBandwidth( p, 0 )
	
	# relabel the partials so that they can still
	# be used for morphing, esp. with harmonic sounds
	for part in p:
		if 0 == part.label()%2:
			part.setLabel( part.label() / 2 )
		else:
			part.setLabel( 0 )
			
	exportname = name + '.2'
	if exportDir:
		print 'synthesizing %i distilled partials, two per harmonic (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, exportname + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, exportname + tag + '.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )
	
	stuff[ exportname ] = ( p, ref, anal )
	
	
# ----------------------------------------------------------------------------

if __name__ == '__main__':
	print __doc__

	print 'Using Loris version %s'%( loris.version() )

	import sys
	odir = os.curdir
	if len( sys.argv ) > 1:
		tag = '.' + sys.argv[1]
		
	doCello154F( odir )
	doCello69F( odir )
	doCello69MF( odir )

	analyze_arco( "cello.sulA.B3", 58.7, odir )
	analyze_arco( "cello.sulC.Gb2", 42, odir )
	analyze_arco( "cello.sulC.Db2", 36.9, odir )
	analyze_arco( "cello.sulD.Bb3", 57.8, odir )	# mediocre
	analyze_at2( "cello.sulD.Bb3", 57.8, odir )		# vastly improved!
	

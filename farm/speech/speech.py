#!/usr/bin/python
"""
speech.py

Analyze and reconstruct a variety of speech sounds, including:
	
	- the fried vocal sample sent by Ben Gillett for testing 
	  the Camel analyzer
	- the "funny people" samples we have used for testing 
	  Loris for many years
	- Kenneth Branagh speaking in a low, slow half-whisper, 
	  speaking the name "Moses"
	- excerpt from a French radio broadcast, included on the 
	  Pierre Schaeffer collection, of a very low (male) voice
	- the "alien threat" sample of Kurt Hebel's voice
	

Last updated: 1 July 2008 by Kelly Fitz
"""

import loris, time, os

orate = 44100

tag = ''

stuff = {}


# ----------------------------------------------------------------------------


# The fried vocal ("nonsense") definitely gives better harmonic tracking when 
# reanalyzed using the tracking analysis and the fundamental extracted in the 
# first analysis, BUT the reconstruction is a little bit worse in that case. 
# 
# Two partials per harmonic are always necessary for decent
# sound, and the sound doesn't improve with the improved tracking.
# One partial per harmonic is completely unlistenable.


def doNonsense( exportDir = '' ):

	name = 'nonsense'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	res = 50
	width = 180
	anal = loris.Analyzer( res, width )
	anal.setFreqFloor( 50 )
	anal.setFreqDrift( 40 )
	anal.setBwRegionWidth( 0 ) # no BW association
	anal.buildFundamentalEnv( 40, 155 )
	
	p = anal.analyze( f.samples(), f.sampleRate() )
	ref = anal.fundamentalEnv()
	
	# distill at 2 Partials per harmonic
 	Fade = 0.001
 	Gap = 0.0001
 	print 'sifting and distilling %i partials (%s)'%(p.size(), time.ctime(time.time()))
	N = 2
	loris.channelize( p, ref, N )
	loris.sift( p, Fade )
	loris.distill( p, Fade, Gap )

# 	
# 	# reanalyze using tracking
# 	print 're-analyzing %s with fundamental track (%s)'%(name, time.ctime(time.time()))
# 	ptk = anal.analyze( f.samples(), f.sampleRate(), ref )
# 	
# 	# distill at N Partials per harmonic
# 	print 'sifting and distilling %i partials (%s)'%(p.size(), time.ctime(time.time()))
# 	loris.channelize( ptk, ref, N )
# 	loris.sift( ptk )
# 	loris.distill( ptk )
# 	
	
	if exportDir:
	
		print 'synthesizing %i distilled partials, %i per harmonic (%s)'%(p.size(), N, time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.%i.recon.aiff'%(N) ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.%i.sdif'%(N) )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )	
				

# 		print 'synthesizing %i distilled tracked partials, %i per harmonic (%s)'%(ptk.size(), N, time.ctime(time.time()))
# 		out_sfile = loris.AiffFile( ptk, orate )
# 		
# 		opath = os.path.join( exportDir, name + tag + '.tk%i.recon.aiff'%(N) ) 
# 		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
# 		out_sfile.setMarkers( f.markers() )
# 		out_sfile.write( opath )
# 		
# 		opath = os.path.join( exportDir, name + tag + '.tk%i.sdif'%(N) )
# 		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
# 		out_pfile = loris.SdifFile( ptk )
# 		out_pfile.setMarkers( f.markers() )
# 		out_pfile.write( opath )	
# 

	stuff[ name ] = ( p, ref, anal )
	
	print 'Done. (%s)'%(time.ctime(time.time()))		


# ----------------------------------------------------------------------------


# The "funny people" analyses were also being reanalyzed with tracking,
# but it does not make much difference. 


def do_funnyPeeple1( exportDir = '' ):

	name = 'funnyPeeple1'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	anal = loris.Analyzer( 155, 400 )
	anal.setFreqFloor( 100 )
	anal.setBwRegionWidth( 0 ) # no BW association
	anal.buildFundamentalEnv( 100, 400 )
	
	p = anal.analyze( f.samples(), f.sampleRate() )
	
	ref = anal.fundamentalEnv()	
	# ref = loris.createF0Estimate( p, 100, 400, 0.01 )
	
	
	# re-analyze with tracking? no, that is worse.
	# print 're-analyzing with tracking'
	# p = anal.analyze( f.samples(), f.sampleRate(), ref )
	
	# Need to specify non-default value for gap time to achieve
 	# good results (as in Loris 1.4)
 	Fade = 0.001
 	Gap = 0.0001
 	
 	# distill at N Partials per harmonic
 	# (one seems to work, almost, two is better)
	print 'distilling %i partials (%s)'%(p.size(), time.ctime(time.time()))
	N = 2
	loris.channelize( p, ref, N )
	loris.distill( p, Fade, Gap )
	
	# relabel the partials so that they can still
	# be used for morphing, esp. with harmonic sounds
	# (but don't remove the zeros, sounds terrible, 
	# worse than one-per-harmonic distillation)
	for part in p:
		if 0 == part.label()%N:
			part.setLabel( part.label() / N )
		else:
			part.setLabel( 0 )	
				
	
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
	
	print 'Done. (%s)'%(time.ctime(time.time()))			


# ----------------------------------------------------------------------------

def do_funnyPeeple2( exportDir = '' ):

	name = 'funnyPeeple2'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	anal = loris.Analyzer( 170, 400 )
	anal.setFreqFloor( 100 )
	anal.setBwRegionWidth( 0 ) # no BW association
	anal.buildFundamentalEnv( 100, 400 )
	
	p = anal.analyze( f.samples(), f.sampleRate() )
	
	ref = anal.fundamentalEnv()
	# ref = loris.createF0Estimate( p, 50, 1000, 0.01 )

	# re-analyze with tracking? no, that is worse.
	# print 're-analyzing with tracking'
	# p = anal.analyze( f.samples(), f.sampleRate(), ref )
	
	# Need to specify non-default value for gap time to achieve
 	# good results (as in Loris 1.4)
 	Fade = 0.001
 	Gap = 0.0001
 	
 	# distill at N Partials per harmonic
 	# (one seems to work, almost, two is better)
	print 'distilling %i partials (%s)'%(p.size(), time.ctime(time.time()))
	N = 2
	loris.channelize( p, ref, N )
	loris.distill( p, Fade, Gap )
	
	# relabel the partials so that they can still
	# be used for morphing, esp. with harmonic sounds
	# (but don't remove the zeros, sounds terrible, 
	# worse than one-per-harmonic distillation)
	for part in p:
		if 0 == part.label()%N:
			part.setLabel( part.label() / N )
		else:
			part.setLabel( 0 )	
	
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
	
	print 'Done. (%s)'%(time.ctime(time.time()))	
	
# ----------------------------------------------------------------------------


# The parameters we once liked for this are resolution 90 and
# window width 200 Hz. More recently, 70 or 80 Hz resolution 
# have sounded better.
# 
# Found that crunchiness is made _worse_ by increasing Distiller 
# fade time. With the default (1 ms), distilled sounds about the 
# same as raw.
# 
# Tracking analysis makes little difference.



def do_moses( exportDir = '' ):

	name = 'moses'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	anal = loris.Analyzer( 80, 200 )
	# anal.setBwRegionWidth( 0 ) # no BW association
	anal.buildFundamentalEnv( 90, 150 )
	
	p = anal.analyze( f.samples(), f.sampleRate() )
	ref = anal.fundamentalEnv()

	
	# distill at N Partials per harmonic
 	Fade = 0.001
 	Gap = 0.0001
 	print 'distilling %i partials (%s)'%(p.size(), time.ctime(time.time()))
	N = 1
	loris.channelize( p, ref, N )
	loris.sift( p, Fade )
	loris.removeLabeled( p, 0 )
	loris.distill( p, Fade, Gap )
	# loris.crop( p, 0, 10 )	# no longer needed as of Loris 1.5
	

# 	print 'performing tracking analysis'
# 	ptk = anal.analyze( f.samples(), f.sampleRate(), ref )
# 
# 	# distill
# 	print 'distilling %i partials (%s)'%(p.size(), time.ctime(time.time()))
# 	loris.channelize( ptk, ref, 1 )
# 	loris.sift( ptk )
# 	loris.removeLabeled( ptk, 0 )
# 	loris.distill( ptk )
	
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
	
	print 'Done. (%s)'%(time.ctime(time.time()))			
	

# ----------------------------------------------------------------------------


# The parameters we once liked for this are resolution 60 and
# window width 200 Hz.
# 
# Tracking analysis definitely sounds worse, we have to do a better
# job of tracking the fundamental in this sound. Improved by changing 
# fundamental tracking bound to 200 Hz, but still imperfect.
# 
# Still has a reverberant quality in the reconstruction.


def do_french( exportDir = '' ):

	name = 'french'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	anal = loris.Analyzer( 60, 150 )
	anal.setBwRegionWidth( 0 ) # no BW association
	anal.buildFundamentalEnv( 60, 200 )
	
	p = anal.analyze( f.samples(), f.sampleRate() )
	ref = anal.fundamentalEnv()
	
	# distill at N Partials per harmonic
	print 'distilling %i partials (%s)'%(p.size(), time.ctime(time.time()))
	N = 1
	loris.channelize( p, ref, N )
	loris.sift( p )
	loris.distill( p )
	
	print 'performing tracking analysis'
	ptk = anal.analyze( f.samples(), f.sampleRate(), ref )

	# distill
 	Fade = 0.001
 	Gap = 0.0001
 	print 'distilling %i partials (%s)'%(p.size(), time.ctime(time.time()))
	loris.channelize( ptk, ref, 1 )
	loris.sift( ptk, Fade )
	loris.removeLabeled( ptk, 0 )
	loris.distill( ptk, Fade, Gap )
	
	
	if exportDir:
	
		print 'synthesizing %i distilled partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.%i.recon.aiff'%(N) ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.%i.sdif'%(N) )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )
		

		print 'synthesizing %i distilled, tracked partials (%s)'%(ptk.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( ptk, orate )
		
		opath = os.path.join( exportDir, name + tag + '.tk.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.tk.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( ptk )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )
		
		
	stuff[ name ] = ( p, ptk, ref, anal )
	
	print 'Done. (%s)'%(time.ctime(time.time()))			


# ----------------------------------------------------------------------------

# This sample was used to compare our analysis/synthesis with the 
# analysis/synthesis tools in Kyma, the subject of my visit
# to CU in March 2003.
# 
# Notes: 
# 
# - Checked various window widths, 160 Hz seems to be the 
# best out of 110, 120, 140, 160.
# 
# - There is important information between harmonics, but not that much of it.
# Sifting and retaining the unlabeled Partials give a good set of harmonic
# Partials and still sounds good in reconstruction. The harmonic Partials
# alone are usable but the reconstruction is slightly un-natural sounding.



def doAlien( exportDir = '' ):

	name = 'alienthreat'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	anal = loris.Analyzer( 60, 160 )
	anal.setAmpFloor( -80 )
	# turn off BW association
	anal.setBwRegionWidth( 0 )
	anal.setFreqDrift( 30 )
	anal.buildFundamentalEnv( 70, 140 )
	p = anal.analyze( f.samples(), f.sampleRate() )
	ref = anal.fundamentalEnv()
	
	# sift and distill
 	Fade = 0.001
 	Gap = 0.0001
 	print 'sifting %i Partials (%s)'%(p.size(), time.ctime(time.time()))
	ref = loris.createFreqReference( p, 70, 140 )
	loris.channelize( p, ref, 1 )
	loris.sift( p, Fade )
	loris.distill( p, Fade, Gap )
	
	loris.setBandwidth( p, 0 )
	
	print 'isolating harmonic partials (%s)'%(time.ctime(time.time()))
	pharm = loris.PartialList( p )
	junk = loris.extractLabeled( pharm, 0 )
	loris.setBandwidth( junk, 1 )

	
	# export sifted
	if exportDir:
		print 'exporting %i sifted partials (%s)'%(p.size(), time.ctime(time.time()))
		ofile = name
		opath = os.path.join( exportDir, ofile + tag + '.sdif' ) 
		fpartials = loris.SdifFile( p )
		fpartials.write( opath )
		
		print 'rendering sifted partials (%s)'%(time.ctime(time.time()))
		fsamps = loris.AiffFile( p, orate )
		opath = os.path.join( exportDir, ofile + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		fsamps.write( opath )
		
		# export harmonics only
		ofile = name + '-harms'
		opath = os.path.join( exportDir, ofile + tag + '.sdif' ) 
		print 'exporting %i harmonic partials (%s)'%(pharm.size(), time.ctime(time.time()))
		fpartials = loris.SdifFile( pharm )
		fpartials.write( opath )
		
		print 'rendering harmonic partials (%s)'%(time.ctime(time.time()))
		fsamps = loris.AiffFile( p, orate )
		opath = os.path.join( exportDir, ofile + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		fsamps.write( opath )
		
		# export non-harmonic junk only
		ofile = name + '-junk'
		opath = os.path.join( exportDir, ofile + tag + '.sdif' ) 
		print 'exporting %i non-harmonic junk partials (%s)'%(junk.size(), time.ctime(time.time()))
		fpartials = loris.SdifFile( junk )
		fpartials.write( opath )
		
		print 'rendering non-harmonic junk partials (%s)'%(time.ctime(time.time()))
		fsamps = loris.AiffFile( junk, orate )
		opath = os.path.join( exportDir, ofile + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		fsamps.write( opath )

	stuff[ name ] = ( p, pharm, junk, anal, ref )

	print 'Done. (%s)'%(time.ctime(time.time()))
	
	
# ----------------------------------------------------------------------------

if __name__ == '__main__':
	print __doc__

	print 'Using Loris version %s'%( loris.version() )

	import sys
	odir = os.curdir
	if len( sys.argv ) > 1:
		tag = '.' + sys.argv[1]
		
 	doNonsense( odir )
 	do_funnyPeeple1( odir )
 	do_funnyPeeple2( odir )
  	do_moses( odir )
 	do_french( odir )
 	doAlien( odir )

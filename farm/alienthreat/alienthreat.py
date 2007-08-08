"""
alienthreat.py

Analyze the "alien threat" sample of Kurt Hebel's voice.
This sample was used to compare our analysis/synthesis with the 
analysis/synthesis tools in Kyma, the subject of my visit
to CU in March 2003.

Notes: 

- Checked various window widths, 160 Hz seems to be the 
best out of 110, 120, 140, 160.

- There is important information between harmonics, but not that much of it.
Sifting and retaining the unlabeled Partials give a good set of harmonic
Partials and still sounds good in reconstruction. The harmonic Partials
alone are usable but the reconstruction is slightly un-natural sounding.

Last updated: 7 August 2007 by Kelly Fitz


"""

import loris, time, os

orate = 44100

tag = ''

stuff = {}

# ----------------------------------------------------------------------------

def doAlien( exportDir = '' ):

	src = 'alienthreat'
	f = loris.AiffFile(src+'.aiff')
	samples = f.samples()
	rate = f.sampleRate()
	
	a = loris.Analyzer( 60, 160 )
	a.setAmpFloor( -80 )
	# turn off BW association
	a.setBwRegionWidth( 0 )
	a.setFreqDrift( 30 )
	a.buildFundamentalEnv( 70, 140 )
	p = a.analyze( samples, rate )
	ref = a.fundamentalEnv()
	
	# sift and distill
	print 'sifting %i Partials (%s)'%(p.size(), time.ctime(time.time()))
	ref = loris.createFreqReference( p, 70, 140 )
	loris.channelize( p, ref, 1 )
	loris.sift( p )
	Fade = 0.001
	loris.distill( p, Fade )
	
	loris.setBandwidth( p, 0 )
	
	stuff[ src ] = ( loris.PartialList( p ), a )
	
	# export sifted
	if exportDir:
		print 'exporting %i sifted partials (%s)'%(p.size(), time.ctime(time.time()))
		ofile = 'threat'
		opath = os.path.join( exportDir, ofile + tag + '.sdif' ) 
		fpartials = loris.SdifFile( p )
		fpartials.write( opath )
		
		print 'rendering sifted partials (%s)'%(time.ctime(time.time()))
		fsamps = loris.AiffFile( p, rate )
		opath = os.path.join( exportDir, ofile + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		fsamps.write( opath )
		
		# export harmonics only
		print 'isolating harmonic partials (%s)'%(time.ctime(time.time()))
		junk = loris.extractLabeled( p, 0 )
		ofile = 'threat.harms'
		opath = os.path.join( exportDir, ofile + tag + '.sdif' ) 
		print 'exporting %i harmonic partials (%s)'%(p.size(), time.ctime(time.time()))
		fpartials = loris.SdifFile( p )
		fpartials.write( opath )
		
		print 'rendering harmonic partials (%s)'%(time.ctime(time.time()))
		fsamps = loris.AiffFile( p, rate )
		opath = os.path.join( exportDir, ofile + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		fsamps.write( opath )
		
		# export non-harmonic junk only
		ofile = 'threat.junk'
		opath = os.path.join( exportDir, ofile + tag + '.sdif' ) 
		print 'exporting %i non-harmonic junk partials (%s)'%(junk.size(), time.ctime(time.time()))
		loris.setBandwidth( junk, 1 )
		fpartials = loris.SdifFile( junk )
		fpartials.write( opath )
		
		print 'rendering non-harmonic junk partials (%s)'%(time.ctime(time.time()))
		fsamps = loris.AiffFile( junk, rate )
		opath = os.path.join( exportDir, ofile + tag + '.recon.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		fsamps.write( opath )

	print 'Done. (%s)'%(time.ctime(time.time()))

# ----------------------------------------------------------------------------

if __name__ == '__main__':
	print __doc__

	print 'Using Loris version %s'%( loris.version() )

	import sys
	odir = os.curdir
	if len( sys.argv ) > 1:
		tag = '.' + sys.argv[1]
		
	doAlien( odir )

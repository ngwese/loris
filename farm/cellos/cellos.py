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

Last updated: 26 May 2006 by Kelly Fitz
"""
print __doc__

import loris, time

print """
Using Loris version %s
"""%loris.version()

orate = 44100


#
# 154.F tone
#
name = 'cello154.F'
anal = loris.Analyzer( 123, 154 )
f = loris.AiffFile( name + '.aiff' )
print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
p = anal.analyze( f.samples(), f.sampleRate() )

# cello154.F distilled using 3d harmonic
ref = loris.createFreqReference( p, 154*0.9, 1541.1 )
loris.channelize( p, ref, 1 )
loris.distill( p )
print 'synthesizing distilled %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( p, orate )
loris.exportAiff( name + '.recon.aiff', samples, orate )
loris.exportSdif( name + '.sdif', p )


#
# 69.F tone	
#
name = 'cello69.F'
anal = loris.Analyzer( 20, 69 )
f = loris.AiffFile( name + '.aiff' )
print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
p = anal.analyze( f.samples(), f.sampleRate() )

# 69.MF tone collated
pcollate = loris.PartialList( p )
loris.collate( pcollate )
print 'synthesizing raw (collated) %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( pcollate, orate )
loris.exportAiff( name + '.raw.aiff', samples, orate )
loris.exportSdif( name + '.raw.sdif', pcollate )


# distill using two partials per harmonic
ref = loris.createFreqReference( p, 69*0.9, 69*1.1 )
loris.channelize( p, ref, 2 )
loris.distill( p )
print 'synthesizing distilled %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( p, orate )
loris.exportAiff( name + '.recon.aiff', samples, orate )
loris.exportSdif( name + '.sdif', p )

#
# 69.MF tone	
#
name = 'cello69.MF'
anal = loris.Analyzer( 34, 138 )
f = loris.AiffFile( name + '.aiff' )
print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
p = anal.analyze( f.samples(), f.sampleRate() )

# 69.MF tone collated
pcollate = loris.PartialList( p )
loris.collate( pcollate )
print 'synthesizing raw (collated) %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( pcollate, orate )
loris.exportAiff( name + '.raw.aiff', samples, orate )
loris.exportSdif( name + '.raw.sdif', pcollate )


# distill using two partials per harmonic
ref = loris.createFreqReference( p, 69*0.9, 69*1.1 )
loris.channelize( p, ref, 2 )
loris.sift( p )
zeros = loris.extractLabeled( p, 0 )
loris.distill( p )
print 'synthesizing distilled %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( p, orate )
loris.exportAiff( name + '.recon.aiff', samples, orate )
loris.exportSdif( name + '.sdif', p )


#
# arco cellos (new May 2006)
#
def nnToHz( nn ):
	import math
	pshift = (nn-69)/12.0
	hz = 440.0 * math.pow( 2.0, pshift )
	return hz

def analyze_arco( name, midiNN ):
	from loris import *
	
	fHz = nnToHz( midiNN )
	Fade = 0.01
	
	print "analyzing", name + ".aiff", "at fundamental", fHz, "Hz", time.ctime(time.time())
	f = AiffFile( name + ".aiff" )
	
	anal = Analyzer( .85*fHz, 1.65*fHz )
	anal.setBwRegionWidth( 0 )
	
	p = anal.analyze( f.samples(), f.sampleRate() )

	print "distilling", name, ",", p.size()," partials, fade time ", Fade * 1000, "ms"
	ref = createFreqReference( p, .9*fHz, 1.1*fHz );
	channelize( p, ref, 1 )
	distill( p, Fade )
	
	# chop off any times before 0
	crop( p, 0, 20 )
	
	print "synthesizing", name,  ",", p.size(), "partials"
	out_sfile = AiffFile( p, orate )

	print "writing", name + ".recon.aiff"
	out_sfile.setMidiNoteNumber( midiNN )
	out_sfile.addMarkers( f.markers() )
	out_sfile.write( name + ".recon.aiff" );

	print "writing", name + ".sdif"
	out_pfile = SdifFile( p )
	out_pfile.addMarkers( f.markers() )
	out_pfile.write( name + ".sdif" );
	

analyze_arco( "cello.sulA.B3", 59 )
analyze_arco( "cello.sulC.Ab2", 44 )	# lousy
analyze_arco( "cello.sulC.Db2", 37 )
analyze_arco( "cello.sulD.Bb3", 58 )	# mediocre


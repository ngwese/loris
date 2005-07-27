"""
french.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the french speech excerpt used in the
dissertation experiments. It is taken from a radio broadcast
included on the Pierre Schaeffer collection. The speaker has
a very low (male) voice, which was difficult to reproduce
without a slightly reverberant quality.

The parameters we once liked for this are resolution 60 and
window width 200 Hz.

notes from trial 1:
	- 80 Hz is unusable
	- 60,200 is a little noisy, as is 40,200
	- 60,300 sounds a little hoarse, as does 40,300
	- 60,100 is unusable
	
notes from trial 2:
	- turned off BW association
	- all of these sound kind of hoarse, 250 are also a little crunchy (?),
	150 windows are better.

Made a fundamental frequency reference in Kyma.

notes from trial 3:
	- tracking analysis seems much worse than non-tracking!
	
notes from trial 4:
	- taking an analysis from trial 3, I generated a new fundamental by
	taking a weighted average of the loudest (>35dB) harmonic partals. 
	Then I used this new fundamental and got much better results than 
	trial 3! This is cool, because it gives me a pretty good way of 
	generating a fundamental reference that I can use in a tracking
	analysis. 
	- try more analysis configuations with this new fundamental
	- do we need to sift?
	- raw synthesis of 250 Hz window sounds better than 150 Hz
	- this is dumb, just noticed that I was generating and then 
	throwing out (writing over) the one partial per harmonic version.
	
notes from trial 5:
	- want to try to make the time-domain synthesized waveform look 
	like the original, trying sifting instead of distilling to minimize
	the amount of manipulation of analysis data
	
	- tried after improving phase correction in ReassignedSpectrum,
	but it doesn't seem to make much difference.

notes from trial 6: 
	- trying my fundamental estimator and the voiced/unvoiced detection 
	scheme that I have been working on, this could be extremely slow
	
Updated 8 Jun 05 to work with Loris 1.3.


Last updated: 8 Jun 2005 by Kelly Fitz
"""
print __doc__

import loris, time

# use this trial counter to skip over
# eariler trials
trial = 6

print "running trial number", trial, time.ctime(time.time())

source = 'french.aiff'
file = loris.AiffFile( source )
samples = file.samples()
rate = file.sampleRate()

def makeSpc( fname, partials ):
	N_SPC_PARTIALS = 200
	spc = loris.PartialList()
	for p in partials:
		if p.label() <= N_SPC_PARTIALS:
			spc.append( p )
	print 'exporting %i spc partials'%spc.size() 
	loris.exportSpc( fname + '.s.spc', spc, 60, 0 )
	loris.exportSpc( fname + '.e.spc', spc, 60, 1 )


# if trial == 1:
#	from trials import *
# 	resolutions = (40,60,80)
# 	widths = ( 100,200,300 )
# 	for r in resolutions:
# 		for w in widths:
# 			p = analyze( source, r, w )
# 			ofile = 'french.%i.%i.aiff'%(r, w)
# 			synthesize( ofile, p )
# 
if trial == 2:
	resolutions = ( 40,60 )
	widths = ( 150,250 )
	for r in resolutions:
		for w in widths:
			a = loris.Analyzer( r, w )
			# turn off BW association for now
			a.setBwRegionWidth( 0 )
			p = a.analyze( samples, rate )
			ofile = 'french.%i.%i.raw'%(r, w)
			# collate
			loris.distill( p )
			# export
			loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
			loris.exportSpc( ofile + '.s.spc', p, 60, 0 ) 
			loris.exportSpc( ofile + '.e.spc', p, 60, 1 ) 

if trial == 3:
	print "obtaining fundamental frequency reference"
	pref = loris.extractLabeled( loris.importSpc('french.fund.clean.spc'), 1 )
	ref = loris.createFreqReference( pref, 30, 300 )
	resolutions = ( 40, 60 )
	widths = ( 150, 250 )
	for r in resolutions:
		for w in widths:
			a = loris.Analyzer( r, w )
			# turn off BW association for now
			a.setBwRegionWidth( 0 )
			p = a.analyze( samples, rate, ref )
			ofile = 'french.%i.%i'%(r, w)
			loris.exportAiff( ofile + '.raw.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
			# channelize
			loris.channelize( p, ref, 1 )
			#loris.sift( p )
			#zeros = extractLabeled( p, 0 )
			#print 'sifted out', zeros.size(), 'partials'
			loris.distill( p )
			# export
			loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
			loris.exportSdif( ofile + '.sdif', p )
			makeSpc( ofile, p )

if trial == 4:
	print "obtaining fundamental frequency reference"
	pref = loris.extractLabeled( loris.importSpc('harmonicfund.s.spc'), 1 )
	ref = loris.createFreqReference( pref, 30, 300 )
	resolutions = ( 40, 60, 80 )
	widths = (  120, 150, 250 )
	for r in resolutions:
		for w in widths:
			a = loris.Analyzer( r, w )
			# turn off BW association for now
			a.setBwRegionWidth( 0 )
			p = a.analyze( samples, rate, ref )
			p2 = loris.PartialList( p )
			ofile = 'french.%i.%i'%(r, w)
			loris.exportAiff( ofile + '.raw.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
			# one partial per harmonic
			# channelize
			loris.channelize( p, ref, 1 )
			#loris.sift( p )
			#zeros = extractLabeled( p, 0 )
			#print 'sifted out', zeros.size(), 'partials'
			loris.distill( p )
			# export
			loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
			loris.exportSdif( ofile + '.sdif', p )
			makeSpc( ofile, p )
			# two partials per harmonic
			p = p2
			# channelize
			loris.channelize( p, ref, 2 )
			#loris.sift( p )
			#zeros = extractLabeled( p, 0 )
			#print 'sifted out', zeros.size(), 'partials'
			loris.distill( p )
			# export
			loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
			loris.exportSdif( ofile + '.sdif', p )
			makeSpc( ofile, p )

if trial == 5:
	print "obtaining fundamental frequency reference"
	pref = loris.extractLabeled( loris.importSpc('harmonicfund.s.spc'), 1 )
	ref = loris.createFreqReference( pref, 30, 300 )
	resolutions = ( 60, )
	widths = ( 150, 250 )
	for r in resolutions:
		for w in widths:
			a = loris.Analyzer( r, w )
			# turn off BW association for now
			a.setBwRegionWidth( 0 )
			p = a.analyze( samples, rate, ref )
			p2 = loris.PartialList( p )
			ofile = 'french.%i.%i.np'%(r, w)
			loris.exportAiff( ofile + '.raw.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
			loris.exportSdif( ofile + '.raw.sdif', p )
			# one partial per harmonic
			# channelize
			loris.channelize( p, ref, 1 )
			loris.sift( p )
			zeros = loris.extractLabeled( p, 0 )
			print 'sifted out', zeros.size(), 'partials'
			loris.distill( p )
			# export
			loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
			loris.exportSdif( ofile + '.sdif', p )
			makeSpc( ofile, p )

# makeReference
# Use the fundamental estimator to construct
# a reference frequency envelope.
#
# Seems like in this case we can just use the energy
# to determine when to save estimates.
CurrentVoiced = -1
Voiced = 1
Unvoiced = 0
MarkVUV = []
def voice( vuv, t ):
	global CurrentVoiced
	if vuv != Voiced and vuv != Unvoiced:
		raise 'invalid voiced/unvoiced specification:', vuv
	if vuv != CurrentVoiced:
		if vuv == Voiced:
			print "switch to voiced at time", t
			MarkVUV.append( loris.Marker( t, "V" ) )
		else:
			print "switch to unvoiced at time", t
			MarkVUV.append( loris.Marker( t, "UV" ) )
		CurrentVoiced = vuv
	

def makeReference( partials, fmin, fmax ):
	from loris import *
	import os,sys
# 	wheresFund = os.getenv('HOME') + os.sep + 'Gumballs' + os.sep + 'Fundamental'
# 	sys.path.append( wheresFund )
# 	sys.path.append( wheresFund + os.sep + '.libs' )
	import fundamental
	ref = LinearEnvelope()
	lowOnes = PartialList()
	for p in partials:
		if p.frequencyAt(0) < 2500:
			lowOnes.append( p )
	est = fundamental.createEstimator( lowOnes, fmin, fmax )
	est.setAmpThreshold( -45 )
	print "constructing refrence frequency envelope..."
	( tmin, tmax ) = timeSpan( lowOnes )
	t = tmin
	while t < tmax:
		e = fundamental.energyDB( lowOnes, t )
		# save fundamental estimates whenever there is
		# sufficient energy:
		if e > -60:
			# can this be used as a voicing estimator too?
			voice( Voiced, t )
			#c = fundamental.spectralCentroid( partials, t )
			try:
				val = est.valueAt( t )
				print "(fundamental estimate at time", t, "is", val, ")"
				ref.insertBreakpoint( t, val )
			except(ValueError):
				print "(FAILED to obtain fundamental estimate at time", t
				print " enegy in low partials is %i dB)"%(e)
		else:
			voice( Unvoiced, t )
		t = t + 0.015
	return ref
	
def addMarkers( markers, file ):
	file.setMarkers( markers )

if trial == 6:
	r = 60
	w = 150	
	a = loris.Analyzer( r, w )
	# turn off BW association for now
	a.setBwRegionWidth( 0 )
	print 'performing raw analysis, no, bandwidth, no tracking'
	raw = a.analyze( samples, rate )
	## construct a reference frequency envelope
	ref = makeReference( raw, 60, 150 )
	print 'performing tracking analysis'
	p = a.analyze( samples, rate, ref )
	# export raw
	fnamebase = 'french.est.raw'
	ofile = loris.AiffFile( p, rate )
	ofile.write( fnamebase + '.aiff' )
	pfile = loris.SdifFile( p )
	pfile.write( fnamebase + '.sdif' )
	# channelize one partial per harmonic
	loris.channelize( p, ref, 1 )
	loris.sift( p )
	zeros = loris.extractLabeled( p, 0 )
	print 'sifted out', zeros.size(), 'partials'
	loris.distill( p )
	# export distilled
	fnamebase = 'french.est'
	ofile = loris.AiffFile( p, rate )
	addMarkers( MarkVUV, ofile )
	ofile.write( fnamebase + '.aiff' )
	pfile = loris.SdifFile( p )
	addMarkers( MarkVUV, pfile )
	pfile.write( fnamebase + '.sdif' )
	makeSpc( fnamebase, p )
	
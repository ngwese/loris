#
#	trials.py
#
#	Kelly Fitz 8 March 2001
#
"""
trials.py

Python script for analyzing and reconstructing a variety 
of sounds used to test the analysis/modification/synthesis
routines in Loris.

Last updated: 8 March 2001 by Kelly Fitz
"""

import loris, time

#
#	define useful functions:
#
def analyze( filename, res, width = 0, bwregionwidth = 2000 ):
	"""usage: partials = analyze( filename, res, width, bwregionwidth )
	
	Analyze a specified file with the given arguments. 
	Frequency resolution must be specified. Window width may
	be specified, and defaults to the frequency resolution.
	Bandwidth region width may be specified, and defaults to 
	2000 Hz. Frequency floor and frequency drift are always set 
	equal to the frequency resolution. 
	"""
	print 'analyzing %s (%s)'%(filename, time.ctime(time.time()))
	# configure Analyzer:
	if width == 0:
		width = res
	a = loris.Analyzer( res, width )
	a.setBwRegionWidth( bwregionwidth )
	# get sample data:
	f = loris.AiffFile( filename )
	samples = f.samples()
	rate = f.sampleRate()
	# analyze and return partials
	partials = a.analyze( samples, rate )
	return partials
	
def synthesize( filename, partials ):
	"""usage: synthesize( filename, partials )
	
	Synthesize a list of partials, and save the samples in
	an AIFF file having the specified path or name.
	"""
	print 'synthesizing %s (%s)'%(filename, time.ctime(time.time()))
	rate = 44100
	bits = 16
	chans = 1
	samples = loris.synthesize( partials, rate )
	loris.exportAiff( filename, samples, rate, chans, bits )
	
def savesdif( partials, filename ):
	"""usage: savesdif( partials, filename )
	
	Save a list of partials in a SDIF file having the 
	specified path or name.
	"""
	print 'exporting %s (%s)'%(filename, time.ctime(time.time()))
	loris.exportSdif( filename, partials ) 
	
def timescale( partials, scale ):
	"""usage: timescale( partials, scale )
	
	Dilate or compress a list of partials by the specified
	scale factor. A scale of 2.0 makes the sound twice as long.
	"""
	# no sound used here will be longer than 20 seconds:
	initial = str( [20.] )
	target = str( [20. * scale] )
	loris.dilate( partials, initial, target )
	return partials
	
def pitchshift( partials, cents ):
	"""usage: pitchshift( partials, cents )
	
	Shift the pitch of a list of quasi-harmonic partials by 
	the specified number of cents. Shifting by 1200 corresponds
	to shifting up by an octave.
	"""
	env = loris.BreakPointEnvelopeWithValue( cents )
	loris.shiftPitch( partials, env )
	return partials
	
def harmonicDistill( partials, fundamental ):
	"""usage: harmonicDistill( partials, fundamental )
	
	Channelize and distill a list of quasi-harmonic partials
	using channels of constant width and center frequency.
	The specified fundamental is used as the center frequency
	of the lowest channel, and the width of all channels.

	No sifting is performed on the partials.
	"""
	env = loris.BreakpointEnvelopeWithValue( fundamental )
	loris.channelize( partials, env, 1 )
	loris.distill( partials )
	return partials
	
def harmonicSift( partials, fundamental ):
	"""usage: harmonicSift( partials, fundamental )
	
	Channelize and distill a list of quasi-harmonic partials
	using channels of constant width and center frequency.
	The specified fundamental is used as the center frequency
	of the lowest channel, and the width of all channels.
	
	Before distilling, the partials are sifted, and partials
	rejected by the Sieve (those labeled 0) are removed.
	
	env = loris.BreakpointEnvelopeWithValue( fundamental )
	loris.channelize( partials, env, 1 )
	# do sifting and removal of sifted-out partials:
	loris.sift( partials )
	iter = partials.begin()
	end = partials.end()
	while not iter.equals( end ):
		p = iter.partial()
		if p.label() == 0:
			next = iter.copy()
			next.next()
			partials.erase( iter )
			iter = next
		else:
			iter.next()
	loris.distill( partials )
	return partials
	
def buildEnvelope( TVpairs ):
	"""usage: BPenv = buildEnvelope( TVpairList )
	
	Builds a BreakpointEnvelope from a list (or tuple)
	of time,value pairs.
	"""
	env = loris.BreakpointEnvelope()
	for (time,value) in TVpairs:
		env.insertBreakpoint( time, value )
	return env

if __name__ == '__main__':
	print __doc__
	
	#
	#	file suffixes:
	#
	source = '.aiff'
	synthesis = '.synth.aiff'
	sdif = '.sdif'
		
	#
	#	sound trials:
	#

	# the temple bell used in my dissertation
	timbre = 'bell'
	p = analyze( timbre + source, 60, 105, 2400 )
	synthesize( timbre + synthesis, p )

	# the cello used in my dissertation (154Hz), 
	# using Lippold's amazing parameters
	timbre = 'cello154'
	fun = 154.
	p = analyze( timbre + source, .4 * fun, 1.8 * fun )
	harmonicDistill( p, fun )
	synthesize( timbre + synthesis, p )

	# Lippold's very low cello (69.2 Hz) (also called OCEL37F),
	# using Lippold's amazing parameters
	timbre = 'cello69'
	fun = 69.2
	p = analyze( timbre + source, .4 * fun, 1.8 * fun )
	harmonicDistill( p, fun )
	synthesize( timbre + synthesis, p )

	# the flute we use everywhere (291 Hz) (3 D)
	timbre = 'flute291'
	fun = 291.
	p = analyze( timbre + source, .8 * fun )
	harmonicDistill( p, fun )
	synthesize( timbre + synthesis, p )

	# sax riff used in dissertation work
	# need a fancy channelizer for this one, and also
	# probably need to break partials at boundaries
	timbre = 'saxriff'
	p = analyze( timbre + source, 90, 140 )
	synthesize( timbre + synthesis, p )


// ===========================================================================
//	Synthesizer.C
//	
//	Implementation of Loris::Synthesizer.
//	
//	Loris synthesis generates a buffer of samples from a 
//	collection of Partials. 
//
//	-kel 16 Aug 99
//
// ===========================================================================

#include "LorisLib.h"

#include "Synthesizer.h"
#include "Exception.h"
#include "SampleBuffer.h"
#include "Oscillator.h"
#include "Partial.h"
#include "Breakpoint.h"

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
	using std::sqrt;
#else
	#include <math.h>
#endif


Begin_Namespace( Loris )

#pragma mark -
#pragma mark construction

// ---------------------------------------------------------------------------
//	Synthesizer constructor
// ---------------------------------------------------------------------------
//	If no Oscillator (or a Null Oscillator) is specified, create a default
//	Oscillator.
//
Synthesizer::Synthesizer( SampleBuffer & buf, double srate, double minBWEfreq, Oscillator * osc ) :
	_samples( buf ),
	_sampleRate( srate ),
	_oscillator( (osc != Null) ? (osc) : (Oscillator::Create()) ),
	_kludger( minBWEfreq )
{
	//	check to make sure that the sample rate is valid:
	Assert( _sampleRate > 0. );
}

// ---------------------------------------------------------------------------
//	Synthesizer destructor
// ---------------------------------------------------------------------------
//
Synthesizer::~Synthesizer( void )
{
	delete _oscillator;
}

#pragma mark -
#pragma mark synthesis

// ---------------------------------------------------------------------------
//	synthesizePartial
// ---------------------------------------------------------------------------
//	No clicks aloud!
//	All Partials begin and end at zero amplitude. If possible, the ramp
//	on or off is pasted on the beginning and end of the Partial, but if 
//	the Partial is too close to the buffer boundary (e.g. it starts at time
//	0.), then the head or tail of the Partial gets smooshed a little. Phase
//	is always corrected, and is never altered by the ramping. 
//
//	The old bandwidth kludge is in here. Breakpoints below a certain 
//	frequency (mBweCutoff) are given bandwidths of zero, because low
//	frequency noise is way too strong otherwise. FIX SOMEDAY!
//	
void
Synthesizer::synthesizePartial( const Partial & p )
{
	if ( p.duration() == 0. )
		return;
		
//	reset the oscillator:
//	Remember that the oscillator only knows about radian frequency! Convert!
	const Breakpoint * bp = p.head();
	double rads = radianFreq( bp->frequency() );
	_oscillator->reset( rads, _kludger.amp( *bp ), _kludger.bw( *bp ), bp->phase() );

//	initialize sample offsets:
	ulong bpSampleOffset = bp->time() * sampleRate() + 0.5;	// cheap, portable rounding

//	synthesize Partial turn-on if necessary and possible:
	const int rampLen = 0.001 * sampleRate() + 0.5;	//	1 ms
	
	if ( bp->amplitude() > 0. ) {
		//	calculate the start time for the ramp, 
		int rampStart = bpSampleOffset - rampLen;
		if ( rampStart < 0 )
			rampStart = 0;
		
		//	roll back the phase so that it is correct
		//	at the time of the first real Breakpoint
		//	(not nec. the time that the top of the amplitude
		//	ramp is reached, if the Partial starts very 
		//	early):
		_oscillator->setPhase( _oscillator->phase() - ( rads * (bpSampleOffset - rampStart) ) );
		
		//	recalculate the bpSampleOffset if necessary:
		if ( bpSampleOffset < rampLen )
			bpSampleOffset = rampLen;
		
		//	ramp up from zero amplitude:
		_oscillator->setAmplitude( 0. );
		
		//	generate rampLen samples before bpSampleOffset
		//	targeting the radian frequency, amplitude, and 
		//	bandwidth of the first real Breakpoint (the 
		//	frequency and bandwidth have been reset to
		//	these same values already, the amplitude to 
		//	zero):
		_oscillator->generateSamples( _samples, rampLen, rampStart, rads,
									_kludger.amp( *bp ), _kludger.bw( *bp ) );
	}

//	synthesize linear-frequency segments:
	for ( bp = bp->next(); bp != Null; bp = bp->next() ) {
		//	compute the number of samples to generate:
		//	By computing each Breakpoint offset this way, 
		//	(instead of computing nsamps from the time difference
		//	between consecutive envelope breakpoints) we
		//	obviate the running fractional sample total we
		//	used to need.
		int nsamps = ( bp->time() * sampleRate() ) - bpSampleOffset + 0.5;
		Assert( nsamps >= 0 );
		
		//	don't generate samples all the way to the end of the
		//	buffer, make sure to leave enough for ramping off:
		if ( bpSampleOffset + nsamps + rampLen > _samples.size() )
			nsamps = _samples.size() - bpSampleOffset - rampLen;
		
		//	generate nsamps samples starting at bpSampleOffset
		//	targeting the radian frequency, amplitude, and 
		//	bandwidth of the next Breakpoint bp:
		_oscillator->generateSamples( _samples, nsamps, bpSampleOffset,
									  radianFreq( bp->frequency() ), 
									  _kludger.amp( *bp ), _kludger.bw( *bp ) );

		//	update the offset:	
		bpSampleOffset += nsamps;
	}
	
//	synthesize Partial turn-off if necessary:
	if ( _oscillator->amplitude() > 0. ) {
		//	generate rampLen samples starting at bpSampleOffset
		//	targeting zero amplitude, and not changing the frequency
		//	or bandwidth:
		_oscillator->generateSamples( _samples, rampLen, bpSampleOffset,
									  _oscillator->radianFreq(), 0., _oscillator->bandwidth() );
	}


}

#pragma mark -
#pragma mark kludging

// ---------------------------------------------------------------------------
//	radianFreq
// ---------------------------------------------------------------------------
//	
inline double 
Synthesizer::radianFreq( double hz ) const
{
	return hz * TwoPi / sampleRate();
}

// ---------------------------------------------------------------------------
//	kludger amp
// ---------------------------------------------------------------------------
//	
inline double
Synthesizer::BweKludger::amp( const Breakpoint & bp ) const
{
	if ( bp.frequency() > _cutoff )
		return bp.amplitude();
	else
		return bp.amplitude() * sqrt(1. - bwclamp( bp.bandwidth() ) );
}

// ---------------------------------------------------------------------------
//	kludger bw
// ---------------------------------------------------------------------------
//	
inline double
Synthesizer::BweKludger::bw( const Breakpoint & bp ) const
{
	if ( bp.frequency() > _cutoff )
		return bwclamp( bp.bandwidth() );
	else
		return 0.;
}

// ---------------------------------------------------------------------------
//	kludger bwclamp
// ---------------------------------------------------------------------------
//	
inline double
Synthesizer::BweKludger::bwclamp( double bw ) const
{
	if( bw > 1. )
		return 1.;
	else if ( bw < 0. )
		return 0.;
	else
		return bw;
}

End_Namespace( Loris )

// ===========================================================================
//	Oscillator.C
//	
//	Implementation of Loris::Oscillator.
//	
//	Loris synthesis generates a buffer of samples from a 
//	collection of Partials. The Loris Synthesizer uses an Oscillator
//	to generate samples according to parameters interpolated from
//	pairs of Breakpoints.
//
//	-kel 31 Aug 99
//
// ===========================================================================


#include "LorisLib.h"
#include "Oscillator.h"
#include "SampleBuffer.h"
#include "Noise.h"
#include "Filter.h"

#if !defined(USE_DEPRECATED_HEADERS)
	#include <cmath>
	using std::fmod;
	using std::cos;
#else
	#include <math.h>
#endif

Begin_Namespace( Loris )

#pragma mark -
#pragma mark construction

// ---------------------------------------------------------------------------
//	Oscillator construction
// ---------------------------------------------------------------------------
//	Initialize state to something, like zeros.
//
Oscillator::Oscillator( void ) :
	_frequency( 0. ),	//	radians per sample
	_amplitude( 0. ),	//	absolute
	_bandwidth( 0. ),	//	bandwidth coefficient (noise energy / total energy)
	_phase( 0. ),		//	radians
	_filter( Mkfilter::Create() )
{
}

// ---------------------------------------------------------------------------
//	Oscillator destruction
// ---------------------------------------------------------------------------
//	Initialize state to something, like zeros.
//
Oscillator::~Oscillator( void )
{
	delete _filter;
}

// ---------------------------------------------------------------------------
//	Oscillator virtual constructor
// ---------------------------------------------------------------------------
//	tell me again, why did I do this?
//
Oscillator *
Oscillator::Create( void )
{
	return new Oscillator();
}

#pragma mark -
#pragma mark state access/mutation
// ---------------------------------------------------------------------------
//	reset
// ---------------------------------------------------------------------------
//
void
Oscillator::reset( Double radf, Double amp, Double bw, Double ph )
{
	setRadianFreq( radf );
	setAmplitude( amp );
	setBandwidth( bw );
	setPhase( ph );
}

#pragma mark -
#pragma mark sample generation

// ---------------------------------------------------------------------------
//	generateSamples
// ---------------------------------------------------------------------------
//	Compute howMany samples and sum them into buffer starting at offset.
//	Modulate BW-enhanced partial parameters from their current values to
//	the specified new values.
//
//	In Lemur, we used to synthesize at zero amplitude above the Nyquist 
//	rate, could stick that in here sometime.
//
void
Oscillator::generateSamples( SampleBuffer & buffer, Int howMany, Int offset,
							 Double targetFreq, Double targetAmp, Double targetBw )
{
	if ( howMany <= 0 )
		return;
		
//	compute trajectories:
	const Double dFreq = (targetFreq - _frequency) / howMany;
	const Double dAmp = (targetAmp - _amplitude) / howMany;
	const Double dBw = (targetBw - _bandwidth) / howMany;
	
	for ( Int i = 0; i < howMany; ++i )
	{
//	compute a sample and add it into the buffer.			
		buffer[ offset + i ] += 
			modulate( _bandwidth ) * _amplitude * oscillate( _phase );
		
//	update the oscillator state:
		_phase += _frequency;	//	_frequency is radians per sample
		_frequency += dFreq;
		_amplitude += dAmp;
		_bandwidth += dBw;
		
	}	// end of sample computation loop

//	normalize the state phase to prevent eventual loss of precision at
//	high oscillation frequencies:
//	(Doesn't really matter much what fmod does, as long as it brings 
//	the phase nearer to zero.)
	_phase = fmod( _phase, TwoPi );
}

#pragma mark -
#pragma mark private helpers

// ---------------------------------------------------------------------------
//	oscillate
// ---------------------------------------------------------------------------
//	Could try doing this with a lookup table too, but it seems to be
//	very little computation compared to modulate.
//
inline Double 
Oscillator::oscillate( Double phase ) const
{
	return cos( phase );
}

// ---------------------------------------------------------------------------
//	modulate
// ---------------------------------------------------------------------------
//	Yuck, Jackson, two square roots!
//
inline Double 
Oscillator::modulate( Double bandwidth ) const
{
//	clamp bandwidth:
	if ( bandwidth > 1. )
		bandwidth = 1.;
	else if ( bandwidth < 0. )
		bandwidth = 0.;
	
//	get a filtered noise sample, use scale as std deviation:
//	can build scale into filter gain.
	Double noise = _filter->nextSample( gaussian_normal() );

//	compute modulation:
//
//	This will give the right amplitude modulation when scaled
//	by the Partial amplitude:
//
//	carrier amp: sqrt( 1. - bandwidth ) * amp
//	modulation index: sqrt( 2. * bandwidth ) * amp
//	filtered noise is modulator
//
	return sqrt( 1. - bandwidth ) + ( noise * sqrt( 2. * bandwidth ) );	
}

End_Namespace( Loris )

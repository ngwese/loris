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

#if !defined( Deprecated_cstd_headers )
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
//	Default f is an auto_ptr with no reference, indicating that a default 
//	filter should be creted and used.
//
//	auto_ptr is used to submit the Filter argument to make explicit the
//	source/sink relationship between the caller and the Oscillator. After
//	the call, the Oscillator will own the Filter, and the client's auto_ptr
//	will have no reference (or ownership).
//
Oscillator::Oscillator( std::auto_ptr< Filter > f ) :
	_frequency( 0. ),	//	radians per sample
	_amplitude( 0. ),	//	absolute
	_bandwidth( 0. ),	//	bandwidth coefficient (noise energy / total energy)
	_phase( 0. )		//	radians
{
	//	initialize the filter:
	setFilter( f );
}

// ---------------------------------------------------------------------------
//	Oscillator copy construction
// ---------------------------------------------------------------------------
//	This will have to be modified if Filter ever becomes a base class.
//
Oscillator::Oscillator( const Oscillator & other ) :
	_frequency( other._frequency ),
	_amplitude( other._amplitude ),
	_bandwidth( other._bandwidth ),
	_phase( other._phase ),
	_filter( new Filter( *other._filter ) )
{
}	

// ---------------------------------------------------------------------------
//	assignment
// ---------------------------------------------------------------------------
//	This will have to be modified if Filter ever becomes a base class.
//
Oscillator &
Oscillator::operator= (const Oscillator & other )
{
	//	do nothing if assigning to self:
	if ( &other != this ) {	
		_frequency = other._frequency;
		_amplitude = other._amplitude;
		_bandwidth = other._bandwidth;
		_phase = other._phase;
		_filter.reset( new Filter( *other._filter ) );
	}
	
	return *this;
}

#pragma mark -
#pragma mark state access/mutation
// ---------------------------------------------------------------------------
//	reset
// ---------------------------------------------------------------------------
//
void
Oscillator::reset( double radf, double amp, double bw, double ph )
{
	setRadianFreq( radf );
	setAmplitude( amp );
	setBandwidth( bw );
	setPhase( ph );
}

// ---------------------------------------------------------------------------
//	setFilter
// ---------------------------------------------------------------------------
//	Default f is an auto_ptr with no reference, indicating that a default 
//	filter should be created and used.
//
//	auto_ptr is used to submit the Filter argument to make explicit the
//	source/sink relationship between the caller and the Oscillator. After
//	the call, the Oscillator will own the Filter, and the client's auto_ptr
//	will have no reference (or ownership).
//
void
Oscillator::setFilter( std::auto_ptr< Filter > f )
{	
	if ( ! f.get() )
		f.reset( new Filter( Filter::NormalCoefs().first, Filter::NormalCoefs().second ) );
	
	_filter = f;
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
//	rate, could stick that in here sometime, except that the Osciillator
//	has no knowledge of sample rate... Should be handled by the Synthesizer.
//
void
Oscillator::generateSamples( SampleBuffer & buffer, long howMany, long offset,
							 double targetFreq, double targetAmp, double targetBw )
{
//	if no samples are generated, set the oscillator state to the
//	target values anyway:
	if ( howMany <= 0 ) {
		_frequency = targetFreq;
		_amplitude = targetAmp;
		_bandwidth = targetBw;
		return;
	}
		
//	compute trajectories:
	const double dFreq = (targetFreq - _frequency) / howMany;
	const double dAmp = (targetAmp - _amplitude) / howMany;
	const double dBw = (targetBw - _bandwidth) / howMany;
	
	for ( int i = 0; i < howMany; ++i )
	{
//	compute a sample and add it into the buffer.
		//	don't insert samples at indices less than zero, but
		//	compute the sample anyway, because the oscillator and
		//	modulator might have state that need to be updated:
		double x = modulate( _bandwidth ) * _amplitude * oscillate( _phase );
		if ( offset + i >= 0 ) 	
			buffer[ offset + i ] += x;
			
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
inline double 
Oscillator::oscillate( double phase ) const
{
	return cos( phase );
}

// ---------------------------------------------------------------------------
//	modulate
// ---------------------------------------------------------------------------
//	Yuck, Jackson, two square roots!
//
inline double 
Oscillator::modulate( double bandwidth ) const
{
//	clamp bandwidth:
	if ( bandwidth > 1. )
		bandwidth = 1.;
	else if ( bandwidth < 0. )
		bandwidth = 0.;
	
//	get a filtered noise sample, use scale as std deviation:
//	can build scale into filter gain.
	double noise = _filter->nextSample( gaussian_normal() );

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

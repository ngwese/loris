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
#include "Filter.h"
#include "random.h"
#include "Exception.h"
#include <vector>

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
#else
	#include <math.h>
#endif

using namespace std;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	Oscillator construction
// ---------------------------------------------------------------------------
//	Initialize state to something, like zeros.
//
Oscillator::Oscillator( void ) :
	_frequency( 0. ),	//	radians per sample
	_amplitude( 0. ),	//	absolute
	_bandwidth( 0. ),	//	bandwidth coefficient (noise energy / total energy)
	_phase( 0. )		//	radians
{
	//	Chebychev order 3, cutoff 500, ripple -1.
	static const double gain = 4.663939184e+04;
	static const double extraScaling = 4.5;	//	was 6.
	static const double maCoefs[] = { 1., 3., 3., 1. }; 
	static const double arCoefs[] = { 0.9320209046, -2.8580608586, 2.9258684252, 0. };
						   
	_filter.reset( new Filter( vector< double >( maCoefs, maCoefs + 4 ), 
							   vector< double >( arCoefs, arCoefs + 4 ),
							   gain / extraScaling ) );
}

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
Oscillator::generateSamples( vector< double > & buffer, long howMany, long offset,
							 double targetFreq, double targetAmp, double targetBw )
{
//	caller is responsible for making sure that
//	the sample buffer indices are valid:
	Assert( offset > 0 );
	Assert( offset + howMany < buffer.size() );
	Assert( howMany >= 0 );
	
//	clamp bandwidth:
	if ( targetBw > 1. )
		targetBw = 1.;
	else if ( targetBw < 0. )
		targetBw = 0.;
	
//	generate and accumulate samples:
//	(if no samples are generated, the oscillator state 
//	will be set below to the target values anyway):
	if ( howMany > 0 )
	{
	//	compute trajectories:
		const double dFreq = (targetFreq - _frequency) / howMany;
		const double dAmp = (targetAmp - _amplitude) / howMany;
		const double dBw = (targetBw - _bandwidth) / howMany;
		
	//	temp local variables for speed:
		double f = _frequency, a = _amplitude, b = _bandwidth, p = _phase;
		double noise, mod, osc, samp;
		for ( int i = 0; i < howMany; ++i )
		{
			//
			//	get a filtered noise sample, use scale as std deviation:
			//	can build scale into filter gain.
			noise = _filter->nextSample( gaussian_normal() );

			//	compute modulation:
			//
			//	This will give the right amplitude modulation when scaled
			//	by the Partial amplitude:
			//
			//	carrier amp: sqrt( 1. - bandwidth ) * amp
			//	modulation index: sqrt( 2. * bandwidth ) * amp
			//	filtered noise is modulator
			//
			mod = sqrt( 1. - b ) + ( noise * sqrt( 2. * b ) );	
			osc = cos( p );
			
			//	compute a sample and add it into the buffer:
			samp = mod * a * osc;
			buffer[ offset + i ] += samp;
				
			//	update the oscillator state:
			p += f;	//	frequency is radians per sample
			f += dFreq;
			a += dAmp;
			b += dBw;
			
		}	// end of sample computation loop
		
		//	update phase from loop variable and normalize 
		//	to prevent eventual loss of precision at
		//	high oscillation frequencies:
		//	(Doesn't really matter much exactly what fmod does, 
		//	as long as it brings the phase nearer to zero.)
		_phase = fmod( p, TwoPi );
	}
	
//	set the state variables to their target values,
//	just in case they didn't arrive exactly (overshooting
//	amplitude or, especially, bandwidth, could be bad, and
//	does happen):
	_frequency = targetFreq;
	_amplitude = targetAmp;
	_bandwidth = targetBw;

}

End_Namespace( Loris )

// ===========================================================================
//	Oscillator.C
//	
//	Implementation of Loris::Oscillator, a Bandwidth-Enhanced Oscillator.
//	
//	Loris::Synthesizer uses an instance of Loris::Oscillator to synthesize
//	bandwidth-enhanced partials obtained from Reassigned Bandwidth-Enhanced
//	analysis data.
//
//	-kel 31 Aug 99
//
// ===========================================================================
#include "Oscillator.h"
#include "Filter.h"
#include "random.h"
#include "Exception.h"
#include "pi.h"
#include <vector>

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
#else
	#include <math.h>
#endif

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif


// ---------------------------------------------------------------------------
//	Oscillator construction
// ---------------------------------------------------------------------------
//	Initialize state and construct a Filter. The only potential failure in 
//	this constructor is a failure in the Filter constructor. In that instance,
//	the Filter is not constructed. In all other instances, the Oscillator is
//	successfully constructed, so there is no potential to leak the Filter's
//	memory.
//
Oscillator::Oscillator( double radf, double a, double bw, double ph /* = 0. */ ) :
	_frequency( radf ),	//	radians per sample
	_amplitude( a ),	//	absolute
	_bandwidth( bw ),	//	bandwidth coefficient (noise energy / total energy)
	_phase( ph ),		//	radians
	_filter( NULL )
{
	//	make a Filter:
	//	Chebychev order 3, cutoff 500, ripple -1.
	static const double filter_gain = 4.663939184e+04;
	static const double extraScaling = 4.5;	//	was 6.
	static const double maCoefs[] = { 1., 3., 3., 1. }; 
	static const double arCoefs[] = { 0., 2.9258684252, -2.8580608586, 0.9320209046 };
						   
	_filter = new Filter( maCoefs, maCoefs + 4, 
						  arCoefs, arCoefs + 4,
						  extraScaling / filter_gain );
						  
	//	don't alias:
	if ( _frequency > Pi )
		_amplitude = 0.;
}

// ---------------------------------------------------------------------------
//	Oscillator destruction
// ---------------------------------------------------------------------------
//
Oscillator::~Oscillator( void )
{
	delete _filter;
}

// ---------------------------------------------------------------------------
//	generateSamples
// ---------------------------------------------------------------------------
//	Accumulate bandwidth-enhanced sinusoidal samples modulating the 
//	oscillator state from its current values of radian frequency,
//	amplitude, and bandwidth to the specified target values, starting
//	at beginIdx and ending at (before) endIdx (no sample is accumulated
//	at endIdx). The indices are positions in the specified buffer.
//
//	The caller must insure that the indices are valid. Target frequency
//	and bandwidth are checked to prevent aliasing and bogus bandwidth
//	enhancement.
//
void
Oscillator::generateSamples( std::vector< double > & buffer, 
							 long beginIdx, long endIdx,
							 double targetFreq, double targetAmp, double targetBw )
{
//	use math functions in namespace std:
	using namespace std;
	
//	caller is responsible for making sure that
//	the sample buffer indices are valid, only
//	check sanity when debugging:
#ifdef Debug_Loris
	Assert( beginIdx > 0 );
	Assert( endIdx <= buffer.size() );
	Assert( endIdx >= beginIdx );
#endif
	
//	clamp bandwidth:
	if ( targetBw > 1. )
		targetBw = 1.;
	else if ( targetBw < 0. )
		targetBw = 0.;
		
//	don't alias:
	if ( targetFreq > Pi )	//	radian Nyquist rate
		targetAmp = 0.;
	
//	generate and accumulate samples:
//	(if no samples are generated, the oscillator state 
//	will be set below to the target values anyway):
	long howMany = endIdx - beginIdx;
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
			//	get a filtered noise sample, use scale as std deviation:
			//	can build scale into filter gain.
			noise = _filter->sample( gaussian_normal() );

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
			buffer[ beginIdx + i ] += samp;
				
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
//	it does happen):
	_frequency = targetFreq;
	_amplitude = targetAmp;
	_bandwidth = targetBw;
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * Oscillator.C
 *
 * Implementation of class Loris::Oscillator, a Bandwidth-Enhanced Oscillator.
 *
 * Kelly Fitz, 31 Aug 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include <Loris_prefix.h>
#include "Oscillator.h"
#include "Filter.h"
#include "random.h"
#include "Exception.h"
#include "notifier.h"
#include <vector>
#include <cmath>

//	Pi:
static const double Pi = M_PI;
static const double TwoPi = 2. * M_PI;

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
	const double filter_gain = 4.663939184e+04;
	const double extraScaling = 6.;
	const double maCoefs[] = { 1., 3., 3., 1. }; 
	const double arCoefs[] = { 0., 2.9258684252, -2.8580608586, 0.9320209046 };
						   
	_filter = new Filter( maCoefs, maCoefs + 4, 
						  arCoefs, arCoefs + 4,
						  extraScaling / filter_gain );
						  
//	clamp bandwidth:
	if ( _bandwidth > 1. )
	{
		debugger << "clamping bandwidth at 1." << endl;
		_bandwidth = 1.;
	}
	else if ( _bandwidth < 0. )
	{ 
		debugger << "clamping bandwidth at 0." << endl;
		_bandwidth = 0.;
	}

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
Oscillator::generateSamples( //std::vector< double > & buffer, 
							 //long beginIdx, long endIdx,
							 double * begin, double * end,
							 double targetFreq, double targetAmp, double targetBw )
{
//	use math functions in namespace std:
	using namespace std;
	
//	caller is responsible for making sure that
//	the sample buffer indices are valid, only
//	check sanity when debugging:
#ifdef Debug_Loris
	//Assert( beginIdx >= 0 );
	//Assert( endIdx <= buffer.size() );
	//Assert( endIdx >= beginIdx );
	Assert( end >= begin );
#endif
	
//	clamp bandwidth:
	if ( targetBw > 1. )
	{
		debugger << "clamping bandwidth at 1." << endl;
		targetBw = 1.;
	}
	else if ( targetBw < 0. )
	{ 
		debugger << "clamping bandwidth at 0." << endl;
		targetBw = 0.;
	}
		
//	don't alias:
	if ( targetFreq > Pi )	//	radian Nyquist rate
	{
		debugger << "ramping out Partial above Nyquist rate" << endl;
		targetAmp = 0.;
	}

//	determine the number of samples to generate
//	(none if the amplitude will be zero throughout):	
	long howMany = end - begin; //endIdx - beginIdx;
	if ( targetAmp == 0. && _amplitude == 0. && howMany > 0 ) 
	{
		// debugger << "no samples to generate at index " << beginIdx << endl;

		//	if we don't need to generate samples, 
		//	update the phase anyway (other params
		//	will be updated below), advance the phase 
		//	by the average frequency times the number
		//	of samples (only do this so that Oscillator
		//	is generally useful, in Loris, Synthesizer
		//	will reset phase after this call if the
		//	amplitude goes to zero):
		_phase += (0.5 * (_frequency + targetFreq)) * howMany;
		_phase = fmod( _phase, TwoPi );
		howMany = 0;
	}

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
		double * putItHere = begin;
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
			//buffer[ beginIdx + i ] += samp;
			*putItHere += samp;
			++putItHere;
				
			//	update the oscillator state:
			p += f;	//	frequency is radians per sample
			f += dFreq;
			a += dAmp;
			b += dBw;
			if (b < 0.)
				b = 0.;
			
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

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

#if HAVE_CONFIG_H
	#include <config.h>
#endif

#include <Oscillator.h>
#include <Filter.h>
// #include "random.h"
#include <Exception.h>
#include <Notifier.h>
#include <vector>
#include <cmath>

#if defined(HAVE_M_PI) && (HAVE_M_PI)
	const double Pi = M_PI;
#else
	const double Pi = 3.14159265358979324;
#endif

//	begin namespace
namespace Loris {


// ************* begin contents of random.h *******************

//	trunc() is a problem. It's not in cmath, officially, though
//	Metrowerks has it in there. SGI has it in math.h which is
//	(erroneously!) included in g++ cmath, but trunc is not imported
//	into std. For these two compilers, could just import std. But
//	trnc doesn't seem to exist anywhere in Linux g++, so use std::modf().
//	DON'T use integer conversion, because long int ins't as long 
//	as double's mantissa, see below.
static inline double trunc( double x ) { double y; std::modf(x, &y); return y; }

// ---------------------------------------------------------------------------
//		random number generator
// ---------------------------------------------------------------------------
//	Taken from "Random Number Generators: Good Ones Are Hard To Find," 
//	Stephen Park and Keith Miller, Communications of the ACM, October 1988,
//	vol. 31, Number 10.
//
//	This version will work as long as floating point values are represented
//	with at least a 46 bit mantissa. The IEEE standard 64 bit floating point
//	format (used on the PPC) has a 53 bit mantissa.
//
//	The correctness of the implementation can be checked by confirming that
// 	after 10000 iterations, the seed, initialized to 1, is 1043618065.
//	I have confirmed this. 
//
//	I have also confirmed that it still works (is correct after 10000 
//	iterations) when I replace the divides with multiplies by oneOverM.
//
//	Returns a uniformly distributed random double on the range [0., 1.).
//
//	-kel 7 Nov 1997.
//
inline double
uniform( void )
{
static const double a = 16807.L;
static const double m = 2147483647.L;	// == LONG_MAX
static const double oneOverM = 1.L / m;
static double seed = 1.0L;

	double temp = a * seed;
	// seed = temp - m * trunc( temp / m );
	seed = temp - m * trunc( temp * oneOverM );
	// return seed / m;
	return seed * oneOverM;
}

#pragma mark -
#pragma mark === normal distribution ===

// ---------------------------------------------------------------------------
//	gaussian_normal
// ---------------------------------------------------------------------------
//	Approximate the normal distribution using the Box-Muller transformation.
//	This is a better approximation and faster algorithm than the 12 u.v. sum.
//	This is slightly different than the thing I got off the web, I (have to)
//	assume (for now) that I knew what I was doing when I altered it.
//
/* inline */ double
gaussian_normal( void )
{
static int iset = 0;	//	boolean really, 
static double gset;

	double r = 1., fac, v1, v2;
	
	if ( ! iset )
	{
		v1 = 2. * uniform() - 1.;
		v2 = 2. * uniform() - 1.;
		r = v1*v1 + v2*v2;
		while( r >= 1. )
		{
			// v1 = 2. * uniform() - 1.;
			v1 = v2;
			v2 = 2. * uniform() - 1.;
			r = v1*v1 + v2*v2;
		}

		fac = std::sqrt( -2. * std::log(r) / r );
		gset = v1 * fac;
		iset = 1;
		return v2 * fac;
	}
	else
	{
		iset = 0;
		return gset;
	}
}

// ************* end contents of random.h *******************





// ---------------------------------------------------------------------------
//	protoype filter
// ---------------------------------------------------------------------------
//	Static local function for obtaining a prototype Filter
//	to use in Oscillator construction. Eventually, allow
//	external (client) specification of the Filter prototype.
//
static const Filter & prototype_filter( void )
{
	//	Chebychev order 3, cutoff 500, ripple -1.
	//
	//	Coefficients obtained from http://www.cs.york.ac.uk/~fisher/mkfilter/
	//	Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
	//
	static const double Gain = 4.663939184e+04;
	static const double ExtraScaling = 6.;
	static const double MaCoefs[] = { 1., 3., 3., 1. }; 
	static const double ArCoefs[] = { 1., 2.9258684252, -2.8580608586, 0.9320209046 };

	static const Filter proto( MaCoefs, MaCoefs + 4, ArCoefs, ArCoefs + 4, ExtraScaling/Gain );
	return proto;
}

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
	_filter( new Filter( prototype_filter() ) )
{
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
//	Oscillator construction
// ---------------------------------------------------------------------------
//	Initialize state (to zero) and construct a Filter.
Oscillator::Oscillator( void ) :
	_frequency( 0. ),	//	radians per sample
	_amplitude( 0. ),	//	absolute
	_bandwidth( 0 ),	//	bandwidth coefficient (noise energy / total energy)
	_phase( 0 ),		//	radians
	_filter( new Filter( prototype_filter() ) )
{
}

// ---------------------------------------------------------------------------
//	Oscillator copy construction
// ---------------------------------------------------------------------------
//	Initialize state (to zero) and construct a Filter.
Oscillator::Oscillator( const Oscillator & other ) :
	_frequency( other._frequency ),	//	radians per sample
	_amplitude( other._amplitude ),	//	absolute
	_bandwidth( other._bandwidth ),	//	bandwidth coefficient (noise energy / total energy)
	_phase( other._phase ),		//	radians
	_filter( new Filter( * (other._filter) ) )
{
}

// ---------------------------------------------------------------------------
//	Oscillator destruction
// ---------------------------------------------------------------------------
//
Oscillator::~Oscillator( void )
{
}

// ---------------------------------------------------------------------------
//	assignment operator
// ---------------------------------------------------------------------------
Oscillator & 
Oscillator::operator=( const Oscillator & rhs )
{
	if ( &rhs != this )
	{
		_frequency = rhs._frequency;
		_amplitude = rhs._amplitude;
		_bandwidth = rhs._bandwidth;
		_phase = rhs._phase;
		_filter.reset( new Filter( *(rhs._filter) ) );
	}
	return *this;
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
		_phase = fmod( _phase, 2. * Pi );
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
		_phase = fmod( p, 2. * Pi );
	}
	
//	set the state variables to their target values,
//	just in case they didn't arrive exactly (overshooting
//	amplitude or, especially, bandwidth, could be bad, and
//	it does happen):
	_frequency = targetFreq;
	_amplitude = targetAmp;
	_bandwidth = targetBw;
}

}	//	end of namespace Loris

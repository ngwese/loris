/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2004 by Kelly Fitz and Lippold Haken
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
 * NoiseGenerator.c++
 *
 * Implementation of a class representing a filtered noise generator, used 
 * as a modulator in bandwidth-enhanced synthesis.
 *
 * Kelly Fitz, 5 June 2003
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include "config.h"
#endif

#include "NoiseGenerator.h"
#include <cmath>

//	begin namespace
namespace Loris {

#pragma mark --- construction ---

// ---------------------------------------------------------------------------
//	default constructor
// ---------------------------------------------------------------------------
//	Use the default Filter (no filtering). Seed defaults to 1.
//
NoiseGenerator::NoiseGenerator( double initSeed ) :
	sample( 0. ),
	u_seed( initSeed ),
	gset( 0 ),
	iset( false )
{
}

// ---------------------------------------------------------------------------
//	initialized constructor
// ---------------------------------------------------------------------------
//	Use the specified Filter. Seed defaults to 1.
//
NoiseGenerator::NoiseGenerator( const Filter & f, double initSeed ) :
	sample( 0. ),
	filter( f ),
	u_seed( initSeed ),
	gset( 0 ),
	iset( false )
{
}

// ---------------------------------------------------------------------------
//	reset
// ---------------------------------------------------------------------------
//	Seed the random number generator and clear the filter's
//	delay line.
//
void 
NoiseGenerator::reset( double newSeed )
{
	u_seed = newSeed;
	filter.clear();
}

#pragma mark --- random number generation ---

// ---------------------------------------------------------------------------
//	dirt-cheap random number generator
// ---------------------------------------------------------------------------
//	Not using this, but could probably get by using it for the
//	stochastic AM. (?)
//
#include <cstdlib>
static inline double
cheapo( void )
{
	using namespace std;
	return (2.0 * rand() / RAND_MAX) - 1.0;
}

// ---------------------------------------------------------------------------
//	uniform random number generator
// ---------------------------------------------------------------------------
//	Taken from "Random Number Generators: Good Ones Are Hard To Find," 
//	Stephen Park and Keith Miller, Communications of the ACM, October 1988,
//	vol. 31, Number 10.
//
//	This version will work as long as floating point values are represented
//	with at least a 46 bit mantissa. The IEEE standard 64 bit floating point
//	format has a 53 bit mantissa.
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
//	trunc() is a problem. It's not in cmath, officially, though
//	Metrowerks has it in there. SGI has it in math.h which is
//	(erroneously!) included in g++ cmath, but trunc is not imported
//	into std. For these two compilers, could just import std. But
//	trnc doesn't seem to exist anywhere in Linux g++, so use std::modf().
//	DON'T use integer conversion, because long int ins't as long 
//	as double's mantissa!
//
static inline double trunc( double x ) { double y; std::modf(x, &y); return y; }

inline double
NoiseGenerator::uniform( void )
{
	static const double a = 16807.L;
	static const double m = 2147483647.L;	// == LONG_MAX
	static const double oneOverM = 1.L / m;
	//static double seed = 1.0L;

	double temp = a * u_seed;
	// seed = temp - m * trunc( temp / m );
	u_seed = temp - m * trunc( temp * oneOverM );
	// return seed / m;
	return u_seed * oneOverM;
}

// ---------------------------------------------------------------------------
//	gaussian_normal
// ---------------------------------------------------------------------------
//	Approximate the normal distribution using the Box-Muller transformation.
//	This is a better approximation and faster algorithm than the 12 u.v. sum.
//	This is slightly different than the thing I got off the web, I (have to)
//	assume (for now) that I knew what I was doing when I altered it.
//
inline double 
NoiseGenerator::gaussian_normal( void )
{
	//static int iset = 0;	//	boolean really, 
	//static double gset;

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
		iset = true;
		return v2 * fac;
	}
	else
	{
		iset = false;
		return gset;
	}
}

#pragma mark --- sample generation ---

// ---------------------------------------------------------------------------
//	next
// ---------------------------------------------------------------------------
//	Generate and return a new sample of filtered noise.
//
double 
NoiseGenerator::next( void )
{
	sample = filter( gaussian_normal() );
	return sample;
}


// ---------------------------------------------------------------------------
//	next
// ---------------------------------------------------------------------------
//	Generate and return a new sample of filtered noise, using the specified
//	mean and standard deviation.
//
double 
NoiseGenerator::next( double mean, double stddev )
{
	sample = filter( (stddev * gaussian_normal()) + mean );
	return sample;
}

}	//	end of namespace Loris

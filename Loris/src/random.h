#ifndef INCLUDE_RANDOM_H
#define INCLUDE_RANDOM_H
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
 * random.h
 * 	
 * Inline implementations of algorithms for generating random numbers
 * with uniform and gaussian distributions.
 *
 * Kelly Fitz, 31 Aug 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <cmath>
//	trunc() is a problem. It's not in cmath, officially, though
//	Metrowerks has it in there. SGI has it in math.h which is
//	(erroneously!) included in g++ cmath, but trunc is not imported
//	into std. For these two compilers, could just import std. But
//	trnc doesn't seem to exist anywhere in Linux g++, so use std::modf().
//	DON'T use integer conversion, because long int ins't as long 
//	as double's mantissa, see below.
static inline double trunc( double x ) { double y; std::modf(x, &y); return y; }

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

//	Get rid of the stupid templates sometime.

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

// ---------------------------------------------------------------------------
//	uniform
// ---------------------------------------------------------------------------
//	Assume that min is less than max, _and_ that the difference between
//	min and max is representable by type T. Returns a value of type T on 
//	the range [min, max).
//
template < class T >
inline T
uniform( T min, T max )
{
	T range = max - min;
	return min + ( range * uniform() );	// should promote T to long dbl
}

#pragma mark -
#pragma mark === normal distribution ===

// ---------------------------------------------------------------------------
//	box_muller
// ---------------------------------------------------------------------------
//	Approximate the normal distribution using the Box-Muller transformation.
//	This is a better approximation and faster algorithm than the 12 u.v. sum.
//	This is slightly different than the thing I got off the web, I (have to)
//	assume (for now) that I knew what I was doing when I altered it.
//
inline double
box_muller( void )
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

// ---------------------------------------------------------------------------
//	gaussian_normal( mean, std_deviation )
// ---------------------------------------------------------------------------
//	Return a normally distributed r.v. with the specified mean and 
//	standard deviation.
//
template < class T >
inline T
gaussian_normal( T mean, T std_deviation )
{
	return mean + ( std_deviation * box_muller() );
}

// ---------------------------------------------------------------------------
//	gaussian_normal
// ---------------------------------------------------------------------------
//	Return a normally distributed r.v. with zero mean and standard deviation 1.
//
inline double
gaussian_normal( void )
{
	return box_muller();
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif /* ndef INCLUDE_RANDOM_H */

#ifndef __noise_generators__
#define __noise_generators__

// ===========================================================================
//	noise.h
//	
//	Inline implementations of algorithms for generating random numbers
//	with uniform and gaussian distributions.
//
//	-kel 31 Aug 99
//
// ===========================================================================

#if !defined(USE_DEPRECATED_HEADERS)
	#include <cmath>
	using std::sqrt;
	using std::log;
	using std::trunc;
#else
	#include <math.h>
#endif

Begin_Namespace( Loris )

#pragma mark -
#pragma mark === uniform distribution ===
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
//	Returns a uniformly distributed random Double on the range [0., 1.).
//
//	-kel 7 Nov 1997.
//
typedef long double ldouble;

static ldouble seed = 1.0L;

//static 
inline ldouble
uniform(void)
{
static const ldouble a = 16807.L;
static const ldouble m = 2147483647.L;	// == LONG_MAX
static const ldouble oneOverM = 1.L / m;

	ldouble temp = a * seed;
	// seed = temp - m * trunc( temp / m );
	seed = temp - m * trunc( temp * oneOverM );
	// return seed / m;
	return seed * oneOverM;
}

// ---------------------------------------------------------------------------
//	SeedRdm
// ---------------------------------------------------------------------------
//
inline void
seed_random(ldouble s)
{
	seed = s;
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
inline ldouble
box_muller( void )
{
static int iset = 0;	//	boolean really, 
static ldouble gset;

	ldouble r = 1., fac, v1, v2;
	
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

		fac = sqrt( -2. * log(r) / r );
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
inline ldouble
gaussian_normal( void )
{
	return box_muller();
}

End_Namespace( Loris )

#endif	// ndef __noise_generators__
#ifndef NOISEGENERATOR_H
#define NOISEGENERATOR_H
/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2008 by Kelly Fitz and Lippold Haken
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
 * NoiseGenerator.h
 *
 * Definition of a class representing a filtered noise generator, used 
 * as a modulator in bandwidth-enhanced synthesis.
 *
 * Kelly Fitz, 5 June 2003
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "Filter.h"

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	class NoiseGenerator
//
class NoiseGenerator
{
//	--- interface ---
public:
	//	construction
	//	copy and assign are free
	explicit NoiseGenerator( double initSeed = 1.0 );
	NoiseGenerator( const Filter & f, double initSeed = 1.0 );

	//	seed the random number generator and clear the filter's
	//	delay line:
	void reset( double newSeed );
	
	//	return the most-recently generated sample:
	double current( void ) const { return sample; }
	
	//	generate and return a new sample of 
	//	filtered noise:
	double next( void );
	double operator() ( void ) { return next(); }
	
	double next( double mean, double stddev = 1. );
	double operator() ( double mean, double stddev = 1. ) { return next( mean, stddev ); }


//	--- implementation ---
private:
	//	random number 
	inline double uniform( void );
	inline double gaussian_normal( void );

	double sample;	//	the most recently-computed noise sample
	Filter filter;	//	filter applied to random number generator

	// random number generator implementation:
	double u_seed;
	double gset;
	bool iset;
	
};


}	//	end of namespace Loris

#endif	/* ndef NOISEGENERATOR_H */

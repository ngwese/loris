#ifndef INCLUDE_OSCILLATOR_H
#define INCLUDE_OSCILLATOR_H
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
 * Oscillator.h
 *
 * Definition of class Loris::Oscillator, a Bandwidth-Enhanced Oscillator.
 *
 * Kelly Fitz, 31 Aug 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <memory>
#include <vector>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

class Filter;	

// ---------------------------------------------------------------------------
//	class Oscillator
//
//	Oscillator represents a single bandwidth-enhanced sinusoidal oscillator
//	used for synthesizing sounds from Reassigned Bandwidth-Enhanced analysis
// 	data. Oscillator encapsulates the oscillator state, including the instan-
//	taneous radian frequency, amplitude, bandwidth, and phase, and a filter
//	object used to generate the bandlimited stochastic modulator.
//
//	Loris::Synthesizer uses an instance of Loris::Oscillator to synthesize
//	bandwidth-enhanced partials obtained from Reassigned Bandwidth-Enhanced
//	analysis data.
//
class Oscillator
{
//	-- instance variables --
//	internal state:
	double _frequency;	//	radians per sample
	double _amplitude;	//	absolute
	double _bandwidth;	//	bandwidth coefficient (noise energy / total energy)
	double _phase;		//	radians

//	filter for stochastic modulation:
	std::auto_ptr< Filter > _filter;

//	-- public interface --
public:
//	construction (with initial state):
	Oscillator( double radf, double a, double bw, double ph );
	~Oscillator( void );
		
//	state access:
	double radianFreq( void ) const { return _frequency; }
	double amplitude( void ) const { return _amplitude; }
	double bandwidth( void ) const { return _bandwidth; }
	double phase( void ) const { return _phase; }

//	state mutation:	
	void setRadianFreq( double x ) { _frequency = x; }
	void setAmplitude( double x ) { _amplitude = x; }
	void setBandwidth( double x ) { _bandwidth = x; }
	void setPhase( double x ) { _phase = x; }

//	sample generation:	
//	Accumulate bandwidth-enhanced sinusoidal samples modulating the 
//	oscillator state from its current values of radian frequency,
//	amplitude, and bandwidth to the specified target values, starting
//	at beginIdx and ending at (before) endIdx (no sample is accumulated
//	at endIdx). The indices are positions in the specified buffer.
//
//	The caller must insure that the indices are valid. Target frequency
//	and bandwidth are checked to prevent aliasing and bogus bandwidth
//	enhancement.
	void generateSamples( //std::vector< double > & buffer, long beginIdx, long endIdx,
						  double * begin, double * end,
						  double targetFreq, double targetAmp, double targetBw );
	
private:
//	-- unimplemented --
//	not implemented until proven useful:
	Oscillator( void );
	Oscillator( const Oscillator & other );
	Oscillator & operator= ( const Oscillator & other );
		
};	//	end of class Oscillator

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif /* ndef INCLUDE_OSCILLATOR_H */

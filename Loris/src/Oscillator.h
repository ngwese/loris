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

//	begin namespace
namespace Loris {

class Filter;	

// ---------------------------------------------------------------------------
//	class Oscillator
//
//	Class Oscillator represents the state of a single bandwidth-enhanced
//	sinusoidal oscillator used for synthesizing sounds from Reassigned
//	Bandwidth-Enhanced analysis data. Oscillator encapsulates the oscillator
//	state, including the instan- taneous radian frequency (radians per
//	sample), amplitude, bandwidth coefficient, and phase, and a filter
//	object used to generate the bandlimited stochastic modulator. Class
//	Synthesizer uses an instance of Oscillator to synthesize
//	bandwidth-enhanced Partials.
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
//	-- construction --
	Oscillator( double radf, double a, double bw, double ph );
	/*	Construct a new Oscillator with the initial state parameters 
		(radian frequencym amplitude, bandwidth coefficient, and phase)
		as specified.
	 */
	 
	Oscillator( void );
	/*	Construct a new Oscillator with all state parameters initialized
		to 0.
	 */
	 
	Oscillator( const Oscillator & other );
	/* 	Construct a new Oscillator that has the same state paramters
		as another Oscillator. The filter used by the new Oscillator
		will have the same coefficients and response as the other
		Oscillator, but the state of the filter delay lines is not 
		copied.
	 */
	 
	~Oscillator( void );
	/* 	Destroy this Oscillator.
	 */
	 
	Oscillator & operator= ( const Oscillator & other );
	/*	Assignment operator: make this Oscillator an exact copy of
		another Oscillator. The filter used by this Oscillator
		will have the same coefficients and response as the other
		Oscillator, but the state of the filter delay lines is not 
		copied.
	 */
	 
//	-- state access --
	double amplitude( void ) const { return _amplitude; }
	/* Return the current instantaneous amplitude of this Oscillator.
	 */
	 
	double bandwidth( void ) const { return _bandwidth; }
	/*	Return the current instantaneous bandwidth coefficient of this
		Oscillator.
	 */
	 
	double phase( void ) const { return _phase; }
	/*	Return the current instantaneous phase of this Oscillator.
	 */
	 
	double radianFreq( void ) const { return _frequency; }
	/*	Return the current instantaneous frequency (in radians per sample)
		of this Oscillator.	
	 */
	 
//	-- state mutation --
	void setAmplitude( double x ) { _amplitude = x; }
	/* 	Set the instantaneous amplitude of this Oscillator.
	 */
	 
	void setBandwidth( double x ) { _bandwidth = x; }
	/* 	Set the instantaneous bandwidth coefficient of this Oscillator.
	 */
	 
	void setPhase( double x ) { _phase = x; }
	/*	Set the instantaneous phase of this Oscillator.
	 */
	 
	void setRadianFreq( double x ) { _frequency = x; }
	/*	Set the instantaneous frequency (in radians per sample) 
		of this Oscillator.
	 */
	 
//	-- sample generation --
	void generateSamples( double * begin, double * end,
						  double targetFreq, double targetAmp, double targetBw );
	/*	Accumulate bandwidth-enhanced sinusoidal samples modulating the
		oscillator state from its current values of radian frequency, amplitude,
		and bandwidth to the specified target values. Accumulate samples into
		the half-open (STL-style) range of doubles, starting at begin, and
		ending before end (no sample is accumulated at end). The caller must
		insure that the indices are valid. Target frequency and bandwidth are
		checked to prevent aliasing and bogus bandwidth enhancement.
	 */
	 
};	//	end of class Oscillator

}	//	end of namespace Loris

#endif /* ndef INCLUDE_OSCILLATOR_H */

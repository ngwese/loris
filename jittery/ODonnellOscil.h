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

#include <NoiseGenerator.h>

#include <vector>

//	begin namespace
namespace Loris {

class Breakpoint;
class Envelope;
class Partial;

// ---------------------------------------------------------------------------
//	class Oscillator
//
//	Class Oscillator represents the state of a single bandwidth-enhanced
//	sinusoidal oscillator used for synthesizing sounds from Reassigned
//	Bandwidth-Enhanced analysis data. Oscillator encapsulates the oscillator
//	state, including the instantaneous radian frequency (radians per
//	sample), amplitude, bandwidth coefficient, and phase, and a 
//	bandlimited stochastic modulator. 
//
//	This version additionally synthesizes jittery bandwidth-enhanced
//	Partials, so it has some extra phase modulation state and two 
//	more bandlimited stochastic modulators.
//
//	Class Synthesizer uses an instance of Oscillator to synthesize
//	bandwidth-enhanced Partials.
//
class Oscillator
{
//	--- implementation _--

	//	stochastic modulators:
	NoiseGenerator bwModulator;
	NoiseGenerator coherentJitter;
	NoiseGenerator incoherentJitter;
	
	//	jitter envelopes:
	//Envelope & jitterGain;
	//Envelope & jitterCoherence;
	
	//	instantaneous oscillator state:
	double i_frequency;	//	radians per sample
	double i_amplitude;	//	absolute
	double i_bandwidth;	//	bandwidth coefficient (noise energy / total energy)
	double i_jitter;	//	jitter gain
	double i_coherence;	//	jitter coherence
	
	//	accumulating phase state:
	double pm_coherent;		//	accumulated coherent modulation
	double pm_incoherent;	//	accumulated inchoerent modulation
	double determ_phase;	//	deterministic phase in radians
							//	(does not include modulation due to jitter)

	//double fadeTime;
	//double sampleRate;

//	--- interface ---
public:
//	--- construction --_
	//Oscillator( double radf, double a, double bw, double ph );
	/*	Construct a new Oscillator with the initial state parameters 
		(radian frequency, amplitude, bandwidth coefficient, and phase)
		as specified.
	 */
	 
	Oscillator( void );
	/*	Construct a new Oscillator with all state parameters initialized
		to 0.
	 */
	 
	 
	//	Copy, assignment, and destruction are free.
	//
	// 	Copied and assigned Oscillators have the duplicate state
	//	variables and the filters have the same coefficients,
	//	but the state of the filter delay lines is not copied.

//	-- the new way --
	 
	void resetEnvelopes( const Breakpoint & bp, double srate );
	/*	Reset the instantaneous envelope parameters 
	 	(frequency, amplitude, bandwidth, and phase).
	 	The sample rate is needed to convert the 
	 	Breakpoint frequency (Hz) to radians per sample.
	 	The state of the jitter parameters is also 
	 	reset (to no jitter).
	  */
	  
	  
	void resetJitter( double jgain, double jcoherence, unsigned long startingSamp );
	/*	Reset the state of the jitter parameters. The
		starting sample index is needed to correctly
		reset the state of the common coherent jitter
		phase modulator.
	 */
	 
	void resetPhase( double ph );
	/*	Reset the phase of the Oscillator to the specified
		value, and clear the accumulated phase modulation. (?)
		Or not.
		This is done when the amplitude of a Partial goes to 
		zero, so that onsets are preserved in distilled
		and collated Partials.
	 */

	void oscillate( double * begin, double * end,
					const Breakpoint & bp, double srate,
					double targetJitter = 0, double targetCoherence = 0, int partialNum = 0 );
	/*	Accumulate bandwidth-enhanced sinusoidal samples modulating the
		oscillator state from its current values of radian frequency, amplitude,
		and bandwidth to the specified target values. Accumulate samples into
		the half-open (STL-style) range of doubles, starting at begin, and
		ending before end (no sample is accumulated at end). The caller must
		insure that the indices are valid. Target frequency and bandwidth are
		checked to prevent aliasing and bogus bandwidth enhancement.
	 */

#if 0	  
//	-- state access --
	double amplitude( void ) const { return i_amplitude; }
	/* Return the current instantaneous amplitude of this Oscillator.
	 */
	 
	double bandwidth( void ) const { return i_bandwidth; }
	/*	Return the current instantaneous bandwidth coefficient of this
		Oscillator.
	 */
	 
	double phase( void ) const { return determ_phase; }
	/*	Return the current instantaneous phase of this Oscillator.
	 */
	 
	double radianFreq( void ) const { return i_frequency; }
	/*	Return the current instantaneous frequency (in radians per sample)
		of this Oscillator.	
	 */
	 
//	-- state mutation --
	void setAmplitude( double x ) { i_amplitude = x; }
	/* 	Set the instantaneous amplitude of this Oscillator.
	 */
	 
	void setBandwidth( double x ) { i_bandwidth = x; }
	/* 	Set the instantaneous bandwidth coefficient of this Oscillator.
	 */
	 
	void setPhase( double x ) { determ_phase = x; }
	/*	Set the instantaneous phase of this Oscillator.
	 */
	 
	void setRadianFreq( double x ) { i_frequency = x; }
	/*	Set the instantaneous frequency (in radians per sample) 
		of this Oscillator.
	 */
	 
//	-- sample generation --
	void generateSamples( double * begin, double * end,
						  double targetFreq, double targetAmp, double targetBw,
						  double targetJitter = 0, double targetCoherence = 0, int partialNum = 0 );
	/*	Accumulate bandwidth-enhanced sinusoidal samples modulating the
		oscillator state from its current values of radian frequency, amplitude,
		and bandwidth to the specified target values. Accumulate samples into
		the half-open (STL-style) range of doubles, starting at begin, and
		ending before end (no sample is accumulated at end). The caller must
		insure that the indices are valid. Target frequency and bandwidth are
		checked to prevent aliasing and bogus bandwidth enhancement.
	 */
	 
#endif	 
	 // void oscillate( const Partial & p, double * begin, double * end );
	 
	 
//	-- jitter modulator configuration --
	 //void resetState( const Partial & p );
	 
	 //inline double radianFreq( double hz );
	 
};	//	end of class Oscillator

}	//	end of namespace Loris

#endif /* ndef INCLUDE_OSCILLATOR_H */

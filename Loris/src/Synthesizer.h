#ifndef INCLUDE_SYNTHESIZER_H
#define INCLUDE_SYNTHESIZER_H
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
 * Synthesizer.h
 *
 * Definition of class Loris::SynthesizerSynthesizer, a synthesizer of 
 * bandwidth-enhanced Partials.
 *
 * Kelly Fitz, 16 Aug 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

class Partial;

// ---------------------------------------------------------------------------
//	class Synthesizer
//	
//	Definition of class of synthesizers of (reassigned) bandwidth-enhanced
//	partials. Synthesizer accumulates samples for one partial at a time
//	at a specified sample rate into a specified sample buffer.
//
//	The Synthesizer does not own its sample buffer, the client 
//	is responsible for its construction and destruction. Many 
//	Synthesizers may share a buffer. (But this class is not thread-safe.)
//
class Synthesizer
{
//	-- instance variables --
	double _sampleRate;					//	in Hz
	double * _sampleBuffer;				//	samples are computed and stored here
	long _sampleBufferSize;				//	length of buffer in samples
		
//	-- public interface --
public:
//	construction:
//	(use compiler-generated destructor)
	Synthesizer( double * buffer, long bufferLength, double srate );
	Synthesizer( const Synthesizer & other );
	~Synthesizer(void);
	
//	synthesis:
//
//	Synthesize a bandwidth-enhanced sinusoidal Partial with the specified 
//	timeShift (in seconds). Zero-amplitude Breakpoints are inserted
//	1 millisecond (Partial::FadeTime()) from either end of the Partial to reduce 
//	turn-on and turn-off artifacts. The client is responsible or insuring
//	that the buffer is long enough to hold all samples from the time-shifted
//	and padded Partials. Synthesizer will not generate samples outside the
//	buffer, but neither will any attempt be made to eliminate clicks at the
//	buffer boundaries.  
	void synthesize( const Partial & p, double timeShift = 0. );	
	
//	access:
	double sampleRate( void ) const { return _sampleRate; }
	
	double * samples( void ) { return _sampleBuffer; }
	const double * samples( void ) const { return _sampleBuffer; }
	
	long numSamples( void ) const { return _sampleBufferSize; }
	
//	-- private helpers --
private:
	inline double radianFreq( double hz ) const;

//	-- not impemented --
//	 not impemented until proven useful:	
	Synthesizer & operator= ( const Synthesizer & other );
	
};	//	end of class Synthesizer

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_SYNTHESIZER_H

#ifndef INCLUDE_SYNTHESIZER_H
#define INCLUDE_SYNTHESIZER_H
/*
 * Copyright (c) 1999-2000 Kelly Fitz and Lippold Haken
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
#include "PartialView.h"
#include <vector>	
#include <memory>	//	for auto_ptr

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
//	The Synthesizer does not own its sample vector, the client 
//	is responsible for its construction and destruction. Many 
//	Synthesizers may share a buffer.
//	
//	auto_ptr is used to pass some objects as arguments to make explicit
//	the source/sink relationship between the caller and the Synthesizer.
//	Synthesizer assumes ownership, and the client's auto_ptr
//	will have no reference (or ownership).
//
class Synthesizer
{
//	-- instance variables --
	double _sampleRate;					//	in Hz
	std::vector< double > & _samples;	//	samples are computed and stored here
	std::auto_ptr< PartialView > _view;	//	a view on Partials, possibly filtered
		
//	-- public interface --
public:
//	construction:
//	(use compiler-generated destructor)
	Synthesizer( std::vector< double > & buf, double srate );
	~Synthesizer(void);

//	copy:
//	Create a copy of other by cloning its PartialView and sharing its
//	sample buffer.
	Synthesizer( const Synthesizer & other );
	
//	synthesis:
//
//	Synthesize a bandwidth-enhanced sinusoidal Partial with the specified 
//	timeShift (in seconds). The Partial parameter data is filtered by the 
//	Synthesizer's PartialView. Zero-amplitude Breakpoints are inserted
//	1 millisecond (Partial::FadeTime()) from either end of the Partial to reduce 
//	turn-on and turn-off artifacts. The client is responsible or insuring
//	that the buffer is long enough to hold all samples from the time-shifted
//	and padded Partials. Synthesizer will not generate samples outside the
//	buffer, but neither will any attempt be made to eliminate clicks at the
//	buffer boundaries.  
	void synthesize( const Partial & p, double timeShift = 0. );	
	
//	access:
	double sampleRate( void ) const { return _sampleRate; }
	
	std::vector< double > & samples( void ) { return _samples; }
	const std::vector< double > & samples( void ) const { return _samples; }
	
	const PartialView & view( void ) const { return *_view; }
	PartialView & view( void ) { return *_view; }
	
//	mutation:
	void setView( const PartialView & v );

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

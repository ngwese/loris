#ifndef INCLUDE_SYNTHESIZER_H
#define INCLUDE_SYNTHESIZER_H
// ===========================================================================
//	Synthesizer.h
//	
//	Class definition for Loris::Synthesizer, a synthesizer of 
//	bandwidth-enhanced Partials.
//
//	-kel 16 Aug 99
//
// ===========================================================================
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

//	copy:
//	Create a copy of other by cloning its PartialView and sharing its
//	sample buffer.
	Synthesizer( const Synthesizer & other );
	
//	synthesis:
//
//	Synthesize a bandwidth-enhanced sinusoidal Partial with the specified 
//	timeShift (in seconds). The Partial parameter data is filtered by the 
//	Synthesizer's PartialView. Zero-amplitude Breakpoints are inserted
//	1 millisecond (FADE_TIME) from either end of the Partial to reduce 
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

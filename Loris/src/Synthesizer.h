#ifndef __Loris_synthesizer__
#define __Loris_synthesizer__

// ===========================================================================
//	Synthesizer.h
//	
//	Class definition for Loris::Synthesizer.
//	
//	Loris synthesis generates a buffer of samples from a 
//	collection of Partials. 
//
//	-kel 16 Aug 99
//
// ===========================================================================

#include "LorisLib.h"
#include "Oscillator.h"
#include "PartialIterator.h"
#include <vector>
#include <memory> 	//	for auto_ptr

Begin_Namespace( Loris )

class Partial;

// ---------------------------------------------------------------------------
//	class Synthesizer
//	
//	Definition of class of synthesizers of reassigned bandwidth-enhanced
//	partials. A Synthesizer generates samples for one partial at a time
//	at a specified sample rate into a specified sample buffer using an
//	Oscillator. The buffer and rate are specified at construction and 
//	are immutable. 
//
//	The Synthesizer does not own its sample vector, the client 
//	is responsible for its construction and destruction. 
//
//	Non-standard Oscillator and PartialIterator may be assigned,
// 	and are then owned by the Synthesizer, that is, they will be 
//	destroyed with the Synthesizer.
//
//	Some properties of the Synthesizer cannot safely (or reasonably) 
//	be changed on the fly: the buffer, the sample rate. Others, like
//	the oscillator, the iterator, the offset, can usefully be modified.
//
//	auto_ptr is used to pass some objects as arguments to make explicit
//	the source/sink relationship between the caller and the Synthesizer.
//	Synthesizer assumes ownership, and the client's auto_ptr
//	will have no reference (or ownership).
//
class Synthesizer
{
//	-- public interface --
public:
//	construction:
	Synthesizer( std::vector< double > & buf, double srate, 
				 std::auto_ptr< Oscillator > osc = std::auto_ptr< Oscillator >() );
	Synthesizer( const Synthesizer & other );
	
	//~Synthesizer( void );	//	use compiler-generated destructor
	
	Synthesizer & operator= ( const Synthesizer & other );

//	access:
	double sampleRate( void ) const { return _sampleRate; }
	
	double offset( void ) const { return _offset; }
	void setOffset( double x ) { _offset = x; }
	
	double fadeTime( void ) const { return _fadeTime; }
	void setFadeTime( double x ) { if (x >= 0.) _fadeTime = x; }
	
	std::vector< double > & samples( void ) { return _samples; }
	const std::vector< double > & samples( void ) const { return _samples; }
	void setSampleBuffer( std::vector< double > & buf ) { _samples = buf; }
	
	Oscillator & oscillator( void ) { return *_oscillator; }
	const Oscillator & oscillator( void ) const { return *_oscillator; }
	void setOscillator( std::auto_ptr< Oscillator > osc = std::auto_ptr< Oscillator >() );
	
	PartialIterator & iterator( void ) { return *_iterator; }
	const PartialIterator & iterator( void ) const { return *_iterator; }
	void setIterator( std::auto_ptr< PartialIterator > iter = std::auto_ptr< PartialIterator >() );
	
//	synthesis:
	void synthesizePartial( const Partial & p );	
	void operator()( const Partial & p ) { synthesizePartial( p ); }
	
//	-- template member functions for synthesis --
//
//	Strictly speaking, we can do without these if necessary.
//
#if !defined(No_template_members)
	template < class Iterator >
	void synthesize( Iterator begin, Iterator end )
	{
		while( begin != end ) {
			synthesizePartial( *(begin++) );
		}
	}
#endif 	//	template members allowed

//	-- private helpers --
private:
	inline long synthesizeEnvelopeSegment( long currentSampleOffset );
	inline long synthesizeFadeIn( long currentSampleOffset );
	inline long synthesizeFadeOut( long currentSampleOffset );
	inline double radianFreq( double hz ) const;

//	-- instance variables --
	double _sampleRate;		//	in Hz
	double _offset;			//	time offset for synthesized Partials, in seconds
	double _fadeTime;		//	default (maximum) fade in/out time for Partials, in seconds
	
	std::vector< double > & _samples;
	
	std::auto_ptr< Oscillator > _oscillator;
	std::auto_ptr< PartialIterator > _iterator;	//	possibly does transformation
	
};	//	end of class Synthesizer

End_Namespace( Loris )

#endif	// ndef __Loris_synthesizer__

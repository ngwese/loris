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

Begin_Namespace( Loris )

class Partial;
class Oscillator;
class SampleBuffer;
class PartialIterator;

// ---------------------------------------------------------------------------
//	class Synthesizer
//	
//	Definition of class of synthesizers of reassigned bandwidth-enhanced
//	partials. A Synthesizer generates samples for one partial at a time
//	at a specified sample rate into a specified sample buffer using an
//	Oscillator. The buffer and rate are specified at construction and 
//	are immutable. 
//
//	The Synthesizer does not own its SampleBuffer, the client 
//	is responsible for its construction and destruction. 
//
//	Non-standard Oscillator and PartialIterator may be assigned,
// 	and are then owned by the Synthesizer, that is, they will be 
//	destroyed with the Synthesizer.
//
class Synthesizer
{
//	-- public interface --
public:
//	construction:
	Synthesizer( SampleBuffer & buf, double srate );
	~Synthesizer( void );

//	access:
	double sampleRate( void ) const { return _sampleRate; }
	
//	for specifying the oscillator:
	void setOscillator( Oscillator * osc = Null );
	
//	for specifying the iterator:
	void setIterator( PartialIterator * iter = Null );
	
//	synthesis:
	void synthesizePartial( const Partial & p );	
	void operator()( const Partial & p ) { synthesizePartial( p ); }
	
//	-- private helpers --
private:
	inline double radianFreq( double hz ) const;

//	-- instance variables --
//	sample rate (Hz):
	double _sampleRate;
	
//	sample buffer:
	SampleBuffer & _samples;
	
//	oscillator:
	Oscillator * _oscillator;

//	partial iterator, possibly does transformation:
	PartialIterator * _iterator;
	
};	//	end of class Synthesizer


End_Namespace( Loris )

#endif	// ndef __Loris_synthesizer__
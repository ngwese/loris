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

// ---------------------------------------------------------------------------
//	€ class Synthesizer
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
//	The Oscillator may be specified at construction (if not, one will be
// 	created), and _is_ owned by the Synthesizer, that is, it will be 
//	destroyed with the Synthesizer.
//
class Synthesizer
{
//	-- public interface --
public:
//	construction:
	Synthesizer( SampleBuffer & buf, Double srate, Oscillator * osc = Null );
	~Synthesizer( void );

//	access:
	Double sampleRate( void ) const { return mSampleRate; }
	
//	synthesis:
	void synthesizePartial( const Partial & p );	
	void operator()( const Partial & p ) { synthesizePartial( p ); }
	
//	-- private helpers --
private:
	inline Double radianFreq( Double hz ) const;

//	-- instance variables --
//	sample rate (Hz):
	Double mSampleRate;

//	sample buffer:
	SampleBuffer & mSamples;
	
//	oscillator:
	Oscillator * mOscillator;

};	//	end of class Synthesizer


End_Namespace( Loris )

#endif	// ndef __Loris_synthesizer__
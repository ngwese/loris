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
class Breakpoint;

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
//	The Oscillator may be specified at construction (if not, one will be
// 	created), and _is_ owned by the Synthesizer, that is, it will be 
//	destroyed with the Synthesizer.
//
class Synthesizer
{
//	-- public interface --
public:
//	construction:
	Synthesizer( SampleBuffer & buf, double srate, double minBWEfreq = 1000., Oscillator * osc = Null );
	~Synthesizer( void );

//	access:
	double sampleRate( void ) const { return _sampleRate; }
	
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

//	BW enhanced synthesis still sounds bad if applied to low frequency
//	partials. For breakpoints below a certain cutoff, it is best to set
//	the bandwidth to zero, and adjust the amplitude to account for the
//	missing noise energy. 
//
//	This kludger does the trick.
//
	struct BweKludger
	{
		//	construction:
		BweKludger( double f ) : _cutoff( f ) {}
		
		//	public inerface:
		inline double amp( const Breakpoint & bp ) const;
		inline double bw( const Breakpoint & bp ) const;

	private:
		//	helper
		inline double bwclamp( double bw ) const;
		
		//	instance variable:
		double _cutoff;

	};	//	end of class BweKludger

	const BweKludger _kludger;
	
};	//	end of class Synthesizer


End_Namespace( Loris )

#endif	// ndef __Loris_synthesizer__
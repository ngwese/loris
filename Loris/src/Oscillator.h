#ifndef __Loris_oscillator__
#define __Loris_oscillator__

// ===========================================================================
//	Oscillator.h
//	
//	Class definition for Loris::Oscillator.
//	
//	Loris synthesis generates a buffer of samples from a 
//	collection of Partials. The Loris Synthesizer uses an Oscillator
//	to generate samples according to parameters interpolated from
//	pairs of Breakpoints.
//
//	-kel 31 Aug 99
//
// ===========================================================================

#include "LorisLib.h"

#include <memory>	//	for auto_ptr

Begin_Namespace( Loris )

class SampleBuffer;
class Filter;

// ---------------------------------------------------------------------------
//	class Oscillator
//
//	Oscillator is designed as a leaf class, and needs some modifications
//	if it is to serve as a base class for other kinds of oscillators.
//	Its not yet clear what a useful level of abstraction would be.
//	
//	auto_ptr is used to pass Filter arguments to make explicit the
//	source/sink relationship between the caller and the Oscillator.
//	Oscillator assumes ownership, and the client's auto_ptr
//	will have no reference (or ownership).
//
//
class Oscillator
{
//	-- public interface --
public:
//	construction:
	Oscillator( std::auto_ptr< Filter > f = std::auto_ptr< Filter >() );
	Oscillator( const Oscillator & other );
	
	// ~Oscillator( void );	//	use compiler-generated

	Oscillator & operator= ( const Oscillator & other );
		
//	state access/mutation:
	double radianFreq( void ) const { return _frequency; }
	double amplitude( void ) const { return _amplitude; }
	double bandwidth( void ) const { return _bandwidth; }
	double phase( void ) const { return _phase; }
	
	void setRadianFreq( double x ) { _frequency = x; }
	void setAmplitude( double x ) { _amplitude = x; }
	void setBandwidth( double x ) { _bandwidth = x; }
	void setPhase( double x ) { _phase = x; }

//	filter access/specification:
	Filter & filter( void ) { return *_filter; }
	const Filter & filter( void ) const { return *_filter; }
	void setFilter( std::auto_ptr< Filter > f = std::auto_ptr< Filter >() );
	
//	reset the whole state at once:
	void reset( double radf, double amp, double bw, double ph );

//	sample generation:	
	void generateSamples( SampleBuffer & buffer, long howMany, long offset,
						  double targetFreq, double targetAmp, double targetBw );
	
//	-- private helpers --
private:
	inline double oscillate( double phase ) const;
	inline double modulate( double bandwidth ) const;
	
//	-- instance variables --
//	internal state:
	double _frequency;	//	radians per sample
	double _amplitude;	//	absolute
	double _bandwidth;	//	bandwidth coefficient (noise energy / total energy)
	double _phase;		//	radians

//	filter for stochastic modulation:
	std::auto_ptr< Filter > _filter;

};	//	end of class Oscillator

End_Namespace( Loris )

#endif	// ndef __Loris_oscillator__
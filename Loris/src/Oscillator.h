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
#include <vector>

Begin_Namespace( Loris )

class Filter;

// ---------------------------------------------------------------------------
//	class Oscillator
//
//	Hey, any comment?
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
	Filter * _filter;

//	-- public interface --
public:
//	construction:
	Oscillator( double radf, double a, double bw, double ph = 0. );
	~Oscillator( void );
		
//	state access/mutation:
	double radianFreq( void ) const { return _frequency; }
	double amplitude( void ) const { return _amplitude; }
	double bandwidth( void ) const { return _bandwidth; }
	double phase( void ) const { return _phase; }
	
	void setRadianFreq( double x ) { _frequency = x; }
	void setAmplitude( double x ) { _amplitude = x; }
	void setBandwidth( double x ) { _bandwidth = x; }
	void setPhase( double x ) { _phase = x; }

//	sample generation:	
	void generateSamples( std::vector< double > & buffer, long howMany, long offset,
						  double targetFreq, double targetAmp, double targetBw );
	
private:
//	-- unimplemented --
//	not implemented until proven useful:
	Oscillator( const Oscillator & other );
	Oscillator & operator= ( const Oscillator & other );
		
};	//	end of class Oscillator

End_Namespace( Loris )

#endif	// ndef __Loris_oscillator__

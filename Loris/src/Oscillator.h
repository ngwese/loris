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

Begin_Namespace( Loris )

class SampleBuffer;
class Mkfilter;

// ---------------------------------------------------------------------------
//	€ class Oscillator
//	
class Oscillator
{
//	-- public interface --
public:
//	construction:
	Oscillator( void );
	~Oscillator( void );
		
//	state access/mutation:
	Double radianFreq( void ) const { return mFrequency; }
	Double amplitude( void ) const { return mAmplitude; }
	Double bandwidth( void ) const { return mBandwidth; }
	Double phase( void ) const { return mPhase; }
	
	void setRadianFreq( Double x ) { mFrequency = x; }
	void setAmplitude( Double x ) { mAmplitude = x; }
	void setBandwidth( Double x ) { mBandwidth = x; }
	void setPhase( Double x ) { mPhase = x; }
	
//	reset the whole state at once:
	void reset( Double radf, Double amp, Double bw, Double ph );

//	sample generation:	
	void generateSamples( SampleBuffer & buffer, Int howMany, Int offset,
						  Double targetFreq, Double targetAmp, Double targetBw );

//	virtual constructors:
	static Oscillator * Create( void );
	
//	-- private helpers --
private:
	inline Double oscillate( Double phase ) const;
	inline Double modulate( Double bandwidth ) const;
	
//	-- instance variables --
//	internal state:
	Double mFrequency;	//	radians per sample
	Double mAmplitude;	//	absolute
	Double mBandwidth;	//	bandwidth coefficient (noise energy / total energy)
	Double mPhase;		//	radians

//	filter for stochastic modulation:
	Mkfilter * mFilter;

};	//	end of class Oscillator

End_Namespace( Loris )

#endif	// ndef __Loris_oscillator__
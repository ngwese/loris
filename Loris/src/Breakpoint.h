#ifndef __Loris_breakpoint__
#define __Loris_breakpoint__
// ===========================================================================
//	Breakpoint.h
//	
//	Class definition for Loris::Breakpoint.
//
//	Loris Partials represent reassigned bandwidth-enhanced model components.
//	A Partial consists of a chain of Breakpoints describing the time-varying
//	frequency, amplitude, and bandwidth (noisiness) of the component.
//
//	-kel 16 Aug 99
//
// ===========================================================================
#include "LorisLib.h"

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class Breakpoint
//
//	Definition of class of objects representing a single breakpoint in the
//	Partial parameter (frequency, amplitude, bandwidth) envelope.
//	Instantaneous phase is also stored, but is only used at the onset of 
//	a partial, or when it makes a transition from zero to nonzero amplitude.
//
//	Leaf class, do not subclass. On second thought...
//
class Breakpoint
{
//	-- public Breakpoint interface --
public:
//	construction:
	Breakpoint( void );	//	needed for STL containability
	Breakpoint( double f, double a, double b, double p = 0. );
	
//	destructor is not virtual, this is not a base class,
//	use compiler-generated:	
//	well, actually...
	virtual ~Breakpoint( void ) {}

//	copy and assign can be compiler-generated:
	//Breakpoint( const Breakpoint & other );
	//Breakpoint & operator=( const Breakpoint & other );

//	attribute access:
	double frequency( void ) const { return _frequency; }
	double amplitude( void ) const { return _amplitude; }
	double bandwidth( void ) const { return _bandwidth; }
	double phase( void ) const { return _phase; }
	
//	attribute mutation:
	void setFrequency( double x ) { _frequency = x; }
	void setAmplitude( double x ) { _amplitude = x; }
	void setBandwidth( double x ) { _bandwidth = x; }
	void setPhase( double x ) { _phase = x; }
	
//	add noise (bandwidth) energy:
	void addNoise( double x );
	
private:
//	-- instance variables --
//	envelope parameters:
	double _frequency;	//	hertz
	double _amplitude;	//	absolute
	double _bandwidth;	//	fraction of total energy that is noise energy
	double _phase;		//	radians
	
};	//	end of class Breakpoint

End_Namespace( Loris )

#endif	// ndef __Loris_breakpoint__

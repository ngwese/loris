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
#include "Breakpoint.h"

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class Breakpoint
//
//	Definition of class of objects representing a single breakpoint in the
//	Partial parameter (frequency, amplitude, bandwidth) envelope.
//	Instantaneous phase is also stored, but is only used at the onset of 
//	a partial, or when it makes a transition from zero to nonzero amplitude.
//
//	Breakpoints have a specific time associated with them, and form a 
//	(time-ordered) doubly-linked list representing the Parameter envelope.
//
//	Leaf class, do not subclass.
//
class Breakpoint
{
//	-- public Breakpoint interface --
public:
//	attribute access:
	virtual Double frequency( void ) const { return _frequency; }
	virtual Double amplitude( void ) const { return _amplitude; }
	virtual Double bandwidth( void ) const { return _bandwidth; }
	virtual Double phase( void ) const { return _phase; }
	virtual Double time( void ) const { return _time; }
	
//	attribute mutation:
	virtual void setFrequency( Double x ) { _frequency = x; }
	virtual void setAmplitude( Double x ) { _amplitude = x; }
	virtual void setBandwidth( Double x ) { _bandwidth = x; }
	virtual void setPhase( Double x ) { _phase = x; }
	
//	construction:
	Breakpoint( Double f, Double a, Double b, Double p = 0. );
	virtual ~Breakpoint( void );

//	copy and assign copy parameters only:
	Breakpoint( const Breakpoint & other );
	Breakpoint & operator=( const Breakpoint & other );

//	access neighbors in the Partial parameter envelope:
//	const and non-const access, should this be public?
	const Breakpoint * prev( void ) const { return _prevBreakpoint; }
	const Breakpoint * next( void ) const { return _nextBreakpoint; }
	
	Breakpoint * prev( void ) { return _prevBreakpoint; }
	Breakpoint * next( void ) { return _nextBreakpoint; }
	
//	virtual constructors:
	static Breakpoint * Clone( const Breakpoint & p );
	static Breakpoint * Create( Double f, Double a, Double b, Double p =  0. );
	static void Destroy( Breakpoint * p );
	
//	-- private envelope link and time access --
//	Only Partials can link Breakpoints and assign times to them.
private:
//	neigbor pointer assigment:	
	void setPrev( Breakpoint * bp ) { _prevBreakpoint = bp; }
	void setNext( Breakpoint * bp ) { _nextBreakpoint = bp; }
	
//	one-step forward link:
	void linkTo( Breakpoint * bp ) { setNext( bp ); if ( bp ) bp->setPrev( this ); }

//	assign time to this Breakpoint:
	void setTime( Double x ) { _time = x; }
	
//	make Partial a friend so that Partials can link their 
//	constituent Breakpoints:
	friend class Partial;
	
//	-- instance variables --
private:

//	envelope parameters:
	Double _frequency;	//	hertz
	Double _amplitude;	//	absolute
	Double _bandwidth;	//	fraction of total energy that is noise energy
	Double _phase;		//	radians
	
//	Breakpoints are not uniformly distributed in time, each
//	must explicitly specify its time:
	Double _time;		//	seconds
	
//	connectivity:
//	Partials are consist of a doubly-linked list of Breakpoints.
	Breakpoint * _prevBreakpoint;
	Breakpoint * _nextBreakpoint;
	
};	//	end of class Breakpoint


End_Namespace( Loris )

#endif	// ndef __Loris_breakpoint__

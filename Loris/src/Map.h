#ifndef __Loris_abstract_map__
#define __Loris_abstract_map__

// ===========================================================================
//	Map.h
//	
//	Map is an abstract base class representing a generic real (double) 
//	function of one real (double) argument. 
//
//	BreakpointMap is a simple subclass of Map, specifying a linear 
//	segment breakpoint function. 
//
//
//	-kel 26 Oct 99
//
// ===========================================================================

#include "LorisLib.h"

#include <vector>

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class Map
//
//	Abstract base class, specifying interface ( valueAt and operator() ).
//	Derived classes must implement valueAt() and clone().
//
class Map
{
//	evaluation:
//	Derived classes must implement valueAt().
public:
	virtual double valueAt( double x ) const = 0;
	double operator() ( double x ) const { return valueAt(x); }
	
	virtual Map * clone( void ) const = 0;
	virtual ~Map( void ) {}
	
//	protect copying and assignment:
//	(use compiler-generated constructor)
protected:
	Map( void ) {}
	Map( const Map & ) {}
	Map & operator= ( const Map & );	//	not defined

};	//	end of abstract class Map

// ---------------------------------------------------------------------------
//	class BreakpointMap
//
//	Linear-segment breakpoint function with infinite extension at each end.
//
class BreakpointMap : public Map
{
public:
//	construction:
//	use compiler-generated versions of these:
	//BreakpointMap( void ) {}
	//BreakpointMap( const BreakpointMap & ) {}
	//~BreakpointMap( void );
	
//	cloning:
	BreakpointMap * clone( void ) const { return new BreakpointMap( *this ); }

//	adding breakpoints:
	void insertBreakpoint( double time, double weight ); 
	
//	evaluation:
	double valueAt( double x ) const;
	
private:
//	-- instance variables --
	typedef std::vector< std::pair< double, double > > BreakpointsVector;
	BreakpointsVector _breakpoints;

};	//	end of class BreakpointMap

End_Namespace( Loris )

#endif	// ndef __Loris_abstract_map__
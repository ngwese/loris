// ===========================================================================
//	Map.C
//	
//	Map is an abstract base class representing a generic real (double) 
//	function of one real (double) argument. 
//
//	BreakpointMap is a simple subclass of Map, specifying a linear 
//	segment breakpoint function. 
//
//	-kel 26 Oct 99
//
// ===========================================================================

#include "LorisLib.h"
#include "Map.h"

#include <vector>
using namespace std;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	Xgte
// ---------------------------------------------------------------------------
//	Function object used for inserting breakpoints in order and
//	retrieving them.
//
struct Xgte
{
	Xgte( double z ) : _z( z ) {}
	boolean operator () ( const pair<double, double> & bp ) const { 
		return bp.first > _z;
	}
private:
	double _z;
};

// ---------------------------------------------------------------------------
//	insertBreakpoint
// ---------------------------------------------------------------------------
//	Insert or replace a breakpoint at x.
//	
void
BreakpointMap::insertBreakpoint( double x, double y )
{
	BreakpointsVector::iterator it = 
		find_if( _breakpoints.begin(), _breakpoints.end(), Xgte(x) );
	
	//	insert or replace:
	if ( (*it).first == x ) {
		(*it).second = y;
	}
	else {
		_breakpoints.insert( it, std::make_pair( x, y ) );
	}
}

// ---------------------------------------------------------------------------
//	valueAt
// ---------------------------------------------------------------------------
//
double
BreakpointMap::valueAt( double x ) const
{
	//	return zero if no breakpoints have been specified:
	if ( _breakpoints.size() == 0 ) {
		return 0.;
	}

	BreakpointsVector::const_iterator it = 
		find_if( _breakpoints.begin(), _breakpoints.end(), Xgte(x) );

	if ( it == _breakpoints.begin() ) {
		//	x is less than the first breakpoint, extend:
		return (*it).second;
	}
	else if ( it == _breakpoints.end() ) {
		//	x is greater than the last breakpoint, extend:
		return _breakpoints.back().second;
	}
	else {
		//	linear interpolation between consecutive breakpoints:
		double xless = (*(it-1)).first;
		double xgreater = (*it).first;
		double alpha = (x -  xless) / (xgreater - xless);
		return ( alpha * (*it).second ) 			//	alpha * ygreater
			 + ( (1. - alpha) * (*(it-1)).second );	//	1-alpha * yless
	}
}

End_Namespace( Loris )

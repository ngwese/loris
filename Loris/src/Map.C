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
//	insertBreakpoint
// ---------------------------------------------------------------------------
//	Insert or replace a breakpoint at x.
//	_breakpoints is a map<double, double>.
//	
void
BreakpointMap::insertBreakpoint( double x, double y )
{
	_breakpoints[x] = y;
}

// ---------------------------------------------------------------------------
//	valueAt
// ---------------------------------------------------------------------------
//	_breakpoints is a map<double, double>, so iterators are references to
//	key/value pairs.
//	
//
double
BreakpointMap::valueAt( double x ) const
{
	//	return zero if no breakpoints have been specified:
	if ( _breakpoints.size() == 0 ) {
		return 0.;
	}

	map< double, double >::const_iterator it = _breakpoints.lower_bound( x );

	if ( it == _breakpoints.begin() ) {
		//	x is less than the first breakpoint, extend:
		return (*it).second;
	}
	else if ( it == _breakpoints.end() ) {
		//	x is greater than the last breakpoint, extend:
		// 	(no direct way to access the last element of a map)
		--it;
		return (*it).second;
	}
	else {
		//	linear interpolation between consecutive breakpoints:
		double xgreater = (*it).first;
		double ygreater = (*it).second;
		--it;
		double xless = (*it).first;
		double yless = (*it).second;
		
		double alpha = (x -  xless) / (xgreater - xless);
		return ( alpha * ygreater ) + ( (1. - alpha) * yless );
	}

}

End_Namespace( Loris )

// ===========================================================================
//	BreakpointEnvelope.C
//	
//	-kel 8 Aug 2000
//
// ===========================================================================
#include "BreakpointEnvelope.h"
#include "notifier.h"

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	constructor
// ---------------------------------------------------------------------------
//
BreakpointEnvelope::BreakpointEnvelope(void)
{
	debugger << "constructing BreakpointEnvelope" << endl;
}

// ---------------------------------------------------------------------------
//	copy constructor
// ---------------------------------------------------------------------------
//
BreakpointEnvelope::BreakpointEnvelope(const BreakpointEnvelope & other) :
	_breakpoints( other._breakpoints )
{
	debugger << "copying BreakpointEnvelope" << endl;
}

// ---------------------------------------------------------------------------
//	destructor
// ---------------------------------------------------------------------------
//
BreakpointEnvelope::~BreakpointEnvelope(void)
{
	debugger << "destroying BreakpointEnvelope" << endl;
}

// ---------------------------------------------------------------------------
//	insertBreakpoint
// ---------------------------------------------------------------------------
//	Insert or replace a breakpoint at x.
//	_breakpoints is a map<double, double>.
//	
void
BreakpointEnvelope::insertBreakpoint( double x, double y )
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
BreakpointEnvelope::valueAt( double x ) const
{
	//	return zero if no breakpoints have been specified:
	if ( _breakpoints.size() == 0 ) 
	{
		return 0.;
	}

	std::map< double, double >::const_iterator it = _breakpoints.lower_bound( x );

	if ( it == _breakpoints.begin() ) 
	{
		//	x is less than the first breakpoint, extend:
		return it->second;
	}
	else if ( it == _breakpoints.end() ) 
	{
		//	x is greater than the last breakpoint, extend:
		// 	(no direct way to access the last element of a map)
		return (--it)->second;
	}
	else 
	{
		//	linear interpolation between consecutive breakpoints:
		double xgreater = it->first;
		double ygreater = it->second;
		--it;
		double xless = it->first;
		double yless = it->second;
		
		double alpha = (x -  xless) / (xgreater - xless);
		return ( alpha * ygreater ) + ( (1. - alpha) * yless );
	}
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

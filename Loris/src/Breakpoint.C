// ===========================================================================
//	Breakpoint.C
//
//	Implementation of Loris::Breakpoint.
//	
//	Loris Partials represent reassigned bandwidth-enhanced model components.
//	A Partial consists of a chain of Breakpoints describing the time-varying
//	frequency, amplitude, and bandwidth (noisiness) of the component.
//
//	-kel 16 Aug 99
//
// ===========================================================================

#include "LorisLib.h"

#include "Partial.h"
#include "Breakpoint.h"
#include "Exception.h"

Begin_Namespace( Loris )

#pragma mark -
#pragma mark construction

// ---------------------------------------------------------------------------
//	Breakpoint default constructor
// ---------------------------------------------------------------------------
//
Breakpoint::Breakpoint( void ) :
	_frequency( 0. ),
	_amplitude( 0. ),
	_bandwidth( 0. ),
	_phase( 0. )
{
}

// ---------------------------------------------------------------------------
//	Breakpoint constructor
// ---------------------------------------------------------------------------
//	Instantaneous phase (p) defaults to 0.
//	Values are not checked for validity.
//
Breakpoint::Breakpoint( double f, double a, double b, double p ) :
	_frequency( f ),
	_amplitude( a ),
	_bandwidth( b ),
	_phase( p )
{
}
/*
// ---------------------------------------------------------------------------
//	Breakpoint destructor
// ---------------------------------------------------------------------------
//
Breakpoint::~Breakpoint( void )
{
}	

// ---------------------------------------------------------------------------
//	Breakpoint copy constructor
// ---------------------------------------------------------------------------
//	Copy parameters only from another Breakpoint.
//	Is this too wierd?
//
Breakpoint::Breakpoint( const Breakpoint & other ) :
	_frequency( other.frequency() ),
	_amplitude( other.amplitude() ),
	_bandwidth( other.bandwidth() ),
	_phase( other.phase() )
{
}
*/	


End_Namespace( Loris )





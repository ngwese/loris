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
//	Breakpoint constructor
// ---------------------------------------------------------------------------
//	Instantaneous phase (p) defaults to 0.
//	Values are not checked for validity.
//
Breakpoint::Breakpoint( Double f, Double a, Double b, Double p ) :
	mFrequency( f ),
	mAmplitude( a ),
	mBandwidth( b ),
	mPhase( p ),
	mTime( 0 ),
	mPrevBreakpoint( Null ),
	mNextBreakpoint( Null )
{
}
	
// ---------------------------------------------------------------------------
//	Breakpoint destructor
// ---------------------------------------------------------------------------
//	Set pointers to Null, so that dangling references aren't linked.
//
Breakpoint::~Breakpoint( void )
{
	mPrevBreakpoint = mNextBreakpoint = Null;
}	

// ---------------------------------------------------------------------------
//	Breakpoint copy constructor
// ---------------------------------------------------------------------------
//	Copy parameters only from another Breakpoint.
//	Is this too wierd?
//
Breakpoint::Breakpoint( const Breakpoint & other ) :
	mFrequency( other.frequency() ),
	mAmplitude( other.amplitude() ),
	mBandwidth( other.bandwidth() ),
	mPhase( other.phase() ),
	mTime( 0 ),
	mPrevBreakpoint( Null ),
	mNextBreakpoint( Null )
{
}
	
// ---------------------------------------------------------------------------
//	operator= (assignment)
// ---------------------------------------------------------------------------
//	Copy parameters only from another Breakpoint.
//	Is this too wierd?
//
Breakpoint &
Breakpoint::operator=( const Breakpoint & other )
{
	if ( this != &other ) {
		mFrequency = other.frequency();
		mAmplitude = other.amplitude();
		mBandwidth = other.bandwidth();
		mPhase = other.phase();
	}
	
	return *this;
}

#pragma mark -
#pragma mark virtual constructors
// ---------------------------------------------------------------------------
//	Clone
// ---------------------------------------------------------------------------
//
Breakpoint * 
Breakpoint::Clone( const Breakpoint & p )
{
	try {
		return new Breakpoint( p );
	}
	catch ( LowMemException & ex ) {
		ex.append( "Failed to Clone a Breakpoint." );
		throw;
		return Null; 	//	not reached
	}
}

// ---------------------------------------------------------------------------
//	Create
// ---------------------------------------------------------------------------
//
Breakpoint * 
Breakpoint::Create( Double f, Double a, Double b, Double p )
{
	try {
		return new Breakpoint( f, a, b, p );
	}
	catch ( LowMemException & ex ) {
		ex.append(  "Failed to Create a Breakpoint." );
		throw;
		return Null; 	//	not reached
	}
}

// ---------------------------------------------------------------------------
//	Destroy
// ---------------------------------------------------------------------------
//
void 
Breakpoint::Destroy( Breakpoint * p )
{
	delete p;
}

End_Namespace( Loris )





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
#include "Breakpoint.h"
#include "Exception.h"

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
	using std::sqrt;
#else
	#include <math.h>
#endif

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif


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

// ---------------------------------------------------------------------------
//	Breakpoint constructor
// ---------------------------------------------------------------------------
//	Compute new amplitude and bandwidth values. Don't remove (add negative)  
//	noise energy in excess of the current noise energy.
//
void 
Breakpoint::addNoise( double noise )
{
	double e = amplitude() * amplitude();	//	current total energy
	double n = e * bandwidth();			//	current noise energy
	
	if ( n < noise ) {
		n = 0;
		e -= n;
	}
	else {
		n += noise;
		e += noise;
	}
	
	//	guard against divide-by-zero:
	if ( e > 0. ) {
		setBandwidth( ( n + noise ) / ( e + noise ) );
		setAmplitude( sqrt( e + noise ) );
	}
	else {
		setBandwidth( 0. );
		setAmplitude( 0. );
	}
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif





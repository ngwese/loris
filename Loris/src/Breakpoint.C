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
#include <cmath>

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
//	addNoise
// ---------------------------------------------------------------------------
//	Compute new amplitude and bandwidth values. Don't remove (add negative)  
//	noise energy in excess of the current noise energy.
//
void 
Breakpoint::addNoise( double noise )
{
	//	compute current energies:
	double e = amplitude() * amplitude();	//	current total energy
	double n = e * bandwidth();				//	current noise energy
	
	//	guard against divide-by-zero, and don't allow
	//	the sinusoidal energy to decrease:
	if ( n + noise > 0. ) 
	{
		//	if new noise energy is positive, total
		//	energy must also be positive:
		Assert( e + noise > 0 );
		setBandwidth( (n + noise) / (e + noise) );
		setAmplitude( std::sqrt(e + noise) );
	}
	else 
	{
		//	if new noise energy is negative, leave 
		//	all sinusoidal energy:
		setBandwidth( 0. );
		setAmplitude( std::sqrt( e - n ) );
	}
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif





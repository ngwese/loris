// ===========================================================================
//	FloatingThreshold.C
//	
//	Implementation of Loris function object FloatingThreshold, a predicate
//	that evaluates Partial amplitude envelopes against a floating threshold.
//
//	If I don't have a c++ file, I'll forget about this thing.
//
//	-kel 23 Jan 2000
//
// ===========================================================================
#include "LorisLib.h"
#include "FloatingThreshold.h"

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
	using std::pow;
#else
	#include <math.h>
#endif

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	FloatingThreshold constructor
// ---------------------------------------------------------------------------
FloatingThreshold::FloatingThreshold( double range_dB ) : 
	_ratio( pow( 10., - 0.05 * range_dB ) ) 
{
}
		
End_Namespace( Loris )

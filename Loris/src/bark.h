#ifndef INCLUDE_BARK_H
#define INCLUDE_BARK_H
// ===========================================================================
//	bark.h
//	
//	Inline function for computing Bark frequency from frequency in hertz.
//
//	-kel 23 Jan 2000
//
// ===========================================================================
#include <cmath>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	bark
// ---------------------------------------------------------------------------
//	Compute Bark frequency from Hertz.
//	Got this formula for Bark frequency from Sciarraba's thesis.
//	Someday, this could be a table lookup.
//
inline double bark( const double freq )
{
	double tmp = std::atan( ( 0.001 / 7.5 ) * freq );
	return  13. * std::atan( 0.76 * 0.001 * freq ) + 3.5 * ( tmp * tmp );
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	//	ndef INCLUDE_BARK_H
#ifndef __bark_frequency__
#define __bark_frequency__
/* ===========================================================================
 *	bark.h
 *	
 *	Inline function for computing Bark frequency from frequency in hertz.
 *
 *	-kel 23 Jan 2000
 *
 * ===========================================================================
*/
#include "LorisLib.h"

#if defined( __cplusplus ) && !defined( Deprecated_cstd_headers )
	#include <cmath>
	using std::atan;
#else
	#include <math.h>
#endif

Begin_Namespace( Loris )

/* ---------------------------------------------------------------------------
 *	bark
 *
 *	Compute Bark frequency from Hertz.
 *	Got this formula for Bark frequency from Sciarraba's thesis.
 *	Someday, this could be a table lookup.
 */
inline double bark( const double freq )
{
	double tmp = atan( ( 0.001 / 7.5 ) * freq );
	return  13. * atan( 0.76 * 0.001 * freq ) + 3.5 * ( tmp * tmp );
}

End_Namespace( Loris )

#endif	//	ndef __bark_frequency__
#ifndef INCLUDE_PI_H
#define INCLUDE_PI_H
/* ===========================================================================
 *	pi.h
 *	
 *	A definition of pi in the Loris namespace.
 *
 *	-kel 9 May 2000
 *
 * ===========================================================================
*/
#if defined( __cplusplus ) && !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

static const double Pi = 3.1415926535897932384626433L;
static const double TwoPi = 2. * Pi;

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	//	ndef INCLUDE_PI_H

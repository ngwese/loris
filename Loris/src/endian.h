#ifndef __endian_h__
#define __endian_h__
/*===========================================================================
 *	endian.h
 *
 *	Utilities for handling systems and files using different byte orders.
 *
 *	-kel 6 Oct 99
 */


#if defined( __cplusplus ) && !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

#ifdef __cplusplus
extern "C" {
#endif

int bigEndianSystem( void );
void swapByteOrder( char * bytes, int n );

#ifdef __cplusplus
}	//	end extern "C"
#endif

#if defined( __cplusplus ) && !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif 	/* ndef __endian_h__ */
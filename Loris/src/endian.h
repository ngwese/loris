#ifndef __endian_h__
#define __endian_h__
/*===========================================================================
 *	endian.h
 *
 *	Utilities for handling systems and files using different byte orders.
 *
 *	-kel 6 Oct 99
 */

#include "LorisLib.h"

Begin_Namespace( Loris )

#ifdef __cplusplus
extern "C" {
#endif

boolean bigEndianSystem( void );
void swapByteOrder( char * bytes, int n );

#ifdef __cplusplus
}	//	end extern "C"
#endif

End_Namespace( Loris )

#endif 	/* ndef __endian_h__ */
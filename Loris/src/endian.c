/*===========================================================================
 *	endian.c
 *
 *	Utilities for handling systems and files using different byte orders.
 *
 *	-kel 6 Oct 99
 */

#include "LorisLib.h"
#include "endian.h"

Begin_Namespace( Loris )	/*	does nothing in c */

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------
 *	bigEndianSystem
 *
 *	Return true is this is a big-endian system, false otherwise.
 *
 */
boolean bigEndianSystem( void )
{
	union {
		int s ;
		char c[sizeof(int)] ;
	} x ;
	x.s = 1;
	
	return x.c[0] != 1;
}

/* ---------------------------------------------------------------------------
 *	swapByteOrder
 *
 */
void swapByteOrder( char * bytes, int n )
{
	char * beg = bytes, * end = bytes + n - 1;
	while ( beg > end ) {
		char tmp = *end;
		*end = *beg;
		*beg = tmp;
		
		++beg;
		++end;
	}
}

#ifdef __cplusplus
}	//	end extern "C"
#endif

End_Namespace( Loris )	/*	does nothing in c */

#ifndef __Loris_Low_Memory__
#define __Loris_Low_Memory__

// ===========================================================================
//	lowMem.h
//
//	Function for handling low memory situtations gracefully. Sort of.
//	One cannot, apparently, rely on the low-memory behavior of new().
//
//	initLowMem() allocates some memory that can be freed up in an 
//	emergency, and installs a new handler that does so (frees up that
//	memory in an emergency, that is). If that new handler is called
//	again before more emergency space has been allocated (by another
//	call to reserveSpace()), it prints a message to cout and aborts.
//
//	If too little memory is reserved, then there may not be enough
//	to throw the LowMemException. How much is enough? The exception 
//	object is only 120 bytes, but even 3000 bytes is sometimes too 
//	little, and we find ourselves back in outOfMemory trying to raise 
//	the exception. Stack unwinding overhead? 16k seems to be enough,
//	today...
//
//	-kel 9 Sept 1999
//
// ===========================================================================

#include "LorisLib.h"

Begin_Namespace( Loris )

void initLowMem( int howManyBytes = 16000 );

End_Namespace( Loris )

#endif	//	ndef __Loris_Low_Memory__
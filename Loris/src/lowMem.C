// ===========================================================================
//	lowMem.C
//
//	Function for handling low memory situtations gracefully. Sort of.
//	One cannot, apparently, rely on the low-memory behavior of new().
//
//	initLowMem() allocates some memory that can be freed up in an 
//	emergency, and installs a new handler that does so (frees up that
//	memory in an emergency, that is). If that new handler is called
//	again before more emergency space has been allocated (by another
//	call to initLowMem()), it prints an error message and aborts.
//
//	Kelly Fitz 
//	-kel 9 Sept 1999
//
// ===========================================================================

#include "LorisLib.h"
#include "Exception.h"
#include "lowMem.h"
#include "Notifier.h"

#if !defined( Deprecated_cstd_headers )
	#include <new>
#else
	#include <new.h>
#endif

using namespace std;

Begin_Namespace( Loris )

static char * _emergencyBytes = Null;


// ---------------------------------------------------------------------------
//		outOfMemory
// ---------------------------------------------------------------------------
//	Install this function as the new handler; it tries to free up enough
//	memory to throw an exception safely.
//
//	If too little memory is reserved, then there may not be enough
//	to throw the LowMemException. How much is enough? The exception 
//	object is only 120 bytes, but even 3000 bytes is sometimes too 
//	little, and we find ourselves back in outOfMemory trying to raise 
//	the exception. Stack unwinding overhead? 16k seems to be enough,
//	today...
//
static void
outOfMemory( void )
{
	if ( _emergencyBytes != Null ) {
		delete[] _emergencyBytes;
		_emergencyBytes = Null;
		Throw( LowMemException, "Can't breathe! Loris is using emergency memory! Please free up some space!");
	}
	else {
		fatalError( "Loris ran completely out of memory, sorry." );
	}
}

// ---------------------------------------------------------------------------
//		initLowMem
// ---------------------------------------------------------------------------
//	Reserve some space for low-memory emergencies.
//	Default is 16000 bytes.
//
void
initLowMem( int howManyBytes )
{
	if ( _emergencyBytes == Null ) {
		_emergencyBytes = new char[howManyBytes];
		set_new_handler( outOfMemory );
	}
}


End_Namespace( Loris )
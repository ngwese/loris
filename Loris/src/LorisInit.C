// ===========================================================================
//	LorisInit.C
//	
//	Implementation of members of Loris::Init_.
//	This class does nothing, except initialize the Loris library in its
//	constructor (private, called only by instance) and finalize it in
//	its destructor.
//
//	A static const Init_ reference is created in every compilation that 
//	includes LorisLib.h, and it is initialized with this static member. 
//	No other instance of Init_ is possible, and Init_ has no functional 
//	members.
//
//	-kel 4 Oct 99
//
// ===========================================================================

#include "LorisLib.h"
#include "Exception.h"
#include "LorisTypes.h"
#include "LowMem.h"
#include "notify.h"

#include <string>

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	Init_ constructor
//
//	Put in here anything that needs to happen in order to initialize Loris.
//
Init_::Init_( void )
{
	// Debug("creating Loris Init__.");
	 
	try {
	 	//	check type sizes:
		Assert( check_types() );
		Assert( check_ctypes() );
		
		//	reserve memory and install a new handler:
		reserveSpace(16000);
	}
	catch( Exception & ex ) {
		ex << "Loris library initialization failed. ";
		fatalError( ex.str() );
	}
}

// ---------------------------------------------------------------------------
//	Init_ destructor
//
//	Put in here anything that needs to happen before Loris termiates.
//
Init_::~Init_( void )
{
	 // Debug("destroying Loris Init__.");
}

// ---------------------------------------------------------------------------
//	Init_ instance access
//
//	A static const reference is created in every compilation that includes
//	LorisLib.h, and it is initialized with this static member. No other
//	instance of Init_ is possible, and Init_ has no functional members.
//
const Init_ &
Init_::instance( void )
{
	static const Init_ inst;
	return inst;
}
		 
End_Namespace( Loris )
// ===========================================================================
//	LorisInit.C
//	
//	Implementation of members of Loris::Init_.
//	This class does nothing, except initialize the Loris library in its
//	constructor and finalize it in its destructor. It is not thread safe,
//	perhaps, but it is re-entrant.
//
//	A static const Init_ reference is created in every compilation that 
//	includes LorisLib.h. 
//
//	-kel 4 Oct 99
//
// ===========================================================================
#include "LorisLib.h"
#include "LorisInit.h"
#include "Exception.h"
#include "LorisTypes.h"
#include "lowMem.h"
#include "Notifier.h"

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//		Init_ counter
// ---------------------------------------------------------------------------
//
int Init_::_init_count = 0;

// ---------------------------------------------------------------------------
//	Init_ constructor
// ---------------------------------------------------------------------------
//
Init_::Init_( void ) 
{
	if ( 0 == _init_count++ )
		initLib();
}

// ---------------------------------------------------------------------------
//	Init_ destructor
// ---------------------------------------------------------------------------
//
Init_::~Init_( void ) 
{
	if ( 0 == --_init_count )
		finalLib();
}

// ---------------------------------------------------------------------------
//	initLib
// ---------------------------------------------------------------------------
//	Put in here anything that needs to happen in order to initialize Loris.
//	No exceptions please.
//
void
Init_::initLib( void ) throw()
{
	debugger << "Initializing Loris library." << endl;
	
	try {
	 	//	check type sizes:
		if( ! check_types() )
			Throw( InvalidObject, "check_types() reports incorrect C++ type sizes for Loris." ); 
		if( ! check_ctypes() )
			Throw( InvalidObject, "check_ctypes() reports incorrect c type sizes for Loris." ); 
		
		//	reserve memory and install a new handler:
		initLowMem(16000);
	}
	catch( Exception & ex ) {
		ex.append( "Loris library initialization failed." );
		fatalError( ex.str() );
	}
	catch( ... ) {
		fatalError( "Loris library initialization failed, unknown exception." );
	}
}

// ---------------------------------------------------------------------------
//	Init_ finalLib
// ---------------------------------------------------------------------------
//	Put in here anything that needs to happen before Loris termiates.
//	No exceptions please.
//
void
Init_::finalLib( void ) throw()
{
	debugger << "Finalizing Loris library." << endl;	
}

End_Namespace( Loris )

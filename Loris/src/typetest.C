// ===========================================================================
//	typetest.C
//	
//	Test that size-critical types are the right size.
//
//	-kel 4 Oct 99
//
// ===========================================================================

#include "LorisLib.h"
#include "LorisTypes.h"
#include "Notifier.h"

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	check_types
// ---------------------------------------------------------------------------
//
boolean
check_types( void )
{
	boolean ret = true;
	
	if ( sizeof( Int_16 ) != 2 )
	{
		notifier << "Size of 16 bit int is " << sizeof(Int_16) << ", should be 2!" << endl;
		ret = false;
	}

	if ( sizeof( Int_32 ) != 4 )
	{
		notifier << "Size of 32 bit int is " << sizeof(Int_32) << ", should be 4!" << endl;
		ret = false;
	}

	if ( sizeof( Uint_32 ) != 4 )
	{
		notifier << "Size of unsigned 32 bit int is " << sizeof(Uint_32) << ", should be 4!" << endl;
		ret = false;
	}

	if ( sizeof( Float_32 ) != 4 )
	{
		notifier << "Size of 32 bit float is " << sizeof(Float_32) << ", should be 4!" << endl;
		ret = false;
	}

	if ( sizeof( Double_64 ) != 8 )
	{
		notifier << "Size of 64 bit double is " << sizeof(Double_64) << ", should be 8!" << endl;
		ret = false;
	}

	return ret;
}


End_Namespace( Loris )
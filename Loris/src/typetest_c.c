/* ===========================================================================
 *	typetest_c.c
 *	
 *	Test that size-critical types are the right size in c.
 *
 *	-kel 4 Oct 99
 *
 */ 
#include "LorisLib.h"
#include "LorisTypes.h"
#include "cnotify.h"

#include <stdio.h>

boolean
check_ctypes( void )
{
	boolean ret = true;
	
	if ( sizeof( Int_16 ) != 2 )
	{
		char cstr[256];
		sprintf( cstr, "Size of 16 bit int is %d, should be 2!", sizeof(Int_16) );
		notify( cstr );
		ret = false;
	}

	if ( sizeof( Int_32 ) != 4 )
	{
		char cstr[256];
		sprintf( cstr, "Size of 32 bit int is %d, should be 4!", sizeof(Int_32) );
		notify( cstr );
		ret = false;
	}

	if ( sizeof( Uint_32 ) != 4 )
	{
		char cstr[256];
		sprintf( cstr, "Size of unsigned 32 bit int is %d, should be 4!", sizeof(Uint_32) );
		notify( cstr );
		ret = false;
	}

	if ( sizeof( Float_32 ) != 4 )
	{
		char cstr[256];
		sprintf( cstr, "Size of 32 bit float is %d, should be 4!", sizeof(Float_32) );
		notify( cstr );
		ret = false;
	}

	if ( sizeof( Double_64 ) != 8 )
	{
		char cstr[256];
		sprintf( cstr, "Size of 64 bit double is %d, should be 8!", sizeof(Double_64) );
		notify( cstr );
		ret = false;
	}

	return ret;
}


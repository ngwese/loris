// ===========================================================================
//	Import.h
//	
//	Implementation of Loris::Import, an abstract base class for
//	objects that import Partials into Loris, including the
//	Template Method Import::importPartials().
//
//
//	-kel 10 Sept 99
//
// ===========================================================================


#include "LorisLib.h"
#include "Import.h"
#include "Partial.h"
#include "Exception.h"

Begin_Namespace( Loris )

#pragma mark -
#pragma mark construction
// ---------------------------------------------------------------------------
//	Import constructor
// ---------------------------------------------------------------------------
//
Import::Import( void )
{
}

// ---------------------------------------------------------------------------
//	Import destructor
// ---------------------------------------------------------------------------
//	Any Partials remaining in the list mPartials will be destroyed 
//	automatically.
//
Import::~Import( void )
{
}


#pragma mark -
#pragma mark Template Method
// ---------------------------------------------------------------------------
//	importPartials
// ---------------------------------------------------------------------------
//	Template Method for importing Partials from somewhere. Derived classes
//	must implement the primitive operations:
//
//		void verifySource( void )	(default is noop)
//		void beginImport( void )	(default is noop)
//		Boolean done( void )
//		void getPartial( void )
//		void endImport( void )		(default is noop)
//
//	Clients should be prepared to catch ImportErrors.
//
void
Import::importPartials( void )
{
	try {
	//	make sure that the file is valid, open, the right format, etc.
		verifySource();

	//	find the chunk with the partials in it,
	//	count the partials(?)
	//	make/get a container for storing the imported partials, 
	//	maybe this will be passed in, or more likely, it will
	//	be part of a class for importing.
	//
	//	can actually allocate all the Partials at once? Only if 
	//	there is some way to get them out of the importer
	//	without copying them all: splice
	//
	//	to avoid hitting the disk thousands of times, probably want
	//	to allocate a read buffer that can be grown when needed,
	//	and also freed at the end.
		beginImport();

	//	loop: read one partial at a time
		while( ! done() ) {
		//	read partial: read Partial header (number of Breakpoints and label)
		//	also initial phase, subsequent phases will be blown off for now.
		//	actually, we can keep a running phase, now that we only use linear
		//	frequency.
		//
		//	then read Breakpoints into a buffer, then loop over the buffer
		//	creating breakpoints and appending them to the Partial.
		//
			getPartial();
		}

	//	when done, free up that buffer? or wait til the destructor?
		endImport();
	}
	catch( ImportError & ex ) {
		ex.append( " Import failed." );
		throw;
	}
}


End_Namespace( Loris )


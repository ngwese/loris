// ===========================================================================
//	ImportLemur5.C
//	
//	Implementation of Loris::ImportLemur5, a concrete subclass of 
//	Loris::Import for importing Partials stored in Lemur 5 alpha files.
//
//
//	-kel 10 Sept 99
//
// ===========================================================================


#include "LorisLib.h"
#include "ImportLemur5.h"

Begin_Namespace( Loris )

#pragma mark -
#pragma mark construction
// ---------------------------------------------------------------------------
//	ImportLemur5 constructor
// ---------------------------------------------------------------------------
//
ImportLemur5::ImportLemur5( void )
{
}

// ---------------------------------------------------------------------------
//	ImportLemur5 destructor
// ---------------------------------------------------------------------------
//
ImportLemur5::~ImportLemur5( void )
{
}

#pragma mark -
#pragma mark primitive operations
// ---------------------------------------------------------------------------
//	verifySource
// ---------------------------------------------------------------------------
//	make sure that the file is valid, open, the right format, etc.
//
void
ImportLemur5::verifySource( void )
{
}

// ---------------------------------------------------------------------------
//	beginImport
// ---------------------------------------------------------------------------
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
//
void
ImportLemur5::beginImport( void )
{
}

// ---------------------------------------------------------------------------
//	done
// ---------------------------------------------------------------------------
//	Return true if the number of Partials read equals the number of
//	Partials in the Lemur file.
//
Boolean
ImportLemur5::done( void )
{
	return true;
}

// ---------------------------------------------------------------------------
//	getPartial
// ---------------------------------------------------------------------------
//	read partial: read Partial header (number of Breakpoints and label)
//	also initial phase, subsequent phases will be blown off for now.
//	actually, we can keep a running phase, now that we only use linear
//	frequency.
//
//	then read Breakpoints into a buffer, then loop over the buffer
//	creating breakpoints and appending them to the Partial.
//
void
ImportLemur5::getPartial( void )
{
}

// ---------------------------------------------------------------------------
//	endImport
// ---------------------------------------------------------------------------
//	when done, free up that buffer? or wait til the destructor?
//
void
ImportLemur5::endImport( void )
{
}

End_Namespace( Loris )

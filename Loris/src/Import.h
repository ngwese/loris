#ifndef __Loris_import__
#define __Loris_import__

// ===========================================================================
//	Import.h
//	
//	Class definition for Loris::Import, an abstract base class for
//	objects that import Partials into Loris.
//
//
//	-kel 10 Sept 99
//
// ===========================================================================


#include "LorisLib.h"
#include "Partial.h"
#include "Exception.h"

#include <list>
using std::list;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class Import
//
//	Import is an abstract base class for objects that bring Partials
//	into Loris. Most often, the Partials will be stored in a file somewhere,
//	but not necessarily. 
//
//	Import has a Template Method (GoF p.325), importPartials() for performing
//	the import. Primitive operations must be implemented by subclasses.
//
//	The imported Partials are stored in a (STL) list that is accessible to
//	clients of Import. Any Partials remaining in the list are destroyed
//	with the Import object. Partials can be transfered from one list to 
//	another _without_ copying using list.splice().
//
class Import
{
//	-- public Import interface --
public:
//	template method for importing partials:
	void importPartials( void );
	
//	access to imported Partials:
	list< Partial > & partials( void ) { return mPartials; }
	const list< Partial > & partials ( void ) const { return mPartials; }
	
//	-- primitve operations --
	//	check that source of Partials is valid or ready:
	virtual void verifySource( void ) {}
	
	//	prepare to import:
	virtual void beginImport( void ) {}
	
	//	derived classes must provide a mean of determining
	//	when the import loop should terminate:
	virtual Boolean done( void ) = 0;
	
	//	derived classes must provide a means of reading 
	//	in a Partial and adding it to the list mPartials.
	virtual void getPartial( void ) = 0;	

	//	clean up after import:
	virtual void endImport( void ) {}

protected:
//	construction:
	Import( void );
	virtual ~Import( void );

//	-- instance variables --
	list< Partial > mPartials;	//	list<> of imported Partials

};	//	end of class Import

// ---------------------------------------------------------------------------
//	class ImportError
//
//	Class of exceptions thrown when there is an error importing
//	Partials.
//
class ImportError : public RuntimeException
{
public: 
	ImportError( const string & str, const string & in = "", Int at = 0L ) : 
		RuntimeException( string("Import Error: ").append( str ), in, at ) {}
		
};	//	end of class RuntimeException


End_Namespace( Loris )

#endif	// ndef __Loris_import__


#ifndef __Loris_import_Lemur5__
#define __Loris_import_Lemur5__

// ===========================================================================
//	ImportLemur5.h
//	
//	Class definition for Loris::ImportLemur5, a concrete subclass of 
//	Loris::Import for importing Partials stored in Lemur 5 alpha files.
//
//
//	-kel 10 Sept 99
//
// ===========================================================================


#include "LorisLib.h"
#include "Import.h"

#include <list>
using std::list;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class ImportLemur5
//
//
class ImportLemur5: public Import
{
//	-- public interface --
public:
//	construction:
	ImportLemur5( void );
	virtual ~ImportLemur5( void );

//	-- primitve operations --
	//	check that Lemur file is valid:
	virtual void verifySource( void );
	
	//	prepare to import:
	virtual void beginImport( void );
	
	//	derived classes must provide a mean of determining
	//	when the import loop should terminate:
	virtual Boolean done( void );
	
	//	derived classes must provide a means of reading 
	//	in a Partial and adding it to the list mPartials.
	virtual void getPartial( void );	

	//	clean up after import:
	virtual void endImport( void );

//	-- instance variables --
private:


};	//	end of class ImportLemur5

End_Namespace( Loris )

#endif	// ndef __Loris_import_Lemur5__


#ifndef INCLUDE_IMPORTLEMUR_H
#define INCLUDE_IMPORTLEMUR_H
// ===========================================================================
//	ImportLemur.h
//	
//	Class definition for Loris::ImportLemur, a concrete subclass of 
//	Loris::Import for importing Partials stored in Lemur 5 alpha files.
//
//
//	-kel 10 Sept 99
//
// ===========================================================================
#include "Partial.h"
#include "Exception.h"

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class ImportLemur
//
class ImportLemur
{
//	-- instance variables --
	PartialList _partials;	//	collect Partials here

//	-- public interface --
public:
//	construction:
//	(compiler can generate destructor)
	ImportLemur( const char * fname, double bweCutoff = 1000 );

//	PartialList access:
	PartialList & partials( void ) { return _partials; }
	const PartialList & partials( void ) const { return _partials; }
	
//	-- unimplemented --
private:
	ImportLemur( const ImportLemur & other );
	ImportLemur  & operator = ( const ImportLemur & rhs );
	
};	//	end of class ImportLemur

// ---------------------------------------------------------------------------
//	class ImportException
//
//	Class of exceptions thrown when there is an error importing
//	Partials.
//
class ImportException : public Exception
{
public: 
	ImportException( const std::string & str, const std::string & where = "" ) : 
		Exception( std::string("Import Error -- ").append( str ), where ) {}		
};

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_IMPORTLEMUR_H


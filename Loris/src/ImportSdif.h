#ifndef INCLUDE_IMPORTSDIF_H
#define INCLUDE_IMPORTSDIF_H
// ===========================================================================
//	SdifReader.h
//	
//	Association of info	to read an SDIF file.  This imports the
//	1TRC SDIF format.
//
//	-lip 4 Jul 00
//
// ===========================================================================
#include "Partial.h"

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class ImportSdif
//	
class ImportSdif
{
//	-- instance variables --
	PartialList _partials;	//	collect Partials here

//	-- public interface --
public:
//	construction:
//	(let compiler generate destructor)
	ImportSdif( const char *infilename );
		
//	PartialList access:
	PartialList & partials( void ) { return _partials; }
	const PartialList & partials( void ) const { return _partials; }
	
//	-- unimplemented --
private:
	ImportSdif( const ImportSdif & other );
	ImportSdif  & operator = ( const ImportSdif & rhs );

};	//	end of class ImportSdif

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif //	ndef INCLUDE_IMPORTSDIF_H

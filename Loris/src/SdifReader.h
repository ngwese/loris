#ifndef INCLUDE_SDIFREADER_H
#define INCLUDE_SDIFREADER_H
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
//	class SdifReader
//	
class SdifReader
{
//	-- instance variables --
	PartialList _partials;	//	collect Partials here

//	-- public interface --
public:
//	construction:
//	(let compiler generate destructor)
	SdifReader( const char *infilename );
		
//	PartialList access:
	PartialList & partials( void ) { return _partials; }
	const PartialList & partials( void ) const { return _partials; }
	
//	-- unimplemented --
private:
	SdifReader( const SdifReader & other );
	SdifReader  & operator = ( const SdifReader & rhs );

};	//	end of class SdifReader

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif //	ndef INCLUDE_SDIFREADER_H

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
#include "Exception.h"
#include <vector>
#include <stdio.h>	//	for FILE

extern "C" {
#include "sdif.h"
#include "sdif-types.h"
}

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

//	Row of matrix data in SDIF 1TRC format.
//	Loris exports both a 6-column (resampled) and 8-column (exact times) format.
//  The 6 column format excludes timeOffset and discardable.
typedef struct {
    sdif_float32 index, freq, amp, phase, noise, label, timeOffset, discardable;
} RowOfLorisData32;

typedef struct {
    sdif_float64 index, freq, amp, phase, noise, label, timeOffset, discardable;
} RowOfLorisData64;


// ---------------------------------------------------------------------------
//	class SdifReader
//	
class SdifReader
{

public:
//	construction:
//	(let compiler generate destructor)
	SdifReader( void );
	
//	writing:
	void read( const char *infilename, std::list<Partial> & partials );
	
//	-- helpers --
private:
	//	envelopes writing:
	void readEnvelopeData( FILE *in, std::vector< Partial * > & partialsVector );
	void readMatrixData( FILE *in, const SDIF_MatrixHeader & mh, 
					const double frameTime, std::vector< Partial * > & partialsVector );
	void readRowData( FILE *in, const SDIF_MatrixHeader & mh, RowOfLorisData64 & trackData );
	void readRow32( FILE *in, const SDIF_MatrixHeader & mh, RowOfLorisData32 & trackData );
	void readRow64( FILE *in, const SDIF_MatrixHeader & mh, RowOfLorisData64 & trackData );
	void addRowToPartials( const RowOfLorisData64 & trackData, const double frameTime, 
					std::vector< Partial * > & partialsVector );
	
};	//	end of class SdifReader

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif //	ndef INCLUDE_SDIFREADER_H

#ifndef INCLUDE_SDIFWRITER_H
#define INCLUDE_SDIFWRITER_H
// ===========================================================================
//	SdifWriter.h
//	
//	Association of info	to completely specify an SDIF file.  This exports the
//	1TRC SDIF format.
//
//	-lip 4 Jul 00
//
// ===========================================================================
#include "Partial.h"
#include <vector>
#include <stdio.h>	//	for FILE

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	BreakpointTime
//
//  Sorted BreakpointTimes are used in finding frame start times in non-resampled SDIF writing.
//
struct BreakpointTime
{
	int index;			// index identifying which partial has the breakpoint
	float time;			// time of the breakpoint
};

// ---------------------------------------------------------------------------
//	class SdifWriter
//	
class SdifWriter
{
//	-- instance variables --
	double _hop;		// frame rate in seconds, or 0.0 if we are not resampling envelopes

public:
//	construction:
//	(let compiler generate destructor)
	SdifWriter( const double hop = 0.0 );
	
//	writing:
	void write( const char *outfilename, const std::list<Partial> & partials );
	
//	-- helpers --
private:
	//	envelopes writing:
	void writeEnvelopeData( FILE *out, const std::vector< Partial * > & partialsVector );
	void writeFrameHeader( FILE *out, const int streamID, const int numTracks, const double frameTime );
	void writeMatrixHeader( FILE *out, const int numTracks );
	void writeMatrixData( FILE *out, const std::vector< Partial * > & partialsVector, 
								const std::vector< int > & activeIndices, 
								const double frameTime, const double nextFrameTime );
	
	//	envelope writing helpers:
	void indexPartials( const PartialList & partials, std::vector< Partial * > & partialsVector );
	int collectActiveIndices( const std::vector< Partial * > & partialsVector, 
								const double frameTime, const double nextFrameTime,
								std::vector< int > & activeIndices );

	//	envelope frame time helpers:
	void makeSortedBreakpointTimes( const std::vector< Partial * > & partialsVector, 
									std::list< BreakpointTime > & allBreakpoints);
	double getNextFrameTime( const double frameTime, std::list< BreakpointTime > & allBreakpoints,
							 std::list< BreakpointTime >::iterator & bpTimeIter);

};	//	end of class SdifWriter




#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif //	ndef INCLUDE_SDIFWRITER_H

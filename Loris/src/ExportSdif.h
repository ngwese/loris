#ifndef INCLUDE_EXPORTSDIF_H
#define INCLUDE_EXPORTSDIF_H
/*
 * Copyright (c) 1999-2000 Kelly Fitz and Lippold Haken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * ExportSdif.h
 *
 * Definition of class ExportSdif, which exports the
 * 1TRC SDIF format.
 *
 * Lippold Haken, 4 July 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "Partial.h"
#include <vector>
#include <stdio.h>	//	for FILE
#include <list>

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
//	class ExportSdif
//	
class ExportSdif
{
//	-- instance variables --
	double _hop;		// frame rate in seconds, or 0.0 if we are not resampling envelopes

public:
//	construction:
//	(let compiler generate destructor)
	ExportSdif( const double hop = 0.0 );
	
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

};	//	end of class ExportSdif

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif //	ndef INCLUDE_EXPORTSDIF_H

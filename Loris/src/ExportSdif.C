/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
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
 * ExportSdif.C
 *
 * Implementation of class ExportSdif, which exports the
 * 1TRC SDIF format.
 *
 * Lippold Haken, 4 July 2000
 * Lippold Haken, 20 October 2000, using IRCAM SDIF library
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "ExportSdif.h"
#include "Exception.h"
#include "notifier.h"
#include "Partial.h"
#include <list>
#include <vector>
#include <cmath>

extern "C" {
#include <sdif.h>
}

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

//  SDIF signature used by Loris.
static SdifSignature lorisSignature = SdifSignatureConst('1TRC');

//  sorted BreakpointTimes are used in finding frame start times in non-resampled SDIF writing.
struct BreakpointTime
{
	int index;			// index identifying which partial has the breakpoint
	float time;			// time of the breakpoint
};

//	prototypes for envelope writing helpers:
static void writeEnvelopeData( SdifFileT *out, const double hop,
							const std::vector< Partial * > & partialsVector );
static void assembleMatrixData( SdifFloat4 *data, 
							const std::vector< Partial * > & partialsVector, 
							const int useExactTiming,
							const std::vector< int > & activeIndices, 
							const double frameTime, const double nextFrameTime );
static void indexPartials( const std::list< Partial > & partials, std::vector< Partial * > & partialsVector );
static int collectActiveIndices( const std::vector< Partial * > & partialsVector, 
							const double hop,
							const double frameTime, const double nextFrameTime,
							std::vector< int > & activeIndices );

//	prototypes for envelope frame time helpers:
static void makeSortedBreakpointTimes( const std::vector< Partial * > & partialsVector,
							const int useExactTiming, 
							std::list< BreakpointTime > & allBreakpoints);
static double getNextFrameTime( const double frameTime,
							std::list< BreakpointTime > & allBreakpoints,
							std::list< BreakpointTime >::iterator & bpTimeIter);


// ---------------------------------------------------------------------------
//	ExportSdif constructor from data in memory
// ---------------------------------------------------------------------------
// Set _hop to frame rate, or to 0.0 if we are not resampling the envelopes.
// If _hop is nonzero we export a 6 column format; if _hop is zero its an 8 column format.
//
ExportSdif::ExportSdif( const double hop ) :
	_hop( hop )
{
	Assert( hop >= 0.0 );
}

// ---------------------------------------------------------------------------
//	write
// ---------------------------------------------------------------------------
// Let exceptions propagate.
//
void
ExportSdif::write( const char *outfilename, const list<Partial> & partials )
{
// 
// Initialize SDIF library.
//
	SdifGenInit("");
	
//
// Open SDIF file for writing.
//
	SdifFileT *out = SdifFOpen(outfilename, eWriteFile);
	if (!out)
		Throw( FileIOException, "Could not open SDIF file for writing." );

	// Write file header information 
	SdifFWriteGeneralHeader( out );    
	
	// Write ASCII header information 
	SdifFWriteAllASCIIChunks( out );    
	
//
// Write SDIF data.
//	
	try 
	{
		// Make vector of pointers to partials.
		std::vector< Partial * > partialsVector;
		indexPartials( partials, partialsVector );
		
		// Write partials to SDIF file.
		writeEnvelopeData( out, _hop, partialsVector );
	}
	catch ( Exception & ex ) 
	{
		ex.append("Failed to write SDIF file.");
		SdifFClose(out);
		SdifGenKill();
		throw;
	}

//
// Close SDIF input file.
//
	SdifFClose(out);

// 
// Done with SDIF library.
//
	SdifGenKill();
}

#pragma mark -
#pragma mark envelope writing helpers
// ---------------------------------------------------------------------------
//	writeEnvelopeData
// ---------------------------------------------------------------------------
//
void
writeEnvelopeData( SdifFileT * out, const double hop,
				const std::vector< Partial * > & partialsVector )
{
//
// Export SDIF file from Loris data.
// Let exceptions propagate.
//

	int streamID = 1; 						// one stream id for all frames
	int useExactTiming = (hop == 0.0);		// hop is nonzero if we are resampling data

//
// Make a sorted list of all breakpoints in all partials, and initialize the list iterater.
// This stuff does nothing if we are writing a resampled SDIF file.
//
	std::list< BreakpointTime > allBreakpoints;
	makeSortedBreakpointTimes( partialsVector, useExactTiming, allBreakpoints );
	std::list< BreakpointTime >::iterator bpTimeIter = allBreakpoints.begin();

//
// Output Loris data in SDIF frame format.
// First frame starts at millisecond of first breakpoint, for exact timing SDIF files.
//
	double nextFrameTime	= useExactTiming 
							? std::floor( 1000.0 * bpTimeIter->time - 0.001 ) / 1000.0 
							: 0.0;

	double frameTime; 
	int endOfAll;
	do 
	{

//
// Go to next frame.
//
		frameTime = nextFrameTime;
		nextFrameTime = useExactTiming 
							? getNextFrameTime( frameTime, allBreakpoints, bpTimeIter )
							: frameTime + hop;

//
// Make a vector of partial indices that includes all partials active at this time.
//
		std::vector< int > activeIndices;
		endOfAll = collectActiveIndices( partialsVector, hop,
									frameTime, nextFrameTime, activeIndices );

//
// Write frame header, matrix header, and matrix data.
// We always have one matrix per frame.
// The matrix size depends on the number of partials active at this time.
//
		int numTracks = activeIndices.size();
		if (numTracks > 0) 
		{
		
			// Allocate matrix data.
			int cols = useExactTiming ? 8 : 6;
			SdifFloat4 *data = new SdifFloat4[numTracks * cols];

			// Fill in matrix data.
			assembleMatrixData( data, partialsVector, useExactTiming, 
								activeIndices, frameTime, nextFrameTime );
								
			// Write out matrix data.
			SdifFWriteFrameAndOneMatrix( out, 
					lorisSignature, streamID, frameTime, 				// frame header
					lorisSignature, eFloat4, numTracks, cols, data);	// matrix 
			
			// Free matrix space.
			delete [] data;
		}
	}
	while (!endOfAll);
}


// ---------------------------------------------------------------------------
//	assembleMatrixData
// ---------------------------------------------------------------------------
//	The activeIndices vector contains indices for partials that have data at this time.
//	Assemble SDIF matrix data for these partials.
//
void
assembleMatrixData( SdifFloat4 *data, 
								const std::vector< Partial * > & partialsVector, 
								const int useExactTiming,
								const std::vector< int > & activeIndices, 
								const double frameTime, const double nextFrameTime )
{	
	// The array matrix data is row-major order at "data".
	SdifFloat4 *trackDataPtr = data;
	
	for ( int i = 0; i < activeIndices.size(); i++ ) 
	{
		
		int index = activeIndices[i];
		Partial * par = partialsVector[index];
		
		// If we are doing exact timing (not resampling):
		// Use data at breakpoint if one is in frame, else fabricate data at frameTime.
		double tim = frameTime;
		double discardable = 0.0;
		if (useExactTiming) 			// not resampling; doing exact timing
		{
			PartialIterator it = par->findPos(frameTime);
			if (it != par->end() && it.time() < nextFrameTime) 
				tim = it.time();		// use data at breakpoint time
			else 
				discardable = 1;		// no breakpoint in frame; interpolated data point
		}
		
		// Fill in values for this row of matrix data.
		*trackDataPtr++ = index;							// first row of matrix   (standard)
		*trackDataPtr++	= par->frequencyAt( tim );			// second row of matrix  (standard)
		*trackDataPtr++ = par->phaseAt( tim );				// third row of matrix   (standard)
		*trackDataPtr++	= par->amplitudeAt( tim );			// fourth row of matrix  (standard)
		*trackDataPtr++ = par->bandwidthAt( tim );			// fifth row of matrix   (loris)
		*trackDataPtr++	= par->label();						// sixth row of matrix   (loris)
		if (useExactTiming) 
		{
			*trackDataPtr++  = tim - frameTime;			// seventh row of matrix (loris optional)
			*trackDataPtr++ = discardable;				// eighth row of matrix  (loris optional)
		}
	}
}


// ---------------------------------------------------------------------------
//	indexPartials
// ---------------------------------------------------------------------------
//	Make a vector of partial pointers. 
//  The vector index will be the sdif 1TRC index for the partial. 
//
void
indexPartials( const std::list< Partial > & partials, std::vector< Partial * > & partialsVector )
{
	for (std::list< Partial >::const_iterator it = partials.begin(); it != partials.end(); ++it)
		if (it->begin() != it->end())
			partialsVector.push_back((Partial *)&(*it));	
}


// ---------------------------------------------------------------------------
//	collectActiveIndices
// ---------------------------------------------------------------------------
//	Collect all partials active in a particular frame. 
//  If we are resampling, then we will want to include partials that become
//  active next frame or were active last frame.
//
//  Return true if time is beyond end of all the partials.
//
int
collectActiveIndices( const std::vector< Partial * > & partialsVector, 
								const double hop,
								const double frameTime, const double nextFrameTime,
								std::vector< int > & activeIndices )
{
	int endOfAll = true;
	
	for (int i = 0; i < partialsVector.size(); i++) 
	{
		// Is there a breakpoint within the frame?
		// if there is no breakpoint and zero amplitude, skip the partial.
		// Resampled partials are included if the next frame (or previous frame) had a breakpoint.
		PartialIterator it = partialsVector[i]->findPos(frameTime - hop);
		if ((it != partialsVector[i]->end() && it.time() < nextFrameTime + hop) 
					|| partialsVector[i]->amplitudeAt(frameTime) != 0.0) 
			activeIndices.push_back(i);	

		if (partialsVector[i]->endTime() > frameTime + hop)
			endOfAll = false;
	}
	return endOfAll;
}


#pragma mark -
#pragma mark frame time helpers


	

// ---------------------------------------------------------------------------
//	makeSortedBreakpointTimes
// ---------------------------------------------------------------------------
//	Collect the times of all breakpoints in the analysis, and sort by time.
//  Sorted breakpoints are used in finding frame start times in non-resampled SDIF writing.
//
struct earlier_time
{
	bool operator()( const BreakpointTime & lhs, const BreakpointTime & rhs ) const
		{ return lhs.time < rhs.time; }
};

void
makeSortedBreakpointTimes( const std::vector< Partial * > & partialsVector, 
						   const int useExactTiming, 
						   std::list< BreakpointTime > & allBreakpoints ) 
{

// If we are resampling envelopes, we don't need the sorted list; just return.
	if (!useExactTiming)
		return;

// Make list of all breakpoint times from all partials.
	for (int i = 0; i < partialsVector.size(); i++) 
	{
		for ( PartialIterator it = partialsVector[i]->begin(); 
			  it != partialsVector[i]->end();
			  ++it ) 
		{
			BreakpointTime bpt;
			bpt.index = i;
			bpt.time = it.time();
			allBreakpoints.push_back( bpt );
		}
	}

// Sort list of all breakpoint times.
	allBreakpoints.sort( earlier_time() );
}

// ---------------------------------------------------------------------------
//	getNextFrameTime
// ---------------------------------------------------------------------------
//	Get time of next frame.
//  This is called only when writing SDIF files with exact timing (envelopes not resampled).
//  This uses the previously sorted allBreakpoints list.
//
//	all Breakpoints should be const, but for some reason, gcc (on SGI at 
//	least) makes trouble converting and comparing iterators and const_iterators.
//
double
getNextFrameTime( const double frameTime,
							std::list< BreakpointTime > & allBreakpoints,
							std::list< BreakpointTime >::iterator & bpTimeIter)
{
//
// Build up vector of partials that have a breakpoint in this frame, update the vector
// as we increase the frame duration.  Return when a partial gets a second breakpoint.
//
	double nextFrameTime = frameTime;
	std::vector< int > partialsWithBreakpointsInFrame;
	std::list< BreakpointTime >::iterator it = bpTimeIter;
	while ( it != allBreakpoints.end() ) 
	{
		// Return if next breakpoint's partial already in this frame.
		for (int i = 0; i < partialsWithBreakpointsInFrame.size(); i++) 
		{
			if ( it->index == partialsWithBreakpointsInFrame[i] )
				return  nextFrameTime;
		}
		
		// Add breakpoint to frame, iterate to soonest breakpoint on any partial.
		partialsWithBreakpointsInFrame.push_back( it->index );
		it++;
		
		// If the new breakpoint is at a new time, it could potentially be the
		//	first breakpoint in the next frame. If there are several breakpoints at
		//	the exact same time (could happen if these envelopes came from a resampled
		//	file), always start the frame at the first of these.
		//
		//	Compute a rounded SDIF frame time for the potential new frame.
		//	Avoid floating point comparison problems by selecting a frame time before
		//	the time of te first breakpoint in the frame.
		if ( it != allBreakpoints.end() &&
				(it->time > bpTimeIter->time + 0.0001 || partialsWithBreakpointsInFrame.size() == 1))
		{
			//	Try rounding to nearest millisecond, use tenth millisecond if necessary.
			nextFrameTime = std::floor( 1000. * it->time - .001 ) / 1000.0;
			if (nextFrameTime < bpTimeIter->time + 0.00001)
				nextFrameTime = std::floor( 10000.0 * it->time - 0.01 ) / 10000.0;
			bpTimeIter = it;
		}
	}

	//	We are at the end of the sound; no "next frame" there.
	bpTimeIter = allBreakpoints.end();
	return frameTime + 100.0;
}		
		
#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

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
 * ExportSdif.C
 *
 * Implementation of class ExportSdif, which exports the
 * 1TRC SDIF format.
 *
 * Lippold Haken, 4 July 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "ExportSdif.h"
#include "Partial.h"
#include "Exception.h"
#include "notifier.h"
#include "pi.h"
#include <cmath>

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
//  The 6 column format excludes timeOffset and discardable; they will be assumed
//  0.0 by the reader.
typedef struct {
    sdif_float32 index, freq, amp, phase, noise, label, timeOffset, discardable;
} RowOfLorisData;

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
	FILE *out;

//
// Open SDIF file for writing.
//
	SDIFresult r = SDIF_OpenWrite(outfilename, &out);
	if (r)
		Throw( FileIOException, "Could not open SDIF file for writing." );

//
// Write SDIF data.
//	
	try 
	{
		// Make vector of pointers to partials.
		std::vector< Partial * > partialsVector;
		indexPartials( partials, partialsVector );
		
		// Write partials to SDIF file.
		writeEnvelopeData( out, partialsVector );
	}
	catch ( Exception & ex ) 
	{
		ex.append("Failed to write SDIF file.");
		SDIF_CloseWrite(out);
		throw;
	}

//
// Close SDIF output file.
//
	SDIF_CloseWrite(out);
}

#pragma mark -
#pragma mark envelope writing
// ---------------------------------------------------------------------------
//	writeEnvelopeData
// ---------------------------------------------------------------------------
//
void
ExportSdif::writeEnvelopeData( FILE * out, const std::vector< Partial * > & partialsVector )
{
//
// Export SDIF file from Loris data.
// Let exceptions propagate.
//

	int streamID = 1; // one stream id for all frames

//
// Make a sorted list of all breakpoints in all partials, and initialize the list iterater.
// This stuff does nothing if we are writing a resampled SDIF file.
//
	std::list< BreakpointTime > allBreakpoints;
	makeSortedBreakpointTimes( partialsVector, allBreakpoints);
	std::list< BreakpointTime >::iterator bpTimeIter = allBreakpoints.begin();

//
// Output Loris data in SDIF frame format.
// First frame starts at millisecond of first breakpoint, for exact timing SDIF files.
//
	double frameTime = (_hop == 0.0) ? std::floor( 1000.0 * bpTimeIter->time - 0.001 ) / 1000.0 : 0.0;
	double nextFrameTime = getNextFrameTime( frameTime, allBreakpoints, bpTimeIter );
	int endOfAll;
	do 
	{

//
// Make a vector of partial indices that includes all partials active at this time.
//
		std::vector< int > activeIndices;
		endOfAll = collectActiveIndices( partialsVector, frameTime, nextFrameTime, activeIndices );

//
// Write frame header, matrix header, and matrix data.
// We always have one matrix per frame.
// The matrix size depends on the number of partials active at this time.
//
		if (activeIndices.size() > 0) 
		{
			writeFrameHeader( out, streamID, activeIndices.size(), frameTime );
			writeMatrixHeader( out, activeIndices.size() );
			writeMatrixData( out, partialsVector, activeIndices, frameTime, nextFrameTime );
		}
//
// Go to next frame.
//
		frameTime = nextFrameTime;
		nextFrameTime = getNextFrameTime( frameTime, allBreakpoints, bpTimeIter );
	} 
	while (!endOfAll);
}

// ---------------------------------------------------------------------------
//	writeFrameHeader
// ---------------------------------------------------------------------------
//
void
ExportSdif::writeFrameHeader( FILE *out, const int streamID, 
							const int numTracks, const double frameTime )
{	
	SDIF_FrameHeader frame_head;

	SDIF_Copy4Bytes(frame_head.frameType, "1TRC");

	frame_head.size = sizeof( frame_head.time ) 
					+ sizeof( frame_head.streamID )
					+ sizeof( frame_head.matrixCount ) 
					+ ( numTracks * sizeof( RowOfLorisData ) );	  // matrix data always multiple of 8 bytes
					
	frame_head.time = (sdif_float64) frameTime;
	frame_head.streamID = streamID;
	frame_head.matrixCount = 1;

	SDIFresult r = SDIF_WriteFrameHeader(&frame_head, out);
	if ( r )
		Throw( FileIOException, std::string("Error writing SDIF frame header: ") + std::string( SDIF_GetErrorString(r)) );
}

// ---------------------------------------------------------------------------
//	writeMatrixHeader
// ---------------------------------------------------------------------------
//
void
ExportSdif::writeMatrixHeader( FILE *out, const int numTracks )
{	
	SDIF_MatrixHeader matrix_head;

	SDIF_Copy4Bytes(matrix_head.matrixType, "1TRC");
	matrix_head.matrixDataType = SDIF_FLOAT32;
	matrix_head.rowCount = numTracks;
	matrix_head.columnCount = (_hop == 0.0) ? 8 : 6;

	SDIFresult r = SDIF_WriteMatrixHeader(&matrix_head, out);
	if ( r ) 
		Throw( FileIOException, std::string("Error writing SDIF matrix header: ") + std::string(SDIF_GetErrorString(r)) );
}

// ---------------------------------------------------------------------------
//	writeMatrixData
// ---------------------------------------------------------------------------
//	The activeIndices vector contains indices for partials that have data at this time.
//	Write SDIF matrix data for these partials.
//
void
ExportSdif::writeMatrixData( FILE *out, const std::vector< Partial * > & partialsVector, 
								const std::vector< int > & activeIndices, 
								const double frameTime, const double nextFrameTime )
{	
	RowOfLorisData trackData;
	
	for ( int i = 0; i < activeIndices.size(); i++ ) 
	{
		
		int index = activeIndices[i];
		Partial * par = partialsVector[index];
		
		// If we are doing exact timing (not resampling):
		// Use data at breakpoint if one is in frame, else fabricate data at frameTime.
		double tim = frameTime;
		double discardable = 0.0;
		if (_hop == 0.0) 				// not resampling; doing exact timing
		{
			PartialIterator it = par->findPos(frameTime);
			if (it != par->end() && it.time() < nextFrameTime) 
				tim = it.time();		// use data at breakpoint time
			else 
				discardable = 1;		// no breakpoint in frame; interpolated data point
		}
		
		// Fill in values for this row of matrix data.
		trackData.index	= index;						// first row of matrix   (standard)
		trackData.freq	= par->frequencyAt( tim );		// second row of matrix  (standard)
		trackData.phase = par->phaseAt( tim );			// third row of matrix   (standard)
		trackData.amp	= par->amplitudeAt( tim );		// fourth row of matrix  (standard)
		trackData.noise = par->bandwidthAt( tim );		// fifth row of matrix   (loris)
		trackData.label	= par->label();					// sixth row of matrix   (loris)
		trackData.timeOffset  = tim - frameTime;		// seventh row of matrix (loris optional)
		trackData.discardable = discardable;			// eighth row of matrix  (loris optional)

		trackData.phase = WrapPhase32(trackData.phase);

		// Write the row of matrix data.
		// Write 6 columns if we are resampling envelope data.
		// Write 8 columns if we are not resampling and doing exact timing.
		SDIFresult r = SDIF_Write4( &trackData, ((_hop == 0.0) ? 8 : 6), out);
		if ( r ) 
			Throw( FileIOException, std::string("Error writing SDIF matrix data: ") + std::string(SDIF_GetErrorString(r)) );
	}
}


#pragma mark -
#pragma mark envelope writing helpers

// ---------------------------------------------------------------------------
//	indexPartials
// ---------------------------------------------------------------------------
//	Make a vector of partial pointers. 
//  The vector index will be the sdif 1TRC index for the partial. 
//
void
ExportSdif::indexPartials( const PartialList & partials, std::vector< Partial * > & partialsVector )
{
	for (PartialList::const_iterator it = partials.begin(); it != partials.end(); ++it)
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
ExportSdif::collectActiveIndices( const std::vector< Partial * > & partialsVector, 
								const double frameTime, const double nextFrameTime,
								std::vector< int > & activeIndices )
{
	int endOfAll = true;
	
	for (int i = 0; i < partialsVector.size(); i++) 
	{
		// Is there a breakpoint within the frame?
		// if there is no breakpoint and zero amplitude, skip the partial.
		// Resampled partials are included if the next frame (or previous frame) had a breakpoint.
		PartialIterator it = partialsVector[i]->findPos(frameTime - _hop);
		if ((it != partialsVector[i]->end() && it.time() < nextFrameTime + _hop) 
					|| partialsVector[i]->amplitudeAt(frameTime) != 0.0) 
			activeIndices.push_back(i);	

		if (partialsVector[i]->endTime() > frameTime + _hop)
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
ExportSdif::makeSortedBreakpointTimes( const std::vector< Partial * > & partialsVector, 
									   std::list< BreakpointTime > & allBreakpoints ) 
{

// If we are resampling envelopes, we don't need the sorted list; just return.
	if (_hop > 0.0)
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
//  This uses the previously sorted allBreakpoints list.
//
//	all Breakpoints should be const, but for some reason, gcc (on SGI at 
//	least) makes trouble converting and comparing iterators and const_iterators.
//
double
ExportSdif::getNextFrameTime( const double frameTime, std::list< BreakpointTime > & allBreakpoints,
							  std::list< BreakpointTime >::iterator & bpTimeIter)
{
//
// If we are resampling envelopes, increment by _hop time and return.
//
	if (_hop > 0.0)
		return frameTime + _hop;
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
				(it->time > bpTimeIter->time + 0.0001 || partialsWithBreakpointsInFrame.size() == 1) )
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

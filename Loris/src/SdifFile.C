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
 * SdifFile.C
 *
 * Implementation of class SdifFile, which reads and writes SDIF files.
 *
 * Lippold Haken, 4 July 2000, using CNMAT SDIF library
 * Lippold Haken, 20 October 2000, using IRCAM SDIF library (tutorial by Diemo Schwarz)
 * Lippold Haken, 22 December 2000, using 1LBL frames
 * Lippold Haken, 27 March 2001, write only 7-column 1TRC, combine reading and writing classes
 * Lippold Haken, 31 Jan 2002, write either 4-column 1TRC or 6-column RBEP
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include <config.h>
#endif

#include<SdifFile.h>
#include<Exception.h>
#include<Notifier.h>
#include<Partial.h>
#include <list>
#include <vector>
#include <cmath>
#include <string>

#if HAVE_M_PI
	const double Pi = M_PI;
#else
	const double Pi = 3.14159265358979324;
#endif

extern "C" {
#include <sdif.h>
}

//	begin namespace
namespace Loris {


//	Row of matrix data in SDIF RBEP, 1TRC, or RBEL format.
//
//  The RBEP matrices are for reassigned bandwidth enhanced partials (in 6 columns).
//	The 1TRC matrices are for sine-only partials (in 4 columns).
//  The first four columns of an RBEP matrix correspond to the 4 columns in 1TRC.
//	In the past, Loris exported a 7-column 1TRC; this is no longer exported, but can be imported.
//
//	The RBEL format always has two columns, index and partial label.
//	The RBEL matrix is optional; it has partial label information (in 2 columns).
int lorisRowMaxElements = 7;
int lorisRowEnhancedElements = 6;
int lorisRowSineOnlyElements = 4;
typedef struct {
    SdifFloat8 index, freqOrLabel, amp, phase, noise, timeOffset, resampledFlag;
} RowOfLorisData;


//  SDIF signatures used by Loris.
#define lorisEnhancedSignature  SdifSignatureConst('R','B','E','P')
#define lorisLabelsSignature    SdifSignatureConst('R','B','E','L')
#define lorisSineOnlySignature  SdifSignatureConst('1','T','R','C')


//	Exception class for handling errors in SDIF library:
class SdifLibraryError : public FileIOException
{
public:
	SdifLibraryError( const std::string & str, const std::string & where = "" ) : 
		FileIOException( std::string("SDIF library error -- ").append( str ), where ) {}
};	//	end of class SdifLibraryError

//	macro to check for SDIF library errors and throw exceptions when
//	they occur, which we really ought to do after every SDIF library
//	call:
#define ThrowIfSdifError( getErr, report )										\
	do {																		\
		SdifErrorT* errPtr = (getErr);											\
		if (errPtr)																\
		{																		\
	        debugger << "SDIF error number " << (int)errPtr->Tag << endl;		\
			std::string s(report);												\
			s.append(", SDIF error message: ");									\
			s.append(errPtr->UserMess);											\
			Throw( SdifLibraryError, s );										\
		}																		\
	} while (false)	


#pragma mark envelope reading helpers
// ---------------------------------------------------------------------------
//	processRow
// ---------------------------------------------------------------------------
//	Add to existing Loris partials, or create new Loris partials for this data.
//
static void
processRow( const SdifSignature msig, const RowOfLorisData & rowData, const double frameTime, 
				  std::vector< Partial > & partialsVector )
{	

//
// Skip this if the data point is not from the original data (7-column 1TRC format).
//
	if (rowData.resampledFlag)
		return;
	
//
// Make sure we have enough partials for this partial's index.
//
	if (partialsVector.size() <= rowData.index)
	{
		partialsVector.resize( long(rowData.index) + 500 );
	}

//
// Create a new breakpoint and insert it.
//	
	if (msig == lorisEnhancedSignature || msig == lorisSineOnlySignature) 
	{
		Breakpoint newbp( rowData.freqOrLabel, rowData.amp, rowData.noise, rowData.phase );
		partialsVector[long(rowData.index)].insert( frameTime + rowData.timeOffset, newbp );
	}
//
// Set partial label.
//
	else if (msig == lorisLabelsSignature) 
	{
		partialsVector[long(rowData.index)].setLabel( (int) rowData.freqOrLabel );
	}
		
}


// ---------------------------------------------------------------------------
//	readLorisMatrices
// ---------------------------------------------------------------------------
// Let exceptions propagate.
//
static void
readLorisMatrices( SdifFileT *file, std::vector< Partial > & partialsVector )
{
	size_t bytesread = 0;
	int eof = false;

//
// Read all frames matching the file selection.
//
	while (!eof) // && !SdifFLastError(file))
	{
		bytesread += SdifFReadFrameHeader(file);
		ThrowIfSdifError( SdifFLastError(file), "Error reading SDIF file" );
		
		// Skip frames until we find one we are interested in.
		while (!SdifFCurrFrameIsSelected(file) 
				|| (SdifFCurrSignature(file) != lorisEnhancedSignature 
					&& SdifFCurrSignature(file) != lorisSineOnlySignature 
					&& SdifFCurrSignature(file) != lorisLabelsSignature))
		{
			ThrowIfSdifError( SdifFLastError(file), "Error reading SDIF file." );

			SdifSkipFrameData(file);
			eof = (SdifFGetSignature(file, &bytesread) == eEof);
			ThrowIfSdifError( SdifFLastError(file), "Error reading SDIF file" );

			if (eof)
				break;		// eof
			bytesread += SdifFReadFrameHeader(file);
			ThrowIfSdifError( SdifFLastError(file), "Error reading SDIF file" );
		}
		
		if (!eof)
		{
			// Access frame header information.
			SdifFloat8 time = SdifFCurrTime(file);
			SdifSignature fsig = SdifFCurrFrameSignature(file);
			SdifUInt4 streamid = SdifFCurrID(file);
			SdifUInt4 nmatrix = SdifFCurrNbMatrix(file);
			ThrowIfSdifError( SdifFLastError(file), "Error reading SDIF file" );

			// Read all matrices in this frame matching the selection.
			for (int m = 0; m < nmatrix; m++)
			{
				bytesread += SdifFReadMatrixHeader(file);
				ThrowIfSdifError( SdifFLastError(file), "Error reading SDIF file" );

				if (SdifFCurrMatrixIsSelected(file))
				{
					ThrowIfSdifError( SdifFLastError(file), "Error reading SDIF file" );
				
					// Access matrix header information.
					SdifSignature msig = SdifFCurrMatrixSignature(file);
					SdifInt4 nrows = SdifFCurrNbRow(file);
					SdifInt4 ncols = SdifFCurrNbCol(file);
					SdifDataTypeET type = SdifFCurrDataType(file);
					ThrowIfSdifError( SdifFLastError(file), "Error reading SDIF file" );

					// Read each row of matrix data.
					for (int row = 0; row < nrows; row++)
					{
						bytesread += SdifFReadOneRow(file);
					ThrowIfSdifError( SdifFLastError(file), "Error reading SDIF file" );

						// Fill a rowData structure.
						RowOfLorisData rowData = { 0.0 };
						SdifFloat8 *rowDataPtr = &rowData.index;
						for (int col = 1; col <= std::min(ncols, lorisRowMaxElements); col++)
						{
							*(rowDataPtr++) = SdifFCurrOneRowCol(file, col);
							ThrowIfSdifError( SdifFLastError(file), "Error reading SDIF file" );
						}
						
						// Add rowData as a new breakpoint in a partial, or,
						// if its a RBEL matrix, read label mapping.
						processRow(msig, rowData, time, partialsVector);
					}
				}
				else
				{
					bytesread += SdifSkipMatrixData(file);
					ThrowIfSdifError( SdifFLastError(file), "Error reading SDIF file" );
				}
				
				bytesread += SdifFReadPadding(file, SdifFPaddingCalculate(file->Stream, bytesread));
				ThrowIfSdifError( SdifFLastError(file), "Error reading SDIF file" );
			}
			
			// read next signature
			eof = (SdifFGetSignature(file, &bytesread) == eEof);
			ThrowIfSdifError( SdifFLastError(file), "Error reading SDIF file" );
		}
	} 
}


#pragma mark -
#pragma mark import sdif
// ---------------------------------------------------------------------------
//	read
// ---------------------------------------------------------------------------
// Let exceptions propagate.
//
static void
read( const char *infilename, std::list<Partial> & partials )
{

// 
// Initialize SDIF library.
//
	SdifGenInit("");
#if !defined(Debug_Loris)
	SdifDisableErrorOutput();
#endif
	
//
// Open SDIF file for reading.
// Note: Currently we do not specify any selection criterion in this call.
//
	SdifFileT *in = SdifFOpen(infilename, eReadFile);
	if (!in)
	{
		SdifGenKill();
		Throw( FileIOException, "Could not open SDIF file for reading." );
	}

	SdifFReadGeneralHeader(in);		// read file header
	SdifFReadAllASCIIChunks(in);	// read ascii header info, such as name-value tables

//
// Read SDIF data.
//	
	try 
	{
	
		// Build up partialsVector.
		std::vector< Partial > partialsVector;
		readLorisMatrices( in, partialsVector );
		
		// Copy partialsVector to partials list.
		for (int i = 0; i < partialsVector.size(); ++i)
		{
			if (partialsVector[i].numBreakpoints() > 0)
			{
				partials.push_back( partialsVector[i] );
			}
		}
	}
	catch ( Exception & ex ) 
	{
		partials.clear();
		ex.append(" Failed to read SDIF file.");
		SdifFClose(in);
		SdifGenKill();
		throw;
	}

//
// Close SDIF input file.
//
	SdifFClose(in);

// 
// Done with SDIF library.
//
	SdifGenKill();
	
//
// Complain if no Partials were imported:
//
	if ( partials.size() == 0 )
	{
		notifier << "No Partials were imported from " << infilename 
				 << ", no (non-empty) 1TRC frames found." << endl;
	}
	
}


// ---------------------------------------------------------------------------
//	SdifFile constructor from data in memory (import)
// ---------------------------------------------------------------------------
//
SdifFile::SdifFile( const std::string & infilename ) 
{
	read( infilename.c_str(), _partials );
}


#pragma mark -
#pragma mark -
#pragma mark envelope writing helpers
// ---------------------------------------------------------------------------
//	makeSortedBreakpointTimes
// ---------------------------------------------------------------------------
//	Collect the times of all breakpoints in the analysis, and sort by time.
//  Sorted breakpoints are used in finding frame start times in SDIF writing.
//
struct BreakpointTime
{
	int index;			// index identifying which partial has the breakpoint
	float time;			// time of the breakpoint
};

struct earlier_time
{
	bool operator()( const BreakpointTime & lhs, const BreakpointTime & rhs ) const
		{ return lhs.time < rhs.time; }
};

static void
makeSortedBreakpointTimes( const std::vector< Partial * > & partialsVector, 
						   std::list< BreakpointTime > & allBreakpoints ) 
{

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
//  This helps make SDIF files with exact timing (7-column 1TRC format).
//  This uses the previously sorted allBreakpoints list.
//
//	All Breakpoints should be const, but for some reason, gcc (on SGI at 
//	least) makes trouble converting and comparing iterators and const_iterators.
//
static double
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
		
		//  If the new breakpoint is at a new time, it could potentially be the
		//	first breakpoint in the next frame. If there are several breakpoints at
		//	the exact same time (could happen if these envelopes came from a spc
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


// ---------------------------------------------------------------------------
//	indexPartials
// ---------------------------------------------------------------------------
//	Make a vector of partial pointers. 
//  The vector index will be the sdif 1TRC index for the partial. 
//
static void
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
//
//  Return true if time is beyond end of all the partials.
//
static int
collectActiveIndices( const std::vector< Partial * > & partialsVector, const bool enhanced,
								const double frameTime, const double nextFrameTime,
								std::vector< int > & activeIndices )
{
	int endOfAll = true;
	
	for (int i = 0; i < partialsVector.size(); i++) 
	{
		// Is there a breakpoint within the frame?
		// Skip the partial if there is no breakpoint and either:
		//		(1) we are writing enhanced format, 
		// 	 or (2) the partial has zero amplitude.
		PartialIterator it = partialsVector[i]->findAfter( frameTime );
		if ( ( it != partialsVector[i]->end() && it.time() < nextFrameTime ) 
					|| (!enhanced && partialsVector[i]->amplitudeAt( frameTime ) != 0.0 ) ) 
			activeIndices.push_back(i);	

		if (partialsVector[i]->endTime() > frameTime )
			endOfAll = false;
	}
	return endOfAll;
}


#pragma mark -
#pragma mark matrix assembly helpers
// ---------------------------------------------------------------------------
//	writeEnvelopeLabels
// ---------------------------------------------------------------------------
//
static void
writeEnvelopeLabels( SdifFileT * out,
				const std::vector< Partial * > & partialsVector )
{
//
// Write Loris labels to SDIF file in a RBEL matrix.
// This precedes the 1TRC data in the file.
// Let exceptions propagate.
//

	int streamID = 2; 				// stream id different from envelope's stream id
	double frameTime = 0.0;

//
// Allocate RBEL matrix data.
//
	int cols = 2;
	SdifFloat4 *data = new SdifFloat4[partialsVector.size() * cols];

//
// For each partial index, specify the partial label.
//
	SdifFloat4 *dp = data;
	int anyLabel = false;
	for (int i = 0; i < partialsVector.size(); i++) 
	{
		int labl = partialsVector[i]->label();
		anyLabel |= (labl != 0);
		*dp++ = i;					// column 1: index
		*dp++ = labl;				// column 2: label
	}	

//							
// Write out matrix data, if there were any labels.
//
	if (anyLabel)
		SdifFWriteFrameAndOneMatrix( out, 
			lorisLabelsSignature, streamID, frameTime, 							// frame header
			lorisLabelsSignature, eFloat4, partialsVector.size(), cols, data);	// matrix 

//	
// Free RBEL matrix space.
//
	delete [] data;
}


// ---------------------------------------------------------------------------
//	assembleMatrixData
// ---------------------------------------------------------------------------
//	The activeIndices vector contains indices for partials that have data at this time.
//	Assemble SDIF matrix data for these partials.
//
static void
assembleMatrixData( SdifFloat4 *data, const bool enhanced,
								const std::vector< Partial * > & partialsVector, 
								const std::vector< int > & activeIndices, 
								const double frameTime )
{	
	// The array matrix data is row-major order at "data".
	SdifFloat4 *rowDataPtr = data;
	
	for ( int i = 0; i < activeIndices.size(); i++ ) 
	{
		
		int index = activeIndices[i];
		Partial * par = partialsVector[index];
		
		// For enhanced format we use exact timing; the activeIndices only includes
		// partials that have breakpoints in this frame.
		// For sine-only format we resample at frame times.
		double tim = (enhanced ? par->findAfter(frameTime).time() : frameTime);
		
		// Must have phase between 0 and 2*Pi.
		double phas = par->phaseAt( tim );
		if (phas < 0)
			phas += 2. * Pi; 
		
		// Fill in values for this row of matrix data.
		*rowDataPtr++ = index;							// first row of matrix   (standard)
		*rowDataPtr++ = par->frequencyAt( tim );		// second row of matrix  (standard)
		*rowDataPtr++ = par->amplitudeAt( tim );		// third row of matrix   (standard)
		*rowDataPtr++ = phas;							// fourth row of matrix  (standard)
		if (enhanced)
		{
			*rowDataPtr++ = par->bandwidthAt( tim );	// fifth row of matrix   (loris)
			*rowDataPtr++ = tim - frameTime;			// sixth row of matrix   (loris)
		}
	}
}


// ---------------------------------------------------------------------------
//	writeEnvelopeData
// ---------------------------------------------------------------------------
//
static void
writeEnvelopeData( SdifFileT * out,
				const std::vector< Partial * > & partialsVector,
				const bool enhanced )
{
//
// Export SDIF file from Loris data.
// Let exceptions propagate.
//

	int streamID = 1; 						// one stream id for all SDIF frames

//
// Make a sorted list of all breakpoints in all partials, and initialize the list iterater.
// This stuff does nothing if we are writing 5-column 1TRC format.
//
	std::list< BreakpointTime > allBreakpoints;
	makeSortedBreakpointTimes( partialsVector, allBreakpoints );
	std::list< BreakpointTime >::iterator bpTimeIter = allBreakpoints.begin();

//
// Output Loris data in SDIF frame format.
// First frame starts at millisecond of first breakpoint, for SDIF files with 7-column 1TRC matrices.
//
	double nextFrameTime	= std::floor( 1000.0 * bpTimeIter->time - 0.001 ) / 1000.0;

	double frameTime; 
	int endOfAll;
	do 
	{

//
// Go to next frame.
//
		frameTime = nextFrameTime;
		nextFrameTime = getNextFrameTime( frameTime, allBreakpoints, bpTimeIter );

//
// Make a vector of partial indices that includes all partials active at this time.
//
		std::vector< int > activeIndices;
		endOfAll = collectActiveIndices( partialsVector, enhanced,
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
			int cols = (enhanced ? lorisRowEnhancedElements : lorisRowSineOnlyElements);
			SdifFloat4 *data = new SdifFloat4[numTracks * cols];

			// Fill in matrix data.
			assembleMatrixData( data, enhanced, partialsVector, activeIndices, frameTime );
								
			// Write out matrix data.
			SdifSignature sig = (enhanced ? lorisEnhancedSignature : lorisSineOnlySignature);
			SdifFWriteFrameAndOneMatrix( out, 
					sig, streamID, frameTime, 					// frame header
					sig, eFloat4, numTracks, cols, data);		// matrix 
			
			// Free matrix space.
			delete [] data;
		}
	}
	while (!endOfAll);
}


#pragma mark -
#pragma mark export sdif

// ---------------------------------------------------------------------------
//	Export
// ---------------------------------------------------------------------------
// Export SDIF file.
//
void
SdifFile::Export( const std::string & filename, const std::list<Partial> & partials, const bool enhanced )
{

// 
// Initialize SDIF library.
//
	SdifGenInit("");
	
//
// Open SDIF file for writing.
//
	SdifFileT *out = SdifFOpen(filename.c_str(), eWriteFile);
	if (!out)
		Throw( FileIOException, "Could not open SDIF file for writing." );

	// Define RBEP matrix and frame type for enhanced partials.
	if (enhanced)
	{
		SdifMatrixTypeT *parsMatrixType = SdifCreateMatrixType(lorisEnhancedSignature,NULL);
		SdifMatrixTypeInsertTailColumnDef(parsMatrixType,"Index");  
		SdifMatrixTypeInsertTailColumnDef(parsMatrixType,"Frequency");  
		SdifMatrixTypeInsertTailColumnDef(parsMatrixType,"Amplitude");  
		SdifMatrixTypeInsertTailColumnDef(parsMatrixType,"Phase");  
		SdifMatrixTypeInsertTailColumnDef(parsMatrixType,"Noise");  
		SdifMatrixTypeInsertTailColumnDef(parsMatrixType,"TimeOffset");  
		SdifPutMatrixType(out->MatrixTypesTable, parsMatrixType);

		SdifFrameTypeT *parsFrameType = SdifCreateFrameType(lorisEnhancedSignature,NULL);
		SdifFrameTypePutComponent(parsFrameType, lorisEnhancedSignature, "RABWE_Partials");
		SdifPutFrameType(out->FrameTypesTable, parsFrameType);
	}

	// Define RBEL matrix and frame type for labels.
	SdifMatrixTypeT *labelsMatrixType = SdifCreateMatrixType(lorisLabelsSignature,NULL);
	SdifMatrixTypeInsertTailColumnDef(labelsMatrixType,"Index");  
	SdifMatrixTypeInsertTailColumnDef(labelsMatrixType,"Label");  
	SdifPutMatrixType(out->MatrixTypesTable, labelsMatrixType);

	SdifFrameTypeT *labelsFrameType = SdifCreateFrameType(lorisLabelsSignature,NULL);
	SdifFrameTypePutComponent(labelsFrameType, lorisLabelsSignature, "RABWE_Labels");
	SdifPutFrameType(out->FrameTypesTable, labelsFrameType);

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

		// Write labels.
		writeEnvelopeLabels( out, partialsVector );
		
		// Write partials to SDIF file.
		writeEnvelopeData( out, partialsVector, enhanced );
	}
	catch ( Exception & ex ) 
	{
		ex.append(" Failed to write SDIF file.");
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

}	//	end of namespace Loris



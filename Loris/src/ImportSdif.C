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
 * ImportSdif.C
 *
 * Implementation of class ImportSdif, which reads an SDIF file.
 *
 * Lippold Haken, 4 July 2000, using CNMAT SDIF library
 * Lippold Haken, 20 October 2000, using IRCAM SDIF library (tutorial by Diemo Schwarz)
 * Lippold Haken, 22 December 2000, using 1LBL frames
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include <Loris_prefix.h>
#include "ImportSdif.h"
#include "Exception.h"
#include "notifier.h"

#include <vector>

extern "C" {
#include <sdif.h>
}

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

//	Row of matrix data in SDIF 1TRC or SDIF 1LBL format.
//	The 1TRC matrices have envelope information (in 5 or 7 columns).
//	The 1LBL matrix is optional; it has partial label information (in 2 columns).
//
//	Loris exports both a 5-column (resampled at frame times) and 7-column (exact times) 1TRC format.
//  The 5 column 1TRC format excludes timeOffset and resampledFlag.
//
//	The 1LBL format always has two columns, index and partial label.
int lorisRowMaxElements = 7;
typedef struct {
    SdifFloat8 index, freqOrLabel, amp, phase, noise, timeOffset, resampledFlag;
} RowOfLorisData;

//  SDIF signature used by Loris.
//	The macro SdifSignatureConst is defined differently in different
//	versions of the SDIF library, and there is no way to determine
//	from the header which version of the library is being used. So 
//	Loris has to have its own flag to identify the Sdif version.
#ifndef SDIF_VERSION
#define SDIF_VERSION 322
#endif

#if (SDIF_VERSION == 322)
static SdifSignature lorisSignature = SdifSignatureConst('1','T','R','C');
static SdifSignature lorisLabels = SdifSignatureConst('1','L','B','L');
#elif (SDIF_VERSION == 320)
static SdifSignature lorisSignature = SdifSignatureConst('1TRC');
static SdifSignature lorisLabels = SdifSignatureConst('1LBL');
#else
#error "Unknown SDIF library version."
#endif


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
// This flag is never set if all data is resampled (5-column 1TRC format).
//
	if (rowData.resampledFlag)
		return;
	
//
// Make sure we have enough partials for this partial's index.
//
	if (partialsVector.size() <= rowData.index)
	{
		partialsVector.resize( rowData.index + 500 );
	}

//
// Create a new breakpoint and insert it.
//	
	if (msig == lorisSignature) 
	{
		Breakpoint newbp( rowData.freqOrLabel, rowData.amp, rowData.noise, rowData.phase );
		partialsVector[rowData.index].insert( frameTime + rowData.timeOffset, newbp );
	}
//
// Set partial label.
//
	else if (msig == lorisLabels) 
	{
		partialsVector[rowData.index].setLabel( (int) rowData.freqOrLabel );
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
	while (!eof && !SdifFLastError(file))
	{
		bytesread += SdifFReadFrameHeader(file);
		
		// Skip frames until we find one we are interested in.
		while (!SdifFCurrFrameIsSelected(file) 
				|| (SdifFCurrSignature(file) != lorisSignature 
					&& SdifFCurrSignature(file) != lorisLabels))
		{
			SdifSkipFrameData(file);
			eof = (SdifFGetSignature(file, &bytesread) == eEof);
			if (eof)
				break;		// eof
			bytesread += SdifFReadFrameHeader(file);
		}
		
		if (!eof)
		{

			// Access frame header information.
			SdifFloat8 time = SdifFCurrTime(file);
			SdifSignature fsig = SdifFCurrFrameSignature(file);
			SdifUInt4 streamid = SdifFCurrID(file);
			SdifUInt4 nmatrix = SdifFCurrNbMatrix(file);
			
			// Read all matrices in this frame matching the selection.
			for (int m = 0; m < nmatrix; m++)
			{
				bytesread += SdifFReadMatrixHeader(file);
				
				if (SdifFCurrMatrixIsSelected(file))
				{
				
					// Access matrix header information.
					SdifSignature msig = SdifFCurrMatrixSignature(file);
					SdifInt4 nrows = SdifFCurrNbRow(file);
					SdifInt4 ncols = SdifFCurrNbCol(file);
					SdifDataTypeET type = SdifFCurrDataType(file);
					
					// Read each row of matrix data.
					for (int row = 0; row < nrows; row++)
					{
						bytesread += SdifFReadOneRow(file);

						// Fill a rowData structure.
						RowOfLorisData rowData = { 0.0 };
						SdifFloat8 *rowDataPtr = &rowData.index;
						for (int col = 1; col <= std::min(ncols, lorisRowMaxElements); col++)
							*(rowDataPtr++) = SdifFCurrOneRowCol(file, col);
						
						// Add rowData as a new breakpoint in a partial, or,
						// if its a 1LBL matrix, read label mapping.
						processRow(msig, rowData, time, partialsVector);
					}
				}
				else
				{
					bytesread += SdifSkipMatrixData(file);
				}
				
				bytesread += SdifFReadPadding(file, SdifFPaddingCalculate(file->Stream, bytesread));
			}
			
			// read next signature
			eof = (SdifFGetSignature(file, &bytesread) == eEof);
		}
	} 

//
// Error messages.
//	
	SdifErrorT* errPtr = SdifFLastError (file);
	if (errPtr)
	{
		debugger << "SDIF error number " << (int)errPtr->Tag << endl;
		if ( errPtr->Tag == eUnDefined )
		{
			Throw(FileIOException, 
			"Error reading SDIF file: Undefined martrix type. "
			"Is the SdifTypes.STYP file accessible to Loris, and does it include the 1LBL definition?");
		}
		else if ( errPtr->Tag == eBadNbData )
		{
			Throw(FileIOException, 
			"Error reading SDIF file: bad martrix data. "
			"Does the SdifTypes.STYP file include the bandwidth-enhanced 1TRC definition?");
		}
		else
		{			
			Throw(FileIOException, "Error reading SDIF file.");
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
//	ImportSdif constructor from data in memory
// ---------------------------------------------------------------------------
//
ImportSdif::ImportSdif( const char *infilename ) 
{
	read( infilename, _partials );
}


#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif




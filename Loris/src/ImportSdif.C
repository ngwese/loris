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
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "ImportSdif.h"
#include "Exception.h"
#include "notifier.h"

#include <sdif.h>
#include <vector>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

//	Row of matrix data in SDIF 1TRC format.
//	Loris exports both a 6-column (resampled) and 8-column (exact times) format.
//  The 6 column format excludes timeOffset and discardable.
int lorisRowElements = 8;
typedef struct {
    SdifFloat8 index, freq, amp, phase, noise, label, timeOffset, discardable;
} RowOfLorisData;

//  SDIF signature used by Loris.
static SdifSignature lorisSignature = SdifSignatureConst('1TRC');

//	prototypes for helpers:
static void read( const char *infilename, std::list<Partial> & partials );
static void readEnvelopeData( SdifFileT *in, std::vector< Partial > & partialsVector );
static void addRowToPartials( const RowOfLorisData & trackData, 
							  const double frameTime, 
							  std::vector< Partial > & partialsVector );

// ---------------------------------------------------------------------------
//	ImportSdif constructor from data in memory
// ---------------------------------------------------------------------------
//
ImportSdif::ImportSdif( const char *infilename ) 
{
	read( infilename, _partials );
}

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
		readEnvelopeData( in, partialsVector );
		
		// Copy partialsVector to partials list.
		for (int i = 0; i < partialsVector.size(); ++i)
		{
			if (partialsVector[i].countBreakpoints() > 0)
			{
				partials.push_back( partialsVector[i] );
			}
		}
	}
	catch ( Exception & ex ) 
	{
		partials.clear();
		ex.append("Failed to read SDIF file.");
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
	
}

#pragma mark -
#pragma mark envelope reading
// ---------------------------------------------------------------------------
//	readEnvelopeData
// ---------------------------------------------------------------------------
// Let exceptions propagate.
//
static void
readEnvelopeData( SdifFileT *file, std::vector< Partial > & partialsVector )
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
				|| SdifFCurrSignature(file) != lorisSignature)
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

						// Fill a trackData structure.
						RowOfLorisData trackData = { 0.0 };
						SdifFloat8 *trackDataPtr = &trackData.index;
						for (int col = 1; col <= min(ncols, lorisRowElements); col++)
							*(trackDataPtr++) = SdifFCurrOneRowCol(file, col);
						
						// Add trackData as a new breakpoint in a partial.
						addRowToPartials(trackData, time, partialsVector);
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
	
	SdifErrorT* errPtr = SdifFLastError (file);
	if (errPtr)
	{
		debugger << "SDIF error number " << (int)errPtr->Tag << endl;
		if ( errPtr->Tag == eUnDefined )
		{
			Throw(FileIOException, 
			"Error reading SDIF file: undefined martrix type. Is the SdifTypes.STYP file accessible?");
		}
		else if ( errPtr->Tag == eBadNbData )
		{
			Throw(FileIOException, 
			"Error reading SDIF file: undefined martrix type. Is the Loris SdifTypes.STYP file accessible?");
		}
		else
		{			
			Throw(FileIOException, "Error reading SDIF file.");
		}
	}
}


// ---------------------------------------------------------------------------
//	addRowToPartials
// ---------------------------------------------------------------------------
//	Add to existing Loris partials, or create new Loris partials for this data.
//
static void
addRowToPartials( const RowOfLorisData & trackData, const double frameTime, 
				  std::vector< Partial > & partialsVector )
{	

//
// Skip this if the data point is discardable.
//
	if (trackData.discardable)
		return;
//
// Create new breakpoint.
//
	Breakpoint newbp( trackData.freq, trackData.amp, trackData.noise, trackData.phase );
	
//
// Check if partial of this index already exists.
// Create a new partial, or add breakpoint to existing partial.
//
	if (partialsVector.size() <= trackData.index)
	{
		partialsVector.resize( trackData.index + 500 );
	}
	
	//	make sure that the label is right and insert the Breakpoint:
	partialsVector[trackData.index].setLabel( (int) trackData.label );
	partialsVector[trackData.index].insert( frameTime + trackData.timeOffset, newbp );
		
}


#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif



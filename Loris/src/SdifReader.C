// ===========================================================================
//	SdifReader.C
//	
//	Association of info	to read an SDIF file.  
//
//	-lip 4 Jul 00
//
// ===========================================================================
#include "SdifReader.h"
#include "Exception.h"
#include "Partial.h"
#include "notifier.h"
#include "pi.h"

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif


// ---------------------------------------------------------------------------
//	SdifReader constructor from data in memory
// ---------------------------------------------------------------------------
//
SdifReader::SdifReader(  ) 
{
}

// ---------------------------------------------------------------------------
//	read
// ---------------------------------------------------------------------------
// Let exceptions propagate.
//
void
SdifReader::read( const char *infilename, std::list<Partial> & partials )
{
	FILE *in;

//
// Open SDIF file for reading.
//
	SDIFresult r = SDIF_OpenRead(infilename, &in);
	if (r)
		Throw( FileIOException, "Could not open SDIF file for reading." );

//
// Read SDIF data.
//	
	try 
	{
	
		// Build up partialsVector.
		std::vector< Partial * > partialsVector;
		readEnvelopeData( in, partialsVector );
		
		// Copy partialsVector to partials list.
		for (int i = 0; i < partialsVector.size(); i++)
			if (partialsVector[i] != NULL)
				partials.push_back( * partialsVector[i] );
	}
	catch ( Exception & ex ) 
	{
		ex.append("Failed to read SDIF file.");
		SDIF_CloseWrite(in);
		throw;
	}

//
// Close SDIF input file.
//
	SDIF_CloseWrite(in);
}


#pragma mark -
#pragma mark envelope reading
// ---------------------------------------------------------------------------
//	readEnvelopeData
// ---------------------------------------------------------------------------
// Let exceptions propagate.
//
void
SdifReader::readEnvelopeData( FILE *in, std::vector< Partial * > & partialsVector )
{
	SDIF_FrameHeader fh;
	SDIF_MatrixHeader mh;
	SDIFresult r;

	// Loop through frames until end of file.
	while (true) 
	{

		// Read the next frame.  Exit if end of data.
		r = SDIF_ReadFrameHeader(&fh, in);
		if (r == ESDIF_END_OF_DATA) 
			return;
		else if (r)
			Throw( FileIOException, std::string("Could not open SDIF file for reading: ") + std::string(SDIF_GetErrorString(r)) );

		// Make sure frame is legal.
		if (fh.size < 16)
			Throw(FileIOException, "SDIF frame size too small.");
		if ((fh.size & 7) != 0)
			Throw(FileIOException, "SDIF frame size not a multiple of 8.");

		for (int j = 0; j < fh.matrixCount; j++) 
		{
		
			// Read matrix header.
    		r = SDIF_ReadMatrixHeader(&mh,in);
			if (r)
				Throw( FileIOException, std::string("Could not read SDIF matrix header: ") + std::string(SDIF_GetErrorString(r)) );

			// Read matrix data and build partials.
    		readMatrixData(in, mh, fh.time, partialsVector);
    	}
    } 
}

// ---------------------------------------------------------------------------
//	readMatrixData
// ---------------------------------------------------------------------------
//  Read all rows in this frame's matrix.
//	Add to existing Loris partials, or create new Loris partials for this data.
//
void
SdifReader::readMatrixData( FILE *in, const SDIF_MatrixHeader & mh, 
							const double time, std::vector< Partial * > & partialsVector )
{	
//
// We must have a 1TRC matrix with at least index, frequency, and amplitude in the matrix data.
//
	if (strcmp(mh.matrixType, "1TRC") && strcmp(mh.matrixType, "1trc")) 
	{
		char str[256];
		sprintf(str, "Cannot import %c%c%c%c SDIF matrices yet!",
			mh.matrixType[0], mh.matrixType[1], mh.matrixType[2], mh.matrixType[3]);
		Throw( FileIOException, str);
	} 
	else if (mh.columnCount < 3) 
	{
		Throw( FileIOException, "Cannot import SDIF matrices with less than 3 columns!");
	}
//
// Read all rows of matrix, and add them to partials.
//
	for (int j = 0; j < mh.rowCount; ++j) {
		RowOfLorisData64 trackData;
		readRowData(in, mh, trackData);
		addRowToPartials(trackData, time, partialsVector);
	}
//
// Skip pad word; SDIF writers always pad to a multiple of 8 bytes.
//
	if (mh.matrixDataType == SDIF_FLOAT32 && (mh.rowCount * mh.columnCount) & 0x1) 
	{
		sdif_float32 pad;
		SDIFresult r = SDIF_Read4(&pad,1,in);
		if ( r )
			Throw( FileIOException, std::string("Error reading SDIF pad: ") + std::string(SDIF_GetErrorString(r)) );
	}
}

// ---------------------------------------------------------------------------
//	readRowData
// ---------------------------------------------------------------------------
//  Read row in this frame's matrix.
//	Add to existing Loris partials, or create new Loris partials for this data.
//
void
SdifReader::readRowData( FILE *in, const SDIF_MatrixHeader & mh, RowOfLorisData64 & trackData )
{
	
//
// Read row with 32-bit or 64-bit floating point data.
// We cannot read any other data types.
//
	if (mh.matrixDataType == SDIF_FLOAT64)
	{
		readRow64( in, mh, trackData );
	}
	else if (mh.matrixDataType == SDIF_FLOAT32) 
	{
		RowOfLorisData32 trackData32;
		readRow32( in, mh, trackData32 );
		
		// Convert 32-bit data to 64-bit data.
		trackData.index			= trackData32.index;
		trackData.freq			= trackData32.freq;
		trackData.amp			= trackData32.amp;
		trackData.phase			= trackData32.phase;
		trackData.noise			= trackData32.noise;
		trackData.label			= trackData32.label;
		trackData.timeOffset	= trackData32.timeOffset;
		trackData.discardable	= trackData32.discardable;
	}
	else 
		Throw( FileIOException, "Cannot import non-floating SDIF data types. ");
}

// ---------------------------------------------------------------------------
//	readRow32
// ---------------------------------------------------------------------------
//  Read 32-bit row in this frame's matrix.
//
void
SdifReader::readRow32( FILE *in, const SDIF_MatrixHeader & mh, RowOfLorisData32 & trackData )
{
	SDIFresult r;	
	
	// Set phase and noise to default values, in case there are no columns for them.
	trackData.phase = 0.0;
	trackData.noise = 0.0;
	trackData.label = 0.0;
	trackData.timeOffset = 0.0;
	trackData.discardable = 0;
	
	// Read matrix data.
	r = SDIF_Read4(&trackData, min((int)mh.columnCount, 8), in);
	if ( r )
		Throw( FileIOException, std::string("Error reading 32-bit SDIF row: ") + std::string(SDIF_GetErrorString(r)) );
	
	// If there are more than 8 columns, discard the rest.
	for (int k = 8; k < mh.columnCount; k++) 
	{
		sdif_float32 data32;
		r = SDIF_Read4(&data32, 1, in);
		if ( r )
			Throw( FileIOException, std::string("Error skipping 32-bit SDIF columns: ") + std::string(SDIF_GetErrorString(r)) );
	}
}


// ---------------------------------------------------------------------------
//	readRow64
// ---------------------------------------------------------------------------
//  Read 64-bit row in this frame's matrix.
//
void
SdifReader::readRow64( FILE *in, const SDIF_MatrixHeader & mh, RowOfLorisData64 & trackData )
{
	SDIFresult r;	
	
	// Set optional fields to default values, in case there are no columns for them.
	trackData.phase = 0.0;
	trackData.noise = 0.0;
	trackData.label = 0.0;
	trackData.timeOffset = 0.0;
	trackData.discardable = 0;

	// Read matrix data.
	r = SDIF_Read8(&trackData, min((int)mh.columnCount, 8), in);
	if ( r )
		Throw( FileIOException, std::string("Error reading 64-bit SDIF row: ") + std::string(SDIF_GetErrorString(r)) );
	
	// If there are more than 8 columns, discard the rest.
	for (int k = 8; k < mh.columnCount; k++) 
	{
		sdif_float64 data64;
		r = SDIF_Read8(&data64, 1, in);
		if ( r )
			Throw( FileIOException, std::string("Error skipping 64-bit SDIF columns: ") + std::string(SDIF_GetErrorString(r)) );
	}
}
		

// ---------------------------------------------------------------------------
//	addRowToPartials
// ---------------------------------------------------------------------------
//	Add to existing Loris partials, or create new Loris partials for this data.
//
void
SdifReader::addRowToPartials( const RowOfLorisData64 & trackData, const double frameTime, 
								std::vector< Partial * > & partialsVector )
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
	Partial * oldpar = (partialsVector.size() <= trackData.index) ? NULL : partialsVector[trackData.index];

	if ( oldpar == NULL ) 
	{
	
		// Create the new partial.
		Partial * newpar = new Partial();
		newpar->setLabel( (int) trackData.label );
		newpar->insert( frameTime + trackData.timeOffset, newbp );
		
		// Make sure partialsVector is big enough for this index, then fill in pointer to the partial.
		if (trackData.index < 0 || trackData.index > 10000000)
			Throw( FileIOException, "SDIF file has track index values are negative or over 10 million.");
		while (partialsVector.size() <= trackData.index)
			partialsVector.push_back( NULL );
		partialsVector[trackData.index] = newpar;
		
	} 
	else 
	{
	
		// Add to existing partial.
		oldpar->insert( frameTime + trackData.timeOffset, newbp );
	}
}



#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif



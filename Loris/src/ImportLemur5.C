// ===========================================================================
//	ImportLemur5.C
//	
//	Implementation of Loris::ImportLemur5, a concrete subclass of 
//	Loris::Import for importing Partials stored in Lemur 5 alpha files.
//
//
//	-kel 10 Sept 99
//
// ===========================================================================


#include "LorisLib.h"
#include "ImportLemur5.h"
#include "File.h"
#include "Exception.h"
#include "Partial.h"
#include "Breakpoint.h"
#include "Notifier.h"

Begin_Namespace( Loris )

#pragma mark -
#pragma mark construction
// ---------------------------------------------------------------------------
//	ImportLemur5 constructor
// ---------------------------------------------------------------------------
//
ImportLemur5::ImportLemur5( File & lemrFile ) : 
	_file( lemrFile ),
	_counter( 0 ),
	Import()
{
}

// ---------------------------------------------------------------------------
//	ImportLemur5 destructor
// ---------------------------------------------------------------------------
//
ImportLemur5::~ImportLemur5( void )
{
	_file.close();
}

#pragma mark -
#pragma mark primitive operations
// ---------------------------------------------------------------------------
//	verifySource
// ---------------------------------------------------------------------------
//	Verify that the file has the correct format and is available for reading.
//
void
ImportLemur5::verifySource( void )
{
	try {
		_file.open( );
		_file.setPosition( 0 );
		
		//	check file type ids:
		Int ids[2], size;
		_file.read( ids[0] );
		_file.read( size );
		_file.read( ids[1] );
		if ( ids[0] != FORM_ID || ids[1] != LEMR_ID ) {
			#ifdef Debug_Loris
			string dbg("Bad file ids: ");
			dbg.append((char *)&ids[0], 4);
			dbg.append(" and ");
			dbg.append((char *)&ids[1], 4);
			Debug(dbg);
			#endif
			
			Throw( ImportError, "File is not formatted correctly for Lemur 5 import." );
		}
		
		//	check file format number:
		AnalysisParamsCk pck;
		readParamsChunk( pck );
		if ( pck.formatNumber != FormatNumber ) {
			Throw( ImportError, "File has wrong Lemur format for Lemur 5 import." );
		}
	}
	catch ( FileAccessException & ex ) {
		//	convert to an Import Error:
		Throw( ImportError, ex.getString() );
	}
		
}

// ---------------------------------------------------------------------------
//	beginImport
// ---------------------------------------------------------------------------
//	Find the Tracks chunk and remember how many tracks there are to import. 
//
//	to avoid hitting the disk thousands of times, probably want
//	to allocate a read buffer that can be grown when needed,
//	and also freed at the end.
//
void
ImportLemur5::beginImport( void )
{
	//	find and read the TrackData chunk,
	//	make a note of how many tracks there are:		
	TrackDataCk tck;
	readTracksChunk( tck );
	_counter = tck.numberOfTracks;
}

// ---------------------------------------------------------------------------
//	done
// ---------------------------------------------------------------------------
//	Return true if the number of Partials read equals the number of
//	Partials in the Lemur file.
//
Boolean
ImportLemur5::done( void )
{
	Assert( _counter >= 0 );
	return _counter == 0;
}

// ---------------------------------------------------------------------------
//	getPartial
// ---------------------------------------------------------------------------
//	Fix phase stuff sometime.
//
void
ImportLemur5::getPartial( void )
{
	//	decrement the counter before we have
	//	a chance to get blown outta here by an
	//	exception:
	--_counter;
	
	//	read the Track header:
	TrackOnDisk tkHeader;
	readTrackHeader( tkHeader );
	
	//	create a Partial:
	mPartials.push_back( Partial() );
	Partial & p = mPartials.back();
	p.setLabel( tkHeader.label );
	
	//	keep running phase and time for Breakpoint construction:
	Double phase = tkHeader.initialPhase;
	Double time = tkHeader.startTime * 0.001;	//	convert to seconds
	
	//	loop: read Peak, create Breakpoint, add to Partial:
	for ( Int i = 0; i < tkHeader.numPeaks; ++i ) {
		//	read Peak:
		PeakOnDisk pkData;
		readPeakData( pkData );
		
		//	create Breakpoint:	
		Breakpoint bp( pkData.frequency, pkData.magnitude, pkData.bandwidth, phase );
		
		//	insert in Partial:
		p.insert( time, bp );
		
		//	update time and phase:
		phase = 0;	//	€€€ do this right!
		time += pkData.ttn * 0.001;
	}
}	
		
// ---------------------------------------------------------------------------
//	endImport
// ---------------------------------------------------------------------------
//
void
ImportLemur5::endImport( void )
{
	_file.close();
}

#pragma mark -
#pragma mark helpers

// ---------------------------------------------------------------------------
//	readChunkHeader
// ---------------------------------------------------------------------------
//	Read the id and chunk size from the current file position.
//	Don't stop exceptions here, let the caller catch them and
//	make an intelligent report.
//
void
ImportLemur5::readChunkHeader( CkHeader & h )
{
	try {
		_file.read( h.id );
		_file.read( h.size );
	}
	catch( FileAccessException & ex ) {
		ex.append( "Failed to read chunk header." );
		throw;
	}
} 

// ---------------------------------------------------------------------------
//	readTracksChunk
// ---------------------------------------------------------------------------
//	Leave file positioned at end of chunk header data and at the beginning 
//	of the first track.
//
void 
ImportLemur5::readTracksChunk( TrackDataCk & ck )
{
	//	rewind:
	_file.setPosition( 0 );
	
	//	find the chunk in the file:
	//	read a chunk header, if it isn't the one we want, skip over it.	
	try {
		for ( readChunkHeader( ck.header ); ck.header.id != TrackDataID; readChunkHeader( ck.header ) ) {
			Assert( ck.header.size > 0 );
			Assert( ! _file.atEOF() );
			if ( ck.header.id == FORM_ID )
				_file.offsetPosition( sizeof(Int_32) );
			else
				_file.offsetPosition( ck.header.size );
		}
		
		//	found it, read it one field at a time:
		_file.read( ck.numberOfTracks );
		_file.read( ck.trackOrder );
	}
	catch ( Exception & ex ) {
		using std::string;
		string s( "No Track Data chunk found in this file." );
		s.append( ex.getString() );
		Throw( ImportError, s );
	}
	
}

// ---------------------------------------------------------------------------
//	readParamsChunk
// ---------------------------------------------------------------------------
//
void
ImportLemur5::readParamsChunk( AnalysisParamsCk & ck )
{
	//	rewind:
	_file.setPosition( 0 );
	
	//	find the chunk in the file:
	//	read a chunk header, if it isn't the one we want, skip over it.
	try {
		for ( readChunkHeader( ck.header ); ck.header.id != AnalysisParamsID; readChunkHeader( ck.header ) ) {
			Assert( ck.header.size > 0 );
			Int p = _file.position();
			Assert( ! _file.atEOF() );
			p = _file.position();
			if ( ck.header.id == FORM_ID ) {
				_file.offsetPosition( sizeof(Int_32) );
			}
			else
				_file.offsetPosition( ck.header.size );
			p = _file.position();
		}
		
		//	found it, read it one field at a time:
		_file.read( ck.formatNumber );
		_file.read( ck.originalFormatNumber );

		_file.read( ck.ftLength );
		_file.read( ck.winWidth );
		_file.read( ck.winAtten );
		_file.read( ck.hopSize );
		_file.read( ck.sampleRate );
		
		_file.read( ck.noiseFloor );
		_file.read( ck.peakAmpRange );
		_file.read( ck.maskingRolloff );
		_file.read( ck.peakSeparation );
		_file.read( ck.freqDrift );
	}
	catch ( Exception & ex ) {
		using std::string;
		string s( "No Parameters chunk found in this file." );
		s.append( ex.getString() );
		Throw( ImportError, s );
	}
}

// ---------------------------------------------------------------------------
//	readTrackHeader
// ---------------------------------------------------------------------------
//	Read from current position.
//
void 
ImportLemur5::readTrackHeader( TrackOnDisk & t )
{
	_file.read( t.startTime );
	_file.read( t.initialPhase );
	_file.read( t.numPeaks );
	_file.read( t.label );
}

// ---------------------------------------------------------------------------
//	readPeakData
// ---------------------------------------------------------------------------
//	Read from current position.
//
void 
ImportLemur5::readPeakData( PeakOnDisk & p )
{
	_file.read( p.magnitude );
	_file.read( p.frequency );
	_file.read( p.interpolatedFrequency );
	_file.read( p.bandwidth );
	_file.read( p.ttn );
}
End_Namespace( Loris )

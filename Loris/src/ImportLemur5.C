// ===========================================================================
//	ImportLemur5.C
//	
//	Implementation of Loris::ImportLemur5, a concrete subclass of 
//	Loris::Import for importing Partials stored in Lemur 5 alpha files.
//
//	-kel 10 Sept 99
//
// ===========================================================================
#include "ImportLemur5.h"
#include "BinaryFile.h"
#include "Exception.h"
#include "Partial.h"
#include "Breakpoint.h"
#include "notifier.h"
#include "pi.h"
#include <algorithm>

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
	using std::fmod;
	using std::sqrt;
#else
	#include <math.h>
#endif


#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif


#pragma mark -
#pragma mark construction
// ---------------------------------------------------------------------------
//	ImportLemur5 constructor
// ---------------------------------------------------------------------------
//
ImportLemur5::ImportLemur5( BinaryFile & lemrFile ) : 
	_file( lemrFile ),
	_bweCutoff( 1000. ),	//	default cutoff in Lemur
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
		_file.seek( 0 );
		if( _file.tell() != 0 )
			Throw( FileIOException, "Couldn't rewind Lemur file (bad open mode?)." );
		
		//	check file type ids:
		int ids[2], size;
		_file.read( ids[0] );
		_file.read( size );
		_file.read( ids[1] );
		
		//	except if there were any read errors:
		if ( ! _file.good() )
			Throw( FileIOException, "Cannot read file in Lemur 5 import.");	
		
		if ( ids[0] != FORM_ID || ids[1] != LEMR_ID ) {
			debugger << "Bad file ids: ";
			debugger << string((char *)&ids[0], 4) << " and " << string((char *)&ids[1], 4);
			
			Throw( ImportException, "File is not formatted correctly for Lemur 5 import." );
		}
		
		//	check file format number:
		AnalysisParamsCk pck;
		readParamsChunk( pck );
		if ( pck.formatNumber != FormatNumber ) {
			Throw( ImportException, "File has wrong Lemur format for Lemur 5 import." );
		}
	}
	catch ( FileIOException & ex ) {
		//	convert to an Import Error:
		Throw( ImportException, ex.str() );
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
bool
ImportLemur5::done( void )
{
	Assert( _counter >= 0 );
	return _counter == 0;
}

// ---------------------------------------------------------------------------
//	getPartial
// ---------------------------------------------------------------------------
//	Convert any FileIOExceptions into ImportExceptions, so that clients can 
//	reasonably expect to catch only ImportExceptions.
//
void
ImportLemur5::getPartial( void )
{
	//	decrement the counter before we have
	//	a chance to get blown outta here by an
	//	exception:
	--_counter;
	
	try {
		//	read the Track header:
		TrackOnDisk tkHeader;
		readTrackHeader( tkHeader );
		
		//	create a Partial:
		Partial p;
		p.setLabel( tkHeader.label );
		
		//	keep running phase and time for Breakpoint construction:
		double phase = tkHeader.initialPhase;
		
		//	convert time to seconds and offset by a millisecond to 
		//	allow for implied onset (Lemur analysis data was shifted
		//	such that the earliest Partial starts at 0).
		double time = tkHeader.startTime * 0.001;	
		
		//	use this to compute phases:
		double prevTtnSec = 0.;
		
		//	loop: read Peak, create Breakpoint, add to Partial:
		for ( int i = 0; i < tkHeader.numPeaks; ++i ) {
			//	read Peak:
			PeakOnDisk pkData;
			readPeakData( pkData );
			
			double frequency = pkData.frequency;
			double amplitude = pkData.magnitude;
			double bandwidth = std::min( pkData.bandwidth, 1.f );
			
			//	fix bandwidth:
			//	Lemur used a cutoff frequency, below which 
			//	bandwidth was ignored; Loris does not, so 
			//	toss out that bogus bandwidth.
			if ( frequency < _bweCutoff ) {
				amplitude *= sqrt(1. - bandwidth);
				bandwidth = 0.;
			}
			//	otherwise, adjust the bandwidth value
			//	to account for the difference in noise
			//	scaling between Lemur and Loris; this
			//	mess doubles the noise modulation index
			//	without changing the sine modulation index,
			//	see Oscillator::modulate(). 
			else {
				amplitude *= sqrt( 1. + (3. * bandwidth) );
				bandwidth = (4. * bandwidth) / ( 1. + (3. * bandwidth) ); 
			}

			//	update phase based on _this_ pkData's interpolated freq:
			phase +=TwoPi * prevTtnSec * pkData.interpolatedFrequency;
			phase = fmod( phase, TwoPi );
			
			//	create Breakpoint:	
			Breakpoint bp( frequency, amplitude, bandwidth, phase );
			
			//	insert in Partial:
			p.insert( time, bp );
			
			//	update time:
			prevTtnSec = pkData.ttn * 0.001;
			time += prevTtnSec;
		}

		if ( p.duration() > 0. ) {
			_partials.push_back( p );
		}
		/*
		else {
			debugger << "import rejecting a Partial of zero duration (" 
					<< tkHeader.numPeaks << " peaks read)" << endl;
		}
		*/
	}
	catch( FileIOException & ex ) {
		Throw( ImportException, ex.str() + "Failed to import a partial from a Lemur file." );
	}
	
}	
		
// ---------------------------------------------------------------------------
//	endImport
// ---------------------------------------------------------------------------
//
void
ImportLemur5::endImport( void )
{
}

#pragma mark -
#pragma mark helpers

// ---------------------------------------------------------------------------
//	readChunkHeader
// ---------------------------------------------------------------------------
//	Read the id and chunk size from the current file position.
//
void
ImportLemur5::readChunkHeader( CkHeader & h )
{
	_file.read( h.id );
	_file.read( h.size );
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
	try {
		//	rewind:
		_file.seek( 0 );
		if( _file.tell() != 0 )
			Throw( FileIOException, "Couldn't rewind Lemur file (bad open mode?)." );
		
		//	find the track data chunk from the file:
		//	read a chunk header, if it isn't the one we want, skip over it.	
		for ( readChunkHeader( ck.header ); ck.header.id != TrackDataID; readChunkHeader( ck.header ) ) {
			//	make sure the chunk looks valid:
			if( ck.header.size < 0 )
				Throw( FileIOException, "Found bogus chunk size." );
				
			if ( ck.header.id == FORM_ID )
				_file.offset( sizeof(Int_32) );
			else
				_file.offset( ck.header.size );
		}
		
		//	found it, read it one field at a time:
		_file.read( ck.numberOfTracks );
		_file.read( ck.trackOrder );
	}
	catch( FileIOException & ex ) {
		Throw( ImportException, ex.str() + "Failed to read badly-formatted Lemur file (bad Track Data chunk)." );
	}
}

// ---------------------------------------------------------------------------
//	readParamsChunk
// ---------------------------------------------------------------------------
//
void
ImportLemur5::readParamsChunk( AnalysisParamsCk & ck )
{
	try {
		//	rewind:
		_file.seek( 0 );
		if( _file.tell() != 0 )
			Throw( FileIOException, "Couldn't rewind Lemur file (bad open mode?)." );

		//	find the Parameters chunk in the file:
		//	read a chunk header, if it isn't the one we want, skip over it.
		for ( readChunkHeader( ck.header ); ck.header.id != AnalysisParamsID; readChunkHeader( ck.header ) ) {
			//	make sure the chunk looks valid:
			if( ck.header.size < 0 )
				Throw( FileIOException, "Found bogus chunk size." );
				
			if ( ck.header.id == FORM_ID )
				_file.offset( sizeof(Int_32) );
			else
				_file.offset( ck.header.size );
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
	catch( FileIOException & ex ) {
		Throw( ImportException, ex.str() + "Failed to read badly-formatted Lemur file (bad Parameters chunk)." );
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
	try {
		_file.read( t.startTime );
		_file.read( t.initialPhase );
		_file.read( t.numPeaks );
		_file.read( t.label );
	}
	catch( FileIOException & ex ) {
		ex.append( "Failed to read track data in Lemur 5 import." );
		throw;
	}
}

// ---------------------------------------------------------------------------
//	readPeakData
// ---------------------------------------------------------------------------
//	Read from current position.
//
void 
ImportLemur5::readPeakData( PeakOnDisk & p )
{
	try {
		_file.read( p.magnitude );
		_file.read( p.frequency );
		_file.read( p.interpolatedFrequency );
		_file.read( p.bandwidth );
		_file.read( p.ttn );
	}
	catch( FileIOException & ex ) {
		ex.append( "Failed to read peak data in Lemur 5 import." );
		throw;
	}
}
#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

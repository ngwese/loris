// ===========================================================================
//	ImportLemur5.C
//	
//	Implementation of Loris::ImportLemur5, a concrete subclass of 
//	Loris::Import for importing Partials stored in Lemur 5 alpha files.
//
//	-kel 10 Sept 99
//
//	THIS IS STILL BUSTED, WON'T WORK IF CHUNKS ARE IN A DIFFERENT ORDER!!!
//
// ===========================================================================
#include "ImportLemur5.h"
#include "Endian.h"
#include "Exception.h"
#include "Partial.h"
#include "Breakpoint.h"
#include "notifier.h"
#include "pi.h"
#include "LorisTypes.h"
#include <algorithm>
#include <cmath>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

//	-- types and ids --
enum { 
		FORM_ID = 'FORM', 
		LEMR_ID = 'LEMR', 
		AnalysisParamsID = 'LMAN', 
		TrackDataID = 'TRKS',
		FormatNumber = 4962 };

//	for reading and writing files, the exact sizes and
//	alignments are critical.
typedef Int_32 ID;
struct CkHeader 
{
	Int_32 id;
	Int_32 size;
};

struct AnalysisParamsCk
{
	//Int_32 ckID;
	//Int_32 ckSize;
	CkHeader header;
	
	Int_32 formatNumber;
	Int_32 originalFormatNumber;
	
	Int_32 ftLength;			//	samples, transform length
	Float_32 winWidth;			//	Hz, main lobe width
	Float_32 winAtten;			//	dB, sidelobe attenuation
	Int_32	hopSize;			//	samples, frame length
	Float_32 sampleRate;		// 	Hz, from analyzed sample
	
	Float_32 noiseFloor;		//	dB (negative)
	Float_32 peakAmpRange;		//	dB, floating relative amplitde threshold
	Float_32 maskingRolloff;	//	dB/Hz, peak masking curve	
	Float_32 peakSeparation;	//	Hz, minimum separation between peaks
	Float_32 freqDrift;			//	Hz, maximum track freq drift over a frame
}; 

struct TrackDataCk
{
	CkHeader header;
	Uint_32	numberOfTracks;
	Int_32	trackOrder;			// enumerated type
	// track data follows
};

struct TrackOnDisk
{
	Double_64	startTime;		// in milliseconds
	Float_32	initialPhase;
	Uint_32		numPeaks;
	Int_32		label;
};

struct PeakOnDisk
{
	Float_32	magnitude;
	Float_32	frequency;
	Float_32	interpolatedFrequency;
	Float_32	bandwidth;
	Double_64	ttn;
};


// ---------------------------------------------------------------------------
//	ImportLemur5 constructor
// ---------------------------------------------------------------------------
//
ImportLemur5::ImportLemur5( std::istream & s ) : 
	_file( s ),
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
}

// ---------------------------------------------------------------------------
//	verifySource
// ---------------------------------------------------------------------------
//	Verify that the file has the correct format and is available for reading.
//
void
ImportLemur5::verifySource( void )
{
	try 
	{
		//	check file type ids:
		Int_32 ids[2], size;
		BigEndian::read( _file, 1, sizeof(ID), (char *)&ids[0] );
		BigEndian::read( _file, 1, sizeof(Uint_32), (char *)&size );
		BigEndian::read( _file, 1, sizeof(ID), (char *)&ids[1] );
		
		//	except if there were any read errors:
		if ( ! _file.good() )
			Throw( FileIOException, "Cannot read stream in Lemur 5 import.");	
		
		if ( ids[0] != FORM_ID || ids[1] != LEMR_ID ) {
			debugger << "Bad file ids: ";
			debugger << std::string((char *)&ids[0], 4) << " and " << std::string((char *)&ids[1], 4);
			debugger << "(backwards on little endian systems)" << endl;
			
			Throw( ImportException, "File is not formatted correctly for Lemur 5 import." );
		}
		
		//	check file format number:
		AnalysisParamsCk pck;
		readParamsChunk( pck );
		if ( pck.formatNumber != FormatNumber ) {
			Throw( ImportException, "File has wrong Lemur format for Lemur 5 import." );
		}
	}
	catch ( FileIOException & ex ) 
	{
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
				amplitude *= std::sqrt(1. - bandwidth);
				bandwidth = 0.;
			}
			//	otherwise, adjust the bandwidth value
			//	to account for the difference in noise
			//	scaling between Lemur and Loris; this
			//	mess doubles the noise modulation index
			//	without changing the sine modulation index,
			//	see Oscillator::modulate(). 
			else {
				amplitude *= std::sqrt( 1. + (3. * bandwidth) );
				bandwidth = (4. * bandwidth) / ( 1. + (3. * bandwidth) ); 
			}

			//	update phase based on _this_ pkData's interpolated freq:
			phase +=TwoPi * prevTtnSec * pkData.interpolatedFrequency;
			phase = std::fmod( phase, TwoPi );
			
			//	create Breakpoint:	
			Breakpoint bp( frequency, amplitude, bandwidth, phase );
			
			//	insert in Partial:
			p.insert( time, bp );
			
			//	update time:
			prevTtnSec = pkData.ttn * 0.001;
			time += prevTtnSec;
		}

		if ( p.duration() > 0. ) {
			partials().push_back( p );
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
	BigEndian::read( _file, 1, sizeof(ID), (char *)&h.id );
	BigEndian::read( _file, 1, sizeof(Int_32), (char *)&h.size );
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
	try 
	{		
		//	find the track data chunk from the file:
		//	read a chunk header, if it isn't the one we want, skip over it.	
		for ( readChunkHeader( ck.header ); ck.header.id != TrackDataID; readChunkHeader( ck.header ) ) {
			//	make sure the chunk looks valid:
			if( ck.header.size < 0 )
				Throw( FileIOException, "Found bogus chunk size." );
				
			if ( ck.header.id == FORM_ID )
				_file.ignore( sizeof(Int_32) );
			else
				_file.ignore( ck.header.size );
		}
		
		//	found it, read it one field at a time:
		BigEndian::read( _file, 1, sizeof(Uint_32), (char *)&ck.numberOfTracks );
		BigEndian::read( _file, 1, sizeof(Int_32), (char *)&ck.trackOrder );
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
	try 
	{
		//	find the Parameters chunk in the file:
		//	read a chunk header, if it isn't the one we want, skip over it.
		for ( readChunkHeader( ck.header ); ck.header.id != AnalysisParamsID; readChunkHeader( ck.header ) ) {
			//	make sure the chunk looks valid:
			if( ck.header.size < 0 )
				Throw( FileIOException, "Found bogus chunk size." );
				
			if ( ck.header.id == FORM_ID )
				_file.ignore( sizeof(Int_32) );
			else
				_file.ignore( ck.header.size );
		}
		
		//	found it, read it one field at a time:
		BigEndian::read( _file, 1, sizeof(Int_32), (char *)&ck.formatNumber );
		BigEndian::read( _file, 1, sizeof(Int_32), (char *)&ck.originalFormatNumber );

		BigEndian::read( _file, 1, sizeof(Int_32), (char *)&ck.ftLength );
		BigEndian::read( _file, 1, sizeof(Float_32), (char *)&ck.winWidth );
		BigEndian::read( _file, 1, sizeof(Float_32), (char *)&ck.winAtten );
		BigEndian::read( _file, 1, sizeof(Int_32), (char *)&ck.hopSize );
		BigEndian::read( _file, 1, sizeof(Float_32), (char *)&ck.sampleRate );
		
		BigEndian::read( _file, 1, sizeof(Float_32), (char *)&ck.noiseFloor );
		BigEndian::read( _file, 1, sizeof(Float_32), (char *)&ck.peakAmpRange );
		BigEndian::read( _file, 1, sizeof(Float_32), (char *)&ck.maskingRolloff );
		BigEndian::read( _file, 1, sizeof(Float_32), (char *)&ck.peakSeparation );
		BigEndian::read( _file, 1, sizeof(Float_32), (char *)&ck.freqDrift );
	}
	catch( FileIOException & ex ) 
	{
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
	try 
	{
		BigEndian::read( _file, 1, sizeof(Double_64), (char *)&t.startTime );
		BigEndian::read( _file, 1, sizeof(Float_32), (char *)&t.initialPhase );
		BigEndian::read( _file, 1, sizeof(Uint_32), (char *)&t.numPeaks );
		BigEndian::read( _file, 1, sizeof(Int_32), (char *)&t.label );
	}
	catch( FileIOException & ex ) 
	{
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
	try 
	{
		BigEndian::read( _file, 1, sizeof(Float_32), (char *)&p.magnitude );
		BigEndian::read( _file, 1, sizeof(Float_32), (char *)&p.frequency );
		BigEndian::read( _file, 1, sizeof(Float_32), (char *)&p.interpolatedFrequency );
		BigEndian::read( _file, 1, sizeof(Float_32), (char *)&p.bandwidth );
		BigEndian::read( _file, 1, sizeof(Double_64), (char *)&p.ttn );
	}
	catch( FileIOException & ex ) 
	{
		ex.append( "Failed to read peak data in Lemur 5 import." );
		throw;
	}
}
#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

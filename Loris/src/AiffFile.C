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
 * AiffFile.cc
 *
 * Implementation of AiffFile class for sample import and export in Loris.
 *
 * Kelly Fitz, 8 Jan 2003 
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#if HAVE_CONFIG_H
	#include <config.h>
#endif

#include <AiffFile.h>

#include <AiffData.h>
#include <Exception.h>
#include <Marker.h>
#include <Notifier.h>
#include <Synthesizer.h>

#include <algorithm>
#include <climits>
#include <fstream>
#include <vector>

//	begin namespace
namespace Loris {

#pragma mark -- construction --

// ---------------------------------------------------------------------------
//	AiffFile constructor from filename
// ---------------------------------------------------------------------------
//	Initialize an instance of AiffFile by importing sample data from
//	the file having the specified filename or path.
//
AiffFile::AiffFile( const std::string & filename ) :
	notenum_( 60 ),
	rate_( 1 )		// rate will be overwritten on import
{
	readAiffData( filename );
}

// ---------------------------------------------------------------------------
//	AiffFile constructor from parameters, no samples.
// ---------------------------------------------------------------------------
//	Initialize an instance of AiffFile having the specified sample 
//	rate, preallocating numFrames samples, initialized to zero.
//
AiffFile::AiffFile( double samplerate, size_type numFrames /* = 0 */ ) :
	notenum_( 60 ),
	rate_( samplerate ),
	samples_( numFrames, 0 )
{
}

// ---------------------------------------------------------------------------
//	AiffFile constructor from sample data
// ---------------------------------------------------------------------------
//	Initialize an instance of AiffFile from a buffer of sample
//	data, with the specified sample rate.
//
AiffFile::AiffFile( const double * buffer, size_type bufferlength, double samplerate ) :
	notenum_( 60 ),
	rate_( samplerate )
{
	samples_.insert( samples_.begin(), buffer, buffer+bufferlength );
}


// ---------------------------------------------------------------------------
//	AiffFile constructor from sample data
// ---------------------------------------------------------------------------
//	Initialize an instance of AiffFile from a buffer of sample
//	data, with the specified sample rate.
//
AiffFile::AiffFile( const std::vector< double > & vec, double samplerate ) :
	notenum_( 60 ),
	rate_( samplerate ),
	samples_( vec.begin(), vec.end() )
{
}

// ---------------------------------------------------------------------------
//	AiffFile copy constructor 
// ---------------------------------------------------------------------------
//	Initialize this and AiffFile that is an exact copy, having
//	all the same sample data, as another AiffFile.
//
AiffFile::AiffFile( const AiffFile & other ) :
	notenum_( other.notenum_ ),
	rate_( other.rate_ ),
	markers_( other.markers_ ),
	samples_( other.samples_ )
{
}
	 
// ---------------------------------------------------------------------------
//	AiffFile assignment operator 
// ---------------------------------------------------------------------------
//	Assignment operator: change this AiffFile to be an exact copy
//	of the specified AiffFile, rhs, that is, having the same sample
//	data.
//
AiffFile & 
AiffFile::operator= ( const AiffFile & rhs )
{
	if ( &rhs != this )
	{
		// 	before modifying anything, make
		//	sure there's enough space:
		samples_.reserve( rhs.samples_.size() );
		markers_.reserve( rhs.markers_.size() );
	
		notenum_ = rhs.notenum_;
		rate_ = rhs.rate_;
		markers_ = rhs.markers_;
		samples_ = rhs.samples_;
		
		//	don't copy the Synthesizer, it doesn't
		//	have any state that needs to be preserved,
		//	reset the auto_ptr, and another Synthesizer
		//	will be constructed if it is needed:
		psynth_.reset();
	}
	return *this;
}

// ---------------------------------------------------------------------------
//	write 
// ---------------------------------------------------------------------------
//	Export the samples represented by this AiffFile to a file having
//	the specified filename or path, using bps bits per sample.
//
void
AiffFile::write( const std::string & filename, unsigned int bps )
{
	static const unsigned int ValidSizes[] = { 8, 16, 24, 32 };
	if ( std::find( ValidSizes, ValidSizes+4, bps ) == ValidSizes+4 )
	{
		Throw( InvalidArgument, "Invalid bits-per-sample." );
	}

	std::ofstream s( filename.c_str() );
	if ( ! s )
	{
		std::string s = "Could not create file \"";
		s += filename;
		s += "\". Failed to write AIFF file.";
		Throw( FileIOException, s );
	}
	
	unsigned long dataSize = 0;

	CommonCk commonChunk;
	configureCommonCk( commonChunk, samples_.size(), 1 /* channel */, bps, rate_ );
	dataSize += commonChunk.header.size + sizeof(CkHeader);
	
	SoundDataCk soundDataChunk;
	configureSoundDataCk( soundDataChunk, samples_, bps );
	dataSize += soundDataChunk.header.size + sizeof(CkHeader);
	
	InstrumentCk instrumentChunk;
	configureInstrumentCk( instrumentChunk, notenum_ );
	dataSize += instrumentChunk.header.size + sizeof(CkHeader);

	MarkerCk markerChunk;
	if ( ! markers_.empty() )
	{
		configureMarkerCk( markerChunk, markers_, rate_ );
		dataSize += markerChunk.header.size + sizeof(CkHeader);
	}
	
	ContainerCk containerChunk;
	configureContainer( containerChunk, dataSize );
	
	try 
	{
		writeContainer( s, containerChunk );
		writeCommonData( s, commonChunk );
		if ( ! markers_.empty() )
			writeMarkerData( s, markerChunk );
		writeInstrumentData( s, instrumentChunk );
		writeSampleData( s, soundDataChunk );
		
		s.close();
	}
	catch ( Exception & ex ) 
	{
		ex.append( " Failed to write AIFF file." );
		throw;
	}
}

#pragma mark -- access --

// ---------------------------------------------------------------------------
//	markers 
// ---------------------------------------------------------------------------
//	Return a reference to the Marker (see Marker.h) container 
//	for this AiffFile. 
//
AiffFile::markers_type & 
AiffFile::markers( void )
{
	return markers_;
}

const AiffFile::markers_type & 
AiffFile::markers( void ) const
{
	return markers_;
}

// ---------------------------------------------------------------------------
//	midiNoteNumber 
// ---------------------------------------------------------------------------
//	Return the fractional MIDI note number assigned to this AiffFile. 
//	If the sound has no definable pitch, note number 60.0 is used.
//
double 
AiffFile::midiNoteNumber( void ) const
{
	return notenum_;
}

// ---------------------------------------------------------------------------
//	numFrames 
// ---------------------------------------------------------------------------
//	Return the number of sample frames represented in this AiffFile.
//	A sample frame contains one sample per channel for a single sample
//	interval (e.g. mono and stereo samples files having a sample rate of
//	44100 Hz both have 44100 sample frames per second of audio samples).
//
 AiffFile::size_type  
 AiffFile::numFrames( void ) const
 {
 	return samples_.size();
 }

// ---------------------------------------------------------------------------
//	sampleRate 
// ---------------------------------------------------------------------------
//	Return the sampling freqency in Hz for the sample data in this
//	AiffFile.
//
double  
AiffFile::sampleRate( void ) const
{
	return rate_;
}

// ---------------------------------------------------------------------------
//	samples 
// ---------------------------------------------------------------------------
//	Return a reference (or const reference) to the vector containing
//	the floating-point sample data for this AiffFile.
//
AiffFile::samples_type & 
AiffFile::samples( void )
{
	return samples_;
}

const AiffFile::samples_type & 
AiffFile::samples( void ) const
{
	return samples_;
}

#pragma mark -- mutation --

// ---------------------------------------------------------------------------
//	addPartial 
// ---------------------------------------------------------------------------
//	Render the specified Partial using the (optionally) specified
//	Partial fade time, and accumulate the resulting samples into
//	the sample vector for this AiffFile.
//	
//	The default fade time is 1 ms.
//
void 
AiffFile::addPartial( const Loris::Partial & p, double fadeTime )
{
	configureSynthesizer( fadeTime );
	psynth_->synthesize( p );
}

// ---------------------------------------------------------------------------
//	setMidiNoteNumber 
// ---------------------------------------------------------------------------
//	Set the fractional MIDI note number assigned to this AiffFile. 
//	If the sound has no definable pitch, use note number 60.0 (the default).
//
void 
AiffFile::setMidiNoteNumber( double nn )
{
	if ( nn < 0 || nn > 128 )
		Throw( InvalidArgument, "MIDI note number outside of the valid range [1,128]" );
	notenum_ = nn;
}

#pragma mark -- helpers --

// ---------------------------------------------------------------------------
//	configureSynthesizer 
// ---------------------------------------------------------------------------
//	Construct a Synthesizer if necessary, and set its fadeTime.
//
void 
AiffFile::configureSynthesizer( double fadeTime )
{
	if ( psynth_.get() == 0 )
	{
		psynth_.reset( new Synthesizer( rate_, samples_ ) );
	}
	psynth_->setFadeTime( fadeTime );
}

// ---------------------------------------------------------------------------
//	readAiffData
// ---------------------------------------------------------------------------
//
void 
AiffFile::readAiffData( const std::string & filename )
{
	ContainerCk containerChunk;
	CommonCk commonChunk;
	SoundDataCk soundDataChunk;
	InstrumentCk instrumentChunk;
	MarkerCk markerChunk;

	try 
	{
		std::ifstream s( filename.c_str() );
	
		//	the Container chunk must be first, read it:
		readChunkHeader( s, containerChunk.header );
		if( containerChunk.header.id != ContainerId )
			Throw( FileIOException, "Found no Container chunk." );
		readContainer( s, containerChunk, containerChunk.header.size );
		
		//	read other chunks, we are only interested in
		//	the Common chunk, the Sound Data chunk, the Markers: 
		CkHeader h;
		while ( readChunkHeader( s, h ) )
		{			
			switch (h.id)
			{
				case CommonId:
					readCommonData( s, commonChunk, h.size );
					if ( commonChunk.channels != 1 )
					{
						Throw( FileIOException, 
							   "Loris only processes single-channel AIFF samples files." );
					}					
					if ( commonChunk.bitsPerSample != 8 &&
						 commonChunk.bitsPerSample != 16 &&
						 commonChunk.bitsPerSample != 24 &&
						 commonChunk.bitsPerSample != 32 )
					{
						Throw( FileIOException, "Unrecognized sample size." );
					}										
					break;
				case SoundDataId:
					readSampleData( s, soundDataChunk, h.size );
					break;
				case InstrumentId:
					readInstrumentData( s, instrumentChunk, h.size );
					break;
				case MarkerId:
					readMarkerData( s, markerChunk, h.size );
					break;
				default:
					s.ignore( h.size );
			}
		}
	
		if ( ! commonChunk.header.id || ! soundDataChunk.header.id )
		{
			Throw( FileIOException, 
				   "Reached end of file before finding both a Common chunk and a Sound Data chunk." );
		}
	}
	catch ( Exception & ex ) 
	{
		ex.append( " Failed to read AIFF file." );
		throw;
	}
	
	
	//	all the chunks have been read, use them to initialize
	//	the AiffFile members:
	rate_ = commonChunk.srate;
	
	if ( instrumentChunk.header.id )
	{
		notenum_ = instrumentChunk.baseNote;
		notenum_ -= 0.01 * instrumentChunk.detune;
	}
	
	if ( markerChunk.header.id )
	{
		for ( int j = 0; j < markerChunk.numMarkers; ++j )
		{
			MarkerCk::Marker & m = markerChunk.markers[j];
			markers_.push_back( Marker( m.position / rate_, m.markerName ) );
		}		
	}
	
	convertBytesToSamples( soundDataChunk.sampleBytes, samples_, commonChunk.bitsPerSample );
	if ( samples_.size() != commonChunk.sampleFrames )
	{
		notifier << "Found " << samples_.size() << " frames of "
				 << commonChunk.bitsPerSample << "-bit sample data." << endl;
		notifier << "Header says there should be " << commonChunk.sampleFrames 
				 << "." << endl;
	}
}


}	//	end of namespace Loris

// ===========================================================================
//	AiffFile.C
//	
//	Association of a sample buffer and the necessary additional info 
//	(sample rate, number of channels, and sample data size in bits)
//	to completely specify an AIFF samples file. Extends the generic
//	Loris::SamplesFile with AIFF i/o.
//
//	-kel 28 Sept 99
//
// ===========================================================================

#include "AiffFile.h"
#include "BinaryFile.h"
#include "Exception.h"
#include "notifier.h"
#include "ieee.h"
#include <algorithm>
#include <string>
#include <iostream>

#if !defined(Deprecated_cstd_headers)
#include <climits>
#else
#include <limits.h>
#endif

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

//	-- chunk types --
enum { 
	ContainerId = 'FORM', 
	AiffType = 'AIFF', 
	CommonId = 'COMM',
	SoundDataId = 'SSND'
};

typedef Int_32 ID;

struct CkHeader {
	Int_32 id;
	ID size;
};

struct ContainerCk
{
	CkHeader header;
	ID formType;
};

struct CommonCk
{
	CkHeader header;
	Int_16 channels;			// number of channels 
	Int_32 sampleFrames;		// channel independent sample frames 
	Int_16 bitsPerSample;		// number of bits per sample 
	IEEE::extended80 srate;		// sampling rate IEEE 10 byte format 
};

struct SoundDataCk
{
	CkHeader header;	
	Uint_32 offset;				
	Uint_32 blockSize;	
	//	sample frames follow
};

//	data type for integer pcm samples of different sizes:
struct I_24 { char data[3]; };
typedef union 
{
	//	different size samples:
	Int_32 s32bits;						//	32 bits sample
	I_24  s24bits;						//	24 bits sample
	Int_16 s16bits;						//	16 bits sample
	char s8bits;						//	8 bits sample
} pcm_sample;

// ---------------------------------------------------------------------------
//	AiffFile constructor from data in memory
// ---------------------------------------------------------------------------
//
AiffFile::AiffFile( double rate, int chans, int bits, std::vector< double > & buf ) :
	SamplesFile( rate, chans, bits, buf )
{
}

// ---------------------------------------------------------------------------
//	AiffFile constructor from data on disk
// ---------------------------------------------------------------------------
//	Read immediately.
//
AiffFile::AiffFile( std::istream & s, std::vector< double > & buf ) :
	SamplesFile( buf )
{
	read( s );
}

// ---------------------------------------------------------------------------
//	AiffFile constructor from filename
// ---------------------------------------------------------------------------
//	Read immediately.
//
AiffFile::AiffFile( const std::string & filename, std::vector< double > & buf ) :
	SamplesFile( buf )
{
	read( filename );
}

// ---------------------------------------------------------------------------
//	AiffFile copy constructor
// ---------------------------------------------------------------------------
//
AiffFile::AiffFile( const SamplesFile & other ) :
	SamplesFile( other )
{
}

// ---------------------------------------------------------------------------
//	read
// ---------------------------------------------------------------------------
//
void
AiffFile::read( const std::string & filename )
{
	std::ifstream s;
	s.open( filename.c_str(), std::ios::in | std::ios::binary ); 
	read(s);
}

// ---------------------------------------------------------------------------
//	read
// ---------------------------------------------------------------------------
//
void
AiffFile::read( std::istream & s )
{
	try {
		readContainer( s );
		readCommon( s );
		readSampleData( s );
	}
	catch ( Exception & ex ) {
		ex.append( "Failed to read AIFF file." );
		throw;
	}
}

// ---------------------------------------------------------------------------
//	write
// ---------------------------------------------------------------------------
//
void
AiffFile::write( const std::string & filename )
{
	std::ofstream s;
	s.open( filename.c_str(), std::ios::out | std::ios::binary ); 
	write(s);
}

// ---------------------------------------------------------------------------
//	write
// ---------------------------------------------------------------------------
//
void
AiffFile::write( std::ostream & s )
{
	validateParams();
	
	try {
		writeContainer( s );
		writeCommon( s );
		writeSampleData( s );
	}
	catch ( Exception & ex ) {
		ex.append( "Failed to write AIFF file." );
		throw;
	}
}

#pragma mark -
#pragma mark read helpers
// ---------------------------------------------------------------------------
//	readChunkHeader
// ---------------------------------------------------------------------------
//	Read the id and chunk size from the current file position.
//	Let exceptions propogate.
//
void
AiffFile::readChunkHeader( std::istream & s, CkHeader & h )
{
	BigEndian::read( s, 1, sizeof(ID), (char *)&h.id );
	BigEndian::read( s, 1, sizeof(Uint_32), (char *)&h.size );
}

// ---------------------------------------------------------------------------
//	readCommon
// ---------------------------------------------------------------------------
//	YUCK this only works if the stream isn't already past the COMM chunk, no 
//	way to rewind a istream!
//
void
AiffFile::readCommon( std::istream & s )
{
	//	first build a Common chunk, so that all the data sizes will 
	//	be correct:
	CommonCk ck;
	
	try {
		//	find the chunk:
		//	read a chunk header, if it isn't the one we want, skip over it.
		for ( readChunkHeader( s, ck.header ); 
			  ck.header.id != CommonId; 
			  readChunkHeader( s, ck.header ) ) 
		{
			if ( ck.header.id == ContainerId )
			{
				s.ignore( 4 * sizeof(char) );
			}
			else
			{
				s.ignore( ck.header.size );
			}
		}

		//	found it.
		//	read in the chunk data:
		BigEndian::read( s, 1, sizeof(Int_16), (char *)&ck.channels );
		BigEndian::read( s, 1, sizeof(Int_32), (char *)&ck.sampleFrames );
		BigEndian::read( s, 1, sizeof(Int_16), (char *)&ck.bitsPerSample );
		//	don't let this get byte-reversed:
		BigEndian::read( s, 10, sizeof(char), (char *)&ck.srate );
	}
	catch( FileIOException & ex ) {
		ex.append( "Failed to read badly-formatted AIFF file (bad Common chunk)." );
		throw;
	}
						
	//	allocate space for the samples:
	long n = ck.sampleFrames * ck.channels;
	if ( n != _samples.size() ) 
	{
		_samples.resize( n, 0. );
	}
	
	_nChannels = ck.channels;
	_sampSize = ck.bitsPerSample;
	_sampleRate = IEEE::ConvertFromIeeeExtended( ck.srate );
}

// ---------------------------------------------------------------------------
//	readContainer
// ---------------------------------------------------------------------------
//
void
AiffFile::readContainer( std::istream & s )
{
	//	first build a Container chunk, so that all the data sizes will 
	//	be correct:
	ContainerCk ck;		
	
	try {
		// Container is always first:
		readChunkHeader( s, ck.header );
		if( ck.header.id != ContainerId )
			Throw( FileIOException, "Found no Container chunk." );
			
		//	read in the chunk data:
		BigEndian::read( s, 1, sizeof(ID), (char *)&ck.formType );
	}
	catch( FileIOException & ex ) {
		ex.append( "Failed to read badly-formatted AIFF file (bad Container chunk)." );
		throw;
	}

	//	make sure its really AIFF:
	if ( ck.formType != AiffType )
		Throw( FileIOException, std::string("Bad form type in AIFF file: ") + std::string( ck.formType, 4 ) );
}	

// ---------------------------------------------------------------------------
//	readSampleData
// ---------------------------------------------------------------------------
//	Read from the current position, assume the header has already been read:
//	YUCK this only works if the stream isn't already past the SSND chunk, no 
//	way to rewind a istream!
//
void
AiffFile::readSampleData( std::istream & s )
{
	//	first build a Sound Data chunk, so that all the data sizes will 
	//	be correct:
	SoundDataCk ck;

	try {
		//	find the chunk:
		//	read a chunk header, if it isn't the one we want, skip over it.
		for ( readChunkHeader( s, ck.header ); 
			  ck.header.id != SoundDataId; 
			  readChunkHeader( s, ck.header ) ) 
		{
				
			//	make sure we didn't run off the edge of the earth:
			if ( ! s.good() )
				Throw( FileIOException, "Failed to read badly-formatted AIFF file (no Sound Data chunk)." );
			
			if ( ck.header.id == ContainerId )
				s.ignore( sizeof(Int_32) );
			else
				s.ignore( ck.header.size );
		}

		//	found it.
		//	read in the chunk data:
		BigEndian::read( s, 1, sizeof(Uint_32), (char *)&ck.offset );
		BigEndian::read( s, 1, sizeof(Uint_32), (char *)&ck.blockSize );

		//	skip ahead to the samples and read them:
		s.ignore( ck.offset );
		readSamples( s );
	}
	catch( FileIOException & ex ) {
		ex.append( "Failed to read badly-formatted AIFF file (bad Sound Data chunk)." );
		throw;
	}
}

// ---------------------------------------------------------------------------
//	readSamples
// ---------------------------------------------------------------------------
//	Let exceptions propogate.
//	Can do lots better by reading all the samples into a buffer at once.
//
void
AiffFile::readSamples( std::istream & s )
{	
	//	use a vector for automatic temporary storage:
	std::vector<char> v( _samples.size() * (_sampSize / 8) );

	//	read integer samples in without byte-swapping:
	notifier << "reading " << v.size() << " bytes " << _samples.size() << " samples" << endl;
	BigEndian::read( s, v.size(), 1, v.begin() );
	for ( long k = 0; k < v.size(); k += 100 )
		notifier << short(v[k]) << " ";
	notifier << endl;

	//	except if there were any read errors:
	if ( ! s.good() )
		Throw( FileIOException, "Failed to read AIFF samples.");

	static const double oneOverMax = 1. / LONG_MAX;	//	defined in climits
	
	switch ( _sampSize ) {
		case 32:
		{	
			Int_32 * z = (Int_32 *)&v[0];
			for (unsigned long i = 0; i < _samples.size(); ++i ) 
			{
				_samples[i] = oneOverMax * z[i];
			}
			break;
		}
/*
		case 24:
		{
			I_24 * z = (I_24 *)&v[0];
			for (unsigned long i = 0; i < _samples.size(); ++i ) 
			{
				Int_32 samp = (Int_32)v[i*_sampSize]
				_samples[i] = oneOverMax * samp;
			}
			break;
		}
*/
		case 16:
		{
			for (long i = 0; i < v.size(); i += 2 ) 
			{
				Int_32 samp = (long(v[i]) << 24) + (long(v[i+1]) << 16);
				_samples[i/2] = oneOverMax * samp; 

				if (i < 100 )
					notifier << (short)v[i] << " " << (short)v[i+1] 
					<< " " << samp << " " << _samples[i] << endl;
			}
			break;
		}
		case 8:
		{
			char * z = &v[0];
			for (unsigned long i = 0; i < _samples.size(); ++i ) 
			{
				Int_32 samp = 0L | z[i];
				_samples[i] = oneOverMax * (samp << 24);
			}
			break;
		}
	}
}

#pragma mark -
#pragma mark write helpers
// ---------------------------------------------------------------------------
//	writeCommon
// ---------------------------------------------------------------------------
//
void
AiffFile::writeCommon( std::ostream & s )
{
	//	first build a Common chunk, so that all the data sizes will 
	//	be correct:
	CommonCk ck;
	ck.header.id = CommonId;
	
	//	size is everything after the header:
	ck.header.size = sizeofCommon() - sizeofCkHeader();
					
	ck.channels = _nChannels;
	ck.sampleFrames = _samples.size() / _nChannels;
	ck.bitsPerSample = _sampSize;
	IEEE::ConvertToIeeeExtended( _sampleRate, & ck.srate );

/*
	debugger << "writing common chunk: " << endl;
	debugger << "header id: " << ck.header.id << endl;
	debugger << "size: " << ck.header.size << endl;
	debugger << "channels: " << ck.channels << endl;
	debugger << "sample frames: " << ck.sampleFrames << endl;
	debugger << "bits per sample: " << ck.bitsPerSample << endl;
	debugger << "rate: " << _sampleRate << "?" << endl;
*/
	
	//	write it out:
	try 
	{
		BigEndian::write( s, 1, sizeof(ID), (char *)&ck.header.id );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.header.size );
		BigEndian::write( s, 1, sizeof(Int_16), (char *)&ck.channels );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.sampleFrames );
		BigEndian::write( s, 1, sizeof(Int_16), (char *)&ck.bitsPerSample );
		//	don't let this get byte-reversed:
		BigEndian::write( s, 10, sizeof(char), (char *)&ck.srate );
	}
	catch( FileIOException & ex ) 
	{
		ex.append( "Failed to write AIFF file Common chunk." );
		throw;
	}
}

// ---------------------------------------------------------------------------
//	writeContainer
// ---------------------------------------------------------------------------
//
void
AiffFile::writeContainer( std::ostream & s )
{
	//	first build a Container chunk, so that all the data sizes will 
	//	be correct:
	ContainerCk ck;
	ck.header.id = ContainerId;
	
	//	size is everything after the header:
	ck.header.size = sizeof(Int_32) + sizeofCommon() + sizeofSoundData();
	
	ck.formType = AiffType;

/*
	debugger << "writing container: " << endl;
	debugger << "header id: " << ck.header.id << endl;
	debugger << "size: " << ck.header.size << endl;
	debugger << "type: " << ck.formType << endl;
*/
	
	//	write it out:
	try 
	{
		BigEndian::write( s, 1, sizeof(ID), (char *)&ck.header.id );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.header.size );
		BigEndian::write( s, 1, sizeof(ID), (char *)&ck.formType );
	}
	catch( FileIOException & ex ) 
	{
		ex.append( "Failed to write AIFF file Container chunk." );
		throw;
	}
}	

// ---------------------------------------------------------------------------
//	writeSampleData
// ---------------------------------------------------------------------------
//
void
AiffFile::writeSampleData( std::ostream & s )
{
	//	first build a Sound Data chunk, so that all the data sizes will 
	//	be correct:
	SoundDataCk ck;
	ck.header.id = SoundDataId;
	
	//	size is everything after the header:
	ck.header.size = sizeofSoundData() - sizeofCkHeader();
				
	//	no block alignment:	
	ck.offset = 0;
	ck.blockSize = 0;
	
/*
	debugger << "writing sample data: " << endl;
	debugger << "header id: " << ck.header.id << endl;
	debugger << "size: " << ck.header.size << endl;
	debugger << "offset: " << ck.offset << endl;
	debugger << "block size: " << ck.blockSize << endl;
*/

	//	write it out:
	try 
	{
		BigEndian::write( s, 1, sizeof(ID), (char *)&ck.header.id );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.header.size );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.offset );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.blockSize );

		writeSamples( s );
	}
	catch( FileIOException & ex ) {
		ex.append( "Failed to write AIFF file Container chunk." );
		throw;
	}
}

// ---------------------------------------------------------------------------
//	writeSamples
// ---------------------------------------------------------------------------
//	Let exceptions propogate.
//
void
AiffFile::writeSamples( std::ostream & s )
{	
	debugger << "writing " << _samples.size() << " samples of size " << _sampSize << endl;

	pcm_sample z;

	switch ( _sampSize ) {
		case 32:
			for (unsigned long i = 0; i < _samples.size(); ++i ) {
				//	convert to integer (clip instead of wrapping):
				z.s32bits = LONG_MAX * std::min( 1.0, std::max(-1.0, _samples[i]) );
			
				//	write the sample:
				BigEndian::write( s, 1, sizeof(Int_32), (char *)&z.s32bits );
			}
			break;
		case 24:
			for (unsigned long i = 0; i < _samples.size(); ++i ) {
				//	convert to integer (clip instead of wrapping):
				z.s32bits = LONG_MAX * std::min( 1.0, std::max(-1.0, _samples[i]) );
			
				//	write the sample:
				BigEndian::write( s, 1, sizeof(I_24), (char *)&z.s24bits );
			}
			break;
		case 16:
			for (unsigned long i = 0; i < _samples.size(); ++i ) {
				//	convert to integer (clip instead of wrapping):
				z.s32bits = LONG_MAX * std::min( 1.0, std::max(-1.0, _samples[i]) );
			
				//	write the sample:
				BigEndian::write( s, 1, sizeof(Int_16), (char *)&z.s16bits );
			}
			break;
		case 8:
			for (unsigned long i = 0; i < _samples.size(); ++i ) {
				//	convert to integer (clip instead of wrapping):
				z.s32bits = LONG_MAX * std::min( 1.0, std::max(-1.0, _samples[i]) );
			
				//	write the sample:
				BigEndian::write( s, 1, sizeof(char), (char *)&z.s8bits );
			}
			break;
	}
	
	//	except if there were any read errors:
	//	(better to check earlier?)
	if ( ! s.good() )
		Throw( FileIOException, "Failed to write AIFF samples.");
}

#pragma mark -
#pragma mark chunk sizes
// ---------------------------------------------------------------------------
//	sizeofCkHeader
// ---------------------------------------------------------------------------
//
Uint_32
AiffFile::sizeofCkHeader( void )
{
	return	sizeof(Int_32) +	//	id
			sizeof(Uint_32);	//	size
}

// ---------------------------------------------------------------------------
//	sizeofCommon
// ---------------------------------------------------------------------------
//
Uint_32
AiffFile::sizeofCommon( void )
{
	return	sizeof(Int_32) +			//	id
			sizeof(Uint_32) +			//	size
			sizeof(Int_16) + 			//	num channels
			sizeof(Int_32) + 			//	num frames
			sizeof(Int_16) + 			//	bits per sample
			sizeof(IEEE::extended80);	//	sample rate
}

// ---------------------------------------------------------------------------
//	sizeofSoundData
// ---------------------------------------------------------------------------
//	No block alignment, the samples start right after the 
//	chunk header info.
//
Uint_32
AiffFile::sizeofSoundData( void )
{
	Uint_32 dataSize = _samples.size() * ( _sampSize / 8 );
	//	must be an even number of bytes:
	if ( dataSize % 2 ) {
		++dataSize;
	}
	
	return	sizeof(Int_32) +	//	id
			sizeof(Uint_32) +	//	size
			sizeof(Uint_32) + 	//	offset
			sizeof(Uint_32) + 	//	block size
			dataSize;			//	sample data
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

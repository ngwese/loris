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

#include "LorisLib.h"
#include "AiffFile.h"
#include "BinaryFile.h"
#include "Exception.h"
#include "Notifier.h"
#include "ieee.h"

#include <algorithm>
#include <string>

//	define a local floating point constant representing the
//	biggest 32 bit sample (can't rely on numeric_limits in
//	under-compliant libraries):
#if !defined( Lacks_numeric_limits )
	#include <limits>
	static const double Maximum_Long = std::numeric_limits<Loris::Int_32>::max();
#else
	#include <limits.h>
	static const double Maximum_Long = LONG_MAX;
#endif

using namespace std;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	AiffFile constructor from data in memory
// ---------------------------------------------------------------------------
//
AiffFile::AiffFile( double rate, int chans, int bits, vector< double > & buf ) :
	SamplesFile( rate, chans, bits, buf )
{
}

// ---------------------------------------------------------------------------
//	AiffFile constructor from data on disk
// ---------------------------------------------------------------------------
//	Read immediately.
//
AiffFile::AiffFile( BinaryFile & file, vector< double > & buf ) :
	SamplesFile( buf )
{
	read( file );
}

// ---------------------------------------------------------------------------
//	AiffFile constructor from filename
// ---------------------------------------------------------------------------
//	Read immediately.
//
AiffFile::AiffFile( const std::string & filename, vector< double > & buf ) :
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
AiffFile::read( const string & filename )
{
	BinaryFile f;
	f.setBigEndian();
	f.view( filename );
	read(f);
}

// ---------------------------------------------------------------------------
//	read
// ---------------------------------------------------------------------------
//
void
AiffFile::read( BinaryFile & file )
{
	try {
		//	rewind:
		file.seek(0);
		if( file.tell() != 0 )
			Throw( FileIOException, "Couldn't rewind AIFF file (bad open mode?)." );
		file.setBigEndian();
		
		readContainer( file );
		readCommon( file );
		readSampleData( file );
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
AiffFile::write( const string & filename )
{
	BinaryFile f;
	f.setBigEndian();
	f.create( filename );
	write(f);
}

// ---------------------------------------------------------------------------
//	write
// ---------------------------------------------------------------------------
//
void
AiffFile::write( BinaryFile & file )
{
	validateParams();
	
	try {
		//	rewind:
		file.seek(0);
		if( file.tell() != 0 )
			Throw( FileIOException, "Couldn't rewind AIFF file (bad open mode?)." );
		file.setBigEndian();
		
		writeContainer( file );
		writeCommon( file );
		writeSampleData( file );
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
AiffFile::readChunkHeader( BinaryFile & file, CkHeader & h )
{
	file.read( h.id );
	file.read( h.size );
}

// ---------------------------------------------------------------------------
//	readCommon
// ---------------------------------------------------------------------------
//
void
AiffFile::readCommon( BinaryFile & file )
{
	//	first build a Common chunk, so that all the data sizes will 
	//	be correct:
	CommonCk ck;
	
	try {
		//	rewind:
		file.seek(0);
		if( file.tell() != 0 )
			Throw( FileIOException, "Couldn't rewind AIFF file (bad open mode?)." );
		
		//	find the chunk:
		//	read a chunk header, if it isn't the one we want, skip over it.
		for ( readChunkHeader( file, ck.header ); 
			  ck.header.id != CommonId; 
			  readChunkHeader( file, ck.header ) ) {
							
			if ( ck.header.id == ContainerId )
				file.offset( sizeof(Int_32) );
			else
				file.offset( ck.header.size );
		}

		//	found it.
		//	read in the chunk data:
		file.read( ck.channels );
		file.read( ck.sampleFrames );
		file.read( ck.bitsPerSample );
		file.read( ck.srate );
	}
	catch( FileIOException & ex ) {
		ex.append( "Failed to read badly-formatted AIFF file (bad Common chunk)." );
		throw;
	}
						
	//	allocate space for the samples:
	try {
		_samples.resize( ck.sampleFrames * ck.channels, 0. );
	}
	catch( LowMemException & ex ) {
		ex.append( "Couldn't allocate buffer for AIFF samples." );
		throw;
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
AiffFile::readContainer( BinaryFile & file )
{
	//	first build a Container chunk, so that all the data sizes will 
	//	be correct:
	ContainerCk ck;
	
	try {
		//	rewind:
		file.seek(0);
		if( file.tell() != 0 )
			Throw( FileIOException, "Couldn't rewind AIFF file (bad open mode?)." );
			
		// Container is always first:
		readChunkHeader( file, ck.header );
		if( ck.header.id != ContainerId )
			Throw( FileIOException, "Found no Container chunk." );
			
		//	read in the chunk data:
		file.read( ck.formType );
	}
	catch( FileIOException & ex ) {
		ex.append( "Failed to read badly-formatted AIFF file (bad Container chunk)." );
		throw;
	}

	//	make sure its really AIFF:
	if ( ck.formType != AiffType )
		Throw( FileIOException, string("Bad form type in AIFF file: ") + string( ck.formType, 4 ) );
}	

// ---------------------------------------------------------------------------
//	readSampleData
// ---------------------------------------------------------------------------
//	Read from the current position, assume the header has already been read:
//
void
AiffFile::readSampleData( BinaryFile & file )
{
	//	first build a Sound Data chunk, so that all the data sizes will 
	//	be correct:
	SoundDataCk ck;

	try {
		//	rewind:
		file.seek(0);
		if( file.tell() != 0 )
			Throw( FileIOException, "Couldn't rewind AIFF file (bad open mode?)." );
		
		//	find the chunk:
		//	read a chunk header, if it isn't the one we want, skip over it.
		for ( readChunkHeader( file, ck.header ); 
			  ck.header.id != SoundDataId; 
			  readChunkHeader( file, ck.header ) ) {
				
			//	make sure we didn't run off the edge of the earth:
			if ( ! file.good() )
				Throw( FileIOException, "Failed to read badly-formatted AIFF file (no Sound Data chunk)." );
			
			if ( ck.header.id == ContainerId )
				file.offset( sizeof(Int_32) );
			else
				file.offset( ck.header.size );
		}

		//	found it.
		//	read in the chunk data:
		file.read( ck.offset );
		file.read( ck.blockSize );

		//	skip ahead to the samples and read them:
		file.offset( ck.offset );
		readSamples( file );
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
//
void
AiffFile::readSamples( BinaryFile & file )
{	
	static const double oneOverMax = 1. / Maximum_Long;	//	defined at top
	
	pcm_sample z;

	switch ( _sampSize ) {
		case 32:
			for (ulong i = 0; i < _samples.size(); ++i ) {
				//	read the sample:
				file.read( z.s32bits );
				
				//	convert to double:
				_samples[i] = oneOverMax * z.s32bits;
			}
			break;
		case 24:
			for (ulong i = 0; i < _samples.size(); ++i ) {
				//	read the sample:
				file.read( z.s24bits );
				
				//	convert to double:
				_samples[i] = oneOverMax * z.s32bits;
			}
			break;
		case 16:
			for (ulong i = 0; i < _samples.size(); ++i ) {
				//	read the sample:
				file.read( z.s16bits );
				
				//	convert to double:
				_samples[i] = oneOverMax * z.s32bits;
			}
			break;
		case 8:
			for (ulong i = 0; i < _samples.size(); ++i ) {
				//	read the sample:
				file.read( z.s8bits );
				
				//	convert to double:
				_samples[i] = oneOverMax * z.s32bits;
			}
			break;
	}
	
	//	except if there were any read errors:
	//	(better to check earlier?)
	if ( ! file.good() )
		Throw( FileIOException, "Failed to read AIFF samples.");
}

#pragma mark -
#pragma mark write helpers
// ---------------------------------------------------------------------------
//	writeCommon
// ---------------------------------------------------------------------------
//
void
AiffFile::writeCommon( BinaryFile & file )
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
	try {
		file.write( ck.header.id );
		file.write( ck.header.size );
		file.write( ck.channels );
		file.write( ck.sampleFrames );
		file.write( ck.bitsPerSample );
		file.write( ck.srate );
	}
	catch( FileIOException & ex ) {
		ex.append( "Failed to write AIFF file Common chunk." );
		throw;
	}
}

// ---------------------------------------------------------------------------
//	writeContainer
// ---------------------------------------------------------------------------
//
void
AiffFile::writeContainer( BinaryFile & file )
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
	try {
		file.write( ck.header.id );
		file.write( ck.header.size );
		file.write( ck.formType );
	}
	catch( FileIOException & ex ) {
		ex.append( "Failed to write AIFF file Container chunk." );
		throw;
	}
}	

// ---------------------------------------------------------------------------
//	writeSampleData
// ---------------------------------------------------------------------------
//
void
AiffFile::writeSampleData( BinaryFile & file )
{
	//	first build a Sound Data chunk, so that all the data sizes will 
	//	be correct:
	SoundDataCk ck;
	ck.header.id = SoundDataId;
	
	//	size is everything after the header:
	ck.header.size = sizeofSoundData() - sizeofCkHeader();
				
	//	no block alignment:	
	ck.offset = 0.;
	ck.blockSize = 0;
	
/*
	debugger << "writing sample data: " << endl;
	debugger << "header id: " << ck.header.id << endl;
	debugger << "size: " << ck.header.size << endl;
	debugger << "offset: " << ck.offset << endl;
	debugger << "block size: " << ck.blockSize << endl;
*/

	//	write it out:
	try {
		file.write( ck.header.id );
		file.write( ck.header.size );
		file.write( ck.offset );
		file.write( ck.blockSize );

		writeSamples( file );
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
AiffFile::writeSamples( BinaryFile & file )
{	
	// debugger << "writing " << _samples.size() << " samples of size " << _sampSize << endl;

	pcm_sample z;

	switch ( _sampSize ) {
		case 32:
			for (ulong i = 0; i < _samples.size(); ++i ) {
				//	convert to integer (clip instead of wrapping):
				z.s32bits = Maximum_Long * min( 1.0, max(-1.0, _samples[i]) );
			
				//	write the sample:
				file.write( z.s32bits );
			}
			break;
		case 24:
			for (ulong i = 0; i < _samples.size(); ++i ) {
				//	convert to integer (clip instead of wrapping):
				z.s32bits = Maximum_Long * min( 1.0, max(-1.0, _samples[i]) );
			
				//	write the sample:
				file.write( z.s24bits );
			}
			break;
		case 16:
			for (ulong i = 0; i < _samples.size(); ++i ) {
				//	convert to integer (clip instead of wrapping):
				z.s32bits = Maximum_Long * min( 1.0, max(-1.0, _samples[i]) );
			
				//	write the sample:
				// file.write( z.s16bits );
				//	this cast shouldn't matter, does it?
				//	??????
				file.write( Int_16(z.s16bits) );
			}
			break;
		case 8:
			for (ulong i = 0; i < _samples.size(); ++i ) {
				//	convert to integer (clip instead of wrapping):
				z.s32bits = Maximum_Long * min( 1.0, max(-1.0, _samples[i]) );
			
				//	write the sample:
				file.write( z.s8bits );
			}
			break;
	}
	
	//	except if there were any read errors:
	//	(better to check earlier?)
	if ( ! file.good() )
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
	
	return	sizeof(Int_32) +	//	id
			sizeof(Uint_32) +	//	size
			sizeof(Uint_32) + 	//	offset
			sizeof(Uint_32) + 	//	block size
			dataSize;			//	sample data
}

End_Namespace( Loris )

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
 * AiffFile.C
 *
 * Implementation of class AiffFile.
 *
 * Kelly Fitz, 28 Sept 99
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include <config.h>
#endif

#include <AiffFile.h>
#include <Endian.h>
#include <Exception.h>
#include <Notifier.h>
#include "ieee.h"
#include <algorithm>
#include <string>
#include <fstream>
#include <climits>

//	in case configure wasn't run (no config.h), 
//	pick some (hopefully-) reasonable values for
//	these things and hope for the best...
#if ! defined( SIZEOF_SHORT )
#define SIZEOF_SHORT 2
#endif

#if ! defined( SIZEOF_INT )
#define SIZEOF_INT 4
#endif

#if ! defined( SIZEOF_LONG )
#define SIZEOF_LONG 4	// not for DEC Alpha!
#endif


#if SIZEOF_SHORT == 2
typedef short 			Int_16;
typedef unsigned short 	Uint_16;
#elif SIZEOF_INT == 2
typedef int 			Int_16;
typedef unsigned int 	Uint_16;
#else
#error "cannot find an appropriate type for 16-bit integers"
#endif

#if SIZEOF_INT == 4
typedef int 			Int_32;
typedef unsigned int 	Uint_32;
#elif SIZEOF_LONG == 4
typedef long 			Int_32;
typedef unsigned long 	Uint_32;
#else
#error "cannot find an appropriate type for 32-bit integers"
#endif


//	begin namespace
namespace Loris {

//	-- chunk types --
enum { 
	ContainerId = 0x464f524d,				// 'FORM' 
	AiffType = 0x41494646,					// 'AIFF' 
	CommonId = 0x434f4d4d,					// 'COMM'
	ApplicationSpecificId = 0x4150504c,		// 'APPL'
	SosEnvelopesId = 0x534f5365,			// 'SOSe'
	SoundDataId = 0x53534e44,				// 'SSND'
};

typedef Int_32 ID;

struct CkHeader {
	Int_32 id;
	Uint_32 size;
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

// ---------------------------------------------------------------------------
//	AiffFile constructor from data on disk
// ---------------------------------------------------------------------------
//	Read immediately.
//
AiffFile::AiffFile( std::istream & s )
{
	if (! s)
		Throw( FileIOException, "Tried to read AIFF data from a bad stream." );

	read( s );
}

// ---------------------------------------------------------------------------
//	AiffFile constructor from filename
// ---------------------------------------------------------------------------
//	Read immediately.
//
AiffFile::AiffFile( const std::string & filename )
{
	std::ifstream s;
	s.open( filename.c_str(), std::ios::in | std::ios::binary ); 
	if (! s)
	{
		std::string err( "Could not open file for reading (maybe it doesn't exist?): " );
		err += filename;
		Throw( FileIOException, err );
	}
	read( s );
}

// ---------------------------------------------------------------------------
//	AiffFile constructor from data in memory
// ---------------------------------------------------------------------------
//	Called only by static export members, write immediately after constructing
//	this way.
//
AiffFile::AiffFile( double rate, int chans, int bits ) :
	_sampleRate( rate ),
	_nChannels( chans ),
	_sampSize( bits )
{
}

// ---------------------------------------------------------------------------
//	channels
// ---------------------------------------------------------------------------
//
int
AiffFile::channels( void ) const
{
	return _nChannels;
}

// ---------------------------------------------------------------------------
//	sampleFrames
// ---------------------------------------------------------------------------
//
unsigned long
AiffFile::sampleFrames( void ) const
{
	const int bytesPerFrame = ( sampleSize() / 8 ) * channels();
	return _bytes.size() / bytesPerFrame;
}

// ---------------------------------------------------------------------------
//	sampleRate
// ---------------------------------------------------------------------------
//
double
AiffFile::sampleRate( void ) const
{
	return _sampleRate;
}

// ---------------------------------------------------------------------------
//	sampleSize
// ---------------------------------------------------------------------------
//
int
AiffFile::sampleSize( void ) const
{
	return _sampSize;
}

// ---------------------------------------------------------------------------
//	partials
// ---------------------------------------------------------------------------
//
int
AiffFile::partials( void ) const
{
	return _partials;
}

// ---------------------------------------------------------------------------
//	enhanced
// ---------------------------------------------------------------------------
//
int
AiffFile::enhanced( void ) const
{
	return _enhanced;
}

// ---------------------------------------------------------------------------
//	hop
// ---------------------------------------------------------------------------
//
double
AiffFile::hop( void ) const
{
	return _hop;
}

// ---------------------------------------------------------------------------
//	getSamples
// ---------------------------------------------------------------------------
//	Once the bytes have been read from disk and the sample size is known,
//	clients can request the samples as doubles; convert the raw sample data 
//	to doubles and stored them in the range provided.
//
void
AiffFile::getSamples( double * bufBegin, double * bufEnd )
{
	//	adjust bufEnd if necessary:
	if ( bufEnd - bufBegin > sampleFrames() * channels() )
		bufEnd = bufBegin + (sampleFrames() * channels());

	debugger << "converting " << bufEnd - bufBegin << " samples of size " << sampleSize() << endl;

	//	shift sample bytes into a long integer, and scale 
	//	to make a double:
	static const double oneOverMax = 1. / LONG_MAX;	//	defined in climits
	long samp;
	std::vector<unsigned char>::size_type bytesIdx = 0;
	switch ( _sampSize ) 
	{
		case 32:
		{	
			while( bufBegin < bufEnd )
			{
				samp = (_bytes[bytesIdx++] << 24);
				samp += (_bytes[bytesIdx++] << 16); 
				samp += (_bytes[bytesIdx++] << 8); 
				samp += _bytes[bytesIdx++];
				*bufBegin = oneOverMax * samp; 
				
				++bufBegin;
			}
			break;
		}
		case 24:
		{
			while( bufBegin < bufEnd )
			{
				samp = (_bytes[bytesIdx++] << 24);
				samp += (_bytes[bytesIdx++] << 16); 
				samp += (_bytes[bytesIdx++] << 8); 
				*bufBegin = oneOverMax * samp; 
				
				++bufBegin;
			}
			break;
		}
		case 16:
		{
			while( bufBegin < bufEnd )
			{
				samp = (_bytes[bytesIdx++] << 24); 
				samp += (_bytes[bytesIdx++] << 16);
				*bufBegin = oneOverMax * samp; 
				
				++bufBegin;
			}
			break;
		}
		case 8:
		{
			while( bufBegin < bufEnd )
			{
				samp = (_bytes[bytesIdx++] << 24);
				*bufBegin = oneOverMax * samp; 
				
				++bufBegin;
			}
			break;
		}
	}
}

// ---------------------------------------------------------------------------
//	read
// ---------------------------------------------------------------------------
//
void
AiffFile::read( std::istream & s )
{
	try 
	{
		//	the Container chunk must be first, read it:
		readContainer( s );
		
		//	read other chunks, we are only interested in
		//	the Common chunk, the Sound Data chunk, and SPC-specific chunk:
		bool foundCOMM = false, foundSSND = false;
		_hop = _partials = _enhanced = 0;
		while ( ! foundCOMM || ! foundSSND )
		{
			if ( s.eof() )
				Throw( FileIOException, 
					   "Reached end of file before finding both a Common chunk and a Sound Data chunk." );
			
			//	read a chunk header, if it isn't the one we want, skip over it.
			CkHeader h;
			readChunkHeader( s, h );
			
			if ( h.id == CommonId )
			{
				readCommonData( s );
				foundCOMM = true;
			}
			else if ( h.id == ApplicationSpecificId )
			{
				readApplicationSpecifcData( s, h.size );
			}
			else if ( h.id == SoundDataId )
			{
				readSampleData( s, h.size );
				foundSSND = true;
			}
			else
			{
				s.ignore( h.size );
			}
		}
	}
	catch ( Exception & ex ) 
	{
		ex.append( " Failed to read AIFF file." );
		throw;
	}
}

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
//	readApplicationSpecifcData
// ---------------------------------------------------------------------------
//	Read the data in the ApplicationSpecific chunk, assume the stream is correctly
//	positioned, and that the chunk header has already been read.
//  Look for data specific to SPC files.
//
void
AiffFile::readApplicationSpecifcData( std::istream & s, int length )
{
	Int_32 signature, enhanced, partials, resolution;
	try 
	{
		BigEndian::read( s, 1, sizeof(Int_32), (char *)&signature );
		
		if ( signature == SosEnvelopesId )
		{
			BigEndian::read( s, 1, sizeof(Int_32), (char *)&enhanced );
			BigEndian::read( s, 1, sizeof(Int_32), (char *)&partials );
			s.ignore( partials * sizeof(Int_32) );
			BigEndian::read( s, 1, sizeof(Int_32), (char *)&resolution );
			s.ignore( length - (4 + partials) * sizeof(Int_32) );
			
			_enhanced = enhanced;
			_partials = partials;
			_hop = resolution * 0.000001;	// resolution is in microseconds
		}
		else
		{
			s.ignore( length - sizeof(Int_32) );
		}
	}
	catch( FileIOException & ex ) 
	{
		ex.append( " Failed to read badly-formatted AIFF file (bad ApplicationSpecific chunk)." );
		throw;
	}
}

// ---------------------------------------------------------------------------
//	readCommonData
// ---------------------------------------------------------------------------
//	Read the data in the Common chunk, assume the stream is correctly
//	positioned, and that the chunk header has already been read.
//
void
AiffFile::readCommonData( std::istream & s )
{
	CommonCk ck;
	try 
	{
		BigEndian::read( s, 1, sizeof(Int_16), (char *)&ck.channels );
		BigEndian::read( s, 1, sizeof(Int_32), (char *)&ck.sampleFrames );
		BigEndian::read( s, 1, sizeof(Int_16), (char *)&ck.bitsPerSample );
		//	don't let this get byte-reversed:
		BigEndian::read( s, 10, sizeof(char), (char *)&ck.srate );
	}
	catch( FileIOException & ex ) 
	{
		ex.append( " Failed to read badly-formatted AIFF file (bad Common chunk)." );
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
AiffFile::readContainer( std::istream & s )
{
	ContainerCk ck;		
	try 
	{
		// Container is always first:
		readChunkHeader( s, ck.header );
		if( ck.header.id != ContainerId )
			Throw( FileIOException, "Found no Container chunk." );
			
		//	read in the chunk data:
		BigEndian::read( s, 1, sizeof(ID), (char *)&ck.formType );
	}
	catch( FileIOException & ex ) 
	{
		ex.append( " Failed to read badly-formatted AIFF file (bad Container chunk)." );
		throw;
	}

	//	make sure its really AIFF:
	if ( ck.formType != AiffType )
		Throw( FileIOException, std::string("Bad form type in AIFF file: ") + std::string( (long)ck.formType, 4 ) );
}	

// ---------------------------------------------------------------------------
//	readSampleData
// ---------------------------------------------------------------------------
//	Read the data in the Sound Data chunk, assume the stream is correctly
//	positioned, and that the chunk header has already been read.
//
void
AiffFile::readSampleData( std::istream & s, unsigned long chunkSize )
{
	SoundDataCk ck;
	try 
	{
		BigEndian::read( s, 1, sizeof(Uint_32), (char *)&ck.offset );
		BigEndian::read( s, 1, sizeof(Uint_32), (char *)&ck.blockSize );
		
		//	compute the actual number of bytes that
		//	can be read from this chunk:
		//	(chunkSize is everything after the header)
		const unsigned long howManyBytes = 
			( chunkSize - ck.offset ) - (2 * sizeof(Uint_32));
		_bytes.resize( howManyBytes, 0 );		//	could throw bad_alloc

		//	skip ahead to the samples and read them:
		s.ignore( ck.offset );
		readSamples( s );
	}
	catch( FileIOException & ex ) 
	{
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
AiffFile::readSamples( std::istream & s )
{	
	debugger << "reading " << _bytes.size() << " bytes of sample data" << endl;

	//	read integer samples without byte swapping: 
	BigEndian::read( s, _bytes.size(), 1, (char*)(&_bytes[0]) );
}

// ---------------------------------------------------------------------------
//	write
// ---------------------------------------------------------------------------
//
void
AiffFile::write( const std::string & filename, const double * bufBegin, const double * bufEnd )
{
	std::ofstream s;
	s.open( filename.c_str(), std::ios::out | std::ios::binary ); 
	write( s, bufBegin, bufEnd );
}

// ---------------------------------------------------------------------------
//	write
// ---------------------------------------------------------------------------
//
void
AiffFile::write( std::ostream & s, const double * bufBegin, const double * bufEnd )
{
	validateParams();
	
	try 
	{
		//	allocate bytes buffer, used to determine data 
		//	size and number of samples (must be allocated
		//	before writing chunks, therefore):
		//	(could throw bad_alloc)
		//	AIFF requires that chunks be an even number of
		//	bytes, even if the actual sample data is an
		//	odd number of bytes. 
		std::vector< double >:: size_type bufferSize = 
			(bufEnd - bufBegin) * (sampleSize() / 8);
		if ( bufferSize % 2 )
			++bufferSize;
		_bytes.resize( bufferSize, 0 );	
		// debugger << "resized output buffer to " << _bytes.size() << " bytes" << endl;
		
		writeContainer( s );
		writeCommon( s );
		writeSampleData( s, bufBegin, bufEnd );
	}
	catch ( Exception & ex ) 
	{
		ex.append( " Failed to write AIFF file." );
		throw;
	}
	catch( std::bad_alloc & ex )
	{
		std::string s("std C++ exception bad_alloc (memory allocation error) in AiffFile::write(): " );
		s.append( ex.what() );
		s.append( " Failed to write AIFF file." );
		Throw( Exception, s );
	}
}

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
					
	ck.channels = channels();
	ck.sampleFrames = sampleFrames();
	ck.bitsPerSample = sampleSize();
	IEEE::ConvertToIeeeExtended( sampleRate(), & ck.srate );

/*
	debugger << "writing common chunk: " << endl;
	debugger << "header id: " << ck.header.id << endl;
	debugger << "size: " << ck.header.size << endl;
	debugger << "channels: " << ck.channels << endl;
	debugger << "sample frames: " << ck.sampleFrames << endl;
	debugger << "bits per sample: " << ck.bitsPerSample << endl;
	debugger << "rate: " << _sampleRate  << endl;
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
AiffFile::writeSampleData( std::ostream & s, const double * bufBegin, const double * bufEnd )
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

		writeSamples( s, bufBegin, bufEnd );
	}
	catch( FileIOException & ex ) 
	{
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
AiffFile::writeSamples( std::ostream & s, const double * bufBegin, const double * bufEnd )
{	
	debugger << "writing " << bufEnd - bufBegin << " samples of size " << sampleSize() << endl;

	//	convert doubles to long integers, and 
	//	then into big endian integer samples:
	long samp;
	std::vector<unsigned char>::size_type bytesIdx = 0;
	switch ( sampleSize() ) 
	{
		case 32:
		{	
			while ( bufBegin < bufEnd )
			{
				samp = long( LONG_MAX * std::min( 1.0, std::max(-1.0, *bufBegin) ) );
				
				_bytes[bytesIdx++] = (samp >> 24) & 0xFF;		//	msb
				_bytes[bytesIdx++] = (samp >> 16) & 0xFF;
				_bytes[bytesIdx++] = (samp >> 8) & 0xFF;
				_bytes[bytesIdx++] = samp & 0xFF;			//	lsb
				
				++bufBegin;
			}
			break;
		}
		case 24:
		{	
			while ( bufBegin < bufEnd )
			{
				samp = long( LONG_MAX * std::min( 1.0, std::max(-1.0, *bufBegin) ) );
				
				_bytes[bytesIdx++] = (samp >> 24) & 0xFF;		//	msb
				_bytes[bytesIdx++] = (samp >> 16) & 0xFF;
				_bytes[bytesIdx++] = (samp >> 8) & 0xFF;		//	lsb
				
				++bufBegin;
			}
			break;
		}
		case 16:
		{	
			while ( bufBegin < bufEnd )
			{
				samp = long( LONG_MAX * std::min( 1.0, std::max(-1.0, *bufBegin) ) );
				
				_bytes[bytesIdx++] = (samp >> 24) & 0xFF;		//	msb
				_bytes[bytesIdx++] = (samp >> 16) & 0xFF;		//	lsb
				
				++bufBegin;
			}
			break;
		}
		case 8:
		{	
			while ( bufBegin < bufEnd )
			{
				samp = long( LONG_MAX * std::min( 1.0, std::max(-1.0, *bufBegin) ) );
				
				_bytes[bytesIdx++] = (samp >> 24) & 0xFF;		//	msb
				
				++bufBegin;
			}
			break;
		}
	}
	
	//	write integer samples without byte swapping: 
	BigEndian::write( s, _bytes.size(), 1, (char*)(&_bytes[0]) );
}

// ---------------------------------------------------------------------------
//	sizeofCkHeader
// ---------------------------------------------------------------------------
//
unsigned long
AiffFile::sizeofCkHeader( void )
{
	return	sizeof(Int_32) +	//	id
			sizeof(Uint_32);	//	size
}

// ---------------------------------------------------------------------------
//	sizeofCommon
// ---------------------------------------------------------------------------
//
unsigned long
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
unsigned long
AiffFile::sizeofSoundData( void )
{
	Uint_32 dataSize = _bytes.size();
	//	must be an even number of bytes:
	if ( dataSize % 2 ) 
	{
		++dataSize;
	}

	return	sizeof(Int_32) +	//	id
			sizeof(Uint_32) +	//	size
			sizeof(Uint_32) + 	//	offset
			sizeof(Uint_32) + 	//	block size
			dataSize;			//	sample data
}

// ---------------------------------------------------------------------------
//	validateParams
// ---------------------------------------------------------------------------
//	Throw InvalidObject exception if the parameters are not valid.
//
void
AiffFile::validateParams( void )
{	
	if ( _sampleRate < 0. )
		Throw( InvalidObject, "Bad sample rate in AiffFile export." );
	
	static const int validChannels[] = { 1, 2, 4 };
	if (! std::find( validChannels, validChannels + 3, _nChannels ) )
		Throw( InvalidObject, "Bad number of channels in AiffFile export." );
	
	static const int validSizes[] = { 8, 16, 24, 32 };
	if (! std::find( validSizes, validSizes + 4, _sampSize ) )
		Throw( InvalidObject, "Bad sample size in AiffFile export." );
	
}

}	//	end of namespace Loris

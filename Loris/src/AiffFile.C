/*
 * Copyright (c) 1999-2000 Kelly Fitz and Lippold Haken
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
#include "AiffFile.h"
#include "Endian.h"
#include "Exception.h"
#include "notifier.h"
#include "ieee.h"
#include "LorisTypes.h"
#include <algorithm>
#include <string>
#include <fstream>
#include <climits>

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

// ---------------------------------------------------------------------------
//	AiffFile constructor from data in memory
// ---------------------------------------------------------------------------
//
AiffFile::AiffFile( double rate, int chans, int bits, std::vector< double > & buf ) :
	_sampleRate( rate ),
	_nChannels( chans ),
	_sampSize( bits ),
	_samples( buf )
{
	validateParams();
}

// ---------------------------------------------------------------------------
//	AiffFile constructor from data on disk
// ---------------------------------------------------------------------------
//	Read immediately.
//
AiffFile::AiffFile( std::istream & s, std::vector< double > & buf ) :
	_sampleRate( 1 ),
	_nChannels( 1 ),
	_sampSize( 1 ),
	_samples( buf )
{
	read( s );
}

// ---------------------------------------------------------------------------
//	AiffFile constructor from filename
// ---------------------------------------------------------------------------
//	Read immediately.
//
AiffFile::AiffFile( const std::string & filename, std::vector< double > & buf ) :
	_sampleRate( 1 ),
	_nChannels( 1 ),
	_sampSize( 1 ),
	_samples( buf )
{
	read( filename );
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
//	THIS WON'T WORK IF CHUNKS ARE IN A DIFFERENT ORDER!!!
//
void
AiffFile::read( std::istream & s )
{
	try 
	{
		//	the Container chunk must be first, read it:
		readContainer( s );
		
		//	read other chunks, we are only interested in
		//	the Common chunk and the Sound Data chunk:
		bool foundCOMM = false, foundSSND = false;
		while ( ! foundCOMM || ! foundSSND )
		{
			//	read a chunk header, if it isn't the one we want, skip over it.
			CkHeader h;
			readChunkHeader( s, h );
			
			if ( h.id == CommonId )
			{
				readCommonData( s );
				foundCOMM = true;
			}
			else if ( h.id == SoundDataId )
			{
				if (! foundCOMM) 	//	 I hope this doesn't happen
				{
					Throw( FileIOException, 
							"Mia culpa! I am not smart enough to read the Sound data before the Common data." );
				}							
				readSampleData( s );
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
		if ( s.eof() )
		{
			ex.append("Reached end of file before finding both a Common chunk and a Sound Data chunk.");
		}
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
	
	try 
	{
		writeContainer( s );
		writeCommon( s );
		writeSampleData( s );
	}
	catch ( Exception & ex ) 
	{
		ex.append( "Failed to write AIFF file." );
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
//	Read the data in the Sound Data chunk, assume the stream is correctly
//	positioned, and that the chunk header has already been read.
//
void
AiffFile::readSampleData( std::istream & s )
{
	SoundDataCk ck;
	try 
	{
		BigEndian::read( s, 1, sizeof(Uint_32), (char *)&ck.offset );
		BigEndian::read( s, 1, sizeof(Uint_32), (char *)&ck.blockSize );

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
//	Can do lots better by reading all the samples into a buffer at once.
//
void
AiffFile::readSamples( std::istream & s )
{	
	//	use a vector for automatic temporary storage:
	std::vector<unsigned char> v( _samples.size() * (_sampSize / 8) );

	//	read integer samples without byte swapping: 
	BigEndian::read( s, _samples.size() * _sampSize / 8, 1, (char*)v.begin() );

	//	shift sample bytes into a long integer, and scale 
	//	to make a double:
	static const double oneOverMax = 1. / LONG_MAX;	//	defined in climits
	long samp;
	switch ( _sampSize ) 
	{
		case 32:
		{	
			for (long i = 0; i < v.size(); i += 4 ) 
			{
				samp = (v[i] << 24) + (v[i+1] << 16) + (v[i+2] << 8) + v[i+3];
				_samples[i/4] = oneOverMax * samp; 
			}
			break;
		}
		case 24:
		{
			for (long i = 0; i < v.size(); i += 3 ) 
			{
				samp = (v[i] << 24) + (v[i+1] << 16) + (v[i+2] << 8);
				_samples[i/3] = oneOverMax * samp; 
			}
			break;
		}
		case 16:
		{
			for (long i = 0; i < v.size(); i += 2 ) 
			{
				samp = (v[i] << 24) + (v[i+1] << 16);
				_samples[i/2] = oneOverMax * samp; 
			}
			break;
		}
		case 8:
		{
			for (long i = 0; i < v.size(); ++i ) 
			{
				samp = (v[i] << 24);
				_samples[i] = oneOverMax * samp; 
			}
			break;
		}
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

	//	use a vector for automatic temporary storage:
	std::vector<unsigned char> v( _samples.size() * (_sampSize / 8) );

	//	convert doubles to long integers, and 
	//	then into big endian integer samples:
	long samp;
	switch ( _sampSize ) 
	{
		case 32:
		{	
			for (long i = 0; i < v.size(); i += 4 ) 
			{
				samp = LONG_MAX * std::min( 1.0, std::max(-1.0, _samples[i/4]) );
				v[i] = (samp >> 24) & 0xFF;		//	msb
				v[i+1] = (samp >> 16) & 0xFF;
				v[i+2] = (samp >> 8) & 0xFF;
				v[i+3] = samp & 0xFF;			//	lsb
			}
			break;
		}
		case 24:
		{	
			for (long i = 0; i < v.size(); i += 3 ) 
			{
				samp = LONG_MAX * std::min( 1.0, std::max(-1.0, _samples[i/3]) );
				v[i] = (samp >> 24) & 0xFF;		//	msb
				v[i+1] = (samp >> 16) & 0xFF;
				v[i+2] = (samp >> 8) & 0xFF;	//	lsb
			}
			break;
		}
		case 16:
		{	
			for (long i = 0; i < v.size(); i += 2 ) 
			{
				samp = LONG_MAX * std::min( 1.0, std::max(-1.0, _samples[i/2]) );
				v[i] = (samp >> 24) & 0xFF;		//	msb
				v[i+1] = (samp >> 16) & 0xFF;	//	lsb
			}
			break;
		}
		case 8:
		{	
			for (long i = 0; i < v.size(); ++i ) 
			{
				samp = LONG_MAX * std::min( 1.0, std::max(-1.0, _samples[i]) );
				v[i] = (samp >> 24) & 0xFF;		//	msb
			}
			break;
		}
	}
	
	//	write integer samples without byte swapping: 
	BigEndian::write( s, _samples.size() * _sampSize / 8, 1, (char*)v.begin() );
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

// ---------------------------------------------------------------------------
//	validateParams
// ---------------------------------------------------------------------------
//	Throw InvalidObject exception if the parameters are not valid.
//
void
AiffFile::validateParams( void )
{	
	if ( _sampleRate < 0. )
		Throw( InvalidObject, "Bad sample rate in SamplesFile." );
	
	static const int validChannels[] = { 1, 2, 4 };
	if (! std::find( validChannels, validChannels + 3, _nChannels ) )
		Throw( InvalidObject, "Bad number of channels in SamplesFile." );
	
	static const int validSizes[] = { 8, 16, 24, 32 };
	if (! std::find( validSizes, validSizes + 4, _sampSize ) )
		Throw( InvalidObject, "Bad sample size in SamplesFile." );
	
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

// ===========================================================================
//	AiffFile.C
//	
//
//
//	-kel 28 Sept 99
//
// ===========================================================================

#include "LorisLib.h"
#include "AiffFile.h"
#include "SampleBuffer.h"
#include "BinaryFile.h"

#include "ieee.h"

#include <algorithm>


Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	AiffFile constructor
// ---------------------------------------------------------------------------
//
AiffFile::AiffFile( double rate, int chans, int bits, SampleBuffer & buf, BinaryFile & file ) :
	_sampleRate( rate ),
	_nChannels( chans ),
	_sampSize( bits ),
	_samples( buf ),
	_file( file )
{
	validateParams();
}

// ---------------------------------------------------------------------------
//	write
// ---------------------------------------------------------------------------
//
void
AiffFile::write( void )
{
	validateParams();
	
	try {
		writeContainer();
		writeCommon();
		writeSampleData();
	}
	catch ( Exception & ex ) {
		ex << "Failed to write AIFF file ";
		throw;
	}
}


#pragma mark -
#pragma mark helpers

// ---------------------------------------------------------------------------
//	validateParams
// ---------------------------------------------------------------------------
//	Throw InvalidObject exception if the parameters are not valid.
//
void
AiffFile::validateParams( void )
{
	using std::find;
	
	if ( _sampleRate < 0. )
		Throw( InvalidObject, "Bad sample rate in AiffFile." );
	
	static const int validChannels[] = { 1, 2, 4 };
	if (! find( validChannels, validChannels + 3, _nChannels ) )
		Throw( InvalidObject, "Bad number of channels in AiffFile." );
	
	static const int validSizes[] = { 8, 16, 24, 32 };
	if (! find( validSizes, validSizes + 4, _sampSize ) )
		Throw( InvalidObject, "Bad sample size in AiffFile." );
	
}

// ---------------------------------------------------------------------------
//	writeCommon
// ---------------------------------------------------------------------------
//
void
AiffFile::writeCommon( void )
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
	
	//	write it out:
	_file.write( ck.header.id );
	_file.write( ck.header.size );
	_file.write( ck.channels );
	_file.write( ck.sampleFrames );
	_file.write( ck.bitsPerSample );
	_file.write( ck.srate, false );	//	don't swap byte order, ever
}

// ---------------------------------------------------------------------------
//	writeContainer
// ---------------------------------------------------------------------------
//
void
AiffFile::writeContainer( void )
{
	//	first build a Container chunk, so that all the data sizes will 
	//	be correct:
	ContainerCk ck;
	ck.header.id = ContainerId;
	
	//	size is everything after the header:
	ck.header.size = sizeof(Int_32) + sizeofCommon() + sizeofSoundData();
	
	ck.formType = AiffType;
	
	//	write it out:
	_file.write( ck.header.id );
	_file.write( ck.header.size );
	_file.write( ck.formType );
}	

// ---------------------------------------------------------------------------
//	writeSampleData
// ---------------------------------------------------------------------------
//
void
AiffFile::writeSampleData( void )
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
	
	//	write it out:
	_file.write( ck.header.id );
	_file.write( ck.header.size );
	_file.write( ck.offset );
	_file.write( ck.blockSize );
	
	writeSamples();
}

// ---------------------------------------------------------------------------
//	writeSamples
// ---------------------------------------------------------------------------
//
void
AiffFile::writeSamples( void )
{	
	using std::max;
	using std::min;
	
	static const double Maximum_Long = (double) LONG_MAX;
	
	union {
		//	32 bits sample
		Int_32 s32bits;
		//	24 bits sample
		struct {
			char data[3];
		}  s24bits;
		//	16 bits sample
		Int_16 s16bits;
		//	8 bits sample
		char s8bits;
	} z;

	switch ( _sampSize ) {
		case 32:
			for (ulong i = 0; i < _samples.size(); ++i ) {
				//	convert to integer (clip instead of wrapping):
				z.s32bits = Maximum_Long * min( 1.0, max(-1.0, _samples[i]) );
			
				//	write the sample:
				_file.write( z.s32bits );
			}
			break;
		case 24:
			for (ulong i = 0; i < _samples.size(); ++i ) {
				//	convert to integer (clip instead of wrapping):
				z.s32bits = Maximum_Long * min( 1.0, max(-1.0, _samples[i]) );
			
				//	write the sample:
				_file.write( z.s24bits );
			}
			break;
		case 16:
			for (ulong i = 0; i < _samples.size(); ++i ) {
				//	convert to integer (clip instead of wrapping):
				z.s32bits = Maximum_Long * min( 1.0, max(-1.0, _samples[i]) );
			
				//	write the sample:
				_file.write( z.s16bits );
			}
			break;
		case 8:
			for (ulong i = 0; i < _samples.size(); ++i ) {
				//	convert to integer (clip instead of wrapping):
				z.s32bits = Maximum_Long * min( 1.0, max(-1.0, _samples[i]) );
			
				//	write the sample:
				_file.write( z.s8bits );
			}
			break;
	}
}

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
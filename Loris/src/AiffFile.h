#ifndef __Loris_aiff_file__
#define __Loris_aiff_file__

// ===========================================================================
//	AiffFile.h
//	
//
//
//	-kel 28 Sept 99
//
// ===========================================================================

#include "LorisLib.h"
#include "ieee.h"

Begin_Namespace( Loris )

class SampleBuffer;
class File;

// ---------------------------------------------------------------------------
//	€ class AiffFile
//
//	The SampleBuffer must be provided by clients; it is not owned by 
//	the AiffFile.
//	
class AiffFile
{
public:
//	construction:
	AiffFile( Double rate, Int chans, Int bits, SampleBuffer & buf, File & file );
	
//	defaults are okay for these, but don't subclass
	//	AiffFile( const AiffFile & other );
	//	~AiffFile( void );
	
//	access/mutation:
	Double sampleRate( void ) const { return _sampleRate; }
	Int numChans( void ) const { return _nChannels; }
	Int sampleSize( void ) const { return _sampSize; }
	
	void setSampleRate( Double x ) { _sampleRate = x; }
	void setNumChannels( Int n ) { _nChannels = n; }
	void setSampleSize( Int n ) { _sampSize = n; }
	
	SampleBuffer & samples( void ) { return _samples; }
	const SampleBuffer & samples( void ) const { return _samples; }
	
//	reading and writing:
	void write( void );
	
//	-- chunk types --
private:
	enum { 
		ContainerId = 'FORM', 
		AiffType = 'AIFF', 
		CommonId = 'COMM',
		SoundDataId = 'SSND'
	};
	
	struct CkHeader {
		Int_32 id;
		Uint_32 size;
	};
	
	struct ContainerCk
	{
		CkHeader header;
		Int_32 formType;
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
	
//	-- helpers --
	//	reading:
	void readCommon( CommonCk & );
	void readContainer( ContainerCk & );
	void readSampleData( SoundDataCk & );
	
	//	writing:
	void writeCommon( void );
	void writeContainer( void );
	void writeSampleData( void );
	void writeSamples( void );
	
	//	data sizes:
	Uint_32 sizeofCommon( void );
	Uint_32 sizeofCkHeader( void );
	Uint_32 sizeofSoundData( void );

	//	checking parameters:
	void validateParams( void );
	
//	-- instance variables --
	Double _sampleRate;	//	in Hz
	Int _nChannels;		//	samples per frame, usually one (mono) in Loris
	Int _sampSize;		//	in bits
	
	SampleBuffer & _samples;
	File & _file;
	
};	//	end of class AiffFile

End_Namespace( Loris )

#endif //	ndef __Loris_aiff_file__
#ifndef __Loris_samples_file__
#define __Loris_samples_file__

// ===========================================================================
//	SamplesFile.h
//	
//	Association of a sample buffer and the necessary additional info 
//	(sample rate, number of channels, and sample data size in bits)
//	to completely specify a sampled sound.
//
//	-kel 6 Oct 99
//
// ===========================================================================

#include "LorisTypes.h"

#include <vector>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif


class BinaryFile;

// ---------------------------------------------------------------------------
//	class SamplesFile
//
//	Base class for different flavors of samples files. Derived classes
//	must implement i/o. 
//
//	The SampleBuffer must be provided by clients; it is not owned by 
//	the AiffFile.
//	
class SamplesFile
{
protected:
//	construction:
	SamplesFile( double rate, int chans, int bits, std::vector< double > & buf );
	SamplesFile( std::vector< double > & buf );
	SamplesFile( const SamplesFile & other );
	
public:
//	virtual destructor, to allow subclassing:
	virtual ~SamplesFile( void ) {}
	
//	access/mutation:
	double sampleRate( void ) const { return _sampleRate; }
	int numChans( void ) const { return _nChannels; }
	int sampleSize( void ) const { return _sampSize; }
	
	void setSampleRate( double x ) { _sampleRate = x; }
	void setNumChannels( int n ) { _nChannels = n; }
	void setSampleSize( int n ) { _sampSize = n; }
	
	std::vector< double > & samples( void ) { return _samples; }
	const std::vector< double > & samples( void ) const { return _samples; }
	
//	reading and writing:
	virtual void read( std::istream & s ) = 0;
	virtual void write( BinaryFile & file ) = 0;
	
//	parameter validation:
protected:	
	void validateParams( void );
	
//	data type for integer pcm samples of different sizes:
	typedef union {
		//	different size samples:
		Int_32 s32bits;						//	32 bits sample
		struct I_24 { char data[3]; }  s24bits;	//	24 bits sample
		Int_16 s16bits;						//	16 bits sample
		char s8bits;						//	8 bits sample
	} pcm_sample;

	
//	-- instance variables --
	double _sampleRate;	//	in Hz
	int _nChannels;		//	samples per frame, usually one (mono) in Loris
	int _sampSize;		//	in bits
	
	std::vector< double > & _samples;
	
};	//	end of class SamplesFile

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif //	ndef __Loris_samples_file__

#ifndef INCLUDE_AIFFFILE_H
#define INCLUDE_AIFFFILE_H
// ===========================================================================
//	AiffFile.h
//	
//	Association of a sample buffer and the necessary additional info 
//	(sample rate, number of channels, and sample data size in bits)
//	to completely specify an AIFF samples file.
//
//	-kel 28 Sept 99
//
// ===========================================================================
#include <vector>
#include <string>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

struct CkHeader;

// ---------------------------------------------------------------------------
//	class AiffFile
//
//	The SampleBuffer must be provided by clients; it is not owned by 
//	the AiffFile.
//	
class AiffFile
{
//	-- instance variables --
	double _sampleRate;	//	in Hz
	int _nChannels;		//	samples per frame, usually one (mono) in Loris
	int _sampSize;		//	in bits
	
	std::vector< double > & _samples;
	
//	-- public interface --
public:
//	construction:
//	(compiler can generate destructor, copy, and assignment)
	AiffFile( double rate, int chans, int bits, std::vector< double > & buf );
	AiffFile( const std::string & filename, std::vector< double > & buf );
	AiffFile( std::istream & s, std::vector< double > & buf );
	
//	reading and writing:
	void read( const std::string & filename );
	void read( std::istream & s );
	void write( const std::string & filename );
	void write( std::ostream & s );
	
//	access/mutation:
	double sampleRate( void ) const { return _sampleRate; }
	int numChans( void ) const { return _nChannels; }
	int sampleSize( void ) const { return _sampSize; }
	
	void setSampleRate( double x ) { _sampleRate = x; }
	void setNumChannels( int n ) { _nChannels = n; }
	void setSampleSize( int n ) { _sampSize = n; }
	
	std::vector< double > & samples( void ) { return _samples; }
	const std::vector< double > & samples( void ) const { return _samples; }
	
//	-- helpers --
private:
	//	reading:
	void readChunkHeader( std::istream & s, CkHeader & h );
	void readCommon( std::istream & s );
	void readContainer( std::istream & s );
	void readSampleData( std::istream & s );
	void readSamples( std::istream & s );

	//	writing:
	void writeCommon( std::ostream & s );
	void writeContainer( std::ostream & s );
	void writeSampleData( std::ostream & s );
	void writeSamples( std::ostream & s );
	
	//	data sizes:
	unsigned long sizeofCommon( void );
	unsigned long sizeofCkHeader( void );
	unsigned long sizeofSoundData( void );

	//	parameter validation:
	void validateParams( void );

};	//	end of class AiffFile

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif //	ndef INCLUDE_AIFFFILE_H


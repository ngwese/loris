#ifndef __Loris_aiff_file__
#define __Loris_aiff_file__

// ===========================================================================
//	AiffFile.h
//	
//	Association of a sample buffer and the necessary additional info 
//	(sample rate, number of channels, and sample data size in bits)
//	to completely specify an AIFF samples file. Extends the generic
//	Loris::SamplesFile with AIFF i/o.
//
//	-kel 28 Sept 99
//
// ===========================================================================

#include "LorisTypes.h"
#include "ieee.h"
#include "SamplesFile.h"
#include <vector>
#include <string>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

struct CkHeader;
class BinaryFile;

// ---------------------------------------------------------------------------
//	class AiffFile
//
//	The SampleBuffer must be provided by clients; it is not owned by 
//	the AiffFile.
//	
class AiffFile : public SamplesFile
{
public:
//	construction:
	AiffFile( double rate, int chans, int bits, std::vector< double > & buf );
	AiffFile( const std::string & filename, std::vector< double > & buf );
	AiffFile( std::istream & s, std::vector< double > & buf );
	
	AiffFile( const SamplesFile & other );
	
//	compiler-generated destructor is okay:
	//	~AiffFile( void );
	
//	reading and writing:
	void read( const std::string & filename );
	void read( std::istream & s );
	void write( const std::string & filename );
	void write( BinaryFile & file );
	
//	-- helpers --
private:
	//	reading:
	void readChunkHeader( std::istream & s, CkHeader & h );
	void readCommon( std::istream & s );
	void readContainer( std::istream & s );
	void readSampleData( std::istream & s );
	void readSamples( std::istream & s );

	//	writing:
	void writeCommon( BinaryFile & file );
	void writeContainer( BinaryFile & file );
	void writeSampleData( BinaryFile & file );
	void writeSamples( BinaryFile & file );
	
	//	data sizes:
	Uint_32 sizeofCommon( void );
	Uint_32 sizeofCkHeader( void );
	Uint_32 sizeofSoundData( void );

};	//	end of class AiffFile

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif //	ndef __Loris_aiff_file__


// ===========================================================================
//	SamplesFile.C
//	
//	Association of a sample buffer and the necessary additional info 
//	(sample rate, number of channels, and sample data size in bits)
//	to completely specify a sampled sound.
//
//	Base class for different flavors of samples files. Derived classes
//	must implement i/o. 
//
//	-kel 6 Oct 99
//
// ===========================================================================

#include "SamplesFile.h"
#include "BinaryFile.h"
#include "Exception.h"

#include <algorithm>
#include <string>
#include <vector>

using namespace std;

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif


// ---------------------------------------------------------------------------
//	SamplesFile constructor from data in memory
// ---------------------------------------------------------------------------
//
SamplesFile::SamplesFile( double rate, int chans, int bits, vector< double > & buf ) :
	_sampleRate( rate ),
	_nChannels( chans ),
	_sampSize( bits ),
	_samples( buf )
{
	validateParams();
}

// ---------------------------------------------------------------------------
//	SamplesFile constructor from data on disk
// ---------------------------------------------------------------------------
//	Derived classes have to do something intelligent here, this file is
//	bogus as-is. Invoke this constructor in the initialization of a 
//	derived object that is going to read immediately from a file.
//
SamplesFile::SamplesFile( vector< double > & buf ) :
	_sampleRate( 1 ),
	_nChannels( 1 ),
	_sampSize( 1 ),
	_samples( buf )
{
	//	read( file ); can't use pure virtual function here
}

// ---------------------------------------------------------------------------
//	SamplesFile copy constructor
// ---------------------------------------------------------------------------
//
SamplesFile::SamplesFile( const SamplesFile & other ) :
	_sampleRate( other._sampleRate ),
	_nChannels( other._nChannels ),
	_sampSize( other._sampSize ),
	_samples( other._samples )
{
}

// ---------------------------------------------------------------------------
//	validateParams
// ---------------------------------------------------------------------------
//	Throw InvalidObject exception if the parameters are not valid.
//
void
SamplesFile::validateParams( void )
{	
	if ( _sampleRate < 0. )
		Throw( InvalidObject, "Bad sample rate in SamplesFile." );
	
	static const int validChannels[] = { 1, 2, 4 };
	if (! find( validChannels, validChannels + 3, _nChannels ) )
		Throw( InvalidObject, "Bad number of channels in SamplesFile." );
	
	static const int validSizes[] = { 8, 16, 24, 32 };
	if (! find( validSizes, validSizes + 4, _sampSize ) )
		Throw( InvalidObject, "Bad sample size in SamplesFile." );
	
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

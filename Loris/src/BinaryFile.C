// ===========================================================================
//	BinaryFile.C
//	
//	Wrapper class for stream-based binary file access.
//
//	-kel 5 Oct 99
//
// ===========================================================================


#include "LorisLib.h"
#include "Exception.h"
#include "Notifier.h"
#include "BinaryFile.h"

#include <fstream>
#include <string>
#include <algorithm>

using namespace std;	//	its everywhere in here

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	BinaryFile constructor
// ---------------------------------------------------------------------------
//	The EndianFlag defaults to BigEndian.
//
BinaryFile::BinaryFile( const string & path, ios::openmode flags, EndianFlag e ) :
	_swapBytes( e != machineType() ),
	_buf( new filebuf() )
{
	_buf->open( path.c_str(), ios::binary | flags );
	if ( ! _buf->is_open() ) {
		string s( "couldn't open File: " );
		s.append( path );
		Throw( FileIOException, s );
	}
}

// ---------------------------------------------------------------------------
//	BinaryFile destructor
// ---------------------------------------------------------------------------
//
BinaryFile::~BinaryFile( void )
{
	delete _buf;
}

// ---------------------------------------------------------------------------
//	BinaryFile Append
// ---------------------------------------------------------------------------
//
BinaryFile *
BinaryFile::Append( const string & path, EndianFlag e )
{
	return new BinaryFile(  path.c_str(), 
							ios::out | ios::app | ios::binary,
							e );
}

// ---------------------------------------------------------------------------
//	BinaryFile Create
// ---------------------------------------------------------------------------
//
BinaryFile *
BinaryFile::Create( const string & path, EndianFlag e )
{
	return new BinaryFile(  path.c_str(), 
							ios::in | ios::out | ios::trunc | ios::binary,
							e );
}

// ---------------------------------------------------------------------------
//	BinaryFile Open
// ---------------------------------------------------------------------------
//
BinaryFile *
BinaryFile::Open( const string & path, EndianFlag e, ios::openmode flags )
{
	return new BinaryFile(  path.c_str(), 
							flags | ios::binary,
							e );
}

// ---------------------------------------------------------------------------
//	position
// ---------------------------------------------------------------------------
//
streampos
BinaryFile::position( void ) const
{
	streampos p = _buf->pubseekoff( 0, ios::cur );
	//	check for invalid position:
	if ( p == streampos(-1)  )
		Throw( FileIOException, "Couldn't find valid binary file stream position.");

	return p;
}

// ---------------------------------------------------------------------------
//	skip
// ---------------------------------------------------------------------------
//
streampos
BinaryFile::skip( streamoff offset )
{
	streampos p = _buf->pubseekoff( offset, ios::cur );
	//	check for invalid position:
	if ( p == streampos(-1) )
		Throw( FileIOException, "Couldn't find valid binary file stream position.");

	return p;
}

// ---------------------------------------------------------------------------
//	setPosition
// ---------------------------------------------------------------------------
//
void
BinaryFile::setPosition( streampos pos )
{
	streampos p = _buf->pubseekpos( pos, ios::beg );
	//	check for invalid position:
	if ( p == streampos(-1) )
		Throw( FileIOException, "Couldn't find valid stream position in BinaryFile::position.");
}

// ---------------------------------------------------------------------------
//	reverseBytes
// ---------------------------------------------------------------------------
void
BinaryFile::reverseBytes( char * c, int n )
{
	if ( n > 1 )
		reverse( c, c + n );
}

// ---------------------------------------------------------------------------
//	readBytes
// ---------------------------------------------------------------------------
void
BinaryFile::readBytes( char * data, int howmany )
{
	_buf->sgetn( data, howmany );
}
	
// ---------------------------------------------------------------------------
//	writeBytes
// ---------------------------------------------------------------------------
void
BinaryFile::writeBytes( char * data, int howmany )
{
	_buf->sputn( data, howmany );
}
	
// ---------------------------------------------------------------------------
//	machineType
// ---------------------------------------------------------------------------
//	Returns the type of machine the program is running on.
//	Copied from arun's code, used to determine whether to 
//	swap bytes.
//
EndianFlag 
BinaryFile::machineType( void )
{
	union {
		short s ;
		char c[sizeof(short)] ;
	} x ;
	x.s = 1 ;
	static const EndianFlag f = ( x.c[0] == 1 ) ? LittleEndian : BigEndian ;
	
	return f;
}

End_Namespace( Loris )



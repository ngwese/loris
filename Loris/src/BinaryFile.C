// ===========================================================================
//	BinaryFile.C
//	
//	Wrapper class for stream-based binary file access.
//
//	-kel 5 Oct 99
//
// ===========================================================================
#include "Exception.h"
#include "notifier.h"
#include "BinaryFile.h"
#include "endian.h"
#include <string>

using namespace std;

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	BigEndian read
// ---------------------------------------------------------------------------
//
void
BigEndian::read( istream & s, int size, long howmany, char * putemHere )
{
	//	read the bytes into data:
	s.read( putemHere, howmany*size );
	
	//	check stream state:
	if ( ! s.good() )
		Throw( FileIOException, "Binary File read failed." );
	Assert( s.gcount() == howmany*size );
	
	//	swap byte order if nec.
	if ( ! bigEndianSystem() && size > 1 ) 
	{
		for ( long i = 0; i < howmany; +i )
		{
			swapByteOrder( putemHere + (i*size), size );
		}
	}
}


// ---------------------------------------------------------------------------
//	BinaryFile constructor
// ---------------------------------------------------------------------------
//
BinaryFile::BinaryFile( void ) :
	_prevOp( op_seek ),
	_swapBytes( false ),
	iostream( & _buf )
{
}

// ---------------------------------------------------------------------------
//	BinaryFile constructor with path
// ---------------------------------------------------------------------------
//	Try to find an approprite open mode for the file.
//	Try edit first, then view, then create. 
//
BinaryFile::BinaryFile( const string & path ) :
	_prevOp( op_seek ),
	_swapBytes( false ),
	iostream( & _buf )
{
	//	wow, is this ever ugly...
	try {
		edit( path );
	}
	catch ( FileIOException & ) {
		try { 
			view( path );
		}
		catch ( FileIOException & ) { 
			try {
				create( path );
			}
			catch ( FileIOException & ex ) { 
				ex.append( "Couldn't find a way to open " ).append( path );
				throw;
			}
		}
	}
}

#pragma mark -
#pragma mark file association
// ---------------------------------------------------------------------------
//	append
// ---------------------------------------------------------------------------
//	Open for appending, create if nec.
//
void
BinaryFile::append( const string & path )
{
	buffer().open( path.c_str(), ios::out | ios::app | ios::binary );
		
	if ( ! buffer().is_open() ) {
		string s( "Couldn't open BinaryFile: " );
		s.append( path );
		Throw( FileIOException, s );
	}
	_prevOp = op_seek;
}

// ---------------------------------------------------------------------------
//	create
// ---------------------------------------------------------------------------
//	Open for reading and writing, erase, create if nec.
//
void
BinaryFile::create( const string & path )
{
	buffer().open( path.c_str(), ios::in | ios::out | ios::trunc | ios::binary );
		
	if ( ! buffer().is_open() ) {
		string s( "Couldn't open BinaryFile: " );
		s.append( path );
		Throw( FileIOException, s );
	}
	_prevOp = op_seek;
}

// ---------------------------------------------------------------------------
//	edit
// ---------------------------------------------------------------------------
//	Open for reading and writing, must exist.
//
void
BinaryFile::edit( const string & path )
{
	buffer().open( path.c_str(), ios::in | ios::out | ios::binary );
		
	if ( ! buffer().is_open() ) {
		string s( "Couldn't open BinaryFile: " );
		s.append( path );
		Throw( FileIOException, s );
	}
	_prevOp = op_seek;
}

// ---------------------------------------------------------------------------
//	view
// ---------------------------------------------------------------------------
//	Open for reading only, must exist.
//
void
BinaryFile::view( const string & path )
{
	buffer().open( path.c_str(), ios::in | ios::binary );
		
	if ( ! buffer().is_open() ) {
		string s( "Couldn't open BinaryFile: " );
		s.append( path );
		Throw( FileIOException, s );
	}
	_prevOp = op_seek;
}

// ---------------------------------------------------------------------------
//	close
// ---------------------------------------------------------------------------
//	Why not? Copied from streams in <fstream>.
//
void
BinaryFile::close( void )
{
	if ( buffer().close() == 0)
		setstate(failbit);
	_prevOp = op_seek;
}

#pragma mark -
#pragma mark file stream position
// ---------------------------------------------------------------------------
//	position
// ---------------------------------------------------------------------------
//
streampos
BinaryFile::tell( void )
{
	return tellp();
}

// ---------------------------------------------------------------------------
//	seek
// ---------------------------------------------------------------------------
//
void
BinaryFile::seek( streampos x )
{
	//if ( eof() )	//	this makes sense, I think, 
	//	clear();	//	but not consistent with standard lib
	seekp( x );
	_prevOp = op_seek;
}

// ---------------------------------------------------------------------------
//	offset
// ---------------------------------------------------------------------------
//
void
BinaryFile::offset( long x, ios::seekdir whence )
{
	//if ( eof() )	//	this makes sense, I think, 
	//	clear();	//	but not consistent with standard lib
	seekp( x, whence ); 
	_prevOp = op_seek;
}

#pragma mark -
#pragma mark endian
// ---------------------------------------------------------------------------
//	setBigEndian
// ---------------------------------------------------------------------------
//
void
BinaryFile::setBigEndian( void )
{
	_swapBytes = ! bigEndianSystem();
}

// ---------------------------------------------------------------------------
//	setLittleEndian
// ---------------------------------------------------------------------------
//
void
BinaryFile::setLittleEndian( void )
{
	_swapBytes = bigEndianSystem();
}

#pragma mark -
#pragma mark helpers
// ---------------------------------------------------------------------------
//	readBytes
// ---------------------------------------------------------------------------
//
void
BinaryFile::readBytes( char * data, int howmany, bool swap )
{
	//	need to seek when changing between read and write:
	if ( _prevOp == op_wr )
		offset( 0 );
	_prevOp = op_rd;

	//	read the bytes into data:
	iostream::read( data, howmany );
	
	//	check stream state:
	if ( ! good() )
		Throw( FileIOException, "Binary File read failed." );
	
	//	swap byte order if nec.
	if ( gcount() == howmany && swap ) 
		swapByteOrder( data, howmany );
}
	
// ---------------------------------------------------------------------------
//	writeBytes
// ---------------------------------------------------------------------------
//
void
BinaryFile::writeBytes( char * data, int howmany, bool swap )
{
	//	need to seek when changing between read and write:
	if ( _prevOp == op_rd )
		offset(0);
	_prevOp = op_wr;
		
	//	swap byte order if nec.
	if ( swap ) 
		swapByteOrder( data, howmany );

	//	write the bytes from data:
	iostream::write( data, howmany );
	
	//	check stream state:
	if ( ! good() )
		Throw( FileIOException, "Binary File write failed." );
	
	}
	
#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif



// ===========================================================================
//	File.C
//	
//	Wrapper class for stream-based binary file access.
//
//	-kel 22 Sept 99
//
// ===========================================================================


#include "LorisLib.h"
#include "Exception.h"
#include "Notifier.h"
#include "File.h"

#include <fstream>
using std::ifstream;
using std::ios;

Begin_Namespace( Loris )

static EndianFlag machineType( void );	//	prototype, defined below

// ---------------------------------------------------------------------------
//	File constructor
// ---------------------------------------------------------------------------
//	The EndianFlag defaults to BigEndian.
//
File::File( const string & s, EndianFlag e ) :
	_swapBytes( e != machineType() ),
	_path( s ),
	_stream()
{
}

// ---------------------------------------------------------------------------
//	File destructor
// ---------------------------------------------------------------------------
//
File::~File( void )
{
}

// ---------------------------------------------------------------------------
//	open
// ---------------------------------------------------------------------------
//
void
File::open( void )
{
	using namespace std;
	
	stream().open( name().c_str(), ios_base::binary | ios_base::in );
	if ( ! stream().is_open() ) {
		string s( "couldn't open File: " );
		s.append( name() );
		Throw( FileIOException, s );
	}
}

// ---------------------------------------------------------------------------
//	openWrite
// ---------------------------------------------------------------------------
//	Always open with read permission too.
//
void
File::openWrite( void )
{
	using namespace std;
	
	stream().open( name().c_str(), ios_base::binary | ios_base::in | ios_base::out );
	
	//	if it failed, make sure the file exists, 
	//	otherwise, we can't open with for input:
	if ( ! stream().is_open() ) {
		stream().open( name().c_str(), ios_base::binary | ios_base::out );
		stream().close();
		stream().open( name().c_str(), ios_base::binary | ios_base::in | ios_base::out );
	}
	
	if ( ! stream().is_open() ) {
		string s( "couldn't open File for writing: " );
		s.append( name() );
		Throw( FileIOException, s );
	}
}

// ---------------------------------------------------------------------------
//	close
// ---------------------------------------------------------------------------
//
void
File::close( void )
{
	stream().flush();
	stream().close();
}

// ---------------------------------------------------------------------------
//	position
// ---------------------------------------------------------------------------
//	fstream has two pairs of position mutators: tellp/seekp and tellg/seekg,
//	the former from ostream and the latter from istream, but these are all 
//	implemented in terms of a common virtual base class, so there is
//	really only one stream and one pointer. Use either, but not both.
//	
//
int
File::position( void )
{
	int posp = stream().tellp();
	if ( posp < 0 )
		Throw( FileIOException, "Couldn't find position of stream pointer in File::position.");
	return posp;
}

// ---------------------------------------------------------------------------
//	offsetPosition
// ---------------------------------------------------------------------------
//	Offset the stream position from its current position.
//
void
File::offsetPosition( int offset )
{
	stream().seekp( offset, ios::cur );
}

// ---------------------------------------------------------------------------
//	setPosition
// ---------------------------------------------------------------------------
//	Set the stream position to the position. If pos is less than zero, 
//	then use it as an offset from the end of the stream.
//
void
File::setPosition( int pos )
{
	ios::seekdir dehr;
	
	if ( pos < 0 ) 
		dehr = ios::end;
	else
		dehr = ios::beg;
	
	stream().seekp( pos, dehr );
}

// ---------------------------------------------------------------------------
//	atEOF
// ---------------------------------------------------------------------------
//
boolean
File::atEOF( void )
{
	return stream().eof();
}

// ---------------------------------------------------------------------------
//	machineType
// ---------------------------------------------------------------------------
//	Returns the type of machine the program is running on.
//	Copied from arun's code, used to determine whether to 
//	swap bytes.
//
static EndianFlag machineType( void )
{
	union {
		short s ;
		char c[2] ;
	} x ;
	x.s = 1 ;
	static const EndianFlag f = ( x.c[0] == 1 ) ? LittleEndian : BigEndian ;
	
	return f;
}

End_Namespace( Loris )



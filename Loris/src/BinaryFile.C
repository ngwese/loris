// ===========================================================================
//	BinaryFile.C
//	
//	Wrapper class for stream-based binary file access.
//
//	-kel 5 Oct 99
//
// ===========================================================================
#include "BinaryFile.h"
#include "Exception.h"
#include <vector>
#include <iostream>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	bigEndianSystem
// ---------------------------------------------------------------------------
//	Return true is this is a big-endian system, false otherwise.
//
static bool bigEndianSystem( void )
{
	union {
		int s ;
		char c[sizeof(int)] ;
	} x ;
	x.s = 1;
	
	return x.c[0] != 1;
}

// ---------------------------------------------------------------------------
//	swapByteOrder
// ---------------------------------------------------------------------------
//	
static void swapByteOrder( char * bytes, int n )
{
	char * beg = bytes, * end = bytes + n - 1;
	while ( beg < end ) {
		char tmp = *end;
		*end = *beg;
		*beg = tmp;
		
		++beg;
		--end;
	}
}

// ---------------------------------------------------------------------------
//	BigEndian read
// ---------------------------------------------------------------------------
//
void
BigEndian::read( std::istream & s, long howmany, int size, char * putemHere )
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
		for ( long i = 0; i < howmany; ++i )
		{
			swapByteOrder( putemHere + (i*size), size );
		}
	}
}

// ---------------------------------------------------------------------------
//	BigEndian write
// ---------------------------------------------------------------------------
//
void
BigEndian::write( std::ostream & s, long howmany, int size, const char * stuff )
{
	//	swap byte order if nec.
	if ( ! bigEndianSystem() && size > 1 ) 
	{
		//	use a temporary vector to automate storage:
		std::vector<char> v( stuff, stuff + (howmany*size) );
		for ( long i = 0; i < howmany; ++i )
		{
			swapByteOrder( & v[i*size], size );
		}
		s.write( &v[0], howmany*size );
	}
	else
	{
		//	read the bytes into data:
		s.write( stuff, howmany*size );
	}
	
	//	check stream state:
	if ( ! s.good() )
		Throw( FileIOException, "Binary File read failed." );
}


#if 0	//	LittleEndian isn't tested
// ---------------------------------------------------------------------------
//	LittleEndian read
// ---------------------------------------------------------------------------
//	Not tested.
//
void
LittleEndian::read( std::istream & s, long howmany, int size, char * putemHere )
{
	//	read the bytes into data:
	s.read( putemHere, howmany*size );
	
	//	check stream state:
	if ( ! s.good() )
		Throw( FileIOException, "Binary File read failed." );
	Assert( s.gcount() == howmany*size );
	
	//	swap byte order if nec.
	if ( bigEndianSystem() && size > 1 ) 
	{
		for ( long i = 0; i < howmany; ++i )
		{
			swapByteOrder( putemHere + (i*size), size );
		}
	}
}

// ---------------------------------------------------------------------------
//	LittleEndian write
// ---------------------------------------------------------------------------
//	Not tested.
//
void
LittleEndian::write( std::ostream & s, long howmany, int size, const char * stuff )
{
	//	swap byte order if nec.
	if ( bigEndianSystem() && size > 1 ) 
	{
		//	use a temporary vector to automate storage:
		std::vector<char> v( stuff, stuff + (howmany*size) );
		for ( long i = 0; i < howmany; ++i )
		{
			swapByteOrder( & v[i*size], size );
		}
		s.write( &v[0], howmany*size );
	}
	else
	{
		//	read the bytes into data:
		s.write( stuff, howmany*size );
	}
	
	//	check stream state:
	if ( ! s.good() )
		Throw( FileIOException, "Binary File read failed." );
}
#endif	//	LittleEndian isn't tested

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif



// ===========================================================================
//	Exception.C
//	
//	Implementaion of Loris::Exception, a generic exception class.
//
//	-kel 17 Aug 99
//
// ===========================================================================

#include "LorisLib.h"
#include "Exception.h"

#include <string>

#if !defined( Deprecated_iostream_headers )
	#include <iostream>
#else
	#include <iostream.h>
#endif

using namespace std;

Begin_Namespace( Loris )

#pragma mark -
#pragma mark construction

// ---------------------------------------------------------------------------
//	Exception constructor
// ---------------------------------------------------------------------------
//	where defaults to empty.
//
Exception::Exception( const string & str, const string & where ) :
	_sbuf( str ),
	ostream( & _sbuf )
{
	_sbuf.append( where );
	_sbuf.append(" ");
}
	
// ---------------------------------------------------------------------------
//	Exception copy constructor
// ---------------------------------------------------------------------------
//	Exception objects are copied when caught by reference or value.
//
Exception::Exception( const Exception & other ) :
	_sbuf( other._sbuf ),
	ostream( & _sbuf )
{
}

// ---------------------------------------------------------------------------
//	Exception destructor
// ---------------------------------------------------------------------------
//
Exception::~Exception( void )
{
}

// ---------------------------------------------------------------------------
//	str
// ---------------------------------------------------------------------------
//
const string &
Exception::str( void ) const
{
	return _sbuf.str();
}

// ---------------------------------------------------------------------------
//	what
// ---------------------------------------------------------------------------
//	std::exception interface.
//
const char *
Exception::what( void ) const
{
	return str().c_str();
}

// ---------------------------------------------------------------------------
//	operator << 
// ---------------------------------------------------------------------------
//	Not a member function of Exception.
//
ostream & 
operator << ( ostream & ostr, const Exception & ex )
{
	return ostr << ex.str();
}

End_Namespace( Loris )

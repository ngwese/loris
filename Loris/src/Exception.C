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
	_sbuf( str )
	//ostream( & _sbuf )
{
	_sbuf.append( where );
	_sbuf.append(" ");
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
//	append 
// ---------------------------------------------------------------------------
//
Exception & 
Exception::append( const string & str )
{
	_sbuf.append(str);
	return *this;
}

End_Namespace( Loris )

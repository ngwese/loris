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
	#include <ostream>
	using std::ostream;
#else
	#include <ostream.h>
#endif

Begin_Namespace( Loris )

#pragma mark -
#pragma mark construction

// ---------------------------------------------------------------------------
//	Exception constructor
// ---------------------------------------------------------------------------
//	where defaults to empty.
//
Exception::Exception( const std::string & str, const std::string & where ) :
	_sbuf( str ),
	ostream( & _sbuf )
{
	_sbuf.append( where );
	_sbuf.append(" ");
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
const std::string &
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

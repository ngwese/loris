// ===========================================================================
//	Exception.C
//	
//	Implementaion of Loris::Exception, a generic exception class.
//	Probably,it would be okay if all this implementation was in the
//	class definition.
//
//	-kel 17 Aug 99
//
// ===========================================================================
#include "LorisLib.h"
#include "Exception.h"
#include <string>

Begin_Namespace( Loris )

#pragma mark -
#pragma mark construction

// ---------------------------------------------------------------------------
//	Exception constructor
// ---------------------------------------------------------------------------
//	where defaults to empty.
//
Exception::Exception( const std::string & str, const std::string & where ) :
	_sbuf( str )
{
	_sbuf.append( where );
	_sbuf.append(" ");
}
	
// ---------------------------------------------------------------------------
//	append 
// ---------------------------------------------------------------------------
//
Exception & 
Exception::append( const std::string & str )
{
	_sbuf.append(str);
	return *this;
}

End_Namespace( Loris )

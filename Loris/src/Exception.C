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
#include "Exception.h"
#include <string>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif


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

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

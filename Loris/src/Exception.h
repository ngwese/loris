#ifndef __Loris_exception__
#define __Loris_exception__
// ===========================================================================
//	Exception.h
//	
//	Class definition for Loris::Exception, a generic exception class.
//
//	-kel 17 Aug 99
//
// ===========================================================================

#include "LorisLib.h"
#include "StringBuffer.h"

//	use standard library strings and streams
#include <string>

#if !defined( Deprecated_iostream_headers )
	#include <ostream>
	using std::ostream;
#else
	#include <ostream.h>
#endif

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class Exception
//
//	Generic exception class for reporting exceptional circumstances.
//	Can be a base class for more specific exception classes in Loris.
//	Inherits streaming capability from ostream, uses its own string
//	buffer (defined below) as a buffer. 
//
class Exception : public ostream
{
//	-- public interface --
public:
//	construction:
	Exception( const std::string & str, const std::string & where = "" );

//	virtual destructor so it Exception can be subclassed:
	virtual ~Exception( void );
	
//	access:
	const std::string & str( void ) const;
	const char * what( void ) const;
	
//	-- instance variable - the string buffer --
protected:
	StringBuffer _sbuf;
	
};	//	end of class Exception

// ---------------------------------------------------------------------------
//	streaming operator for Exceptions:
//	(defined in Exception.C)
//
ostream & operator << ( ostream & str, const Exception & ex );	

// ---------------------------------------------------------------------------
//	class AssertionFailure
//
class AssertionFailure : public Exception
{
public: 
	AssertionFailure( const std::string & str, const std::string & where = "" ) : 
		Exception( std::string("Assertion failed -- ").append( str ), where ) {}
		
};	//	end of class AssertionFailure

// ---------------------------------------------------------------------------
//	class LowMemException
//
class LowMemException : public Exception
{
public: 
	LowMemException( const std::string & str, const std::string & where = "" ) : 
		Exception( std::string("Low Memory Exception -- ").append( str ), where ) {}
		
};	//	end of class LowMemException

// ---------------------------------------------------------------------------
//	class IndexOutOfBounds
//
class IndexOutOfBounds : public Exception
{
public: 
	IndexOutOfBounds( const std::string & str, const std::string & where = "" ) : 
		Exception( std::string("Index out of bounds -- ").append( str ), where ) {}
		
};	//	end of class LowMemException

// ---------------------------------------------------------------------------
//	class FileIOException
//
class FileIOException : public Exception
{
public: 
	FileIOException( const std::string & str, const std::string & where = "" ) : 
		Exception( std::string("File i/o error -- ").append( str ), where ) {}
		
};	//	end of class FileAccessError

// ---------------------------------------------------------------------------
//	class InvalidObject
//
class InvalidObject : public Exception
{
public: 
	InvalidObject( const std::string & str, const std::string & where = "" ) : 
		Exception( std::string("Invalid configuration or object -- ").append( str ), where ) {}
		
};	//	end of class InvalidObject

// ---------------------------------------------------------------------------
//	macros for throwing exceptions
//
//	The compelling reason for using macros instead of inlines for all these
//	things is that the __FILE__ and __LINE__ macros will be useful.
//

#define __STR(x) __VAL(x)
#define __VAL(x) #x
#define	Throw( exType, report )												\
	throw exType( report, " ( " __FILE__ " line: " __STR(__LINE__) " )" )

#define Assert(test)														\
	do {																	\
		if (!(test)) Throw( AssertionFailure, #test );						\
	} while (false)


End_Namespace( Loris )

#endif	// ndef __Loris_exception__
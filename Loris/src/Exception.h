#ifndef __Loris_exception__
#define __Loris_exception__
// ===========================================================================
//	Exception.h
//	
//	Class definition for Loris::Exception, a generic exception class.
//
//	This could be derived from std::logic_error for greater ease of
//	handling, but that would make it difficult (impossible?) to add
//	to the notification string, and this feature is used in many places,
//	for better or worse.
//
//	-kel 17 Aug 99
//
// ===========================================================================
#include <string>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class Exception
//
//	Generic exception class for reporting exceptional circumstances.
//	Can be a base class for more specific exception classes in Loris.
//
class Exception
{
//	-- public interface --
public:
//	construction:
	Exception( const std::string & str, const std::string & where = "" );

//	virtual destructor so it Exception can be subclassed:
	virtual ~Exception( void ) {}
	
//	access:
	const std::string & str( void ) const { return _sbuf; }
	const char * what( void ) const { return _sbuf.c_str(); }
	
	Exception & append( const std::string & str );

//	-- instance variable - the string --
protected:
	std::string _sbuf;
	
};	//	end of class Exception

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
//	class IndexOutOfBounds
//
class IndexOutOfBounds : public Exception
{
public: 
	IndexOutOfBounds( const std::string & str, const std::string & where = "" ) : 
		Exception( std::string("Index out of bounds -- ").append( str ), where ) {}
		
};	//	end of class IndexOutOfBounds

// ---------------------------------------------------------------------------
//	class FileIOException
//
class FileIOException : public Exception
{
public: 
	FileIOException( const std::string & str, const std::string & where = "" ) : 
		Exception( std::string("File i/o error -- ").append( str ), where ) {}
};	//	end of class FileIOException

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
//	class InvalidIterator
//
//	Class of exceptions thrown when an Iterator is found to be badly configured
//	or otherwise invalid.
//
class InvalidIterator : public InvalidObject
{
public: 
	InvalidIterator( const std::string & str, const std::string & where = "" ) : 
		InvalidObject( std::string("Invalid Iterator -- ").append( str ), where ) {}
};	//	end of class InvalidIterator

// ---------------------------------------------------------------------------
//	class InvalidArgument
//
//	Class of exceptions thrown when an argument is found to be invalid.
//
class InvalidArgument : public Exception
{
public: 
	InvalidArgument( const std::string & str, const std::string & where = "" ) : 
		Exception( std::string("Invalid Argument -- ").append( str ), where ) {}
};	//	end of class InvalidArgument

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


#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef __Loris_exception__

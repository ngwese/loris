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
	
#if defined(__sgi) && ! defined(__GNUC__)
//	copying:
//	(exception objects are copied once when caught by reference,
//	or twice when caught by value)
//	This should be generated automatically by the compiler.
protected:
	Exception( const Exception & other ) : _sbuf( other._sbuf ) {}
#endif // lame compiler
	
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
		
#if defined(__sgi) && ! defined(__GNUC__)
//	copying:
//	(exception objects are copied once when caught by reference,
//	or twice when caught by value)
//	This should be generated automatically by the compiler.
	AssertionFailure( const AssertionFailure & other ) : Exception( other ) {}
#endif // lame compiler
		
};	//	end of class AssertionFailure

/*
//
//	remove this class, we no longer install a new handler that
//	would cause this exception to be thrown in place of std::bad_alloc.
//	We never did anything with this except pass it on anyway.
//
// ---------------------------------------------------------------------------
//	class LowMemException
//
class LowMemException : public Exception
{
public: 
	LowMemException( const std::string & str, const std::string & where = "" ) : 
		Exception( std::string("Low Memory Exception -- ").append( str ), where ) {}

#if defined(__sgi) && ! defined(__GNUC__)
//	copying:
//	(exception objects are copied once when caught by reference,
//	or twice when caught by value)
//	This should be generated automatically by the compiler.
	LowMemException( const LowMemException & other ) : Exception( other ) {}
#endif // lame compiler
		
};	//	end of class LowMemException
*/

// ---------------------------------------------------------------------------
//	class IndexOutOfBounds
//
class IndexOutOfBounds : public Exception
{
public: 
	IndexOutOfBounds( const std::string & str, const std::string & where = "" ) : 
		Exception( std::string("Index out of bounds -- ").append( str ), where ) {}

#if defined(__sgi) && ! defined(__GNUC__)
//	copying:
//	(exception objects are copied once when caught by reference,
//	or twice when caught by value)
//	This should be generated automatically by the compiler.
	IndexOutOfBounds( const IndexOutOfBounds & other ) : Exception( other ) {}
#endif // lame compiler
		
};	//	end of class LowMemException

// ---------------------------------------------------------------------------
//	class FileIOException
//
class FileIOException : public Exception
{
public: 
	FileIOException( const std::string & str, const std::string & where = "" ) : 
		Exception( std::string("File i/o error -- ").append( str ), where ) {}

#if defined(__sgi) && ! defined(__GNUC__)
//	copying:
//	(exception objects are copied once when caught by reference,
//	or twice when caught by value)
//	This should be generated automatically by the compiler.
	FileIOException( const FileIOException & other ) : Exception( other ) {}
#endif // lame compiler
		
};	//	end of class FileAccessError

// ---------------------------------------------------------------------------
//	class InvalidObject
//
class InvalidObject : public Exception
{
public: 
	InvalidObject( const std::string & str, const std::string & where = "" ) : 
		Exception( std::string("Invalid configuration or object -- ").append( str ), where ) {}

#if defined(__sgi) && ! defined(__GNUC__)
//	copying:
//	(exception objects are copied once when caught by reference,
//	or twice when caught by value)
//	This should be generated automatically by the compiler.
	InvalidObject( const InvalidObject & other ) : Exception( other ) {}
#endif // lame compiler
		
};

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

#if defined(__sgi) && ! defined(__GNUC__)
//	copying:
//	(exception objects are copied once when caught by reference,
//	or twice when caught by value)
//	This should be generated automatically by the compiler.
	InvalidIterator( const InvalidIterator & other ) : InvalidObject( other ) {}
#endif // lame compiler
		
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


#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef __Loris_exception__

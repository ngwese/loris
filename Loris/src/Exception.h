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

//	use standard library strings and streams
#include <string>
using std::string;

#if !defined(USE_DEPRECATED_HEADERS)
	#include <iostream>
	using std::ostream;
#else
	#include <iostream.h>
#endif

Begin_Namespace( Loris )

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
	Exception( const string & str, const string & in = "", Int at = 0L );

//	virtual destructor so it Exception can be subclassed:
	virtual ~Exception( void );
	
//	reporting:
	void printOn( char * str ) const;
	void streamOn( ostream & str ) const;
	
//	access:
	const string & getString( void ) const { return mReportString; }
	
//	mutation (only appending is allowed):
	void append( const string & str );

//	-- instance variables --
protected:
	string mReportString;

};	//	end of class Exception

//	streaming operator for Exceptions:
ostream & operator << ( ostream & str, const Exception & ex );

// ---------------------------------------------------------------------------
//	class RuntimeException
//
class RuntimeException : public Exception
{
public: 
	RuntimeException( const string & str, const string & in = "", Int at = 0L ) : 
		Exception( string("Runtime Exception: ").append( str ), in, at ) {}
		
};	//	end of class RuntimeException

// ---------------------------------------------------------------------------
//	class AssertionFailure
//
class AssertionFailure : public Exception
{
public: 
	AssertionFailure( const string & str, const string & in = "", Int at = 0L ) : 
		Exception( string("Assertion failed: ").append( str ), in, at ) {}
		
};	//	end of class AssertionFailure

// ---------------------------------------------------------------------------
//	class LowMemException
//
class LowMemException : public Exception
{
public: 
	LowMemException( const string & str, const string & in = "", Int at = 0L ) : 
		Exception( string("Low Memory Exception: ").append( str ), in, at ) {}
		
};	//	end of class LowMemException

// ---------------------------------------------------------------------------
//	class IndexOutOfBounds
//
class IndexOutOfBounds : public Exception
{
public: 
	IndexOutOfBounds( const string & str, const string & in = "", Int at = 0L ) : 
		Exception( string("Index out of bounds: ").append( str ), in, at ) {}
		
};	//	end of class LowMemException

// ---------------------------------------------------------------------------
//	macros for throwing exceptions
//
//	The compelling reason for using macros instead of inlines for all these
//	things is that the __FILE__ and __LINE__ macros will be useful.
//
#define	Throw( exType, report )	throw exType( report, __FILE__, __LINE__ )

#define Assert(test)														\
	do {																	\
		if (!(test)) Throw( AssertionFailure, #test );						\
	} while (false)


End_Namespace( Loris )

#endif	// ndef __Loris_exception__
#ifndef INCLUDE_EXCEPTION_H
#define INCLUDE_EXCEPTION_H
/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * Exception.h
 *
 * Definition of class Exception, a generic exception class, and 
 * commonly-used derived exception classes.
 *
 * Kelly Fitz, 17 Aug 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <exception>
#include <string>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	class Exception
//
//	Class Exception is a generic exception class for reporting exceptional 
//	circumstances in Loris. Exception is derived from std:exception, 
//	and is the base for a hierarchy of derived exception classes
//	in Loris:
//
//	class Exception
//	
//		class AssertionFailure (thrown by the Assert(invariant) macro)
//		class IndexOutOfBounds
//		class FileIOException
//		class InvalidObject
//		
//			class InvalidIterator
//			class InvalidArgument
//			class InvalidPartial (defined in Partial.h)
//		
//
class Exception : public std::exception
{
//	-- instance variable --
protected:
	std::string _sbuf;	//	string for storing the exception description
	
//	-- public interface --
public:
//	-- construction --
	Exception( const std::string & str, const std::string & where = "" );
	/*	Construct a new Exception with the specified description and, optionally
		a string identifying the location at which the exception as thrown. The
		Throw( Exception_Class, description_string ) macro generates a location
		string automatically using __FILE__ and __LINE__.
	 */
	 
	virtual ~Exception( void ) throw() {}
	/* 	Destroy this Exception.
	 */

//	-- std::exception interface --
	const char * what( void ) const throw() { return _sbuf.c_str(); }
	/*	Return a description of this Exception in the form of a
		C-style string (char pointer).
	 */
	 
//	-- description string access --
	Exception & append( const std::string & str );
	/*	Append the specified string to this Exception's description,
		and return a reference to this Exception.
	 */
	 
	const std::string & str( void ) const { return _sbuf; }
	/* 	Return a refernce to this Exception's description string.
	 */

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
		if (!(test)) Throw( Loris::AssertionFailure, #test );				\
	} while (false)


}	//	end of namespace Loris

#endif /* ndef INCLUDE_EXCEPTION_H */

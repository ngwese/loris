#ifndef __Loris_string_buffer__
#define __Loris_string_buffer__

// ===========================================================================
//	StringBuffer.h
//	
//	Class definition for Loris::StringBuffer, a simple streambuf subclass
//	that uses a std::string for storage.
//
//	-kel 5 Oct 99
//
// ===========================================================================


#include "LorisLib.h"

//	use standard library strings:
#include <string>

//	use std::streambuf as base class:
#if !defined(USE_DEPRECATED_HEADERS)
	#include <iosfwd>
	#include <streambuf>
#else
	#include <ostream.h>
#endif

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class StringBuffer
//
//	Should be able to use a std::stringbuf for this, but
//	MSL seems to implement it incorrectly (str() freezes
//	the string, according to their docs, and that's wrong)
//	and MIPSPro doesn't seem to implement it at all, so
//	its easier for now to implement the simple thing I need.
//
class StringBuffer : public std::streambuf
{
public:
	//	construction:
	StringBuffer( const std::string & s = "" ) : _str(s) {}
	
	//	string access:
	std::string & str( void ) { return _str; }
	const std::string & str( void ) const { return _str; }
	
	//	appending characters directly 
	//	(without using the enclosing stream):
	StringBuffer & append( const std::string & s ) { _str.append( s ); return *this; }
	
protected:
	//	called every time a character is written:
	virtual int_type overflow( int_type c ) 
	{
		if ( c != EOF ) {
			_str.push_back(c);
		}
		return c;
	}

private:
	//	buffer characters in a string:
	std::string _str;
	
};	//	end of class StringBuffer

End_Namespace( Loris )

#endif 	//	ndef __Loris_string_buffer__
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

#include <stdio.h>
#include <string>

#if !defined(USE_DEPRECATED_HEADERS)
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
//	in defaults to empty, at to 0.
//
Exception::Exception( const string & str, const string & in, Int at ) :
	mReportString( str )
{
	if ( ! in.empty() ) {
		mReportString.append( " in " );
		mReportString.append( in );
	}
	
	if ( at > 0 ) {
		mReportString.append( " at " );
		char s[16];
		sprintf( s, "%ld", at );
		mReportString.append( s );
	}
}
	

// ---------------------------------------------------------------------------
//	Exception destructor
// ---------------------------------------------------------------------------
//
Exception::~Exception( void )
{
}

#pragma mark -
#pragma mark reporting
// ---------------------------------------------------------------------------
//	printOn
// ---------------------------------------------------------------------------
//	This is much less cool than streamOn() because I can't check that str is
//	long enough for the report string. To be safe, make sure that no more than
//	255 characters are written on str.
//
void
Exception::printOn( char * str ) const
{
	strncpy( str, mReportString.c_str(), 255 );
}


// ---------------------------------------------------------------------------
//	streamOn
// ---------------------------------------------------------------------------
//
void
Exception::streamOn( ostream & str ) const
{
	str << mReportString;
}

// ---------------------------------------------------------------------------
//	operator << 
// ---------------------------------------------------------------------------
//	Not a member function of Exception.
//
ostream & 
operator << ( ostream & str, const Exception & ex )
{
	ex.streamOn( str );
	return str;
}

#pragma mark -
#pragma mark mutation
// ---------------------------------------------------------------------------
//	append
// ---------------------------------------------------------------------------
//
void 
Exception::append( const string & str )
{
	mReportString.append( " " );
	mReportString.append( str );
}


End_Namespace( Loris )

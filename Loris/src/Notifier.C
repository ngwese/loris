// ===========================================================================
//	Notifier.C
//	
//	Implementation of Loris::NotifierBuf, a base class for notification
//	buffers with a default implementation that uses the console for 
//	reporting.
//
//	Stream classes for notification and debugging are also implemented here,
//	they are just streams that use NotifierBufs.
//
//	C++ notification functions and streams in Loris namespace defined at bottom.
//
//	-kel 9 Sept 99
//
// ===========================================================================

#include "LorisLib.h"
#include "Notifier.h"
#include "Exception.h"

#if !defined( Deprecated_iostream_headers )
	#include <iostream>
#else
	#include <iostream.h>
#endif

//	MIPSPro is especially lame:
#if defined(__sgi) && ! defined(__GNUC__)
	#define IOSfmtflags int
#else
	#define IOSfmtflags ios::fmtflags
#endif	

using namespace std;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	stream instances
// ---------------------------------------------------------------------------
//	declared extern in Notifier.h
NotifierStream & notifier = NotifierStream::instance();
DebuggerStream & debugger = DebuggerStream::instance();


// ---------------------------------------------------------------------------
//	NotifierBuf constructor
// ---------------------------------------------------------------------------
//
NotifierBuf::NotifierBuf( const string & s ) :
	StringBuffer( s )
{
}

// ---------------------------------------------------------------------------
//	post
// ---------------------------------------------------------------------------
//	Derived classes can override the reporting behavior to put the 
//	notification somewhere other than standard-out.
//
//	If block is true, post() should not return until the
//	user confirms receipt of the notification. The logistics
//	of this interaction can also be overridden by derived classes.
//
//	post(true) is called by NotifierStream::confirm(), which is 
//	in turn called by fatalError() (see below). 
//
void 
NotifierBuf::post( boolean block )
{	
	cout << str() << endl;
	erase();
	
	if ( block ) {
		string resp;
		cout << "confirm (or type 'throw' to take exception): ";
		IOSfmtflags oldflags = cin.flags();

		 //cin >> noskipws >> resp;
		//	is this the same? MIPSPro doesn't have noskipws
		cin.unsetf( ios::skipws );
		cin >> resp;

		cin.flags( oldflags );
		cin.clear();
		
		if ( resp == "throw" ) {
			Throw(Exception, "User took  exception to notification." );
		}
	}
}

// ---------------------------------------------------------------------------
//	NotifierStream instance
// ---------------------------------------------------------------------------
//
NotifierStream &
NotifierStream::instance( void )
{
	static NotifierStream _s;
	return _s;
}


// ---------------------------------------------------------------------------
//	DebuggerStream instance
// ---------------------------------------------------------------------------
//
DebuggerStream &
DebuggerStream::instance( void )
{
	static DebuggerStream _s;
	return _s;
}


End_Namespace( Loris )


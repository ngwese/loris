// ===========================================================================
//	Notifier.C
//	
//	Implementation of Loris::NotifierBuf, a base class for notification
//	buffers with a default implementation that uses the console for 
//	reporting.
//
//	Stream objects for notification and debugging are also defined here,
//	they are just streams that use NotifierBufs. To change the notification
//	behavior, derive a new NotifierBuf and use the setbuffer() members of
//	NoitifierStream and DebuggerStream to assign the new buffer type
//	to the global notification and debugging streams.
//
//	-kel 9 Sept 99
//
// ===========================================================================
#include "LorisLib.h"
#include "Notifier.h"
#include "Exception.h"

#include "StringBuffer.h"

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
//	declared extern in Notifier.h, use thse like cout and cerr.
//
NotifierStream notifier;
DebuggerStream debugger;

// ---------------------------------------------------------------------------
//	class NotifierBuf
//
//	streambuf derivative that buffers output in a std::string (this 
//	behavior inherited from Loris::StringBuffer) and posts it as a
//	notification when a newline is received. The default post is to 
//	cout, derived classes may override post() to demonstrate more 
//	enlightened behavior.
//
//	When post is called (explicitly, as by NotifierStream::comfirm()) 
//	with blocking true, the notification blocks until the user confirms 
//	or takes exception. Dervied classes override post() to implement more 
//	sophisticated interaction with the user.
//
//	Collapse this class with StringBuffer (otherwise unused) and 
//	stick it in the implementation file, along with definitions of
//	member functions that use it. (can't use auto_ptr anymore, but
//	also get rid of the buffer setting operations anyway.)
//	
//
class NotifierBuf : public StringBuffer
{
//	-- public interface --
public:
//	construction:
	NotifierBuf( const std::string & s = "" );
	
//	virtual destructor so NotifierBuf can be subclassed:
//	(use compiler generated, StringBuffer has virtual destructor)
	//virtual ~NotifierBuf( void );	
	
//	posting:
//	Derived classes can override the reporting behavior to put the 
//	notification somewhere other than standard-out.
//	If block is true, post() should not return until the
//	user confirms receipt of the notification. The logistics
//	of this confirmation can also be overridden by derived classes. 
	virtual void post( boolean block = false );
		
protected:
	//	called every time a character is written:
	virtual int_type overflow( int_type c ) 
	{
		StringBuffer::overflow( c );
		if ( c == '\n' )
			post();
		return c;
	}
	
};	//	end of class NotifierBuf

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
		cout << "Proceed? (Type 'throw' to take exception, or anything else to proceed.): ";
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
//	NotifierStream constructor
// ---------------------------------------------------------------------------
//
NotifierStream::NotifierStream( void ) :
	_note( new NotifierBuf() )
{
	ostream::init( _note );
}
/*
// ---------------------------------------------------------------------------
//	NotifierStream setbuffer
// ---------------------------------------------------------------------------
//	Assign a new buffer to the stream.
//
NotifierBufPtr 
NotifierStream::setbuffer( NotifierBufPtr b )
{
	NotifierBufPtr ret = _note;
	_note = b;
	ostream::init( _note.get() );
	return ret;
}
*/
// ---------------------------------------------------------------------------
//	DebuggerStream constructor
// ---------------------------------------------------------------------------
//
#if !defined( Debug_Loris )
//	to do nothing at all, need a dummy streambuf:
struct dummybuf : public NotifierBuf
{
	virtual int_type overflow( int_type c ) { return c; }
};
#endif

DebuggerStream::DebuggerStream( void ) :
#if defined( Debug_Loris )
		_note( new NotifierBuf() )
#else
		_note( new dummybuf() )
#endif
{
	ostream::init( _note );
}
/*
// ---------------------------------------------------------------------------
//	DebuggerStream setbuffer
// ---------------------------------------------------------------------------
//	Assign a new buffer to the stream, or ignore if not debugging.
//
NotifierBufPtr
DebuggerStream::setbuffer( NotifierBufPtr b )
{
#if defined( Debug_Loris )
	NotifierBufPtr ret = _note;
	_note = b;
	ostream::init( _note );
	return ret;
#else
	return b;
#endif
}
*/

End_Namespace( Loris )


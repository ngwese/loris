#ifndef __Loris_notifier__
#define __Loris_notifier__
// ===========================================================================
//	Notifier.h
//	
//	Class definition for Loris::NotifierBuf, a base class for notification
//	buffers with a default implementation that uses the console for 
//	reporting.
//
//	Stream classes for notification and debugging are also defined here,
//	they are just streams that use NotifierBufs.
//
//	C++ notification functions and streams in Loris namespace defined at bottom.
//
//	-kel 9 Sept 99
//
// ===========================================================================
#include "LorisLib.h"
#include "StringBuffer.h"
#include <string>

//	macros for non-compliant compilers:
#if !defined( Deprecated_iostream_headers )
	#include <iostream>
	#define STDostream std::ostream
	#define STDstreambuf std::streambuf
#else
	#include <iostream.h>
	#define STDostream ostream
	#define STDstreambuf streambuf
#endif

Begin_Namespace( Loris )

//	import endl and ends from std:
using std::endl;
using std::ends;

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
//	class NotifierStream
//
//	ostream based on a NotifierBuf.
//
class NotifierStream : public STDostream
{
public:
	static NotifierStream & instance( void );
	void confirm( void ) { _note.post( true ); } // block until confirmed
	
//protected:
	NotifierStream( void ) : STDostream( & _note ) {}
	NotifierBuf _note;
	
};	//	end of class NotifierStream
	
extern NotifierStream & notifier;	//	local in Notifier.C

// ---------------------------------------------------------------------------
//	class DebuggerStream
//
//	When debugging is enabled, DebuggerStream is pretty much identical
//	to NotifierStream. When debugging is disabled, DebuggerStream uses
//	a dummy streambuf in place of the NotifierBuf, so the characters are
//	just ignored.
//
class DebuggerStream : public STDostream
{
public:
	static DebuggerStream & instance( void );
	
//protected:
	DebuggerStream( void ) : STDostream( & _note ) {}
	
#if defined( Debug_Loris )
	NotifierBuf _note;
#else
	//	to do nothing at all, need a dummy streambuf:
	struct dummybuf : public STDstreambuf
	{
		virtual int_type overflow( int_type c ) { return c; }
	} _note;
#endif
	
};	//	end of class DebuggerStream

extern DebuggerStream & debugger;	//	local in Notifier.C

// ---------------------------------------------------------------------------
//	prototype for a one-shot notifiers:
//
void notify( const std::string & s );
void debug( const std::string & s );

// ---------------------------------------------------------------------------
//	prototype for a one-shot error notifier:
//	This one displays its message and aborts.
//
void fatalError( const std::string & s );

End_Namespace( Loris )

#endif	// ndef __Loris_notifier__

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
//	they are just streams that use NotifierBufs. To change the notification
//	behavior, derive a new NotifierBuf and use the setbuffer() members of
//	NoitifierStream and DebuggerStream to assign the new buffer type
//	to the global notification and debugging streams.
//
//	C++ notification functions and streams in Loris namespace are
//	prototyped at bottom.
//
//	-kel 9 Sept 99
//
// ===========================================================================
#include "LorisLib.h"
#include "StringBuffer.h"
#include <string>
#include <memory>

//	macros for non-compliant compilers:
#if !defined( Deprecated_iostream_headers )
	#include <iostream>
	#define STDostream std::ostream
#else
	#include <iostream.h>
	#define STDostream ostream
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
//
class NotifierStream : public STDostream
{
public:
	//	construction:
	NotifierStream( void ) { setbuffer(); }
	
	//	force post and block until confirmed:
	void confirm( void ) { _note->post( true ); }
	
	//	assign a new buffer to the stream:
	std::auto_ptr< NotifierBuf > 
	setbuffer( std::auto_ptr< NotifierBuf > b = std::auto_ptr< NotifierBuf >(new NotifierBuf()) );

private:	
	std::auto_ptr< NotifierBuf > _note;
	
};	//	end of class NotifierStream
	
extern NotifierStream notifier;	//	local in Notifier.C

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
	//	construction:
	DebuggerStream( void );
	
	//	assign a new buffer to the stream:
	std::auto_ptr< NotifierBuf > 
#if defined( Debug_Loris )
	setbuffer( std::auto_ptr< NotifierBuf > b = std::auto_ptr< NotifierBuf >(new NotifierBuf()) );
#else
	setbuffer( std::auto_ptr< NotifierBuf > b = std::auto_ptr< NotifierBuf >() );
#endif		
	
private:
#if !defined( Debug_Loris )
	//	to do nothing at all, need a dummy streambuf:
	struct dummybuf : public NotifierBuf
	{
		virtual int_type overflow( int_type c ) { return c; }
	};
#endif

	//	the buffer:
	std::auto_ptr< NotifierBuf > _note;

};	//	end of class DebuggerStream

extern DebuggerStream debugger;	//	local in Notifier.C

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

#ifndef __Loris_notifier__
#define __Loris_notifier__
// ===========================================================================
//	Notifier.h
//	
//	Class definition for Loris::Notifier, a base class for notification
//	objects with a default implementation that uses the console for 
//	reporting.
//
//	The interface is about right here, but the class heirarchy might 
//	need to be altered to make it more flexible for subclassing.
//
//	-kel 9 Sept 99
//
// ===========================================================================

#include "LorisLib.h"
#include "StringBuffer.h"

#include <string>

#if !defined( Deprecated_iostream_headers )
	#include <iostream>
	#define STDostream std::ostream
	#define STDstreambuf std::streambuf
	#define STDstreamsize std::streamsize
#else
	#include <iostream.h>
	#define STDostream ostream
	#define STDstreambuf streambuf
	#define STDstreamsize int
#endif

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class Notifier
//
//	Least common denominator notification object. Uses console for i/o (post).
//	Derived classes can do otherwise, may need to do this differently to
//	compile apps with no console at all, probably need to abstract the
//	interface. 
//	Inherits streaming from ostream.
//
class Notifier : public STDostream
{
//	-- public interface --
public:
//	construction:
	Notifier( const std::string & s = "" );
	Notifier( STDstreambuf & buf, const std::string & s = "" );
	
//	virtual destructor so Notifier can be subclassed:
//	(use compiler generated, STDostream has virtual destructor)
	//virtual ~Notifier( void );	
	
//	posting:
//	Derived classes can override the reporting behavior to put the 
//	notification somewhere other than standard-out.
//	If block is true, post() should not return until the
//	user confirms receipt of the notification. The logistics
//	of this confirmation can also be overridden by derived classes. 
	virtual void post( boolean block = false );
	
//	-- instance variable - the string buffer --
protected:
	StringBuffer _sbuf;
	
};	//	end of class Notifier

// ---------------------------------------------------------------------------
//	class Debugger
//
//	When debugging is enabled, this can be just a Notifier. When debugging
//	is disabled though, it should be gutted by substituting a dummy buffer,
//	that does nothing at all with the characters it gets, for the StringBuffer
//	used by Notifier.
//
class Debugger 
#if Debug_Loris
	//	inherit everything from Notifier:
	: public Notifier
	{
	public:
		Debugger( const std::string s = "" ) : Notifier( s ) {}
	};
#else
	//	do nothing at all:
	 : public STDostream
	{
	public:
		Debugger( void ) : STDostream( & dumb ) {}
		Debugger( const std::string ) : STDostream( & dumb ) {}
		
		//	post does nothing at all:
		virtual void post( boolean = false ) {}
		
		//	to do nothing at all, need a dummy streambuf:
		class dummybuf : public STDstreambuf
		{
		protected:
			//	called every time a character is written:
			virtual int_type overflow( int_type c ) { return c; }
			
			//	called when lots of characters are written:
			virtual STDstreamsize xsputn( const char *, STDstreamsize n ) { return n; }
		};	//	end of class dummybuf
		
	private:
		dummybuf dumb;
	};
#endif	//	Debug_Loris	

End_Namespace( Loris )

#endif	// ndef __Loris_notifier__

#ifndef __Loris_notifier__
#define __Loris_notifier__
// ===========================================================================
//	Notifier.h
//	
//	Stream classes for notification and debugging are defined here,
//	they are just streams that use NotifierBufs. 
//
//	-kel 9 Sept 99
//
// ===========================================================================
#include "LorisLib.h"
#include <string>

//	macros for non-compliant compilers:
#if !defined( Deprecated_iostream_headers )
	#include <iostream>
	#define STDostream std::ostream
#else
	#include <iostream.h>
	#define STDostream ostream
#endif

Begin_Namespace( Loris )

#if !defined( Deprecated_iostream_headers )
	//	import endl and ends from std into Loris:
	using std::endl;
	using std::ends;
#endif

class NotifierBuf;

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
	NotifierStream( void );
	
private:	
	NotifierBuf * _note;
	
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
	
private:
	//	the buffer:
	NotifierBuf * _note;

};	//	end of class DebuggerStream

extern DebuggerStream debugger;	//	local in Notifier.C

End_Namespace( Loris )

#endif	// ndef __Loris_notifier__

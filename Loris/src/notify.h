#ifndef __Loris_stream_notify__
#define __Loris_stream_notify__

// ===========================================================================
//	notify.h
//	
//	C++ notification functions and streams in Loris namespace.
//	Definitions in Notifier.C.
//
//	-kel 26 Oct 99
//
// ===========================================================================

#include "LorisLib.h"
#include "Notifier.h"
#include <string>

Begin_Namespace( Loris )

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

// ---------------------------------------------------------------------------
//	streams for notification, declared in Notifier.C.
//
extern Notifier notifier;
extern Debugger debugger;

End_Namespace( Loris )

#endif	// ndef __Loris_stream_notify__
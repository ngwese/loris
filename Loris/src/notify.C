// ===========================================================================
//	notify.C
//	
//	One-shot notifications, including c-linkable ones not in namespace.
//
//	-kel 11 Nov 99
//
// ===========================================================================

#include "LorisLib.h"
#include "Notifier.h"
#include "cnotify.h"

#if !defined( Deprecated_cstd_headers )
	#include <cstdio>
#else
	#include <stdio.h>
#endif

using namespace std;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	notify
// ---------------------------------------------------------------------------
//	One-shot notification.
//
void
notify( const string & s )
{
	notifier << s << endl;
}

// ---------------------------------------------------------------------------
//	debug
// ---------------------------------------------------------------------------
//	One-shot debug notification.
//
void
debug( const string & s )
{
	debugger << s << endl;
}

// ---------------------------------------------------------------------------
//	fatalError
// ---------------------------------------------------------------------------
//	One-shot error notification that displays even if the app terminates 
//	immediately. For console apps this is nothing special, for gui apps,
//	a blocking alert dialog is needed.
//
void
fatalError( const string & s )
{
	try {
		notifier << s << " (aborting)";
		notifier.confirm(); // block until confirmed
	}
	catch (...) {
	}
	abort();
}

End_Namespace( Loris )

// ---------------------------------------------------------------------------
//	notify
// ---------------------------------------------------------------------------
//	One-shot c-callable notification, not in namespace.
//	Prototype in cnotify.h.
//
extern "C" void
notify( const char * cstr )
{
	Loris::notify( string(cstr) );
}

// ---------------------------------------------------------------------------
//	debug
// ---------------------------------------------------------------------------
//	One-shot c-callable debug notification, not in namespace.
//	Prototype in cnotify.h.
//
extern "C" void
debug( const char * cstr )
{
	Loris::debug( string(cstr) );
}

// ---------------------------------------------------------------------------
//	fatalError
// ---------------------------------------------------------------------------
//	One-shot c-callable error notification, not in namespace.
//	Prototype in cnotify.h.
//
extern "C" void
fatalError( const char * cstr )
{
	Loris::fatalError( string(cstr) );
}


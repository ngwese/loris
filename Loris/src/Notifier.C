// ===========================================================================
//	Notifier.C
//	
//	Implementation of Loris::Notifier.
//
//	-kel 9 Sept 99
//
// ===========================================================================


#include "LorisLib.h"
#include "Notifier.h"
#include "cnotify.h"

#if !defined(USE_DEPRECATED_HEADERS)
	#include <iostream>
	#include <cstdio>
#else
	#include <iostream.h>
	#include <stdio.h>
#endif

using namespace std;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	Notifier constructor
// ---------------------------------------------------------------------------
//
Notifier::Notifier( const string s )
{
	ss << s;
}

// ---------------------------------------------------------------------------
//	Notifier destructor
// ---------------------------------------------------------------------------
//
Notifier::~Notifier( void )
{
}

// ---------------------------------------------------------------------------
//	report
// ---------------------------------------------------------------------------
//	Derived classes can override the reporting behavior to put the 
//	notification somewhere other than standard-out.
//
void 
Notifier::report( void )
{
	cout << ss.str() << endl;
}

// ---------------------------------------------------------------------------
//	notify
// ---------------------------------------------------------------------------
//	One-shot notification.
//
void
notify( const string s )
{
	Notifier n;
	n << s;
	n.report();
}

// ---------------------------------------------------------------------------
//	fatalError
// ---------------------------------------------------------------------------
//	One-shot error notification that displays even if the app terminates 
//	immediately. For console apps this is nothing special, for gui apps,
//	a blocking alert dialog is needed.
//
void
fatalError( const string s )
{
	cerr << s << endl << endl;
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


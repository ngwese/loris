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
#include "Exception.h"
#include "cnotify.h"

#if !defined( Deprecated_iostream_headers )
	#include <iostream>
	using std::ostream;
#else
	#include <iostream.h>
#endif

#if !defined( Deprecated_cstd_headers )
	#include <cstdio>
	using std::abort;
#else
	#include <stdio.h>
#endif

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	Notifier constructor
// ---------------------------------------------------------------------------
//
Notifier::Notifier( const std::string & s ) :
	_sbuf( s ),
	ostream( & _sbuf )
{
}

// ---------------------------------------------------------------------------
//	post
// ---------------------------------------------------------------------------
//	Derived classes can override the reporting behavior to put the 
//	notification somewhere other than standard-out.
//	If block is true, post() should not return until the
//	user confirms receipt of the notification. The logistics
//	of this confirmation can also be overridden by derived classes. 
//
void 
Notifier::post( boolean block )
{
	using namespace std;
	
	cout << _sbuf.str() << endl;
	
	if ( block ) {
		std::string resp;
		cout << "confirm (or type 'throw' to take exception): ";
		ios::fmtflags oldflags = cin.flags();
		cin >> noskipws >> resp;
		cin.flags( oldflags );
		cin.clear();
		
		if ( resp == "throw" ) {
			Throw(Exception, "User took  exception to notification." );
		}
	}
}

// ---------------------------------------------------------------------------
//	notify
// ---------------------------------------------------------------------------
//	One-shot notification.
//
void
notify( const std::string & s )
{
	Notifier n(s);
	n.post();
}

// ---------------------------------------------------------------------------
//	fatalError
// ---------------------------------------------------------------------------
//	One-shot error notification that displays even if the app terminates 
//	immediately. For console apps this is nothing special, for gui apps,
//	a blocking alert dialog is needed.
//
void
fatalError( const std::string & s )
{
	try {
		Notifier n(s);
		n << " (aborting)";
		n.post( true ); // block until confirmed
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
	Loris::notify( std::string(cstr) );
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
	Loris::fatalError( std::string(cstr) );
}


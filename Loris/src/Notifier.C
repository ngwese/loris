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

#if !defined(USE_DEPRECATED_HEADERS)
	#include <iostream>
#else
	#include <iostream.h>
#endif

using namespace std;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	Notifier constructor
// ---------------------------------------------------------------------------
//
Notifier::Notifier( string s )
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
notify( string s )
{
	Notifier n;
	n << s;
	n.report();
}

End_Namespace( Loris )


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
//	notification streams
// ---------------------------------------------------------------------------
//	These are declared extern in Notifier.h, so they can be used
//	anywhere.
//
Notifier notifier;
Debugger debugger;

// ---------------------------------------------------------------------------
//	Notifier constructor
// ---------------------------------------------------------------------------
//
Notifier::Notifier( const string & s ) :
	_sbuf( s ),
	ostream( & _sbuf )
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
//	of this confirmation can also be overridden by derived classes. 
//
void 
Notifier::post( boolean block )
{	
	cout << _sbuf.str() << endl;
	_sbuf.erase();
	
	if ( block ) {
		string resp;
		cout << "confirm (or type 'throw' to take exception): ";
		IOSfmtflags oldflags = cin.flags();

		// cin >> noskipws >> resp;
		//	is this the same? MIPSPro doesn't have noskipws
		cin.unsetf( skipws );
		cin >> resp;

		cin.flags( oldflags );
		cin.clear();
		
		if ( resp == "throw" ) {
			Throw(Exception, "User took  exception to notification." );
		}
	}
}

End_Namespace( Loris )


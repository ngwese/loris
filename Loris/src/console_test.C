/*
 *  Hello World for the CodeWarrior
 *  © 1997-1998 Metrowerks Corp.
 *
 *  Questions and comments to:
 *       <mailto:support@metrowerks.com>
 *       <http://www.metrowerks.com/>
 */


#include "LorisLib.h"

#if !defined(USE_DEPRECATED_HEADERS)
	#include <iostream>
#else
	#include <iostream.h>
#endif

#include "Synthesizer.h"
#include "Breakpoint.h"
#include "Partial.h"
#include "Exception.h"
#include "LowMem.h"
#include "Notifier.h"

using namespace std;
using namespace Loris;


int main()
{	
	cout << "size of Int is " << sizeof(Int) << endl;
	cout << "size of Double is " << sizeof(Double) << endl;
	
	reserveSpace( );
	
	notify( "Here we go!" );

	Int * goober = Null;
	try {
		goober = new Int[10000000];
	}
	catch ( LowMemException & ex ) {
		Notifier n;
		n << ex << "\n";
		n << "goober is " << (void *)goober << "\n";
		n.report();
	}
	
	
	try {
		cout << "constructing 3 Breakpoints (on stack)." << endl;
		Breakpoint p1( 100, 0.5, 0. );
		cout << "p1 frequency is " << p1.frequency() << endl;
		Breakpoint p2( 200, 0.4, 0.5 );
		cout << "p2 frequency is " << p2.frequency() << endl;
		Breakpoint p3( 100, 0.5, 0. );
		cout << "p3 frequency is " << p3.frequency() << endl;
		
		cout << "constructing a Partial...";
		Partial dehr;
		cout << "inserting three Breakpoints." << endl;
		dehr.insert( 0., p1 );
		dehr.insert( 1., p2 );
		dehr.insert( 2., p3 );
		
		cout << "copying a Partial." << endl;
		Partial * duh = new Partial( dehr );
		
		Breakpoint * p;
		for ( p = dehr.head(); p != NULL; p = p->next() )
			p->setFrequency( p->frequency() * 2. );
			
		cout << "unmodified partial (duh):" << endl;
		for ( p = duh->head(); p != NULL; p = p->next() )
			cout << "\t" << p->frequency() << " at "<< p->time() << endl;
		cout << endl;
		
		cout << "modified partial (dehr):" << endl;
		for ( p = dehr.head(); p != NULL; p = p->next() )
			cout << "\t" << p->frequency() << " at " << p->time() << endl;
		cout << endl;
		
		cout << "duration is " << dehr.duration() << endl;
		cout << "removing 0.75 to 1.25" << endl;
		dehr.remove( 0.75, 1.25 );
		for ( p = dehr.head(); p != NULL; p = p->next() )
			cout << "\t" << p->frequency() << " at " << p->time() << endl;
		cout << endl;
		cout << "duration is " << dehr.duration() << endl;
		
	}
	catch ( Exception & ex ) {
		ex.streamOn( cout );
	}
			
	return 0;
}


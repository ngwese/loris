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

#include <sstream>

#include "Synthesizer.h"
#include "Breakpoint.h"
#include "Partial.h"
#include "Exception.h"
#include "LowMem.h"
#include "Notifier.h"

#include "SampleVector.h"

#include "AiffFile.h"

#include "File.h"
#include "ImportLemur5.h"


#if !defined(USE_DEPRECATED_HEADERS)
	#include <cstdio>
#else
	#include <stdio.h>
#endif

#include "File.h"

#include "ieee.h"

using namespace std;
using namespace Loris;


int main()
{	
	try {		
		File f("Berenice:Loris:try these:flute3.lemr");
		ImportLemur5 imp(f);
		
		cout << "importing partials from " << f.name() << "..." << endl;
		
		imp.importPartials();
		list< Partial > l;
		l.splice( l.begin(), imp.partials() );
		
		cout << "done" << endl;
		
		//	find the longest:
		Double t = 0.;
		for (list< Partial >::iterator it = l.begin(); it != l.end(); ++it ) {
			if ( (*it).endTime() > t )
				t = (*it).endTime();
			//cout << "\tpartial: " << (*it).startTime() << " to " << (*it).endTime() << endl;
		}
				
		cout << "found " << l.size() << " partials, total duration is " << t << endl;
		
		
		const Int srate = 44100;
		const Int nsamps = srate * t;
		SampleVector buf( nsamps );
		
		cout << "synthesizing... ";
		Synthesizer synth( buf, srate );
		Int c = 0;
		for (list< Partial >::iterator it = l.begin(); it != l.end(); ++it ) {
			synth.synthesizePartial( *it );
			if ( ++c % 100 == 0  )
				cout << "\t" << c << " partials..." << endl;
		}
			
		cout << "done." << endl << "writing..." << endl;
		
		for ( Int z = 1; z < 4; ++z ) {
			Int ssize = z * 8;
			
			stringstream s;
			s << "Berenice:Loris:try these:flute3." << ssize << ".aiff";
		
			cout << s.str() << "...";
			
			File fout( s.str() );
			AiffFile sf( srate, 1, ssize, buf, fout );
			sf.write();
		
			cout << "done." << endl;
		}
		
		/*
		cout << "writing old way..." << endl;
		
		FSSpec spec = { 0, 0, "\pBerenice:Loris:try these:newflute3.aiff" };
		FSpCreate( &spec, 'LEMR', 'AIFF', 0 );
		
		AIFFfile fout( spec, srate, 1, 16 );
		fout.setSamples( & buf[0], buf.size() );
		fout.write();
		fout.setSamples( Null, 0 );
		*/
		cout << "done." << endl;
	}
	catch ( Exception & ex ) {
		ex.streamOn( cout );
	}

	/*
	fstream f;	
	try {
		File f("foo", LittleEndian );
		
		cout << "opening file 'foo'" << endl;
		f.openWrite();
		
		short x = 1;
		long y = 2;
		short z = 3;
		
		cout << "packing: " << x << " " << y << " " << z << endl;
		f.write( x );
		f.write( y );
		f.write( z );
		
		cout << "closing file 'foo'" << endl;
		f.close();
		
		cout << "opening file 'foo'" << endl;
		f.open();
		
		f.read( x );
		f.read( y );
		f.read( z );
		
		cout << "unpacking: " << x << " " << y << " " << z << endl;
		
		cout << "closing file 'foo'" << endl;
		f.close();
	}
	catch ( Exception & ex ) {
		ex.streamOn( cout );
	}
	*/
	/*
	try {		
		const Int srate = 44100;
		const Double duration = 3.;
		
		Breakpoint bp( 100, 0.5, 0. );
		Partial p;
		p.insert( 0.1, bp );
		p.insert( 0.5 * duration, bp );
		
		bp.setBandwidth( 0.5 );
		bp.setAmplitude( 0.2 );
		p.insert( duration, bp );
		
		const Int nsamps = srate * duration;
		Double * samps = new Double[nsamps];
		for ( Int k = 0; k < nsamps; ++k )
			samps[k] = 0.;
		SimpleSampleBuffer buf( samps, nsamps );
		
		Synthesizer synth( buf, srate );
		synth.synthesizePartial( p );
		
		FSSpec spec = { 0, 0, "\pfoo.aiff" };
		FSpCreate( &spec, 'LEMR', 'AIFF', 0 );
		
		AIFFfile f( spec, srate, 1, 16 );
		f.setSamples( samps, nsamps );
		f.write();
		f.setSamples( Null, 0 );
			
	}
	catch( Exception & ex ) {
		ex.streamOn( cout );
	}
	*/
	/*
	FILE * fp = fopen( "Berenice:foo", "r" );
	if ( fp == Null ) {
		cout << "fopen() returned Null." << endl;
		exit(0);
	}
	char c[32];
	int numread = fread( c, sizeof(char), 31, fp );
	c[numread] = '\n';
	cout << "read: " << c << endl;
	fclose( fp );

	exit(0);
	*/
	
	/* new handler test
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
	*/
	
	/* partial copying test
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
	*/
	
	return 0;
}


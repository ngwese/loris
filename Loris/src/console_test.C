/*
 *  Hello World for the CodeWarrior
 *  © 1997-1998 Metrowerks Corp.
 *
 *  Questions and comments to:
 *       <mailto:support@metrowerks.com>
 *       <http://www.metrowerks.com/>
 */

#include "LorisLib.h"

#if !defined( Deprecated_iostream_headers)
	#include <iostream>
#else
	#include <iostream.h>
#endif

#if !defined( Deprecated_cstd_headers )
	#include <cstdio>
#else
	#include <stdio.h>
#endif

#include "Synthesizer.h"
#include "Exception.h"
#include "Notifier.h"
#include "SampleVector.h"
#include "AiffFile.h"
#include "BinaryFile.h"
#include "ImportLemur5.h"
#include "LoFreqBweKludger.h"
#include "Breakpoint.h"

#include "Morph.h"
#include "Distiller.h"

using namespace std;
using namespace Loris;

//
//	test app using a console interface
//

int main()
{	
	try {	
		//	import 4A partials:
		string name = ":::ncsa morph:carhorn1.lemr";
		BinaryFile f2;
		f2.view(name);
		ImportLemur5 imp2(f2);
		
		cout << "importing partials from " << name << "..." << endl;
		imp2.importPartials();
		cout << "done" << endl;
		f2.close();
		
		//	dilating car horn:
		cout << "dilating..." << endl;
		list< Partial > car;
		const double currentArray[] = { 0.3, 1. };
		const double desiredArray[] = { 0.5, 4. };
		vector< double > cur( currentArray, currentArray + 2 );
		vector< double > des( desiredArray, desiredArray + 2 );
		for ( list<Partial>::iterator it = imp2.partials().begin(); it != imp2.partials().end(); ++it ) {
			car.push_back( dilate( *it, cur, des ) );
		}
		cout << "done." << endl;
		imp2.partials().erase( imp2.partials().begin(), imp2.partials().end() );


		//	import 4Bb partials:
		name = ":::ncsa morph:elephant3.lemr";
		BinaryFile f;
		f.view(name);
		ImportLemur5 imp(f);
		
		cout << "importing partials from " << name << "..." << endl;
		imp.importPartials();
		cout << "done" << endl;
		f.close();

/*		
		cout << "distilling..." << endl;
		list< Partial > hoowah;
		//cel.splice( cel.begin(), imp2.partials() );
		//const double goober[] = { 0.5, 4.5 };
		///const double othergoober[] = { 0.1, 3. };
		//vector< double > cur( goober, goober + 2 );
		//vector< double > des( othergoober, othergoober + 2 );
		//for ( list<Partial>::iterator it = imp.partials().begin(); it != imp.partials().end(); ++it ) {
			
		Distiller dis;
		for ( int label = 1; label < 100; ++label ) {
			list< Partial > l = select( imp.partials(), label );
			hoowah.push_back( dis.distill( l ) );
		}
		cout << "done." << endl;
		imp.partials().erase( imp.partials().begin(), imp.partials().end() );

		
		// Shift my frequencies.
//		double shiftRatio = 1. / sqrt(2);
		double shiftRatio = pow(2, 1./12.);
		for ( list<Partial>::iterator it = hoowah.begin(); it != hoowah.end(); ++it ) {
			for ( Breakpoint * bp = (*it).head(); bp != Null; bp = bp->next() ) {
				bp->setFrequency( bp->frequency() * shiftRatio );
			}
		}
*/

		//	Create my weight function:
		WeightFunction w;
		w.insertBreakpoint( 0.6, 0. ); // start with clar
		w.insertBreakpoint( 2., 1. );  // end with long amount of flute action.
		
		WeightFunction flat;
		flat.insertBreakpoint( 0., 0. );
		flat.insertBreakpoint( 1., 0. );
		
		for (double z = 0.; z < 3.0; z += 0.2) {
			cout << "weight at " << z << " is " << w.weightAtTime(z) << endl;
		}
		
		Morph m;
		m.setFreqFunction( w );
		m.setAmpFunction( flat );
		m.setBwFunction( flat );

	
		cout << "morphine..." << endl;
		m.doit( car, imp.partials() );
		cout << "done." << endl;


		//	synthesize:
		const int srate = 44100;
		const int nsamps = srate * 6;
		SampleVector buf( nsamps );
		Synthesizer synth( buf, srate );
		synth.setIterator( new LoFreqBweKludger(2000.) );
		
		cout << "synthesizing" << endl;
		int c = 0;
		for ( list< Partial >::iterator it = m.partials().begin(); 
			  it != m.partials().end(); 
			  ++it ) {
			synth.synthesizePartial( *it );
			if ( ++c % 10 == 0  )
				cout << "\t" << c << " partials..." << endl;
		}
		
		//	write out samples:
		string newname(":::ncsa morph:elecar9.aiff");
		cout << "writing " << newname << endl;
		AiffFile sfout( srate, 1, 16, buf );
		sfout.write( BinaryFile( newname ) );
		
		cout << "done." << endl;

/*
		//	import flute partials:
		string name(":::sample_sounds:swell.lemr");
		BinaryFile f;
		f.view(name);
		ImportLemur5 imp(f);
		
		cout << "importing partials from " << name << "..." << endl;
		imp.importPartials();
		cout << "done" << endl;
	
		//	find the duration:
		double t = 0.;
		for ( list< Partial >::iterator it = imp.partials().begin(); 
			  it != imp.partials().end(); 
			  ++it ) {
			if ( (*it).endTime() > t )
				t = (*it).endTime();
		}
				
		cout << "found " << imp.partials().size() << " partials, total duration is " << t << endl;
		
		//	synthesize:
		const int srate = 44100;
		const int nsamps = srate * t;
		SampleVector buf( nsamps );
		Synthesizer synth( buf, srate );
		synth.setIterator( new LoFreqBweKludger(1000.) );
		
		cout << "synthesizing" << endl;
		int c = 0;
		for ( list< Partial >::iterator it = imp.partials().begin(); 
			  it != imp.partials().end(); 
			  ++it ) {
			synth.synthesizePartial( *it );
			if ( ++c % 100 == 0  )
				cout << "\t" << c << " partials..." << endl;
		}
		
		//	write out samples:
		string newname(":::sample_sounds:swell.aiff");
		cout << "writing " << newname << endl;
		AiffFile sfout( srate, 1, 16, buf );
		sfout.write( BinaryFile( newname ) );
		
		cout << "done." << endl;
*/
	}
	catch ( Exception & ex ) {
		cout << ex;
	}

	return 0;
}


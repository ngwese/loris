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
#include "SampleVector.h"
#include "AiffFile.h"
#include "BinaryFile.h"
#include "ImportLemur5.h"
#include "LoFreqBweKludger.h"
#include "Breakpoint.h"

#include "Morph.h"
#include "Distiller.h"
#include "Map.h"
#include "Dilator.h"
#include "notify.h"

using namespace std;
using namespace Loris;

//
//	test app using a console interface
//

int main()
{	
	try {
		//	import 4A partials:
		string name = ":::morphing:ncsa morph:carhorn1.lemr";
		BinaryFile f2;
		f2.view(name);
		ImportLemur5 imp2(f2);
		
		cout << "importing partials from " << name << "..." << endl;
		imp2.importPartials();
		cout << "done" << endl;
		f2.close();
		
		//	dilating car horn:
		cout << "dilating..." << endl;
		const double currentArray[] = { 0.3, 1. };
 		const double desiredArray[] = { 0.5, 4. };
 		//const double currentArray[] = { 0.1, 0.75, 0.5 };
		//const double desiredArray[] = { 0.3, 0.3, 4.};
		Dilator d( currentArray, currentArray + 2, desiredArray, desiredArray + 2 );
		double maxtime = 0.;
		for ( list<Partial>::iterator it = imp2.partials().begin(); it != imp2.partials().end(); ++it ) {
			d.dilate( *it );
			maxtime = max( maxtime, (*it).endTime() );
		}
		cout << "done." << endl;
		
		/*
		//	synthesize:
		const int carsrate = 44100;
		SampleVector carbuf( carsrate * maxtime );
		Synthesizer carsynth( carbuf, carsrate );
		carsynth.setIterator( new LoFreqBweKludger(1000.) );
		
		cout << "synthesizing" << endl;
		int z = 0;
		for ( list< Partial >::iterator it = imp2.partials().begin(); 
			  it != imp2.partials().end(); 
			  ++it ) {
			carsynth.synthesizePartial( *it );
			if ( ++z % 10 == 0  )
				cout << "\t" << z << " partials..." << endl;
		}
		
		//	write out samples:
		string carname(":::morphing:ncsa morph:dilatedhorn.aiff");
		cout << "writing " << carname << endl;
		AiffFile sfcar( carsrate, 1, 16, carbuf );
		sfcar.write( BinaryFile( carname ) );
		cout << "done." << endl;

		return 0;
		*/
		
		//	import 4Bb partials:
		name = ":::morphing:ncsa morph:elephant3.lemr";
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
		BreakpointMap w;
		w.insertBreakpoint( 0.6, 0. ); // start with clar
		w.insertBreakpoint( 2., 1. );  // end with long amount of flute action.
		
		BreakpointMap flat;
		flat.insertBreakpoint( 0., 0. );
		flat.insertBreakpoint( 1., 0. );
		
		for (double z = 0.; z < 3.0; z += 0.2) {
			cout << "weight at " << z << " is " << w(z) << endl;
		}
		
		Morph m( w );
		//m.setFrequencyFunction( w );
		//m.setAmplitudeFunction( w );
		//m.setBandwidthFunction( w );

	
		cout << "morphine..." << endl;
		m.morph( imp2.partials(), imp.partials() );
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
		string newname(":::morphing:ncsa morph:newelecar.aiff");
		cout << "writing " << newname << endl;
		AiffFile sfout( srate, 1, 16, buf );
		sfout.write( BinaryFile( newname ) );
		
		cout << "done." << endl;

	}
	catch ( Exception & ex ) {
		cout << ex;
	}

	return 0;
}


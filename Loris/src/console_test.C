/*
 *  Hello World for the CodeWarrior
 *  © 1997-1998 Metrowerks Corp.
 *
 *  Questions and comments to:
 *       <mailto:support@metrowerks.com>
 *       <http://www.metrowerks.com/>
 */

//	Hey I am just adding this comment here, see.

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

using namespace std;
using namespace Loris;

//	hey I am just adding this comment down here see?

int main()
{	
	try {		
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
	}
	catch ( Exception & ex ) {
		cout << ex;
	}

	return 0;
}


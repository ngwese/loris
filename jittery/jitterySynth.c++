// ---------------------------------------------------------------------------
//	jitterySynth.c++
//
//	Jittery synthesis program: specify a sdif file, jitter gain, and 
//	attraction.
//
//	kel June 2003
//
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>


// Loris headers
#include <AiffFile.h>
#include <Breakpoint.h>
#include <BreakpointEnvelope.h>
#include <Filter.h>
#include <NoiseGenerator.h>
#include <Partial.h>
#include <PartialList.h>
#include <PartialUtils.h>
#include <SdifFile.h>
#include <SpcFile.h>

#include <ODonnellOscil.h>
#include <JitterySynthesizer.h> 

using namespace std;
using namespace Loris;	

#define SRATE 44100

// ---------------------------------------------------------------------------
//	printUsageAndQuit

void printUsageAndQuit( const char * prog )
{
	cout << "Usage:\n\t" << prog << " sdifname gain attraction [outfilename]\n" << endl;
	cout << "\tjitter gain must be greater than 0." << endl;
	cout << "\tattraction must be between 0 and 1." << endl;
	cout << endl;
	exit(1);
}

// ---------------------------------------------------------------------------
//	main

int main( int argc, char* argv[] )
{
	if ( argc < 4 )
		printUsageAndQuit( argv[0] );
		
	string IFName = argv[1];
	double J = -1, A = -1;
	
	J = atof( argv[2] );
	A = atof( argv[3] );
	
	if ( J < 0 || A < 0 || A > 1 )
		printUsageAndQuit( argv[0] );
		
	string OFName;
	if ( argc > 4 )
		OFName = argv[4];
	else
		OFName = "jitterout.aiff";
	
	try
	{		
		cout << "Importing " << IFName << endl;
		cout << "Generating aiff file " << OFName << endl;

		// import the Partials
		SdifFile f( IFName );
		PartialList & partials = f.partials();
		pair<double,double> span = PartialUtils::timeSpan( partials.begin(), partials.end() );
		cout << "imported " << partials.size() << " partials spanning " 
			 << span.second << " seconds" << endl;
		
		vector< double > output( long(span.second * SRATE) );
		Synthesizer synth( SRATE, output );

		cout << "rendering " << partials.size() << " partials with jitter" << endl;
		BreakpointEnvelope jitter( J );
		BreakpointEnvelope coherence( A );
		synth.configureJitter( jitter, coherence, 500 );
		
		synth( partials.begin(), partials.end() );
		
		cout << "exporting " << output.size() << " samples" << endl;
		AiffFile::Export( OFName, SRATE, 1, 24, 
						  &( output[0] ), &( output[output.size() ]) );
	}
	catch( exception & ex )
	{
		cout << ex.what() << endl;
		return 1;
	}	
	
	cout << "Hey, I'm spent!" << endl;
	return 0;
}


//	
//	console_test.C 
//
//	Macintosh C++ test client for Loris.
//

//	all the Loris headers you could ever want or need:
#include "LorisLib.h"
#include "Synthesizer.h"
#include "Exception.h"
#include "AiffFile.h"
#include "BinaryFile.h"
#include "ImportLemur5.h"
#include "LoFreqBweKludger.h"
#include "Breakpoint.h"
#include "Oscillator.h"
#include "Filter.h"
#include "Morph.h"
#include "Distiller.h"
#include "Map.h"
#include "Dilator.h"
#include "notify.h"

//	all the standard headers you could ever want or need:
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

#include <memory>

//	welcome to namespaces:
using namespace std;
using namespace Loris;

//	client class for performing a two-way morph:
//	(client source code is in the directory Loris/clients)
#include "TwoWayMorph.h"

//	here's a few, defined below:
void configCarElephantMorph( TwoWayMorph & m );
void configClarinetFluteMorph( TwoWayMorph & m );

//
//	test app using a console interface
//
int main()
{	
	//	use the client class TwoWayMorph:
	TwoWayMorph m;
	configClarinetFluteMorph(m);
	
	//	go:
	m.doMorph();

	return 0;
}

//
//	car->elephant morph:
//
void
configCarElephantMorph( TwoWayMorph & m )
{
	m.setFile0( ":::morphing:ncsa morph:carhorn1.lemr" );
	m.setFile1( ":::morphing:ncsa morph:elephant3.lemr" );
	
	//	time points for dilation:
	//	Initial time points can be specified for both sounds,
	//	but both are assumed to have the same target time points.
	m.addTimePoint0( 0.3 );
	m.addTimePoint0( 1. );
	m.addTargetTimePoint( 0.5 );
	m.addTargetTimePoint( 4. );
	
	//	pitch shifting for either or both, in halfsteps:
	m.setPitchShift0( -3 );
	
	//	morphing functions:
	//	Individual morphing functions can also be set using 
	//	freqyBreakpoint(), ampBreakpoint(), and bwBreakpoint().
	m.morphBreakpoint( 0.6, 0. );
	m.morphBreakpoint( 2., 1. );
	
	m.setDestFile(":::morphing:ncsa morph:newelecar.aiff");
}

//
//	clarinet->flute morph:
//
void configClarinetFluteMorph( TwoWayMorph & m )
{
	m.setFile0( ":::sample_sounds:clarinet4G#.lemr" );
	m.setFile1( ":::sample_sounds:flute4D.lemr" );
	
	//	pitch shifting for either or both, in halfsteps:
	m.setPitchShift0( -4 );
	m.setPitchShift1( 2 );
	
	//	morphing functions:
	//	Individual morphing functions can also be set using 
	//	freqyBreakpoint(), ampBreakpoint(), and bwBreakpoint().
	m.morphBreakpoint( 0.6, 0. );
	m.morphBreakpoint( 2., 1. );
	
	m.setDestFile(":::sample_sounds:clariflute.aiff");	
}

/*
 *  Copyright © 1997-2002 Metrowerks Corporation.  All Rights Reserved.
 *
 *  Questions and comments to:
 *       <mailto:support@metrowerks.com>
 *       <http://www.metrowerks.com/>
 */

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

#if defined(__MWERKS__) && __ide_target("Original")
	#include <Synthesizer.h>
#else
	#define JITTER
	#include <ODonnellOscil.h>
	#include <JitterySynthesizer.h> 
#endif


using namespace std;
using namespace Loris;	

#define SRATE 44100

// ---------------------------------------------------------------------------
//	doCello

static void doCello( void )
{
	const std::string IFName = "cello154.F.sdif";
	const std::string OFName = "jello";
	const double Resolution = 123;
	const double MLW = 154;

	cout << "Importing " << IFName << endl;
	cout << "Generating aiff and spc files " << OFName << endl;

	// import the Partials
	SdifFile f( IFName );
	PartialList & partials = f.partials();
	pair<double,double> span = PartialUtils::timeSpan( partials.begin(), partials.end() );
	cout << "imported " << partials.size() << " partials spanning " 
		 << span.second << " seconds." << endl;
	
	// render and export:
	cout << "rendering " << partials.size() << " partials." << endl;
	
	vector< double > output( span.second * SRATE );
	Synthesizer synth( SRATE, output );
	synth( partials.begin(), partials.end() );

	cout << "exporting " << output.size() << " samples." << endl;
	AiffFile::Export( OFName + ".aiff", SRATE, 1, 24, 
					  &(output[0]), &(output[0])+output.size() );

	//cout << "exporting " << partials.size() << " SDIF partials." << endl;
	//SdifFile::Export( OFName + ".sdif", partials );
	
	#define NUM_SPC_PARTIALS 128
	partials.erase( remove_if( partials.begin(), partials.end(), 
							   PartialUtils::label_greater( NUM_SPC_PARTIALS ) ),
					partials.end() );

	cout << "exporting " << partials.size() << " SPC partials." << endl;
	SpcFile::Export( OFName + ".s.spc", partials, 60, false );
}

// ---------------------------------------------------------------------------
//	testNoise
static void testNoise( void )
{
#ifdef JITTER
	NoiseGenerator gen;
	
	std::vector< double > v1( SRATE ), v2( SRATE );
	const double s = 13446;
	gen.reset( s );
	std::generate( v1.begin(), v1.end(), gen );
	gen.reset( s );
	std::generate( v2.begin(), v2.end(), gen );
	
	//	these two should be the same, because there's
	//	no filter:
	Assert( v1 == v2 );
	cout << "exporting " << v1.size() << " unfiltered samples." << endl;
	AiffFile::Export( "unfiltered.aiff", SRATE, 1, 24, 
					  &(v1[0]), &(v1[0])+v1.size() );
	
	const double b[] = { 1. };
	const double a[] = { 1., 0.99 };
	Filter phil( b, b+1, a, a+2, 1 / 100. );
	gen = NoiseGenerator( phil, s );
	
	std::generate( v1.begin(), v1.end(), gen );
	gen.reset( s );
	std::generate( v2.begin(), v2.end(), gen );
	
	//	these two should be the same, because I reset:
	Assert( v1 == v2 );

	cout << "exporting " << v1.size() << " filtered samples." << endl;
	AiffFile::Export( "filtered.aiff", SRATE, 1, 24, 
					  &(v1[0]), &(v1[0])+v1.size() );
					  
	//	Chebychev order 3, cutoff 500 (at 44k), ripple -1.
	//
	//	Coefficients obtained from http://www.cs.york.ac.uk/~fisher/mkfilter/
	//	Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
	//
	static const double Gain = 4.663939184e+04;
	static const double MaCoefs[] = { 1., 3., 3., 1. }; 
	static const double ArCoefs[] = { 1., 2.9258684252, -2.8580608586, 0.9320209046 };
	phil = Filter( MaCoefs, MaCoefs + 4, ArCoefs, ArCoefs + 4, 1 / Gain );

	gen = NoiseGenerator( phil, s );
	
	std::generate( v1.begin(), v1.end(), gen );
	
	cout << "exporting " << v1.size() << " really filtered samples." << endl;
	AiffFile::Export( "cheby.aiff", SRATE, 1, 24, 
					  &(v1[0]), &(v1[0])+v1.size() );
#else
	cout << "cannot run testNoise with the new NoiseGenerator (wrong target), skipping." << endl;
#endif
}

// ---------------------------------------------------------------------------
//	testOscil

static void testOscil( void )
{
#ifndef JITTER
	cout << "cannot test old oscillotor using testOscil, skipping." << endl;
#else

	Breakpoint bp( 210, 0.2, 0, 0 );
	Oscillator osc;
	osc.resetEnvelopes( bp, SRATE );
	double jitter = 1;
	
	//	generate incoherent jitter
	double coherence = 0;
	osc.resetJitter( jitter, coherence, 0 );
	
	std::vector< double > samps( SRATE );
	osc.oscillate( &samps.front(), 1+&samps.back(), bp, SRATE, jitter, coherence, 1 );
	
	cout << "exporting " << samps.size() << " incoherently jittery sinusoidal samples." << endl;
	AiffFile::Export( "osctest.co0.aiff", SRATE, 1, 24, 
					  &samps.front(), 1+&samps.back() );
					  
	//	generate coherent jitter
	coherence = 1;
	osc.resetJitter( jitter, coherence, 0 );
	
	std::fill( samps.begin(), samps.end(), 0 );
	osc.oscillate( &samps.front(), 1+&samps.back(), bp, SRATE, jitter, coherence, 1 );
	
	cout << "exporting " << samps.size() << " coherently jittery sinusoidal samples." << endl;
	AiffFile::Export( "osctest.co1.aiff", SRATE, 1, 24, 
					  &samps.front(), 1+&samps.back() );
	

#endif
}

// ---------------------------------------------------------------------------
//	testSines

static void testSines( void )
{
	const double Pi = 3.14159265358979324;;
	
	//	make N harmonic partials
	const int N = 24;
	PartialList partials;
	for ( int i = 1; i < N+1; ++i )
	{
		Partial p;
		p.setLabel( i );
		
		const double startTime = i * 0.01;
		const double f1 = 90 * i;
		const double f2 = 110 * i;
		const double startFreq = f1 + (startTime * (f2-f1));
		const double amp = 0.6 / N;
		const double phase = startTime * (Pi * ( f1 + startFreq ));
		
		p.insert( startTime, Breakpoint( startFreq, amp, 0, phase ) );
		p.insert( 1.0, Breakpoint( f2, amp, 0, 0 ) );
		p.insert( 2.0, Breakpoint( 0.5*(f1+f2), amp, 0, 0 ) );
		p.insert( 3.0, Breakpoint( f1, amp, 0, 0 ) );
		
		partials.push_back( p );
	}
	
	// render and export:
	cout << "rendering " << partials.size() << " harmonic partials." << endl;
	
	vector< double > output( 3.1 * SRATE );
	Synthesizer synth( SRATE, output, 0.05 );
	
#ifdef JITTER
	BreakpointEnvelope jitter;
	const double J = 2;		// jitter amount
	jitter.insertBreakpoint( 0, J );
	jitter.insertBreakpoint( 1.25, J );
	jitter.insertBreakpoint( 1.5, 0.0 );
	const double A = 1.0; 		// strength of attractor
	BreakpointEnvelope coherence( A );
	
	cout << "synthesizing with coherence cutoff at Partial number 27." << endl;
	synth.configureJitter( jitter, coherence, 27 );
	synth( partials.begin(), partials.end() );
	
	cout << "exporting " << output.size() << " samples." << endl;
	AiffFile::Export( "coherentsines.aiff", SRATE, 1, 24, 
					  &(output[0]), &(output[0])+output.size() );
	
	std::fill( output.begin(), output.end(), 0 );
					  
	cout << "synthesizing with coherence cutoff at Partial number 0." << endl;
	synth.configureJitter( jitter, coherence, 0 );
	synth( partials.begin(), partials.end() );
	cout << "exporting " << output.size() << " samples." << endl;
	AiffFile::Export( "incoherentsines.aiff", SRATE, 1, 24, 
					  &(output[0]), &(output[0])+output.size() );
#else
	synth( partials.begin(), partials.end() );
	cout << "exporting " << output.size() << " samples." << endl;
	AiffFile::Export( "harmonics.aiff", SRATE, 1, 24, 
					  &(output[0]), &(output[0])+output.size() );
#endif
}

// ---------------------------------------------------------------------------
//	testBland

static void testBland( void )
{
#ifdef JITTER
	const double J = 0.15;		// jitter amount
	BreakpointEnvelope jitter( J );
	const double A = 1.0; 		// strength of attractor
	BreakpointEnvelope coherence( A );
#endif

	string infiles[] = { "blandmeow", "blandbell", "blandcello" };
	
	for ( int i = 0; i < 3; ++i )
	{
		string IFName = infiles[i] + ".sdif";
		string OFName = infiles[i];
		
		cout << "Importing " << IFName << endl;
		cout << "Generating aiff files " << OFName + ".xxx.aiff" << endl;

		// import the Partials
		SdifFile f( IFName );
		PartialList & partials = f.partials();
		pair<double,double> span = PartialUtils::timeSpan( partials.begin(), partials.end() );
		cout << "imported " << partials.size() << " partials spanning " 
			 << span.second << " seconds." << endl;
		
		vector< double > output( span.second * SRATE );
		Synthesizer synth( SRATE, output );
#ifdef JITTER
		cout << "rendering " << partials.size() << " partials with coherent modulation." << endl;
		synth.configureJitter( jitter, coherence, 500 );
		synth( partials.begin(), partials.end() );
		
		cout << "exporting " << output.size() << " samples." << endl;
		AiffFile::Export( OFName + ".coherent.aiff", SRATE, 1, 24, 
						  &( output[0] ), &( output[output.size() ]) );
		
		cout << "rendering " << partials.size() << " partials with incoherent modulation." << endl;
		synth.configureJitter( jitter, coherence, 0 );
		std::fill( output.begin(), output.end(), 0 );
		synth( partials.begin(), partials.end() );
		
		cout << "exporting " << output.size() << " samples." << endl;
		AiffFile::Export( OFName + ".incoherent.aiff", SRATE, 1, 24, 
						  &( output[0] ), &( output[output.size() ]) );
#else
		cout << "rendering " << partials.size() << " partials without modulation." << endl;
		synth( partials.begin(), partials.end() );
		
		cout << "exporting " << output.size() << " samples." << endl;
		AiffFile::Export( OFName + ".noPM.aiff", SRATE, 1, 24, 
						  &( output[0] ), &( output[output.size() ]) );
#endif		
	}

}


// ---------------------------------------------------------------------------
//	main

int main()
{
	using namespace std;
	
	cout << "Hello World, this is the jittery synthesizer!" << endl;
	
	try
	{
		// doCello();
		testNoise();
		// testSines();
		testOscil();
		testBland();
	}
	catch( exception & ex )
	{
		cout << ex.what() << endl;
		return 1;
	}	
	
	cout << "Hey, I'm spent!" << endl;
	return 0;
}
/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 *	simplemorph.C
 *
 *	Very simple Loris instrument tone morphing demonstration using 
 *	the C++ interface to Loris.
 *
 * Kelly Fitz, 7 Dec 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "AiffFile.h"
#include "Analyzer.h"
#include "BreakpointEnvelope.h"
#include "Channelizer.h"
#include "Dilator.h"
#include "Distiller.h"
#include "Exception.h"
#include "ExportSdif.h"
#include "ExportSpc.h"
#include "Handle.h"
#include "ImportSdif.h"
#include "Morpher.h"
#include "Partial.h"
#include "Synthesizer.h"
#include "notifier.h"

#include <iostream>
#include <vector>
#include <list>
#include <cmath>

using namespace Loris;

//	forward declarations:
static void getFreqReference( const std::list< Partial > & partials, int numSamples,
							  double minFreq, double maxFreq,
							  BreakpointEnvelope * env_ptr );

int main( )
{
	std::cout << "Welcome to the very simple Loris morphing demo!" << endl;
	std::cout << "Kelly Fitz 2000" << endl << endl;
	std::cout << "Generates a simple linear morph between a " << endl;
	std::cout << "clarinet and a flute." << endl << endl;
	
	try 
	{
		//	analyze clarinet tone
		std::cout << "analyzing clarinet 3G#" << endl;
		Analyzer a(390);
		AiffFile f( "clarinet.aiff" );
		std::vector< double > v( f.sampleFrames() );
		f.getSamples( v.begin(), v.end() );
		
		std::list< Partial > clar;
		a.analyze( v.begin(), v.end(), f.sampleRate() );
		clar.splice( clar.end(), a.partials() );
		
		//	make sure that SDIF I/O is working:
		std::cout << "exporting sdif" << endl;
		ExportSdif xp( 0 );
		xp.write( "clar.sdif", clar );
		std::cout << "importing sdif" << endl;
		ImportSdif ip("clarinet.sdif");
		clar.clear();
		clar.splice( clar.end(), ip.partials() );
		std::cout << "that was fun." << endl;
		
		BreakpointEnvelope clarRef;
		getFreqReference( clar, 20, 0, 1000, &clarRef );
							  
		Channelizer ch( clarRef, 1 );
		ch.channelize( clar.begin(), clar.end() );
		
		Distiller still;
		still.distill( clar );

		std::cout << "shifting pitch of " << clar.size() << " Partials by 600 cents" << endl;
		double pscale = std::pow(2., (0.01 * -600) /12.);
		for ( std::list< Partial >::iterator pIter = clar.begin(); 
			  pIter != clar.end(); 
			  ++pIter ) 
		{
			for ( PartialIterator jack = pIter->begin(); jack != pIter->end(); ++jack ) 
			{		
				jack->setFrequency( jack->frequency() * pscale );
			}
		}	


		// check clarinet synthesis:
		std::cout << "checking clarinet synthesis" << endl;
		std::fill( v.begin(), v.end(), 0. );
		Synthesizer synth( f.sampleRate(), v.begin(), v.end() );
		for ( std::list< Partial >::iterator pIter = clar.begin(); 
			  pIter != clar.end(); 
			  ++pIter ) 
		{
			synth.synthesize( *pIter );
		}
		AiffFile::Export( "clarOK.aiff", f.sampleRate(), 1, 16, v.begin(), v.end() ); 	
		
		//	analyze flute tone
		std::cout << "analyzing flute 3D" << endl;
		a.configure(270);
		f = AiffFile( "flute.aiff" );
		v = std::vector< double >( f.sampleFrames() );
		f.getSamples( v.begin(), v.end() );
		
		std::list< Partial > flut;
		a.analyze( v.begin(), v.end(), f.sampleRate() );
		flut.splice( flut.end(), a.partials() );

		BreakpointEnvelope flutRef;
		getFreqReference( flut, 20, 0, 1000, &flutRef );
							  
		ch = Channelizer( flutRef, 1 );
		ch.channelize( flut.begin(), flut.end() );

		still.distill( flut );

		// check flute synthesis:
		std::cout << "checking flute synthesis" << endl;
		std::fill( v.begin(), v.end(), 0. );
		synth = Synthesizer( f.sampleRate(), v.begin(), v.end() );
		for ( std::list< Partial >::iterator pIter = flut.begin(); 
			  pIter != flut.end(); 
			  ++pIter ) 
		{
			synth.synthesize( *pIter );
		}
		AiffFile::Export( "flutOK.aiff", f.sampleRate(), 1, 16, v.begin(), v.end() ); 	
		
			
		// perform temporal dilation
		double flute_times[] = {0.4, 1.};
		double clar_times[] = {0.2, 1.};
		double tgt_times[] = {0.3, 1.2};

		std::cout << "dilating sounds to match (" << tgt_times[0] << ", " 
				  << tgt_times[1] << ")" << endl;
		std::cout << "flute times: (" << flute_times[0] << ", " 
				  << flute_times[1] << ")" << endl;
		Dilator dil( flute_times, tgt_times, 2 );
		dil.dilate( flut.begin(), flut.end() );
		
		std::cout << "clarinet times: (" << clar_times[0] << ", " 
				  << clar_times[1] << ")" << endl;
		Dilator dil2( clar_times, tgt_times, 2 );	//	 no assignment operator yet
		dil2.dilate( clar.begin(), clar.end() );
		
		
		// perform morph:
		std::cout << "morphing flute and clarinet" << endl;
		BreakpointEnvelope mf;
		mf.insertBreakpoint( 0.6, 0 );
		mf.insertBreakpoint( 2, 1 );
		Morpher m( mf );
		m.morph( clar.begin(), clar.end(), flut.begin(), flut.end() );

		// check flute synthesis:
		std::cout << "synthesizing morph" << endl;
		std::cout << "computing duration..." << endl;
		double maxtime = 0.;
		std::list< Partial >::const_iterator it;
		for ( it = m.partials().begin(); it != m.partials().end(); ++it ) 
		{
			maxtime = std::max( maxtime, it->endTime() );
		}
		std::cout << maxtime << " seconds" << endl;

		v = std::vector< double >( (maxtime + Partial::FadeTime()) * f.sampleRate() );
		synth = Synthesizer( f.sampleRate(), v.begin(), v.end() );
		for ( std::list< Partial >::iterator pIter = m.partials().begin(); 
			  pIter != m.partials().end(); 
			  ++pIter ) 
		{
			synth.synthesize( *pIter );
		}
		AiffFile::Export( "morph.test.aiff", f.sampleRate(), 1, 16, v.begin(), v.end() ); 	

	}
	catch( Exception & ex ) 
	{
		std::cout << "Caught Loris exception: " << ex.what() << endl;
		return 1;
	}
	catch( std::exception & ex ) 
	{
		std::cout << "Caught std C++ exception: " << ex.what() << endl;
		return 1;
	}

	std::cout << "bye" << endl;
	return 0;
}

//
//	This function simulates the behavior of the createFreqReference()
//	function in the procedural interface (it hasn't yet made it into
//	the C++ core of Loris).
//
static void getFreqReference( const std::list< Partial > & partials, int numSamples,
							  double minFreq, double maxFreq,
							  BreakpointEnvelope * env_ptr )
{
	if ( numSamples <= 0 )
		Throw( InvalidArgument, "number of samples in frequency reference must be positive." );
	
	if ( maxFreq < minFreq )
		std::swap( minFreq, maxFreq );
		
	//	find the longest Partial in the given frequency range:
	std::list< Partial >::const_iterator longest = partials.end();
	for ( std::list< Partial >::const_iterator it = partials.begin(); 
		  it != partials.end(); 
		  ++it ) 
	{
		//	evaluate the Partial's frequency at its loudest
		//	(highest sinusoidal amplitude) Breakpoint:
		PartialConstIterator partialIter = it->begin();
		double maxAmp = 
			partialIter->amplitude() * std::sqrt( 1. - partialIter->bandwidth() );
		double time = partialIter.time();
		
		for ( ++partialIter; partialIter != it->end(); ++partialIter ) 
		{
			double a = partialIter->amplitude() * 
						std::sqrt( 1. - partialIter->bandwidth() );
			if ( a > maxAmp ) 
			{
				maxAmp = a;
				time = partialIter.time();
			}
		}			
		double compareFreq = it->frequencyAt( time );
		
		
		if ( compareFreq < minFreq || compareFreq > maxFreq )
			continue;
			
		if ( longest == partials.end() || it->duration() > longest->duration() ) 
		{
			longest = it;
		}
	}	
	
	if ( longest == partials.end() ) 
	{
		Throw( InvalidArgument, "no partials found in the specified frequency range" );
	}

	//	find n samples, ignoring the end points:
	double dt = longest->duration() / (numSamples + 1.);
	for ( int i = 0; i < numSamples; ++i ) 
	{
		double t = longest->startTime() + ((i+1) * dt);
		double f = longest->frequencyAt(t);
		env_ptr->insertBreakpoint( t, f );
	}
}

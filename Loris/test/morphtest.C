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
 *	morphtest.C
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

#include <AiffFile.h>
#include <Analyzer.h>
#include <BreakpointEnvelope.h>
#include <Channelizer.h>
#include <Dilator.h>
#include <Distiller.h>
#include <Exception.h>
#include <FrequencyReference.h>
#include <Morpher.h>
#include <Notifier.h>
#include <Partial.h>
#include <SdifFile.h>
#include <Synthesizer.h>

#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include <cstdlib>
#include <string>

using namespace Loris;

int main( )
{
	std::cout << "Welcome to the very simple Loris C++ morphing demo!" << endl;
	std::cout << "Kelly Fitz 2000" << endl << endl;
	std::cout << "Generates a simple linear morph between a " << endl;
	std::cout << "clarinet and a flute using the C++ library." << endl << endl;
	
	std::string path(""); 
	if ( std::getenv("srcdir") ) 
	{
		path = std::getenv("srcdir");
		path = path + "/";
	}

	try 
	{
		//	analyze clarinet tone
		std::cout << "analyzing clarinet 3G#" << endl;
		Analyzer a(415*.8, 415*1.6);
		AiffFile f( path + "clarinet.aiff" );
		std::vector< double > v( f.sampleFrames() );
		f.getSamples( v.begin(), v.end() );
		
		std::list< Partial > clar;
		a.analyze( v.begin(), v.end(), f.sampleRate() );
		clar.splice( clar.end(), a.partials() );
		
		FrequencyReference clarRef( clar.begin(), clar.end(), 0, 1000, 20 );
		Channelizer ch( clarRef.envelope() , 1 );
		ch.channelize( clar.begin(), clar.end() );
		
		Distiller still;
		still.distill( clar );

		//	make sure that SDIF I/O is working:
		std::cout << "exporting sdif" << endl;
		SdifFile::Export( "clarinet.ctest.sdif", clar );
		std::cout << "importing sdif" << endl;
		SdifFile ip("clarinet.ctest.sdif");
		clar.clear();
		clar.splice( clar.end(), ip.partials() );
		std::cout << "that was fun." << endl;
		
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
		synth.synthesize( clar.begin(), clar.end() );
		AiffFile::Export( "clarOK.ctest.aiff", f.sampleRate(), 1, 16, v.begin(), v.end() ); 	
		
		//	analyze flute tone
		std::cout << "analyzing flute 3D" << endl;
		a = Analyzer(270);
		f = AiffFile( path + "flute.aiff" );
		v = std::vector< double >( f.sampleFrames() );
		f.getSamples( v.begin(), v.end() );
		
		std::list< Partial > flut;
		a.analyze( v.begin(), v.end(), f.sampleRate() );
		flut.splice( flut.end(), a.partials() );

		FrequencyReference flutRef( flut.begin(), flut.end(), 0, 1000, 20 );
		ch = Channelizer( flutRef.envelope(), 1 );
		ch.channelize( flut.begin(), flut.end() );

		still.distill( flut );

		// check flute synthesis:
		std::cout << "checking flute synthesis" << endl;
		std::fill( v.begin(), v.end(), 0. );
		synth = Synthesizer( f.sampleRate(), v.begin(), v.end() );
		synth.synthesize( flut.begin(), flut.end() );
		AiffFile::Export( "flutOK.ctest.aiff", f.sampleRate(), 1, 16, v.begin(), v.end() ); 	
		
			
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

		v = std::vector< double >( long( (maxtime + Partial::FadeTime()) * f.sampleRate() ) );
		synth = Synthesizer( f.sampleRate(), v.begin(), v.end() );
		synth.synthesize(  m.partials().begin(), m.partials().end() );
		AiffFile::Export( "morph.ctest.aiff", f.sampleRate(), 1, 16, v.begin(), v.end() ); 	

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

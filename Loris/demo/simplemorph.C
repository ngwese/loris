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
#include "Distiller.h"
#include "Exception.h"
#include "Handle.h"
#include "Partial.h"
#include "Synthesizer.h"
#include "notifier.h"

#include <iostream>
#include <vector>
#include <list>
#include <cmath>

using namespace Loris;

int main( )
{
	std::cout << "Welcome to the very simple Loris morphing demo!" << endl;
	std::cout << "Kelly Fitz 2000" << endl << endl;
	std::cout << "Generates a simple linear morph between a " << endl;
	std::cout << "clarinet and a flute." << endl << endl;
	
	/*
	std::cout << "here is a BreakpointEnvelope" << endl;
	BreakpointEnvelope env;
	
	{
		std::cout << "here is another (copied) BreakpointEnvelope" << endl;
		BreakpointEnvelope * env2 = new BreakpointEnvelope( env );
		env2->insertBreakpoint( 1., 2. );
		std::cout << "deleting the copied BreakpointEnvelope" << endl;
		delete env2;
	}
	*/
	try 
	{
	//	analyze clarinet tone
		std::cout << "analyzing clarinet 3G#" << endl;
		Analyzer a(270);
		AiffFile f( "clarinet.aiff" );
		std::vector< double > v( f.sampleFrames() );
		f.getSamples( v.begin(), v.end() );
		
		std::list< Partial > clar;
		a.analyze( v.begin(), v.end(), f.sampleRate() );
		clar.splice( clar.end(), a.partials() );

		Handle< BreakpointEnvelope > clarRef( 416 );
		Channelizer ch( Handle< Envelope >(clarRef), 1 );
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
		Synthesizer synth( f.sampleRate(), v.begin(), v.end() );
		for ( std::list< Partial >::iterator pIter = clar.begin(); 
			  pIter != clar.end(); 
			  ++pIter ) 
		{
			synth.synthesize( *pIter );
		}
		AiffFile::Export( "clarOK.aiff", f.sampleRate(), 1, 16, v.begin(), v.end() ); 	
		
		
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
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
 *	lorisUtilities_pi.C
 *
 *	A component of the C-linkable procedural interface for Loris. 
 *
 *	Main components of the Loris procedural interface:
 *	- object interfaces - Analyzer, Synthesizer, Partial, PartialIterator, 
 *		PartialList, PartialListIterator, Breakpoint, BreakpointEnvelope,  
 *		and SampleVector need to be (opaque) objects in the interface, 
 * 		either because they hold state (e.g. Analyzer) or because they are 
 *		fundamental data types (e.g. Partial), so they need a procedural 
 *		interface to their member functions. All these things need to be 
 *		opaque pointers for the benefit of C.
 *	- non-object-based procedures - other classes in Loris are not so stateful,
 *		and have sufficiently narrow functionality that they need only 
 *		procedures, and no object representation.
 *	- utility functions - some procedures that are generally useful but are
 *		not yet part of the Loris core are also defined.
 *	- notification and exception handlers - all exceptions must be caught and
 *		handled internally, clients can specify an exception handler and 
 *		a notification function (the default one in Loris uses printf()).
 *
 *	This file defines the utility functions that are useful, and in many 
 *	cases trivial in C++ (usign the STL for example) but are not represented
 *	by classes in the Loris core.
 *
 * Kelly Fitz, 10 Nov 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#define LORIS_OPAQUE_POINTERS 0
#include "loris.h"
#include "lorisException_pi.h"

#include "BreakpointEnvelope.h"
#include "Exception.h"
#include "notifier.h"
#include "Partial.h"
#include "PartialUtils.h"

#include <algorithm>
#include <list>
#include <memory>

using namespace Loris;

/* ---------------------------------------------------------------- */
/*		utility functions
/*
/*	These procedures are generally useful but are not yet  
	represented by classes in the Loris core.
 */
 
/* ---------------------------------------------------------------- */
/*        copyByLabel        
/*
/*	Append copies of Partials in the source PartialList having the
	specified label to the destination PartialList. The source list
	is unmodified.
 */
extern "C"
void copyByLabel( const PartialList * src, long label, PartialList * dst )
{
	try 
	{
		std::list< Partial >::const_iterator it = src->begin();
		for ( it = std::find_if( it, src->end(), 
					std::bind2nd( PartialUtils::label_equals(), label ) );
			  it != src->end();
			  it = std::find_if( ++it, src->end(), 
					std::bind2nd( PartialUtils::label_equals(), label ) ) )
		{
			 dst->push_back( *it );
		}
		
		
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in selectPartials(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in selectPartials(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        createFreqReference        
/*
/*	Return a newly-constructed BreakpointEnvelope comprising the
	specified number of samples of the frequency envelope of the
	longest Partial in a PartialList. Only Partials whose frequency
	at the Partial's loudest (highest amplitude) breakpoint is
	within the given frequency range are considered. 
	
	For very simple sounds, this frequency reference may be a 
	good first approximation to a reference envelope for
	channelization (see channelize()).
	
	Clients are responsible for disposing of the newly-constructed 
	BreakpointEnvelope.
 */
extern "C"
BreakpointEnvelope * 
createFreqReference( PartialList * partials, int numSamples,
					 double minFreq, double maxFreq )
{
	try 
	{
		ThrowIfNull((PartialList *) partials);
		
		if ( numSamples <= 0 )
			Throw( InvalidArgument, "number of samples in frequency reference must be positive." );
		
		if ( maxFreq < minFreq )
			std::swap( minFreq, maxFreq );
			
		//	find the longest Partial in the given frequency range:
		PartialList::iterator longest = partials->end();
		for ( PartialList::iterator it = partials->begin(); 
			  it != partials->end(); 
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
				
			if ( longest == partials->end() || it->duration() > longest->duration() ) 
			{
				longest = it;
			}
		}	
		
		if ( longest == partials->end() ) 
		{
			Throw( InvalidArgument, "no partials found in the specified frequency range" );
		}
	
		//	use auto_ptr to manage memory in case 
		//	an exception is generated (hard to imagine):
		std::auto_ptr< BreakpointEnvelope > env_ptr( new BreakpointEnvelope() );

		//	find n samples, ignoring the end points:
		double dt = longest->duration() / (numSamples + 1.);
		for ( int i = 0; i < numSamples; ++i ) 
		{
			double t = longest->startTime() + ((i+1) * dt);
			double f = longest->frequencyAt(t);
			env_ptr->insertBreakpoint( t, f );
		}
		
		return env_ptr.release();
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in createFreqReference(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in createFreqReference(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return NULL;
}

/* ---------------------------------------------------------------- */
/*        scaleAmp        
/*
/*	Scale the amplitude of the Partials in a PartialList according 
	to an envelope representing a time-varying amplitude scale value.
 */
extern "C"
void scaleAmp( PartialList * partials, BreakpointEnvelope * ampEnv )
{
	try
	{
		ThrowIfNull((PartialList *) partials);
		ThrowIfNull((BreakpointEnvelope *) ampEnv);

		notifier << "scaling amplitude of " << partials->size() << " Partials" << endl;

		for ( PartialList::iterator pIter = partials->begin(); 
			  pIter != partials->end(); 
			  ++pIter ) 
		{
			for ( PartialIterator jack = pIter->begin(); jack != pIter->end(); ++jack ) 
			{		
				jack->setAmplitude( jack->amplitude() * ampEnv->valueAt(jack.time()) );
			}
		}	
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in scaleAmp(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in scaleAmp(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        scaleNoiseRatio        
/*
/*	Scale the relative noise content of the Partials in a PartialList 
	according to an envelope representing a (time-varying) noise energy 
	scale value.
 */
extern "C"
void scaleNoiseRatio( PartialList * partials, BreakpointEnvelope * noiseEnv )
{
	try 
	{
		ThrowIfNull((PartialList *) partials);
		ThrowIfNull((BreakpointEnvelope *) noiseEnv);

		notifier << "scaling noise ratio of " << partials->size() << " Partials" << endl;

		for ( PartialList::iterator pIter = partials->begin(); 
			  pIter != partials->end(); 
			  ++pIter ) 
		{
			for ( PartialIterator jack = pIter->begin(); jack != pIter->end(); ++jack ) 
			{		
				//	compute new bandwidth value:
				double bw = jack->bandwidth();
				if ( bw < 1. ) 
				{
					double ratio = bw  / (1. - bw);
					ratio *= noiseEnv->valueAt(jack.time());
					bw = ratio / (1. + ratio);
				}
				else 
				{
					bw = 1.;
				}
				
				jack->setBandwidth( bw );
			}
		}	
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in scaleNoiseRatio(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in scaleNoiseRatio(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        shiftPitch        
/*
/*	Shift the pitch of all Partials in a PartialList according to 
	the given pitch envelope. The pitch envelope is assumed to have 
	units of cents (1/100 of a halfstep).
 */
extern "C"
void shiftPitch( PartialList * partials, BreakpointEnvelope * pitchEnv )
{
	try 
	{
		ThrowIfNull((PartialList *) partials);
		ThrowIfNull((BreakpointEnvelope *) pitchEnv);

		notifier << "shifting pitch of " << partials->size() << " Partials" << endl;
		
		for ( PartialList::iterator pIter = partials->begin(); 
			  pIter != partials->end(); 
			  ++pIter ) 
		{
			for ( PartialIterator jack = pIter->begin(); jack != pIter->end(); ++jack ) 
			{		
				//	compute frequency scale:
				double scale = 
					std::pow(2., (0.01 * pitchEnv->valueAt(jack.time())) /12.);				
				jack->setFrequency( jack->frequency() * scale );
			}
		}	
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in shiftPitch(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in shiftPitch(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}


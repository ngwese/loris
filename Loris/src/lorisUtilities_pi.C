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

#include "loris.h"
#include "lorisException_pi.h"

#include <BreakpointEnvelope.h>
#include <Exception.h>
#include <Notifier.h>
#include <Partial.h>
#include <PartialUtils.h>

#include <algorithm>
#include <cmath>
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

		PartialList::iterator listPos;
		for ( listPos = partials->begin(); listPos != partials->end(); ++listPos ) 
		{
			PartialIterator envPos;
			for ( envPos = listPos->begin(); envPos != listPos->end(); ++envPos ) 
			{		
				envPos.breakpoint().setAmplitude( envPos.breakpoint().amplitude() * ampEnv->valueAt(envPos.time()) );
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

		PartialList::iterator listPos;
		for ( listPos = partials->begin(); listPos != partials->end(); ++listPos ) 
		{
			PartialIterator envPos;
			for ( envPos = listPos->begin(); envPos != listPos->end(); ++envPos ) 
			{		
				//	compute new bandwidth value:
				double bw = envPos.breakpoint().bandwidth();
				if ( bw < 1. ) 
				{
					double ratio = bw  / (1. - bw);
					ratio *= noiseEnv->valueAt(envPos.time());
					bw = ratio / (1. + ratio);
				}
				else 
				{
					bw = 1.;
				}
				
				envPos.breakpoint().setBandwidth( bw );
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
		
		PartialList::iterator listPos;
		for ( listPos = partials->begin(); listPos != partials->end(); ++listPos ) 
		{
			PartialIterator envPos;
			for ( envPos = listPos->begin(); envPos != listPos->end(); ++envPos ) 
			{		
				//	compute frequency scale:
				double scale = 
					std::pow(2., (0.01 * pitchEnv->valueAt(envPos.time())) /12.);				
				envPos.breakpoint().setFrequency( envPos.breakpoint().frequency() * scale );
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


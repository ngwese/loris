/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2004 by Kelly Fitz and Lippold Haken
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
#include <functional>
#include <iterator>
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
/*	Old name for copyLabeled.
 */
extern "C"
void copyByLabel( const PartialList * src, long label, PartialList * dst )
{
	copyLabeled( src, label, dst );
}

/* ---------------------------------------------------------------- */
/*        copyLabeled        
/*
/*	Append copies of Partials in the source PartialList having the
	specified label to the destination PartialList. The source list
	is unmodified.
 */
extern "C"
void copyLabeled( const PartialList * src, long label, PartialList * dst )
{
	try 
	{
		ThrowIfNull((PartialList *) src);
		ThrowIfNull((PartialList *) dst);
		
		/*
		std::list< Partial >::const_iterator it = src->begin();
		for ( it = std::find_if( it, src->end(), PartialUtils::label_equals(label) );
			  it != src->end();
			  it = std::find_if( ++it, src->end(), PartialUtils::label_equals(label) ) )
		{
			 dst->push_back( *it );
		}
		*/
		std::remove_copy_if( src->begin(), src->end(), std::back_inserter( *dst ),
							 std::not1( PartialUtils::label_equals(label) ) );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in copyLabeled(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in copyLabeled(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}
 
/* ---------------------------------------------------------------- */
/*        crop        
/*
/*	Trim Partials by removing Breakpoints outside a specified time span.
	Insert a Breakpoint at the boundary when cropping occurs.
 */
extern "C"
void crop( PartialList * partials, double t1, double t2 )
{
	try
	{
		ThrowIfNull((PartialList *) partials);

		notifier << "cropping " << partials->size() << " Partials" << endl;

		std::for_each( partials->begin(), partials->end(), PartialUtils::crop( t1, t2 ) );	
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in crop(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in crop(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        extractLabeled        
/*
/*	Remove Partials in the source PartialList having the specified
    label from the source list and append them to the destination 
    PartialList.
 */
extern "C"
void extractLabeled( PartialList * src, long label, PartialList * dst )
{
	try 
	{
		ThrowIfNull((PartialList *) src);
		ThrowIfNull((PartialList *) dst);

		/*
		std::list< Partial >::iterator it = src->begin();
		for ( it = std::find_if( it, src->end(), PartialUtils::label_equals(label) );
			  it != src->end();
			  it = std::find_if( it, src->end(), PartialUtils::label_equals(label) ) )
		{
			std::list< Partial >::iterator remove_me = it++;
			dst->splice( dst->end(), *src, remove_me );
		}
		*/
		std::list< Partial >::iterator it = 
			std::stable_partition( src->begin(), src->end(), 
								   std::not1( PartialUtils::label_equals(label) ) );
		dst->splice( dst->end(), *src, it, src->end() );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in extractLabeled(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in extractLabeled(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        removeLabeled        
/*
/*	Remove from a PartialList Partials having the specified label. 
 */
extern "C"
void removeLabeled( PartialList * src, long label )
{
	try 
	{
		ThrowIfNull((PartialList *) src);
		std::list< Partial >::iterator it = 
			std::remove_if( src->begin(), src->end(), 
							PartialUtils::label_equals(label) );
		src->erase( it, src->end() );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in removeLabeled(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in removeLabeled(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}


/* ---------------------------------------------------------------- */
/*        spliceByLabel        
/*
/*	Old name for extractLabeled. 
 */
extern "C"
void spliceByLabel( PartialList * src, long label, PartialList * dst )
{
	extractLabeled( src, label, dst );
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

		std::for_each( partials->begin(), partials->end(), PartialUtils::scale_amp( *ampEnv ) );
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
/*        scaleBandwidth        
/*
/*	Scale the bandwidth of the Partials in a PartialList according 
	to an envelope representing a time-varying bandwidth scale value.
 */
extern "C"
void scaleBandwidth( PartialList * partials, BreakpointEnvelope * bwEnv )
{
	try
	{
		ThrowIfNull((PartialList *) partials);
		ThrowIfNull((BreakpointEnvelope *) bwEnv);

		notifier << "scaling bandwidth of " << partials->size() << " Partials" << endl;

		std::for_each( partials->begin(), partials->end(), PartialUtils::scale_bandwidth( *bwEnv ) );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in scaleBandwidth(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in scaleBandwidth(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        scaleFrequency        
/*
/*	Scale the frequency of the Partials in a PartialList according 
	to an envelope representing a time-varying frequency scale value.
 */
extern "C"
void scaleFrequency( PartialList * partials, BreakpointEnvelope * freqEnv )
{
	try
	{
		ThrowIfNull((PartialList *) partials);
		ThrowIfNull((BreakpointEnvelope *) freqEnv);

		notifier << "scaling frequency of " << partials->size() << " Partials" << endl;

		std::for_each( partials->begin(), partials->end(), PartialUtils::scale_frequency( *freqEnv ) );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in scaleFrequency(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in scaleFrequency(): " );
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

		std::for_each( partials->begin(), partials->end(), PartialUtils::scale_noise_ratio( *noiseEnv ) );
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
		
		std::for_each( partials->begin(), partials->end(), PartialUtils::shift_pitch( *pitchEnv ) );
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

/* ---------------------------------------------------------------- */
/*        shiftTime       
/*
/*	Shift the time of all the Breakpoints in all Partials in a 
	PartialList by a constant amount.
 */
extern "C"
void shiftTime( PartialList * partials, double offset )
{
	try 
	{
		ThrowIfNull((PartialList *) partials);

		notifier << "shifting time of " << partials->size() << " Partials" << endl;
		
		std::for_each( partials->begin(), partials->end(), PartialUtils::shift_time( offset ) );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in shiftTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in shiftTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}



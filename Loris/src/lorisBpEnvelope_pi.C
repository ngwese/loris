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
 *	lorisBpEnvelope_pi.C
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
 *	This file defines the procedural interface for the Loris 
 *	BreakpointEnvelope class.
 *
 * Kelly Fitz, 10 Nov 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include "config.h"
#endif

#include "loris.h"
#include "lorisException_pi.h"

#include "BreakpointEnvelope.h"
#include "Notifier.h"

using namespace Loris;


/* ---------------------------------------------------------------- */
/*		BreakpointEnvelope object interface								
/*
/*	A BreakpointEnvelope represents a linear segment breakpoint 
	function with infinite extension at each end (that is, the 
	values past either end of the breakpoint function have the 
	values at the nearest end).
 */
 
/* ---------------------------------------------------------------- */
/*        createBreakpointEnvelope
/*
/*	Construct and return a new BreakpointEnvelope having no 
	breakpoints and an implicit value of 0. everywhere, 
	until the first breakpoint is inserted.			
 */
extern "C"
BreakpointEnvelope * createBreakpointEnvelope( void )
{
	try 
	{
		debugger << "creating BreakpointEnvelope" << endl;
		return new BreakpointEnvelope();
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in createBreakpointEnvelope(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in createBreakpointEnvelope(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return NULL;
}

/* ---------------------------------------------------------------- */
/*        copyBreakpointEnvelope
/*
/*	Construct and return a new BreakpointEnvelope that is an
	exact copy of the specified BreakpointEnvelopes, having 
	an identical set of breakpoints.	
 */
extern "C"
BreakpointEnvelope * copyBreakpointEnvelope( const BreakpointEnvelope * ptr_this )
{
	try 
	{
		debugger << "copying BreakpointEnvelope" << endl;
		return new BreakpointEnvelope( *ptr_this );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in copyBreakpointEnvelope(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in copyBreakpointEnvelope(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return NULL;
}

/* ---------------------------------------------------------------- */
/*        destroyBreakpointEnvelope       
/*
/*	Destroy this BreakpointEnvelope. 								
 */
extern "C"
void destroyBreakpointEnvelope( BreakpointEnvelope * ptr_this )
{
	try 
	{
		ThrowIfNull((BreakpointEnvelope *) ptr_this);
		
		debugger << "deleting BreakpointEnvelope" << endl;
		delete ptr_this;
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in destroyAnalyzer(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in destroyAnalyzer(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        breakpointEnvelope_insertBreakpoint      
/*
/*	Insert a breakpoint representing the specified (time, value) 
	pair into this BreakpointEnvelope. If there is already a 
	breakpoint at the specified time, it will be replaced with 
	the new breakpoint.
 */
extern "C"
void breakpointEnvelope_insertBreakpoint( BreakpointEnvelope * ptr_this,
					double time, double val )
{
	try 
	{
		ThrowIfNull((BreakpointEnvelope *) ptr_this);
		
		debugger << "inserting point (" << time << ", " << val 
				 << ") into BreakpointEnvelope" << endl;
		ptr_this->insertBreakpoint(time, val);
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in breakpointEnvelope_insertBreakpoint(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in breakpointEnvelope_insertBreakpoint(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        breakpointEnvelope_valueAt     
/*
/*	Return the interpolated value of this BreakpointEnvelope at the 
	specified time.							
 */
extern "C"
double breakpointEnvelope_valueAt( const BreakpointEnvelope * ptr_this, 
								   double time )
{
	try 
	{
		ThrowIfNull((BreakpointEnvelope *) ptr_this);
		return ptr_this->valueAt(time);
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in breakpointEnvelope_valueAt(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in breakpointEnvelope_valueAt(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}


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
 *	lorisPartialList_pi.C
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
 *	This file contains the procedural interface for the Loris 
 *	PartialList (std::list< Partial >) class.
 *
 * Kelly Fitz, 10 Nov 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "loris.h"
#include "lorisException_pi.h"

#include <Partial.h>
#include <Notifier.h>

#include <list>

using namespace Loris;


/* ---------------------------------------------------------------- */
/*		PartialList object interface								
/*
/*	A PartialList represents a collection of Bandwidth-Enhanced 
	Partials, each having a trio of synchronous, non-uniformly-
	sampled breakpoint envelopes representing the time-varying 
	frequency, amplitude, and noisiness of a single bandwidth-
	enhanced sinusoid.

	For more information about Bandwidth-Enhanced Partials and the  
	Reassigned Bandwidth-Enhanced Additive Sound Model, refer to
	the Loris website: www.cerlsoundgroup.org/Loris/.

	In C++, a PartialList * is a std::list< Loris::Partial > *.
 */ 

/* ---------------------------------------------------------------- */
/*        createPartialList        
/*
/*	Return a new empty PartialList.
 */
extern "C"
PartialList * createPartialList( void )
{
	try 
	{
		debugger << "creating empty PartialList" << endl;
		return new std::list< Partial >;
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in createPartialList(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in createPartialList(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return NULL;
}

/* ---------------------------------------------------------------- */
/*        destroyPartialList        
/*
/*	Destroy this PartialList.
 */
extern "C"
void destroyPartialList( PartialList * ptr_this )
{
	try 
	{
		ThrowIfNull((PartialList *) ptr_this);

		debugger << "deleting PartialList containing " << ptr_this->size() << " Partials" << endl;
		delete ptr_this;
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in destroyPartialList(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in destroyPartialList(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        partialList_clear        
/*
/*	Remove (and destroy) all the Partials from this PartialList,
	leaving it empty.
 */
extern "C"
void partialList_clear( PartialList * ptr_this )
{
	try 
	{
		ThrowIfNull((PartialList *) ptr_this);
		ptr_this->clear();
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in partialList_clear(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in partialList_clear(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        partialList_copy        
/*
/*	Make this PartialList a copy of the source PartialList by making
	copies of all of the Partials in the source and adding them to 
	this PartialList.
 */
extern "C"
void partialList_copy( PartialList * dst, const PartialList * src )
{
	try 
	{
		ThrowIfNull((PartialList *) dst);
		ThrowIfNull((PartialList *) src);

		debugger << "copying PartialList containing " << src->size() << " Partials" << endl;
		*dst = *src;
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in partialList_copy(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in partialList_copy(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        partialList_size        
/*
/*	Return the number of Partials in this PartialList.
 */
extern "C"
unsigned long partialList_size( const PartialList * ptr_this )
{
	try 
	{
		ThrowIfNull((PartialList *) ptr_this);
		return ptr_this->size();
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in partialList_size(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in partialList_size(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        partialList_splice        
/*
/*	Splice all the Partials in the source PartialList onto the end of
	this PartialList, leaving the source empty.
 */
extern "C"
void partialList_splice( PartialList * dst, PartialList * src )
{
	try 
	{
		ThrowIfNull((PartialList *) dst);
		ThrowIfNull((PartialList *) src);

		debugger << "splicing PartialList containing " << src->size() << " Partials" 
				 << " into PartialList containing " << dst->size() << " Partials"<< endl;
		dst->splice( dst->end(), *src );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in partialList_splice(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in partialList_splice(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}


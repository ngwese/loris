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
 *	lorisSampleVector_pi.C
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
 *	This file contains the procedural interface for a SampleVector 
 *	(std::vector< double >) class.
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

#include <Notifier.h>

#include <vector>

using namespace Loris;

/* ---------------------------------------------------------------- */
/*		SampleVector object interface								
/*
/*	A SampleVector represents a vector of floating point samples of
	an audio waveform. In Loris, the samples are assumed to have 
	values on the range (-1., 1.) (though this is not enforced or 
	checked). 

	In C++, a SampleVector * is a std::vector< double > *.
 */

/* ---------------------------------------------------------------- */
/*        createSampleVector        
/*
/*	Return a new SampleVector having the specified number of samples,
	all of whose value is 0.
 */
extern "C"
SampleVector * createSampleVector( unsigned long size )
{
	try 
	{
		debugger << "creating SampleVector of length " << size << endl;
		return new std::vector< double >( size );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in createSampleVector(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in createSampleVector(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return NULL;
}

/* ---------------------------------------------------------------- */
/*        destroySampleVector        
/*
/*	Destroy this SampleVector.
 */
extern "C"
void destroySampleVector( SampleVector * ptr_this )
{
	try 
	{
		ThrowIfNull((SampleVector *) ptr_this);
		debugger << "deleting SampleVector" << endl;
		delete ptr_this;
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in destroySampleVector(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in destroySampleVector(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        sampleVector_copy        
/*
/*	Make this SampleVector a copy of the source SampleVector, having 
	the same number of samples, and samples at every position in this
	SampleVector having the same value as the corresponding sample in
	the source.
 */
extern "C"
void sampleVector_copy( SampleVector * dst, const SampleVector * src )
{
	try 
	{
		ThrowIfNull((SampleVector *) dst);
		ThrowIfNull((SampleVector *) src);

		debugger << "copying SampleVector of size " << src->size() << endl;
		*dst = *src;
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in sampleVector_copy(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in sampleVector_copy(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        sampleVector_getAt        
/*
/*	Return the value of the sample at the given position (index) in
	this SampleVector.
 */
extern "C"
double sampleVector_getAt( const SampleVector * ptr_this, unsigned long idx )
{
	try 
	{
		ThrowIfNull((SampleVector *) ptr_this);
		// return ptr_this->at(idx);
		return (*ptr_this)[idx];	//	g++ doesn't have at()?
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in sampleVector_getAt(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in sampleVector_getAt(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0.;
}

/* ---------------------------------------------------------------- */
/*        sampleVector_setAt        
/*
/*	Set the value of the sample at the given position (index) in
	this SampleVector.
 */
extern "C"
void sampleVector_setAt( SampleVector * ptr_this, unsigned long idx, double x )
{
	try 
	{
		ThrowIfNull((SampleVector *) ptr_this);
		//ptr_this->at(idx) = x;
		(*ptr_this)[idx] = x;	//	g++ doesn't implement at()?
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in sampleVector_setAt(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in sampleVector_setAt(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        sampleVector_getLength        
/*
/*	Return the number of samples represented by this SampleVector.
 */
extern "C"
unsigned long sampleVector_getLength( const SampleVector * ptr_this )
{
	try 
	{
		ThrowIfNull((SampleVector *) ptr_this);
		return ptr_this->size();
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in sampleVector_getLength(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in sampleVector_getLength(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        sampleVector_setLength        
/*
/*	Change the number of samples represented by this SampleVector. If
	the given size is greater than the current size, all new samples 
	will have value 0. If the given size is less than the current 
	size, then samples in excess of the given size are removed.
 */
extern "C"
void sampleVector_setLength( SampleVector * ptr_this, unsigned long size )
{
	try 
	{
		ThrowIfNull((SampleVector *) ptr_this);
		debugger << "resizing SampleVector to size " << size << endl;
		ptr_this->resize(size, 0.);
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in sampleVector_setLength(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in sampleVector_setLength(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}



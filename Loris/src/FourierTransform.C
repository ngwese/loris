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
 * FourierTransform.C
 *
 * Implementation of class Loris::FourierTransform, support for the
 * FFTW library (www.fftw.org). Requires a compiled fttw library.
 *
 * Kelly Fitz, 14 Feb 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include <config.h>
#endif

#include <FourierTransform.h>
#include <Exception.h>
#include <Notifier.h>
#include <cstring>	//	for memcpy
#include <set>

#if defined(HAVE_FFTW3_H)
	#include <fftw3.h>
	//	#include <bench-user.h> ???? whatsis? from gogins
#else
	#include <fftw.h>
#endif

using namespace std;
using namespace Loris;

//	prototypes for local (file-scope) functions that 
//	manage the shared buffer, used for out-of-place
//	transform computations:
static void reserve( long len );
static void release( long len );
static fftw_complex * sharedBuffer = NULL;

//	"die hook" for FFTW, which otherwise try to write to a
//	non-existent console.
static void fftw_die_Loris(const char * s)
{
	notifier << "The FFTW library used by Loris has encountered a fatal error: " << s << endl;
	exit(EXIT_FAILURE);
}

// ---------------------------------------------------------------------------
//  Check_Types
// ---------------------------------------------------------------------------
//	Attempt to verify that std::complex< double >
//	and fftw_complex are really identical, that is, 
//	they are the same size and have the same memory
//	layout.
//
//	Some of this is performed using compile-time assertions,
//	that are part of the configure script.
//	The only thing that I haven't already verified at
//	configure-time is that the two complex types store
//	their real and imaginary parts in the same order; 
//	hard to see how we can do this at compile-time, 
//	since the implementation of std::complex is private.
//
//	According to the FFTW folks, the C++ folks have more or
//	less agreed to standardize the complex<> layout to 
//	be binary-compatible with fftw_complex, so all of
//	the reinterpret_cast stuff in here should be OK.
//	Doesn't hurt to check though.
//
//	In an ideal world, we wouldn't rely on these types
//	being identical, but in this world, relaxing that 
// 	assuption would cost us (in copying)...
//
static bool Check_Types( void )
{
	debugger << "checking memory layout of std::complex<double> and fftw_complex" << endl;
	static bool checked = false;
	if ( ! checked ) 
	{
		std::complex<double> cplxstd(1234.5678, 9876.5432);
		fftw_complex * cplxfftw = reinterpret_cast<fftw_complex*>(&cplxstd);
		if ( c_re( *cplxfftw ) != cplxstd.real() ||
			 c_im( *cplxfftw ) != cplxstd.imag() ) 
		{
			Throw( InvalidObject, 
				   "FourierTransform found std::complex< double > and fftw_complex to be different." );
		}
	}
	
	debugger << "found them to be identical" << endl;
	return true;
}

static bool CHECKED_TYPES = Check_Types();

// ---------------------------------------------------------------------------
//	FourierTransform constructor
// ---------------------------------------------------------------------------
//	The private buffer _buffer has to be allocated immediately, so that it can be 
//	accessed for loading. The shared output buffer for FFTW can be allocated
//	later, in makePlan(), using reserve().
//
FourierTransform::FourierTransform( long len ) :
	_buffer( len ),
	_plan( NULL )
{
#if !defined(HAVE_FFTW3_H)
	//	FFTW calls fprintf a lot, which may be a problem in
	//	non-console-enabled applications. Catch fftw_die()
	//	calls by routing the error message to our own Notifier
	//	and exiting, using the function defined above.
	//
	//	(version 2 only)
	fftw_die_hook = fftw_die_Loris;
#endif

	//	zero:
	fill( _buffer.begin(), _buffer.end(), 0. );
}

// ---------------------------------------------------------------------------
//	FourierTransform destructor
// ---------------------------------------------------------------------------
//	Release the plan and the shared buffer. The instance vector will clean
//	up itself.
//
FourierTransform::~FourierTransform( void )
{
	if ( _plan != NULL ) 
	{
		fftw_destroy_plan( _plan );
		release( size() );
	}
}

// ---------------------------------------------------------------------------
//	transform
// ---------------------------------------------------------------------------
//	Compute a Fourier transform of the current contents of the transform
//	buffer.
//
void
FourierTransform::transform( void )
{
//	make a plan, if necessary:
	if ( _plan == NULL ) 
		makePlan();
	
//	sanity:
	Assert( _plan != NULL );
	Assert( sharedBuffer != NULL );

//	crunch:	
#if defined(HAVE_FFTW3_H)
	fftw_execute(_plan); 	// repeat as needed
#else
	fftw_one( _plan, reinterpret_cast<fftw_complex*>(&_buffer[0]), sharedBuffer );
#endif

//	copy output into (private) complex buffer:
//	(fftw_complex and std::complex< double > had better be the same!)
//	std::copy( sharedBuffer, sharedBuffer + size(), 
//			   reinterpret_cast<fftw_complex*>(&_buffer.front()) );
	memcpy(sharedBuffer, &_buffer.front(), size() * sizeof(fftw_complex));
}

// ---------------------------------------------------------------------------
//	makePlan
// ---------------------------------------------------------------------------
//	Ensure that the shared buffer is large enough for this transform, then
//	create a plan specific to this transform's length and input buffer, and
//	the current shared output buffer. This member is invoked automatically
//	the first time transform() is called, but clients wanting to make sure 
//	that the plan is optimal may invoke it at other times (if they suspect
//	that the shared buffer has been reallocated, for example).
//
//	May throw an allocation exception if the shared buffer cannot be allocated.
//	Throws InvalidObject if the plan cannot be created.
//
void
FourierTransform::makePlan( void )
{
	//	reserve the shared buffer:
	//	(Do this first, so that, when release is called
	//	below, to decrement the instance count for this
	//	transform size, the shared buffer is not deallocated
	//	and shrunk, only to be immediately grown again.
	//	In this way, no buffer reallocation will occur if
	//	the shared buffer is already big enough for this
	//	transform.)
	reserve( size() );
	
	//	check for an existing plan:
	if ( _plan != NULL ) 
	{
		fftw_destroy_plan( _plan );
		release( size() );
	}
	
	//	create a plan:
#if defined(HAVE_FFTW3_H)
	_plan = fftw_plan_dft_1d( size(), 
							  reinterpret_cast<fftw_complex*>(&_buffer.front()), 
							  sharedBuffer, FFTW_FORWARD, FFTW_ESTIMATE );
#else
	_plan = fftw_create_plan_specific( size(), 
									   FFTW_FORWARD,
									   FFTW_ESTIMATE,
									   reinterpret_cast<fftw_complex*>(&_buffer.front()), 
									   1,
									   sharedBuffer, 
									   1); 
#endif									   
	//	verify:
	if ( _plan == NULL )
		Throw( InvalidObject, "FourierTransform could not make a (fftw) plan." );
}

// ---------------------------------------------------------------------------
//	reservations
// ---------------------------------------------------------------------------
//	Access a multiset of the lengths of all existing FourierTransform
//	instances. It is sorted with greater<> so that the first element
//	is the length of the largest existing transform.
//	 
static multiset< long, greater<long> > & reservations( void ) 
{
	static multiset< long, greater<long> > _mset;
	return _mset;
}

// ---------------------------------------------------------------------------
//	reserve
// ---------------------------------------------------------------------------
//	Register a new transform by adding its length to the reservations.
//	Reallocate the shared buffer if necessary.
//	Might throw a low memory exception.
//
static void reserve( long len )
{
	//	allocate a bigger shared buffer if necessary:
	if ( reservations().empty() || * reservations().begin() < len )
	{
		debugger << "Allocating shared buffer of size " << len << endl;
		delete[] sharedBuffer;
		sharedBuffer = NULL;	//	to prevent deleting again
		sharedBuffer = new fftw_complex[ len ];
	}
	
	reservations().insert( len );
}

// ---------------------------------------------------------------------------
//	release
// ---------------------------------------------------------------------------
//
static void release( long len )
{
	//	find a reservation for this length:
	multiset< long, greater<long> >::iterator pos = reservations().find( len );
	if ( pos == reservations().end() ) 
	{
		debugger << "wierd, couldn't find a reservation for a transform of length " << len << endl;
		return;
	}
	
	//	erase it:
	reservations().erase( pos );
	
	//	shrink the shared buffer if possible:
	if ( reservations().empty() || * reservations().begin() < len ) 
	{
		debugger << "Releasing shared buffer of size " << len << endl;
		delete[] sharedBuffer;
		sharedBuffer = NULL;
		
		if ( ! reservations().empty() ) 
		{
			debugger << "Allocating shared buffer of size " << * reservations().begin() << endl;
			sharedBuffer = new fftw_complex[ * reservations().begin() ];
		}
	}
}

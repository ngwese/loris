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
#include <fftw.h>
#include <set>

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
//	Gnarly code adopted from Alexandrescu (Modern C++ Design) for compile-time
//	checking of type information. Need to make sure that fftw_complex and 
//	std::complex<double> are the same size and have the same memory layout.
//	Try to do that at compile-time, much better than doing it at runtime.
//	Actually, best of all would be at configure-time...
//
template<int> struct CompileTimeError;
template<> struct CompileTimeError<true> {};
#define STATIC_CHECK(expr, msg) \
    { CompileTimeError<((expr) != 0)> ERROR_##msg; (void)ERROR_##msg; }

template <class T, class U> struct CompareTypes { enum { same = 0 }; };
template<class T> struct CompareTypes<T, T> { enum { same = 1 }; };

static void Check_Types( void )
{
	//	check to make sure that std::complex< double >
	//	and fftw_complex are really identical:

	//	if this won't compile, fftw_real is not defined to be double,
	//	and this class won't work under those conditions:
	STATIC_CHECK((CompareTypes<double, fftw_real>::same!=0), fftw_real_IS_NOT_DOUBLE);

	//	if this won't compile, then the two complex types have 
	//	different sizes, and therefore don't have the same memory 
	//	layout, and this FourierTransform class won't work:
	STATIC_CHECK( sizeof(std::complex<double>) == sizeof(fftw_complex), 
				  fftw_complex_DIFFERENT_SIZE_FROM_std_complex_double );
				  
	//	if this doesn't compile, then probably the previous one didn't
	//	compile either, but it doesn't hurt to verify that the layout
	//	of the complex types is trivial:
	STATIC_CHECK( sizeof(fftw_complex) == sizeof(double) + sizeof(double),
				  fftw_complex_LAYOUT_IS_NONTRIVIAL );

	static bool checked = false;
	if ( ! checked ) 
	{
		//	the only thing that I haven't already verified at
		//	compile-time is that the two complex types store
		//	their real and imaginary parts in the same order; 
		//	hard to see how we can do this at compile-time, 
		//	since the implementation of std::complex is private:
		std::complex<double> cplxstd(1234.5678, 9876.5432);
		fftw_complex * cplxfftw = (fftw_complex *)&cplxstd;
		if ( c_re( *cplxfftw ) != cplxstd.real() ||
			 c_im( *cplxfftw ) != cplxstd.imag() ) 
		{
			Throw( InvalidObject, 
				   "FourierTransform found std::complex< double > and fftw_complex to be different." );
		}
	}
}

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
	//	FFTW calls fprintf a lot, which may be a problem in
	//	non-console-enabled applications. Catch fftw_die()
	//	calls by routing the error message to our own Notifier
	//	and exiting, using the function defined above.
	fftw_die_hook = fftw_die_Loris;
	
	//	perform type checks, mostly compile-time:
	Check_Types();

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
	fftw_one( _plan, (fftw_complex *)(&_buffer[0]), sharedBuffer );
	
//	copy output into (private) complex buffer:
//	(fftw_complex and std::complex< double > had better be the same!)
	std::copy( sharedBuffer, sharedBuffer + size(), (fftw_complex *)(&_buffer[0]) );
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
	_plan = fftw_create_plan_specific( size(), 
									   FFTW_FORWARD,
									   FFTW_ESTIMATE,
									   (fftw_complex *)(&_buffer[0]), 
									   1,
									   sharedBuffer, 
									   1); 
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

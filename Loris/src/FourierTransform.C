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
#include "FourierTransform.h"
#include "Exception.h"
#include "notifier.h"
#include "fftw.h"
#include <set>

using namespace std;
using namespace Loris;

//	prototypes for local (file-scope) functions that 
//	manage the shared buffer, used for out-of-place
//	transform computations:
static void reserve( long len );
static void release( long len );
static fftw_complex * sharedBuffer = NULL;

// ---------------------------------------------------------------------------
//	FourierTransform constructor
// ---------------------------------------------------------------------------
//	The private buffer _buffer has to be allocated immediately, so that it can be 
//	accessed for loading. The shared output buffer for FFTW can be allocated
//	later, in makePlan(), using reserve().
//
FourierTransform::FourierTransform( long len ) :
	_size( len ),
	_buffer( new complex< double >[ len ] ),
	_plan( NULL )
{
	//	this is insane, too yucky, just use the fftw_complex
	//	struct everywhere. sigh.
	//	-- sanity --
	//	check to make sure that std::complex< double >
	//	and fftw_complex are really identical:
	static bool checked = false;
	if ( ! checked ) {
		//	check that fftw_real is a double in the version
		//	of the FFTW library that we linked:
		if ( fftw_sizeof_fftw_real() != sizeof( double ) ) {
			Throw( InvalidObject, 
				   "FourierTransform found fftw_real is not the same size as double." );
		}
		
		#if 0
		/*	check that storage for the two complex types is
		//	the same:
		union {
			std::complex< double > _std;
			fftw_complex _fftw;
		} u;
		u._std = std::complex<double>(1234.5678, 9876.5432);
		*/
		std::complex<double> cplxstd(1234.5678, 9876.5432);
		fftw_complex cplxfftw = (fftw_complex)cplxstd;
		if ( c_re( cplxfftw ) != cplxstd.real() ||
			 c_im( cplxfftw ) != cplxstd.imag() ) {
			Throw( InvalidObject, 
				   "FourierTransform found std::complex< double > and fftw_complex to be different." );
		}
		#endif
	}	//	end of sanity check

	//	zero:
	fill( _buffer, _buffer + len, 0. );
}

// ---------------------------------------------------------------------------
//	FourierTransform destructor
// ---------------------------------------------------------------------------
//	Release the plan and the shared buffer.
//
FourierTransform::~FourierTransform( void )
{
	delete[] _buffer;
	if ( _plan != NULL ) {
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
	if ( _plan == NULL ) {
		makePlan();
	}
	
//	sanity:
	Assert( _plan != NULL );
	Assert( _buffer != NULL );
	Assert( sharedBuffer != NULL );

//	crunch:	
	fftw_one( _plan, (fftw_complex *)_buffer, sharedBuffer );
	
//	copy output into (private) complex buffer:
	for ( long i = 0; i < size(); ++i ) 
	{
		_buffer[i] = complex< double >( sharedBuffer[i].re, sharedBuffer[i].im );
	}
}

// ---------------------------------------------------------------------------
//	makePlan
// ---------------------------------------------------------------------------
//	Ensure that the shared buffer is large enough for this transform, then
//	create a plan specifid to this transform's length and input buffer, and
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
	if ( _plan != NULL ) {
		fftw_destroy_plan( _plan );
		release( size() );
	}
	
	//	create a plan:
	_plan = fftw_create_plan_specific( size(), 
									   FFTW_FORWARD,
									   FFTW_ESTIMATE,
									   (fftw_complex *)_buffer, 
									   1,
									   sharedBuffer, 
									   1); 
	//	verify:
	if ( _plan == NULL ) {
		Throw( InvalidObject, "FourierTransform could not make a (fftw) plan." );
	}
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
	if ( reservations().empty() || * reservations().begin() < len ) {
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
	if ( pos == reservations().end() ) {
		debugger << "wierd, couldn't find a reservation for a transform of length " << len << endl;
		return;
	}
	
	//	erase it:
	reservations().erase( pos );
	
	//	shrink the shared buffer if possible:
	if ( reservations().empty() || * reservations().begin() < len ) {
		debugger << "Releasing shared buffer of size " << len << endl;
		delete[] sharedBuffer;
		sharedBuffer = NULL;
		
		if ( ! reservations().empty() ) {
			debugger << "Allocating shared buffer of size " << * reservations().begin() << endl;
			sharedBuffer = new fftw_complex[ * reservations().begin() ];
		}
	}
}

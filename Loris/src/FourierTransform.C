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
	#include "config.h"
#endif

#include "FourierTransform.h"
#include "Exception.h"
#include "Notifier.h"

#if defined(HAVE_FFTW3_H)
	#include <fftw3.h>
   #define c_re(c) ((c)[0])
   #define c_im(c) ((c)[1])
#else
	#include <fftw.h>
#endif

// ===========================================================================
//	No longer matters that fftw and this class use the same floating point
//	data format. The insulating implementation class now does its job of
// really insulating clients completely from FFTW, by copying data between
// buffers of std::complex< double > and fftw_complex, rather than 
// relying on those two complex types to have the same memory layout.
// The overhead of copying is probably not significant, compared to 
// the expense of computing the spectra. 
//
//	about complex math functions for fftw_complex:
//
//	These functions are all defined as templates in <complex>.
//	Regrettably, they are all implemented using real() and 
//	imag() _member_ functions of the template argument, T. 
//	If they had instead been implemented in terms of the real()
//	and imag() (template) free functions, then I could just specialize
//	those two for the fftw complex data type, and the other template
//	functions would work. Instead, I have to specialize _all_ of
//	those functions that I want to use. I hope this was a learning 
//	experience for someone... In the mean time, the alternative I 
//	have is to take advantage of the fact that fftw_complex and 
//	std::complex<double> have the same footprint, so I can just
//	cast back and forth between the two types. Its icky, but it 
//	works, and its a lot faster than converting, and more palatable
//	than redefining all those operators.
//
//	On the subject of brilliant designs, fftw_complex is defined as
//	a typedef of an anonymous struct, as in typedef struct {...} fftw_complex,
//	so I cannot forward-declare that type.
//
// In other good news, the planning structure got a slight name change
// in version 3, making it even more important to remove all traces of
// FFTW from the FourierTransform class definition.
//
// ===========================================================================


//	begin namespace
namespace Loris {

using std::complex;
using std::vector;

// die hook defined below:
static void fftw_die_Loris( const char * s );

#pragma mark --- private implementation class ---

// ---------------------------------------------------------------------------
//  FTimpl
//
// Insulating implementation class to insulate clients
// completely from everything about the interaction between
// Loris and FFTW. There is more copying of data between buffers,
// but this is not the expensive part of computing Fourier transforms
// and we don't have to do unsavry things that require std::complex
// and fftw_complex to have the same memory layout (we could even get
// by with single-precision floats in FFTW if necessary). Also got
// rid of lots of shared buffer stuff that just made the implementation
// lots more complicated than necessary. This one is simple, if not
// as memory efficient.
//
class FTimpl
{
public:
   fftw_plan plan;
   FourierTransform::size_type N;
   fftw_complex * ftIn;    // actually, these can be the same for FFTW3
   fftw_complex * ftOut;
   
   // Construct an implementation instance:
   // allocate an input buffer, and an output buffer
   // and make a plan.
   FTimpl( FourierTransform::size_type sz ) : 
      plan( 0 ), N( sz ), ftIn( 0 ), ftOut( 0 ) 
   {      
      // allocate buffers:
      ftIn = (fftw_complex *)fftw_malloc( sizeof( fftw_complex ) * N );
      ftOut = (fftw_complex *)fftw_malloc( sizeof( fftw_complex ) * N );
      if ( 0 == ftIn || 0 == ftOut )
      {
         fftw_free( ftIn );
         fftw_free( ftOut );
         throw RuntimeError( "cannot allocate Fourier transform buffers" );
      }
      
   	//	create a plan:
   #if defined(HAVE_FFTW3_H)
   	plan = fftw_plan_dft_1d( N, ftIn, ftOut, FFTW_FORWARD, FFTW_ESTIMATE );
   #else
   	plan = fftw_create_plan_specific( N, FFTW_FORWARD, FFTW_ESTIMATE,
   									          ftIn, 1, ftOut, 1 );
   #endif									   
   	//	verify:
   	if ( 0 == plan )
   	{
   		Throw( RuntimeError, "FourierTransform could not make a (fftw) plan." );
   	}
   }
   
   // Destroy the implementation instance:
   // dump the plan.
   ~FTimpl( void )
   {
   	if ( 0 != plan )
   	{
   		fftw_destroy_plan( plan );
   	}         
   	
   	fftw_free( ftIn );
      fftw_free( ftOut );
   }
   
   // Copy complex< double >'s from a buffer into ftIn, 
   // the buffer must be as long as ftIn.
   void loadInput( const complex< double > * bufPtr )
   {
      for ( FourierTransform::size_type k = 0; k < N; ++k )
      {
         c_re( ftIn[ k ] ) = bufPtr->real();
         c_im( ftIn[ k ] ) = bufPtr->imag();
         ++bufPtr;
      }
   }
   
   // Copy complex< double >'s from ftOut into a buffer,
   // which must be as long as ftOut.
   void copyOutput( complex< double > * bufPtr ) const
   {
      for ( FourierTransform::size_type k = 0; k < N; ++k )
      {
         *bufPtr = complex< double >( c_re( ftOut[ k ] ), c_im( ftOut[ k ] ) );
         ++bufPtr;
      }
   }
}; // end of class FTimpl

#pragma mark --- FourierTransform members ---

// ---------------------------------------------------------------------------
//	FourierTransform constructor
// ---------------------------------------------------------------------------
//! Initialize a new FourierTransform of the specified size.
//!
//! \param  len is the length of the transform in samples (the
//!         number of samples in the transform)
//! \throw  RuntimeError if the necessary buffers cannot be 
//!         allocated, or there is an error configuring FFTW.
//
FourierTransform::FourierTransform( size_type len ) :
	_buffer( len ),
	_impl( new FTimpl( len ) )
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
	std::fill( _buffer.begin(), _buffer.end(), 0. );
}

// ---------------------------------------------------------------------------
//	FourierTransform copy constructor
// ---------------------------------------------------------------------------
//! Initialize a new FourierTransform that is a copy of another,
//! having the same size and the same buffer contents.
//!
//! \param  rhs is the instance to copy
//! \throw  RuntimeError if the necessary buffers cannot be 
//!         allocated, or there is an error configuring FFTW.
//
FourierTransform::FourierTransform( const FourierTransform & rhs ) :
	_buffer( rhs._buffer ),
	_impl( new FTimpl( rhs._buffer.size() ) ) // not copied
{
}

// ---------------------------------------------------------------------------
//	FourierTransform destructor
// ---------------------------------------------------------------------------
//! Free the resources associated with this FourierTransform.
//
FourierTransform::~FourierTransform( void )
{	
   delete _impl;
}

// ---------------------------------------------------------------------------
//	FourierTransform assignment operator
// ---------------------------------------------------------------------------
//! Make this FourierTransform a copy of another, having
//! the same size and buffer contents.
//!
//! \param  rhs is the instance to copy
//! \return a refernce to this instance
//! \throw  RuntimeError if the necessary buffers cannot be 
//!         allocated, or there is an error configuring FFTW.
//
FourierTransform &
FourierTransform::operator=( const FourierTransform & rhs )
{
   if ( this != &rhs )
   {
      _buffer = rhs._buffer;
      
      // The implementation instance is not assigned, 
      // but a new one is created.
      delete _impl;
      _impl = 0;
      _impl = new FTimpl( _buffer.size() );
   }
   
   return *this;
}

// ---------------------------------------------------------------------------
//	size
// ---------------------------------------------------------------------------
//! Return the length of the transform (in samples).
//! 
//! \return the length of the transform in samples.
FourierTransform::size_type 
FourierTransform::size( void ) const 
{ 
   return _buffer.size(); 
}
	
// ---------------------------------------------------------------------------
//	transform
// ---------------------------------------------------------------------------
//! Compute the Fourier transform of the samples stored in the 
//! transform buffer. The samples stored in the transform buffer
//! (accessed by index or by iterator) are replaced by the 
//! transformed samples, in-place. 
//
void
FourierTransform::transform( void )
{
   // copy data into the transform input buffer:
   _impl->loadInput( &_buffer.front() );

   //	crunch:	
#if defined(HAVE_FFTW3_H)
	fftw_execute( _impl->plan );
#else
	fftw_one( _impl->plan, _impl->ftIn, _impl->ftOut );	
#endif

	// copy the data out of the transform output buffer:
	_impl->copyOutput( &_buffer.front() );
}

// ---------------------------------------------------------------------------
// fftw_die_Loris
// ---------------------------------------------------------------------------
//	"die hook" for FFTW, which otherwise try to write to a
//	non-existent console.
static void fftw_die_Loris( const char * s )
{
	notifier << "The FFTW library used by Loris has encountered a fatal error: " << s << endl;
	exit(EXIT_FAILURE);
}


}	//	end of namespace Loris

#ifndef INCLUDE_FOURIERTRANSFORM_H
#define INCLUDE_FOURIERTRANSFORM_H
/*
 * Copyright (c) 1999-2000 Kelly Fitz and Lippold Haken
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
 * FourierTransform.h
 *
 * Definition of class Loris::FourierTransform, support for the
 * FFTW library (www.fftw.org). Requires a compiled fttw library.
 *
 * Kelly Fitz, 15 Feb 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

// ===========================================================================
//	Make sure that fftw and this class use the same floating point
//	data format and that fftw is compiled with int having at least 
//	four bytes.
//
//	Also contains inline template transform loading free functions.
//
//	about complex math functions for fftw_complex:
//
//	These functions are all defined as templates in <complex>.
//	Regrettably, they are all implemented using real() and 
//	imag() _member_ functions of the template argument, T. 
//	If they had instead been implemented in terms of the real()
//	and imag() (template) free functions, then I could ust specialize
//	those two for the fftw complex data type, and the other template
//	functions would work. Instead, I have to specialize _all_ of
//	those functions that I want to use. I hope this was a learning 
//	experience for someone... In the mean time, the alternative I 
//	have is to take advantage of the fact that fftw_complex and 
//	std::complex<double> have the same footprint, so I can just
//	cast back and forth between the two types. Its icky, but it 
//	works, and its a lot faster than converting, and mor palatable
//	than redefining all those operators.
// ===========================================================================

#include <complex>
#include <functional>

//	fftw.h defines the type fftw_real, which will 
//	help ensure that fftw_complex has the same memory 
//	footprint as the complex<> type:
#include "fftw.h"

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class FourierTransform
//
//	FourierTransform is a class wrapper for the FFTW library (www.fftw.org).
//	Uses the FFTW library to perform efficient transforms of arbitrary
//	length. Clients store and access the in-place transform data as a
//	c-array of complex<double>. Internally, the transform is computed 
//	out-of-place using two c-arrays of FFTW's complex type, including 
//	a shared temporary output buffer (making this class not at all 
//	thread-safe). 
//
//	Does not (yet) support the use of "wisdom" in fftw for greater
//	optimization. Uses the fftw complex transform (as opposed to
//	the real transform, rfftw).
//
//	Loading functions (non-member templates) are defined after the class.
//
class FourierTransform 
{
//	-- public interface --
public:
//	construction:
	FourierTransform( long len );
	~FourierTransform( void );	
	
//	copy and assignment:
	//	use compiler-generated:
	// FourierTransform( const FourierTransform & );
	// FourierTransform & operator= ( const FourierTransform & );
		
//	transform length access:
	long size( void ) const { return _size; }
	
//	spectrum access:
	std::complex< fftw_real > & operator[] ( unsigned long index )
		{ return _buffer[index]; }
	const std::complex< fftw_real > & operator[] ( unsigned long index ) const
		{ return _buffer[index]; }
		
//	iterator access, for STL algorithms:
	typedef std::complex< fftw_real > * iterator;
	iterator begin( void ){ return _buffer; }
	iterator end( void ) { return _buffer + _size; }
		
	typedef const std::complex< fftw_real > * const_iterator;
	const_iterator begin( void ) const { return _buffer; }
	const_iterator end( void ) const { return _buffer + _size; }
		
//	spectrum computation:
	void transform( void );
	
//	planning:
	void makePlan( void );

//	-- instance variables --
private:
	const long _size;
	std::complex< fftw_real > * _buffer;
	
	//	fftw planning structure:
	fftw_plan_struct * _plan;
	
};	//	end of class FourierTransform

//
//	template loading functions:
//

// ---------------------------------------------------------------------------
//	load
// ---------------------------------------------------------------------------
//	Load a FourierTransform with the half-open range begin-end.
//
//	Does not check bounds.
//
template< class Iterator >
FourierTransform & 
load( FourierTransform & t, Iterator begin, Iterator end )
{
	FourierTransform::iterator it = 
		std::copy( begin, end, t.begin() );
	std::fill( it, t.end(), 0. );
	return t;
}

// ---------------------------------------------------------------------------
//	load
// ---------------------------------------------------------------------------
//	Load a FourierTransform with the half-open range begin-end, 
//	rotating so that the first sample in the transform is center.
//
//	Does not check bounds.
//
template< class Iterator >
FourierTransform & 
load( FourierTransform & t, Iterator begin, Iterator center, Iterator end )
{
	//	my reference (Josuttis) differs from my implementations
	//	in the return value of copy_backward (the first unmodified
	//	element vs. the last modified element); prezeroing is
	//	less efficient, but always safe: 
	std::fill( t.begin(), t.end(), 0. );
	//FourierTransform::iterator it1 = 
		std::copy( center, end, t.begin() );
	//FourierTransform::iterator it2 = 
		std::copy_backward( begin, center, t.end() );
	//std::fill( it1, ++it2, 0. );
	return t;
}

// ---------------------------------------------------------------------------
//	load
// ---------------------------------------------------------------------------
//	Load a FourierTransform with the half-open range begin-end, 
//	multiplying by a window starting at winBegin, and
//	rotating so that the first sample in the transform is center.
//
//	Does not check bounds.
//
template< class Iterator1, class Iterator2 >
FourierTransform & 
load( FourierTransform & t, Iterator1 begin, Iterator1 center, Iterator1 end, Iterator2 winBegin )
{
	FourierTransform::iterator tmiddle = t.begin() + std::distance( begin, center );
	FourierTransform::iterator it = 
		std::transform( begin, end, winBegin, t.begin(), multiplies< double >() );
	std::fill( it, t.end(), 0. );
	std::rotate( t.begin(), tmiddle, t.end() );
	return t;
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif // ndef INCLUDE_FOURIERTRANSFORM_H

#ifndef INCLUDE_FOURIERTRANSFORM_H
#define INCLUDE_FOURIERTRANSFORM_H
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
// ===========================================================================

#include <complex>
#include <functional>
#include <vector>

//	begin namespace
namespace Loris {

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
//	This class assumes (and REQUIRES) that fftw_real (defined in fftw.h) 
//	is type double, AND MOREOVER, that the memory layout of fftw_complex
//	is the same as std::complex< double >. Some attempt is made to verify
//	this condition in the constructor.
//
//	Does not (yet) support the use of "wisdom" in fftw for greater
//	optimization. Uses the fftw complex transform (as opposed to
//	the real transform, rfftw).
//
class FourierTransform 
{
//	-- instance variables --
	typedef std::vector< std::complex< double > > VecComplex;
	VecComplex _buffer;
	
	//	fftw planning structure:
	//	This is disgusting, but the name of the planning
	//	structure in FFTW3 changed from fftw_plan_struct to
	//	fftw_plan_s, and I am having a hard time figuring
	//	out how to support both versions (2 and 3) without
	//	including the fftw header in here, where it definitely
	//	should not be.
	//fftw_plan_struct * _plan;
	void * _plan;
	
//	-- public interface --
public:
//	construction:
	FourierTransform( long len );
	~FourierTransform( void );	
		
//	transform length access:
	long size( void ) const { return _buffer.size(); }
	
//	spectrum access:
//	(inline for efficiency)
	std::complex< double > & operator[] ( unsigned long index )
		{ return _buffer[index]; }
	const std::complex< double > & operator[] ( unsigned long index ) const
		{ return _buffer[index]; }
		
//	iterator access, for STL algorithms:
//	(inline for efficiency)
	typedef VecComplex::iterator iterator;
	iterator begin( void )	{ return _buffer.begin(); }
	iterator end( void )	{ return _buffer.end(); }
		
	typedef VecComplex::const_iterator const_iterator;
	const_iterator begin( void ) const	{ return _buffer.begin(); }
	const_iterator end( void ) const 	{ return _buffer.end(); }
		
//	spectrum computation:
	void transform( void );
	
//	planning:
//	(probably only called internally)
	void makePlan( void );
	
private:	
//	copy and assignment unimplemented:
//	(this class has pointers, cannot use compiler-generated
//	copy and assignment)
	FourierTransform( const FourierTransform & );
	FourierTransform & operator= ( const FourierTransform & );

};	//	end of class FourierTransform


}	//	end of namespace Loris

#endif /* ndef INCLUDE_FOURIERTRANSFORM_H */

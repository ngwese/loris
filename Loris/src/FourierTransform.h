#ifndef __fftw_wrapper__
#define __fftw_wrapper__
// ===========================================================================
//	FourierTransform.h
//
//	Class definition for Loris::FourierTransform, support for the
//	FFTW library (www.fftw.org). Requires a compiled fttw library.
//
//	Make sure that fftw and this class use the same floating point
//	data format and that fftw is compiled with int having at least 
//	four bytes.
//
//	-kel 15 Feb 00
//
// ===========================================================================
#include "LorisLib.h"
#include <complex>

//	declare the fftw plan struct type:
struct fftw_plan_struct; 

Begin_Namespace( Loris )

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
	std::complex< double > & operator[] ( unsigned long index )
		{ return _buffer[index]; }
	const std::complex< double > & operator[] ( unsigned long index ) const
		{ return _buffer[index]; }
		
//	iterator access, for STL algorithms:
	typedef std::complex< double > * iterator;
	iterator begin( void ){ return _buffer; }
	iterator end( void ) { return _buffer + _size; }
		
	typedef const std::complex< double > * const_iterator;
	const_iterator begin( void ) const { return _buffer; }
	const_iterator end( void ) const { return _buffer + _size; }
		
//	spectrum computation:
	void transform( void );
	
//	planning:
	void makePlan( void );

//	-- instance variables --
private:
	const long _size;
	std::complex< double > * _buffer;
	
	//	fftw planning structure:
	fftw_plan_struct * _plan;
	
};	//	end of class FourierTransform

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

End_Namespace( Loris )

#endif // ndef __fftw_wrapper__
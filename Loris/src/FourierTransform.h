#ifndef __fftw_wrapper__
#define __fftw_wrapper__
// ===========================================================================
//	FourierTransform.h
//
//	Class definition for Loris::FourierTransform.
//	Uses the FFTW library to perform efficient transforms of arbitrary
//	length. Clients store and access the in-place transform data as a
//	vector of complex<double>. Internally, the transform is computed 
//	out-of-place using two c-arrays of FFTW's complex type.
//
//	Still needs some work on the implementation to make it efficient.
//
//	-kel 15 Feb 00
//
// ===========================================================================
#include "LorisLib.h"
#include <vector>
#include <complex>

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class FourierTransform
//
//	No-frills FFT object.
//
//	For greatest flexibility, load and transform are separate
//	operations. 
//
//
class FourierTransform 
{
//	-- public interface --
public:
//	construction:
	FourierTransform( long len );
	
	//	use compiler-generated:
	// ~FourierTransform( void ) {}	
	
//	copy and assignment:
	//	use compiler-generated:
	// FourierTransform( const FourierTransform & );
	// FourierTransform & operator= ( const FourierTransform & );
		
//	transform length access:
	long size( void ) const { return _size; } //_z.size(); }
	
//	spectrum access:
	std::complex< double > & operator[] ( unsigned long index )
		{ return _z[index]; }
	const std::complex< double > & operator[] ( unsigned long index ) const
		{ return _z[index]; }
		
//	iterator access:
//	(is this good?)
//	what did I need this for?
	//typedef std::vector< std::complex< double > >::iterator iterator;
	typedef std::complex< double > * iterator;
	iterator begin( void ){ return _z; } //.begin(); }
	iterator end( void ) { return _z + _size; } //.end(); }
		
	//typedef std::vector< std::complex< double > >::const_iterator const_iterator;
	typedef const std::complex< double > * const_iterator;
	const_iterator begin( void ) const { return _z; } //.begin(); }
	const_iterator end( void ) const { return _z + _size; } //.end(); }
		
//	input:
	//void load( const std::vector< double > & buf );
	//void loadAndRotate( const std::vector< double > & buf );

//	spectrum computation:
	void transform( void );
	//void transform( const std::vector< double > & buf );
	//void operator() ( const std::vector< double > & buf ) { transform( buf ); }
	
//	-- instance variables --
private:
	//std::vector< std::complex< double > > _z;
	const long _size;
	std::complex< double > * _z, * _out;
	
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
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
	long size( void ) const { return _z.size(); }
	
//	spectrum access:
	std::complex< double > & operator[] ( unsigned long index )
		{ return _z[index]; }
	const std::complex< double > & operator[] ( unsigned long index ) const
		{ return _z[index]; }
		
//	iterator access:
//	(is this good?)
	typedef std::vector< std::complex< double > >::iterator iterator;
	iterator begin( void ){ return _z.begin(); }
	iterator end( void ) { return _z.end(); }
		
	typedef std::vector< std::complex< double > >::const_iterator const_iterator;
	const_iterator begin( void ) const { return _z.begin(); }
	const_iterator end( void ) const { return _z.end(); }
		
//	input:
	void load( const std::vector< double > & buf );
	void loadAndRotate( const std::vector< double > & buf );

//	spectrum computation:
	void transform( void );
	void transform( const std::vector< double > & buf );
	void operator() ( const std::vector< double > & buf ) { transform( buf ); }
	
//	-- instance variables --
private:
	std::vector< std::complex< double > > _z;
	
};	//	end of class FourierTransform

End_Namespace( Loris )

#endif // ndef __fftw_wrapper__
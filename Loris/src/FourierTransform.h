#ifndef __fourier_transform__
#define __fourier_transform__
// ===========================================================================
//	FourierTransform.h
//
//	Class definition for Loris::FourierTransform.
//
//	-kel 7 Dec 99
//
// ===========================================================================
#include "LorisLib.h"

#include <vector>
#include <complex.h>

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
	FourierTransform( ulong len );
	
	//	use compiler-generated:
	// ~FourierTransform( void ) {}	
	
//	copy and assignment:
	//	use compiler-generated:
	// FourierTransform( const FourierTransform & );
	// FourierTransform & operator= ( const FourierTransform & );
		
//	transform length access:
	int size( void ) const { return _z.size(); }
	
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
	
//	-- helpers --
private:
	void fillReverseBinaryTable( void );
	void decimationStep( long span );
	
//	-- instance variables --
private:
	std::vector< std::complex< double > > _z;
	std::vector< unsigned long > _revBinaryTable;
	
};	//	end of class FourierTransform

End_Namespace( Loris )

#endif // ndef __fourier_transform__
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
//
class FourierTransform 
{
//	-- public interface --
public:
//	construction:
	FourierTransform( unsigned long len );
	
	//	use compiler-generated:
	// ~FourierTransform( void ) {}	
	
//	copy and assignment:
	//	use compiler-generated:
	// FourierTransform( const FourierTransform & );
	// FourierTransform & operator= ( const FourierTransform & );
		
//	transform length access:
	int length( void ) const { return _z.size(); }
	
//	spectrum access:
	std::complex< double > & operator[] ( unsigned long index )
		{ return _z[index]; }
	const std::complex< double > & operator[] ( unsigned long index ) const
		{ return _z[index]; }

//	spectrum computation:
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
// ===========================================================================
//	FourierTransform.C
//
//	Implementation of Loris::FourierTransform.
//
//	-kel 7 Dec 99
//
// ===========================================================================
#include "FourierTransform.h"
#include "Exception.h"

Begin_Namespace( Loris )
// ---------------------------------------------------------------------------
//	FourierTransform constructor
// ---------------------------------------------------------------------------
//
FourierTransform::FourierTransform( unsigned long len ) :
	_z( len ),
	_revBinaryTable( len )
{
	//	this will except if len is not 
	//	a power of two:
	fillReverseBinaryTable();
}

// ---------------------------------------------------------------------------
//	transform
// ---------------------------------------------------------------------------
void
FourierTransform::transform( const std::vector< double > & buf )
{
//	load input:
//	¥¥¥¥¥¥¥¥¥¥
	
//	permute input to reverse binary order:
	for ( long i = 0; i < length(); ++i ) {
		//	only swap once:
		if ( _revBinaryTable[i] > i ) {	
			swap( _z[i], _z[ _revBinaryTable[i] ] );
		}
	} 
	
//	do decimation-in-time butterfly steps:
	for ( long span = 1;  span < length();  span = span << 1 ) {
		decimationStep( span );
	}
}

// ---------------------------------------------------------------------------
//	fillReverseBinaryTable
// ---------------------------------------------------------------------------
//	Except if the length is not a power of two.
//
void
FourierTransform::fillReverseBinaryTable( void )
{
	//	compute log length:
	long len = _revBinaryTable.size();
	long loglen, n;
	for ( loglen = 1, n = 2; n != len; ++loglen, n = n << 1 ) {
		if ( n > len )
			Throw( InvalidObject, "FourierTransform length must be a power of two." );
	}
	
	//	fill the table:
	for (long i = 0; i < len; ++i) {
		_revBinaryTable[i] = 0;

		for (long j = 0; j < loglen; ++j) {
			// get the jth bit and make it the Size-j th bit 
			long temp = ((1 << j) & i) >> j;
			_revBinaryTable[i] |= (temp << loglen - j - 1);
		}
	}
}

// ---------------------------------------------------------------------------
//	decimationStep
// ---------------------------------------------------------------------------
//	Perform one step of the decimation-in-time butterfly algorithm
//	for length a power of 2. The input is assumed to be permuted
//	to reverse binary order, so the cosine and sine factors can
//	be generated and used in normal order.
//
void
FourierTransform::decimationStep( long span )
{
	const long twospan	= span << 1;	// for loop termination
	const double dangle = Pi / span;
	
	//	loop variables:
	long i;
	double angle;	// for last step, this is 2pi/n
	
	for ( i = 0, angle = 0.0;  i < span;  ++i, angle += dangle ) {
		double sine = sin( angle );
		double cosine = cos( angle );
		
		for (long j = i;  j < length();  j += twospan)
		{	
			complex< double > 
				temp( cosine * _z[j + span].real() + sine * _z[j + span].imag(),
					  - sine * _z[j + span].real() + cosine * _z[j + span].imag() );
				
			_z[j + span] = _z[j] - temp;
			_z[j] += temp;
		}
	}
}

End_Namespace( Loris )

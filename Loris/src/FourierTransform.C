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
#include "fft.h"
//#include "AiffFile.h"	//	for debugging only

using namespace std;

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
//	Compute the Fourier transform of the specified buffer. 
//	The caller should window the buffer if desired. 
//	The buffer will be zero-padded or truncated if it
//	is the wrong size. The input will also be rotated to
//	align its phase reference with the middle of the buffer.
//
void
FourierTransform::transform( const vector< double > & buf )
{
	loadAndRotate( buf );
	transform();
}

// ---------------------------------------------------------------------------
//	transform
// ---------------------------------------------------------------------------
//	Compute a Fourier transform of the current contents of the transform
//	buffer.
//
void
FourierTransform::transform( void )
//#define __inscrutible__
#ifdef __inscrutible__
{
//	setup fft buffers:
	static long bufsize = 0;
	static double * real = Null;
	static double * imag = Null;
	if ( bufsize < size() ) {
		try {
			delete[] real;
			real = Null;	//	to prevent deleting again
			delete[] imag;
			imag = Null;	//	to prevent deleting again
			bufsize = 0;
			real = new double[ size() ];
			imag = new double[ size() ];
			bufsize = size();
		}
		catch( LowMemException & ex ) {
			bufsize = 0;
			delete[] real;
			real = Null;	//	to prevent deleting again
			delete[] imag;
			imag = Null;	//	to prevent deleting again
			ex.append( "couldn't allocate fft buffers." );
			throw;
		}
	}
	
//	permute input to reverse binary order:
	for ( long i = 0; i < size(); ++i ) {
		//	only swap once:
		if ( _revBinaryTable[i] > i ) {	
			swap( _z[i], _z[ _revBinaryTable[i] ] );
		}
	} 
	
//	copy input into local buffers:
	for ( long i = 0; i < size(); ++i ) {
		real[i] = _z[i].real();
		imag[i] = _z[i].imag();
	}
	
//	do decimation-in-time butterfly steps:
	for ( long span = 1;  span < size();  span = span * 2 ) {
		butterfly( real, imag, span, bufsize );
	}

//	copy output into complex buffer:
	for ( long i = 0; i < size(); ++i ) {
		_z[i] = complex< double >( real[i], imag[i] );
	}
	
}
#else
{
//	permute input to reverse binary order:
	for ( long i = 0; i < size(); ++i ) {
		//	only swap once:
		if ( _revBinaryTable[i] > i ) {	
			swap( _z[i], _z[ _revBinaryTable[i] ] );
		}
	} 
	
//	do decimation-in-time butterfly steps:
	for ( long span = 1;  span < size();  span = span * 2 ) {
		decimationStep( span );
	}
}
#endif

// ---------------------------------------------------------------------------
//	load
// ---------------------------------------------------------------------------
//	Load samples from the specified buffer into the transform
//	buffer, truncating or zero-padding if the lengths differ.
//
void
FourierTransform::load( const vector< double > & buf )
{
	if ( buf.size() < _z.size() ) {
		copy( buf.begin(), buf.end(), _z.begin() );
		//	zero the rest:
		fill( _z.begin() + buf.size(), _z.end(), 0. );
	}
	else {
		copy( buf.begin(), buf.begin() + _z.size(), _z.begin() );
	}
}

// ---------------------------------------------------------------------------
//	loadAndRotate
// ---------------------------------------------------------------------------
//	Load samples from the specified buffer into the transform buffer, 
//	rotating so that the transform's phase is referenced to the middle
//	sample of the input buffer. The buffer is zero-padded or truncated
//	as necessary. Truncation occurs at both ends, so that the phase
//	reference is always the middle sample of the input buffer, which is
//	usually desired for windowed transform input. If not, use load().
//
//	If the buffer is even-length, then it has no center sample to align
//	with the beginning of the transform buffer. (That's why odd-length
//	windows are preferred in this business.) In this case, the rotation
//	is half a sample too far (the other choice is half a sample not
//	far enough), with the second half of the buffer at the beginning
//	of the transform buffer.
//
void
FourierTransform::loadAndRotate( const vector< double > & buf )
{
	if ( buf.size() < _z.size() ) {
		//	pre-zero:
		fill( _z.begin(), _z.end(), 0. );
		//	copy second half of buf into beginning of _z:
		copy( buf.begin() + buf.size() / 2, buf.end(), _z.begin() );
		//	copy second half of buf into end of _z:
		copy( buf.begin(), buf.begin() + buf.size() / 2, _z.end() - buf.size() / 2 );
	}
	else {
		//	copy _z.size()/2 samples starting at middle of
		//	buf into first half of _z:
		copy( buf.begin() + buf.size() / 2, 
			  buf.begin() + (buf.size() / 2) + (_z.size() / 2), 
			  _z.begin() );
		//	copy _z.size()/2 samples ending at middle of
		//	buf into second half of _z:
		copy( buf.begin(), 
			  buf.begin() + (_z.size() / 2),
			  _z.begin() + (_z.size() / 2) );
	}
/*
	vector< double > v( _z.size() );
	struct dick {
		double operator()( const complex<double> & c ) { return c.real(); }
	};
	std::transform( _z.begin(), _z.end(), v.begin(), dick() );
	AiffFile a( 44100., 1, 16, v );
	a.write("ftbuffer.aiff");
*/
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
//	to reverse binary order.
//
void
FourierTransform::decimationStep( long span )
{
	const long twospan	= span * 2;
	const double dangle = Pi / span;
	
	long i;
	double angle;
	for ( i = 0, angle = 0.0;  i < span;  ++i, angle += dangle ) {
		//	negative angle agrees with the O&S definition 
		//	of the complex exponential W.
		//	(also it appears to give the right results)
		complex< double > W = polar( 1., - angle );
		
		for ( long j = i;  j < size();  j += twospan ) {	
			complex< double > temp( _z[j + span] * W );				
			_z[j + span] = _z[j] - temp;
			_z[j] += temp;
		}
	}
}

End_Namespace( Loris )

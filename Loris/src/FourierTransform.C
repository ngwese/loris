// ===========================================================================
//	FourierTransform.C
//
//	Implementation of Loris::FourierTransform.
//
//	how about these:
//
//		load( iter begin, iter center, iter end )
//		load( iter begin, iter center, iter end, iter windowBegin )
//		load( iter begin, iter end, long offset )
//		load( iter begin, iter end, iter windowBegin, long offset )
//
//	rotation is so boring that we sould either make it automatic (probaly not)
//	or just make a rotate() member. We always rotate to center the window, so
//	really the center iterator is a thing to align with the beginning of the
//	transform, right? It never makes sense not to align a windowed transform, so 
//	we can get rid of #4. So, when do we need that offset? Maybe never. Certainly
//	we don't need to rotate if we are using offset, so just build rotation into the
//	first two, and blow off the third, or don't do rotation there.
//
//	Oh, I know, need to handle the case when we have only a few samples in the 
//	beginning of the window, but we still want to center the window. Then we probably
//	need an offset or rotate argument. If we support padding at one end (i.e. 
//	samples in just the beginning of the window), it would be nice to support it
//	it at both ends (i.e. samples just at the end of the window).
//
//	HEY this fftw stuff uses int for transform size, better hope its four bytes!
//
//	-kel 14 Feb 00
//
// ===========================================================================
#include "FourierTransform.h"
#include "Exception.h"
#include "Notifier.h"
#include "fftw.h"

using namespace std;
using namespace Loris;

// ---------------------------------------------------------------------------
//	FourierTransform constructor
// ---------------------------------------------------------------------------
//
FourierTransform::FourierTransform( long len ) :
	//_z( len, complex<double>(0.,0.) )
	_size( len ),
	_z( new complex< double >[ len ] ),
	_plan( Null )
{
	//	check to make sure that std::complex< double >
	//	and fftw_complex are really identical:
	static boolean checked = false;
	if ( ! checked ) {
		//	check that fftw_real is a double in the version
		//	of the FFTW library that we linked:
		if ( fftw_sizeof_fftw_real() != sizeof( double ) ) {
			Throw( InvalidObject, 
				   "FourierTransform found fftw_real is not the same size as double." );
		}
		
		//	check that storage for the two complex types is
		//	the same:
		union {
			std::complex< double > _std;
			fftw_complex _fftw;
		} u;
		u._std = std::complex<double>(1234.5678, 9876.5432);
		if ( c_re( u._fftw ) != u._std.real() ||
			 c_im( u._fftw ) != u._std.imag() ) {
			Throw( InvalidObject, 
				   "FourierTransform found std::complex< double > and fftw_complex to be different." );
		}
	}
	
	fill( _z, _z + len, 0. );
}

// ---------------------------------------------------------------------------
//	FourierTransform destructor
// ---------------------------------------------------------------------------
//
FourierTransform::~FourierTransform( void )
{
	//	anybody's guess whether this handles Null pointers,
	//	check first:
	if ( _plan != Null ) {
		fftw_destroy_plan( _plan );
	}
}

// ---------------------------------------------------------------------------
//	transform
// ---------------------------------------------------------------------------
//	Compute the Fourier transform of the specified buffer. 
//	The caller should window the buffer if desired. 
//	The buffer will be zero-padded or truncated if it
//	is the wrong size. The input will also be rotated to
//	align its phase reference with the middle of the buffer.
/*
void
FourierTransform::transform( const vector< double > & buf )
{
	loadAndRotate( buf );
	transform();
}
*/
// ---------------------------------------------------------------------------
//	transform
// ---------------------------------------------------------------------------
//	Compute a Fourier transform of the current contents of the transform
//	buffer.
//
void
FourierTransform::transform( void )
{
//	setup fft buffers:
	static long bufsize = 0;
	//static fftw_complex * fftwIn = Null;
	static fftw_complex * fftwOut = Null;
	//static fftw_plan plan = Null;	//	this is stupidity, fftw_plan is a pointer type
									// 	unknown whether you can destroy an uninitialized
									//	plan, seems unlikely.
		
	//	this is inefficient, a big transform leaves
	//	this huge turd (fftwOut) lying around forever:
	if ( bufsize < size() ) {
		try {
			debugger << "FFTW allocating static buffer for size " << size() << endl;
			//delete[] fftwIn;
			//fftwIn = Null;	//	to prevent deleting again
			delete[] fftwOut;
			fftwOut = Null;	//	to prevent deleting again
			bufsize = 0;
			//fftwIn = new fftw_complex[ size() ];
			fftwOut = new fftw_complex[ size() ];
			bufsize = size();
		}
		catch( LowMemException & ex ) {
			bufsize = 0;
			//delete[] fftwIn;
			//fftwIn = Null;	//	to prevent deleting again
			delete[] fftwOut;
			fftwOut = Null;	//	to prevent deleting again
			ex.append( "couldn't prepare FourierTransform." );
			throw;
		}
	}
	
	//	make a plan, if necessary:
	Assert( _z != Null );
	Assert( fftwOut != Null );
	if ( _plan == Null ) {
		_plan = fftw_create_plan_specific( bufsize, FFTW_FORWARD,
										  FFTW_ESTIMATE,
										  (fftw_complex *)_z, 1,
										  fftwOut, 1); 
	}
	
/*	copy input into fftw buffers:
	for ( long i = 0; i < size(); ++i ) {
		fftwIn[i].re = _z[i].real();
		fftwIn[i].im = _z[i].imag();
	}
*/	
//	cruch:	
	fftw_one( _plan, (fftw_complex *)_z, fftwOut );
	
//	copy output into (private) complex buffer:
	for ( long i = 0; i < size(); ++i ) {
		_z[i] = complex< double >( fftwOut[i].re, fftwOut[i].im );
	}
	
}
/*
// ---------------------------------------------------------------------------
//	load
// ---------------------------------------------------------------------------
//	Load samples from the specified buffer into the transform
//	buffer, truncating or zero-padding if the lengths differ.
//
void
FourierTransform::load( const vector< double > & buf )
{
	if ( buf.size() < size() ) {
		copy( buf.begin(), buf.end(), begin() );
		//	zero the rest:
		fill( begin() + buf.size(), end(), 0. );
	}
	else {
		copy( buf.begin(), buf.begin() + size(), begin() );
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
	if ( buf.size() < size() ) {
		//	pre-zero:
		fill( begin(), end(), 0. );
		//	copy second half of buf into beginning of _z:
		copy( buf.begin() + buf.size() / 2, buf.end(), begin() );
		//	copy second half of buf into end of _z:
		copy( buf.begin(), buf.begin() + buf.size() / 2, end() - buf.size() / 2 );
	}
	else {
		//	copy _z.size()/2 samples starting at middle of
		//	buf into first half of _z:
		copy( buf.begin() + buf.size() / 2, 
			  buf.begin() + (buf.size() / 2) + (size() / 2), 
			  begin() );
		//	copy _z.size()/2 samples ending at middle of
		//	buf into second half of _z:
		copy( buf.begin(), 
			  buf.begin() + (size() / 2),
			  begin() + (size() / 2) );
	}
}
*/
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
#include "notifier.h"
#include "fftw.h"
#include <set>

using namespace std;
using namespace Loris;

static void reserve( long len );
static void release( long len );

// ---------------------------------------------------------------------------
//	FourierTransform constructor
// ---------------------------------------------------------------------------
//	The private buffer _z has to be allocated immediately, so that it can be 
//	accessed for loading. The shared output buffer for FFTW can be allocated
//	later, in transform(), using reserve().
//
FourierTransform::FourierTransform( long len ) :
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
	
	//	zero:
	fill( _z, _z + len, 0. );
}

// ---------------------------------------------------------------------------
//	FourierTransform destructor
// ---------------------------------------------------------------------------
//	Release the plan and the shared buffer.
//
FourierTransform::~FourierTransform( void )
{
	delete[] _z;
	if ( _plan != Null ) {
		fftw_destroy_plan( _plan );
		release( size() );
	}
}

// ---------------------------------------------------------------------------
//	transform
// ---------------------------------------------------------------------------
//	Compute a Fourier transform of the current contents of the transform
//	buffer.
//
static fftw_complex * sharedBuffer = Null;
void
FourierTransform::transform( void )
{
	//	make a plan, if necessary:
	if ( _plan == Null ) {
		//	try to reserve space for a shared buffer,
		//	might except:
		reserve( size() );
		_plan = fftw_create_plan_specific( size(), FFTW_FORWARD,
										  FFTW_ESTIMATE,
										  (fftw_complex *)_z, 1,
										  sharedBuffer, 1); 
		Assert( _plan != Null );								  
	}
	
	//	sanity:
	Assert( _z != Null );
	Assert( sharedBuffer != Null );

//	cruch:	
	fftw_one( _plan, (fftw_complex *)_z, sharedBuffer );
	
//	copy output into (private) complex buffer:
	for ( long i = 0; i < size(); ++i ) {
		_z[i] = complex< double >( sharedBuffer[i].re, sharedBuffer[i].im );
	}
}

// ---------------------------------------------------------------------------
//	reserve
// ---------------------------------------------------------------------------
//	Might throw a low memory exception.
//
static multiset< long, greater<long> > reservations;
static void reserve( long len )
{
	//	allocate a bigger shared buffer if necessary:
	if ( reservations.empty() || * reservations.begin() < len ) {
		debugger << "Allocating shared buffer of size " << len << endl;
		delete[] sharedBuffer;
		sharedBuffer = Null;	//	to prevent deleting again
		sharedBuffer = new fftw_complex[ len ];
	}
	
	reservations.insert( len );
}

// ---------------------------------------------------------------------------
//	release
// ---------------------------------------------------------------------------
//
static void release( long len )
{
	//	find a reservation for this length:
	multiset< long, greater<long> >::iterator pos = reservations.find( len );
	if ( pos == reservations.end() ) {
		debugger << "wierd, couldn't find a reservation for a transform of length " << len << endl;
		return;
	}
	
	//	erase it:
	reservations.erase( pos );
	
	//	shrink the shared buffer if possible:
	if ( reservations.empty() || * reservations.begin() < len ) {
		debugger << "Releasing shared buffer of size " << len << endl;
		delete[] sharedBuffer;
		sharedBuffer = Null;
		
		if ( ! reservations.empty() ) {
			debugger << "Allocating shared buffer of size " << * reservations.begin() << endl;
			sharedBuffer = new fftw_complex[ * reservations.begin() ];
		}
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
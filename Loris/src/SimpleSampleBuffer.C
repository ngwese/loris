// ---------------------------------------------------------------------------
//	SimpleSampleBuffer.C
//
//	Implementation of a concrete SampleBuffer (SampleBuffer.h) that
//	stores its samples in a dumb c-style array of doubles that it
//	owns. 
//
//	Kelly Fitz 
//	Feb 1999
// ---------------------------------------------------------------------------

#include "LorisLib.h"
#include "SimpleSampleBuffer.h"

#include <algorithm>
using std::copy;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	SimpleSampleBuffer constructor
// ---------------------------------------------------------------------------
//	Assume responsibility for the c-style array of samples:
//
SimpleSampleBuffer::SimpleSampleBuffer( double * samples, int howMany ) : 
	mArray( samples ),
	mSize( howMany )
{ 
}

// ---------------------------------------------------------------------------
//	SimpleSampleBuffer copy constructor
// ---------------------------------------------------------------------------
//
SimpleSampleBuffer::SimpleSampleBuffer( const SimpleSampleBuffer & other ) :
	mArray( new double[ other.mSize ] ),
	mSize( other.mSize )
{
	copy( other.mArray, other.mArray + other.mSize, mArray );
}

// ---------------------------------------------------------------------------
//	SimpleSampleBuffer default constructor
// ---------------------------------------------------------------------------
// 	uninitialized construction is empty buffer:
//	this is pretty safe given SampleBuffer's interface,
//	although, as always, operator[] is not checked, so
//	it can give undefined results it the client doesn't
//	do the bounds-checking.
//
SimpleSampleBuffer::SimpleSampleBuffer( ulong len ) : 
	mArray( new double[ len ] ),
	mSize( len )
{
	//	initialize to zeros:
	for ( int i = 0; i < len; ++i )
		mArray[i] = 0.;
}

// ---------------------------------------------------------------------------
//	SimpleSampleBuffer destructor
// ---------------------------------------------------------------------------
//
SimpleSampleBuffer::~SimpleSampleBuffer( void )
{
	delete[] mArray;
	mArray = NULL;
}

// ---------------------------------------------------------------------------
//	assignment operator
// ---------------------------------------------------------------------------
//	Might except if a new array cannot be allocated. Object will be
//	unchanged, in that case.
//
SimpleSampleBuffer & 
SimpleSampleBuffer::operator= ( const SimpleSampleBuffer & other )
{
	if ( &other != this ) {
		//	get rid of old samples, or maybe reuse
		if ( other.mSize != mSize ) {
			double * z = new double[ other.mSize ];
			mSize = other.mSize;
			delete[] mArray;
			mArray = z;
		}
		//	copy the samples:
		copy( other.mArray, other.mArray + other.mSize, mArray );
	}
	return *this;
}

// ---------------------------------------------------------------------------
//	grow
// ---------------------------------------------------------------------------
//	Might except if a new array cannot be allocated. Object will be
//	unchanged, in that case.
//
void
SimpleSampleBuffer::grow( int n )
{
	if ( size() < n ) {
		//	allocate a new array:
		double * z = new double[ n ];
		mSize = n;
	
		//	copy the samples:
		copy( mArray, mArray + mSize, z );
	
		//	get rid of the old array.
		delete[] mArray;
		mArray = z;
	}
		
}

End_Namespace( Loris )

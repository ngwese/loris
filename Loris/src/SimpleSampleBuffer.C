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
SimpleSampleBuffer::SimpleSampleBuffer( Double * samples, Int howMany ) : 
	mArray( samples ),
	mSize( howMany )
{ 
}

// ---------------------------------------------------------------------------
//	SimpleSampleBuffer copy constructor
// ---------------------------------------------------------------------------
//
SimpleSampleBuffer::SimpleSampleBuffer( const SimpleSampleBuffer & other ) :
	mArray( new Double[ other.mSize ] ),
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
SimpleSampleBuffer::SimpleSampleBuffer( Uint len ) : 
	mArray( new Double[ len ] ),
	mSize( len )
{
	//	initialize to zeros:
	for ( Int i = 0; i < len; ++i )
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
//
SimpleSampleBuffer & 
SimpleSampleBuffer::operator= ( const SimpleSampleBuffer & other )
{
	if ( &other != this )
	{
		//	get rid of old samples, or maybe reuse
		if ( other.mSize != mSize )
		{
			delete[] mArray;
			try
			{
				mArray = new Double[ other.mSize ];
				mSize = other.mSize;
			}
			catch( ... )
			{
				//	don't leave it in an inconsistent state:
				mArray = NULL;
				mSize = 0;
				throw;
			}
		}
		//	copy the samples:
		copy( other.mArray, other.mArray + other.mSize, mArray );
	}
	return *this;
}


End_Namespace( Loris )

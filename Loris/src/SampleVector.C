// ---------------------------------------------------------------------------
//	SampleVector.C
//
//	Implementation of a concrete SampleBuffer (SampleBuffer.h) that
//	stores its samples in a standard template library vector. 
//
//	Kelly Fitz 
//	Feb 1999
// ---------------------------------------------------------------------------

#include "LorisLib.h"
#include "SampleVector.h"

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	SampleVector constructor
// ---------------------------------------------------------------------------
//
SampleVector::SampleVector( const double * b, const double * e ) : 
	v( b, e )
{ 
}

// ---------------------------------------------------------------------------
//	SampleVector copy constructor
// ---------------------------------------------------------------------------
//
SampleVector::SampleVector( const SampleVector & other ) :
	v( other.v )
{
}

// ---------------------------------------------------------------------------
//	SampleVector default constructor
// ---------------------------------------------------------------------------
// 	Uninitialized construction is empty buffer:
//	this is pretty safe given SampleBuffer's interface,
//	although, as always, operator[] is not checked, so
//	it can give undefined results it the client doesn't
//	do the bounds-checking.
//
SampleVector::SampleVector( int len ) :
	v( len, 0 )
{
}

// ---------------------------------------------------------------------------
//	assignment operator
// ---------------------------------------------------------------------------
//
SampleVector & 
SampleVector::operator= ( const SampleVector & other )
{
	if ( &other != this )
	{
		v = other.v;
	}
	return *this;
}

End_Namespace( Loris )

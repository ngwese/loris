#ifndef __Simple_Sample_Buffer__
#define __Simple_Sample_Buffer__

// ---------------------------------------------------------------------------
//	SimpleSampleBuffer.h
//
//	Definition of a concrete SampleBuffer (SampleBuffer.h) that
//	stores its samples in a dumb c-style array of doubles that it
//	owns. 
//
//	Kelly Fitz 
//	Feb 1999
// ---------------------------------------------------------------------------

#include "LorisLib.h"
#include "SampleBuffer.h"

#include <algorithm>
using std::copy;

Begin_Namespace( Loris )

// ===========================================================================
//		€ SimpleSampleBuffer
// ===========================================================================
//	
class SimpleSampleBuffer : public SampleBuffer
{
public:
//	construction:
//	assume responsibility for the c-style array of samples:
	SimpleSampleBuffer( double * samples, int howMany );
	
//	construction from a range:
//	(this is more likely to compile correctly
//	if it is defined in the class definition)
#if !defined( NO_TEMPLATE_MEMBERS )
	template< class InputIterator >
	SimpleSampleBuffer( InputIterator b, InputIterator e ) :
		mArray( new double[ e - b ] ),
		mSize( e - b )
	{
		//	copy the samples:
		copy( b, e, mArray );
	}
#endif

//	copying:
	SimpleSampleBuffer( const SimpleSampleBuffer & other );
	
// 	uninitialized construction is empty buffer:
//	this is pretty safe given SampleBuffer's interface,
//	although, as always, operator[] is not checked, so
//	it can give undefined results it the client doesn't
//	do the bounds-checking.
	explicit SimpleSampleBuffer( ulong len = 0 );
	
//	assignment:
	SimpleSampleBuffer & operator= ( const SimpleSampleBuffer & other );
	
//	destruction:
	~SimpleSampleBuffer( void );
	
//	public SampleBuffer interface:
virtual void grow( int n );
virtual int size( void ) const { return mSize; }

//	indexed access:
virtual double & operator[]( ulong index )  { return mArray[index]; }
virtual const double & operator[]( ulong index ) const  { return mArray[index]; }

//	instance variables:
	double * mArray;	//	samples
	int mSize;
	double mSamplingFrequency;	

};	//	end of class SimpleSampleBuffer

End_Namespace( Loris )


#endif	//	ndef __Simple_Sample_Buffer__
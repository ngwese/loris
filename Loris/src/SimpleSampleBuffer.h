#ifndef __Simple_Sample_Buffer__
#define __Simple_Sample_Buffer__

// ---------------------------------------------------------------------------
//	SimpleSampleBuffer.h
//
//	Implementation of a concrete SampleBuffer (SampleBuffer.h) that
//	stores its samples in a dumb c-style array of doubles that it
//	owns. 
//
//	Kelly Fitz 
//	Feb 1999
// ---------------------------------------------------------------------------

#include "SampleBuffer.h"

#include <algorithm>

Lemur_Begin_Namespace( Lemur )
Lemur_Using( std::copy )

// ===========================================================================
//		€ SimpleSampleBuffer
// ===========================================================================
//	
class SimpleSampleBuffer : public SampleBuffer
{
public:
//	construction:
//	assume responsibility for the c-style array of samples:
	SimpleSampleBuffer( Double * samples, Int32 howMany, Double sr ) : 
		mArray( samples ),
		mSize( howMany ),
		mSamplingFrequency( sr )
	{ 
	}
	
//	construction from another collection:
	template< class InputIterator >
	SimpleSampleBuffer( InputIterator b, InputIterator e, Double sr ) :
		mArray( new Double[ e - b ] ),
		mSize( e - b ),
		mSamplingFrequency( sr )
	{
		//	copy the samples:
		copy( b, e, mArray );
	}

//	copying:
	SimpleSampleBuffer( const SimpleSampleBuffer & other ) :
		mArray( new Double[ other.mSize ] ),
		mSize( other.mSize ),
		mSamplingFrequency( other.mSamplingFrequency )
	{
		copy( other.mArray, other.mArray + other.mSize, mArray );
	}
	
// 	uninitialized construction is empty buffer:
//	this is pretty safe given SampleBuffer's interface,
//	although, as always, operator[] is not checked, so
//	it can give undefined results it the client doesn't
//	do the bounds-checking.
	SimpleSampleBuffer( void ) : 
		mArray( NULL ),
		mSize( 0 ),
		mSamplingFrequency( 1. )
	{
	}
	
//	assignment:
	SimpleSampleBuffer & operator= ( const SimpleSampleBuffer & other )
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
					mSamplingFrequency = other.mSamplingFrequency;
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
	
//	destruction:
	~SimpleSampleBuffer( void )
	{
		delete[] mArray;
		mArray = NULL;
	}
	
//	public SampleBuffer interface:
virtual Double samplingFrequency( void ) const { return mSamplingFrequency; }
virtual Int32 size( void ) const { return mSize; }

//	indexed access:
virtual Double & operator[]( Uint32 index )  { return mArray[index]; }
virtual const Double & operator[]( Uint32 index ) const  { return mArray[index]; }

//	instance variables:
	Double * mArray;	//	samples
	Int32 mSize;
	Double mSamplingFrequency;	

};	//	end of class SimpleSampleBuffer

Lemur_End_Namespace( Lemur )


#endif	//	ndef __Simple_Sample_Buffer__
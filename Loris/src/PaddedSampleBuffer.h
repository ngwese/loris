#ifndef __Padded_Sample_Buffer__
#define __Padded_Sample_Buffer__

// ---------------------------------------------------------------------------
//	PaddedSampleBuffer.h
//
//	Implementation of a concrete SampleBuffer (SampleBuffer.h) that
//	stores its samples in a standard template library deque so that it
//	can easily be zero padded at either end. 
//
//	Kelly Fitz 
//	Feb 1999
// ---------------------------------------------------------------------------

#include "SampleBuffer.h"

#include <deque>

Lemur_Begin_Namespace( Lemur )
Lemur_Using( std::deque )

// ===========================================================================
//		€ PaddedSampleBuffer
// ===========================================================================
//	
class PaddedSampleBuffer : public SampleBuffer
{
public:
//	construction:
//	padding can be specified at construction, or performed later, or both!
	PaddedSampleBuffer( const Double * b, const Double * e, Double sr, 
						Uint32 pad_front, Uint32 pad_back ) : 
		v( b, e ),
		mSamplingFrequency( sr )
	{ 
		padFront( pad_front );
		padBack( pad_back );
	}

	PaddedSampleBuffer( const Double * b, const Double * e, Double sr, Uint32 pad = 0 ) : 
		v( b, e ),
		mSamplingFrequency( sr )
	{ 
		padBack( pad );
	}

	PaddedSampleBuffer( const PaddedSampleBuffer & other ) :
		v( other.v ),
		mSamplingFrequency( other.mSamplingFrequency )
	{
	}
	
// 	uninitialized construction is empty buffer:
//	this is pretty safe given SampleBuffer's interface,
//	although, as always, operator[] is not checked, so
//	it can give undefined results it the client doesn't
//	do the bounds-checking.
	PaddedSampleBuffer( void ) : 
		mSamplingFrequency( 1. )
	{
	}
	
//	assignment:
	PaddedSampleBuffer & operator= ( const PaddedSampleBuffer & other )
	{
		if ( &other != this )
		{
			v = other.v;
			mSamplingFrequency = other.mSamplingFrequency;
		}
		return *this;
	}
	
//	public SampleBuffer interface:
virtual Double samplingFrequency( void ) const { return mSamplingFrequency; }
virtual Int32 size( void ) const { return v.size(); }

//	indexed access:
virtual Double & operator[]( Uint32 index )  { return v[index]; }
virtual const Double & operator[]( Uint32 index ) const  { return v[index]; }

// 	zero padding:
	void padFront( Uint32 numZeros ) 
			{ if ( numZeros > 0 ) v.insert( v.begin(), numZeros, 0. ); }
	void padBack( Uint32 numZeros )	
			{ if ( numZeros > 0 ) v.insert( v.end(), numZeros, 0. ); }

//	instance variables:
private:
	deque< Double > v;	//	samples
	Double mSamplingFrequency;	

};	//	end of class __Padded_Sample_Buffer__

Lemur_End_Namespace( Lemur )


#endif	//	ndef __Sample_Vector__
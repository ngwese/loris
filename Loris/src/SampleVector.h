#ifndef __Sample_Vector__
#define __Sample_Vector__

// ---------------------------------------------------------------------------
//	SampleVector.h
//
//	Implementation of a concrete SampleBuffer (SampleBuffer.h) that
//	stores its samples in a standard template library vector. 
//
//	Kelly Fitz 
//	Feb 1999
// ---------------------------------------------------------------------------

#include "SampleBuffer.h"

#include <vector>

Lemur_Begin_Namespace( Lemur )
Lemur_Using( std::vector )

// ===========================================================================
//		€ SampleVector
// ===========================================================================
//	
class SampleVector : public SampleBuffer
{
public:
//	construction:
	SampleVector( const Double * b, const Double * e, Double sr ) : 
		v( b, e ),
		mSamplingFrequency( sr )
	{ 
	}

	SampleVector( const SampleVector & other ) :
		v( other.v ),
		mSamplingFrequency( other.mSamplingFrequency )
	{
	}
	
// 	uninitialized construction is empty buffer:
//	this is pretty safe given SampleBuffer's interface,
//	although, as always, operator[] is not checked, so
//	it can give undefined results it the client doesn't
//	do the bounds-checking.
	SampleVector( void ) : 
		mSamplingFrequency( 1. )
	{
	}
	
//	assignment:
	SampleVector & operator= ( const SampleVector & other )
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

//	instance variables:
private:
	vector<Double> v;	//	samples
	Double mSamplingFrequency;	

};	//	end of class SampleVector

Lemur_End_Namespace( Lemur )


#endif	//	ndef __Sample_Vector__
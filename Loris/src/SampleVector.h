#ifndef __Sample_Vector__
#define __Sample_Vector__

// ---------------------------------------------------------------------------
//	SampleVector.h
//
//	Definition of a concrete SampleBuffer (SampleBuffer.h) that
//	stores its samples in a standard template library vector. 
//
//	Kelly Fitz 
//	Feb 1999
// ---------------------------------------------------------------------------

#include "LorisLib.h"
#include "SampleBuffer.h"

#include <vector>
using std::vector;

Begin_Namespace( Loris )

// ===========================================================================
//		SampleVector
// ===========================================================================
//	
class SampleVector : public SampleBuffer
{
public:
//	construction:
	SampleVector( const double * b, const double * e );
	SampleVector( const SampleVector & other );
	
//	construction from a range:
//	(this is more likely to compile correctly
//	if it is defined in the class definition)
#if !defined( NO_TEMPLATE_MEMBERS )
	template< class InputIterator >
	SampleVector( InputIterator b, InputIterator e ) :
		v( b, e )
	{
	}
#endif

	
// 	uninitialized construction is empty buffer:
//	this is pretty safe given SampleBuffer's interface,
//	although, as always, operator[] is not checked, so
//	it can give undefined results it the client doesn't
//	do the bounds-checking.
	explicit SampleVector( int len = 0 );
	
//	assignment:
	SampleVector & operator= ( const SampleVector & other );
	
//	public SampleBuffer interface:
virtual void grow( int n )  
		{ if (size() < n) v.insert( v.end(), n-size(), 0. ); }
virtual int size( void ) const { return v.size(); }

//	indexed access:
virtual double & operator[]( ulong index )  { return v[index]; }
virtual const double & operator[]( ulong index ) const  { return v[index]; }

//	instance variables:
private:
	vector< double > v;	//	the samples

};	//	end of class SampleVector

End_Namespace( Loris )


#endif	//	ndef __Sample_Vector__
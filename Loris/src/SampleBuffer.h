#ifndef __Abstract_Sample_Buffer__
#define __Abstract_Sample_Buffer__

// ---------------------------------------------------------------------------
//	SampleBuffer.h
//
//	Definition of an abstract base class providing the interface 
//	for objects representing a single-channel buffer of floating 
//	point samples at a specified sampling frequency. Clients of
//	SampleBuffer are freed from details of the storage and access
//	of the samples.
//
//	Iterators for SampleBuffers are defined in BufferIterator.h.
//
//	Kelly Fitz 
//	Feb 1999
// ---------------------------------------------------------------------------

#include "LorisLib.h"
#include "Exception.h"

#include "BufferIterator.h"

Begin_Namespace( Loris )

// ===========================================================================
//	SampleBuffer
//
//	Abstract base class providing the interface for objects
//	representing a buffer of (double precision) floating point
//	samples.
//
//	Derived classes must implement:
//		void grow( int )
//		int size() const
//		double & operator[]( ulong )
//		const double & operator[]( ulong ) const
//
// ===========================================================================
//	
class SampleBuffer
{
public:
//	public interface:
	virtual void grow( int ) = 0;
	virtual int size( void ) const = 0;

//	indexed access:
	virtual double & operator[]( ulong ) = 0;
	virtual const double & operator[]( ulong ) const = 0;

//	boundary-checked indexed access, implemented
//	in terms of the above virtual methods:
	double & at( ulong );
	const double & at( ulong ) const;

protected:
//	construction, dervied classes only:
	SampleBuffer( void ) {}
	SampleBuffer( const SampleBuffer & ) {}

//	don't permit assignment:	
private:
	SampleBuffer & operator= ( const SampleBuffer & );	// not implemented
	
//	destruction:
//	(virtual to allow subclassing)
public:
	virtual	~SampleBuffer( void ) {}

//	Iterator access:
//	see BufferIterator.h for Iterator class definitions.
public:
	typedef Iterator_< SampleBuffer, double > Iterator;
	typedef ConstIterator_< SampleBuffer, double > ConstIterator;
	
	Iterator begin( void ) { return Iterator( *this, 0 ); }
	Iterator end( void ) { return Iterator( *this, size() ); }

	ConstIterator begin( void ) const { return ConstIterator( *this, 0 ); }
	ConstIterator end( void ) const { return ConstIterator( *this, size() ); }

};	//	end of abstract class SampleBuffer



End_Namespace( Loris )


#endif	//	ndef __Abstract_Sample_Buffer__
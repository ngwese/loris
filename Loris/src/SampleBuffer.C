// ---------------------------------------------------------------------------
//	SampleBuffer.C
//
//	Implementation of non-virtual members for an abstract 
//	base class providing the interface for objects
//	representing a buffer of (double precision) floating 
//	point samples at a specified sampling frequency.
//
//	Kelly Fitz 
//	Feb 1999
// ---------------------------------------------------------------------------

#include "LorisLib.h"
#include "SampleBuffer.h"

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	at
// ---------------------------------------------------------------------------
//	Boundary-checked access.
//
double & 
SampleBuffer::at( ulong index )
{
	if ( index >= size() )
		//	except
		Throw( IndexOutOfBounds, "bad index in SampleBuffer::at()." );
	
	//	else return sample at index:
	return operator[]( index );
}

// ---------------------------------------------------------------------------
//	at (const)
// ---------------------------------------------------------------------------
//	Boundary-checked access, for const SampleBuffers.
//
const double & 
SampleBuffer::at( ulong index ) const
{
	if ( index >= size() )
		//	except
		Throw( IndexOutOfBounds, "bad index in SampleBuffer::at()." );
	
	//	else return sample at index:
	return operator[]( index );
}


End_Namespace( Loris )

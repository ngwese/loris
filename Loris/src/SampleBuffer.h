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
//	Like the STL vector template, SampleBuffer offers two indexing
//	mechanisms, operator[] which is not bounds-checked, and at() which
//	performs bounds-checking, then calls operator[]. Note that even
//	operator[] is not as fast as a vector or dumb array access, as
//	it is a virtual function call. 
//
//	Includes a complete specification for random-access Iterators 
//	that can traverse any object that implements SampleBuffer.
//	SampleBuffer::Iterator conforms to the STL specification for 
//	random access iterators, which means that the interface is
//	lot bulkier than the GoF Iterator interface, but also means 
//	that it can be used with any of the STL algorithms. 
//
//	Unlike STL iterators, SampleBuffer::Iterators cannot be dereferenced
//	if they are out-of-bounds, they will except (because they use the
//	bounds-checked accessors in SampleBuffer). Moreover, Iterators
//	must refer to a particular instance of SampleBuffer (thus there is
//	no default, uninitialized constructor) and that instance cannot be
//	changed. Any attempt to change the subject of the Iterator will
//	except. Binary operations on Iterators that don't make sense 
//	for Iterators having different subjects also except under those
//	conditions.
//
//	Iterators are _not_ claimed to be thread-safe.
//	
//	Kelly Fitz 
//	Feb 1999
// ---------------------------------------------------------------------------

#include <iterator>

#include "LorisLib.h"
#include "Exception.h"

using std::iterator;
using std::random_access_iterator_tag;

Begin_Namespace( Loris )

// ===========================================================================
//	SampleBuffer
//
//	Abstract base class providing the interface for objects
//	representing a buffer of (double precision) floating point
//	samples.
//
//	Derived classes must implement:
//		Int size() const
//		Double & operator[]( Uint )
//		const Double & operator[]( Uint ) const
//
// ===========================================================================
//	
class SampleBuffer
{
public:
//	public interface:
virtual Int size( void ) const = 0;

//	indexed access:
virtual Double & operator[]( Uint ) = 0;
virtual const Double & operator[]( Uint ) const = 0;

//	boundary-checked indexed access, implemented
//	in terms of the above virtual methods:
	Double & at( Uint );
	const Double & at( Uint ) const;

protected:
//	construction, dervied classes only:
	SampleBuffer( void ) {}
	SampleBuffer( const SampleBuffer & ) {}

//	don't permit assignment:	
private:
	SampleBuffer & operator= ( const SampleBuffer & );	// not implemented
	
//	destruction:
public:
virtual	~SampleBuffer( void ) {}

//
//	Iterator definition
//
//	Define a random access iterator that can 
//	traverse any object derived from SampleBuffer. 
//
//	Could use a template to allow me to define const
//	and non-const types without typing everything 
//	twice. The drawback to that approach is that 
//	I can't provide a conversion from Iterator to a 
//	ConstIterator.
//
//	So, use the approach in the STL, two separate
//	definitions, mostly identical. 
//	
//	Need forward declaration of class ConstIterator
//	so that we can make it a friend. This is a bug, 
//	I think. Shouldn't need this.
	class ConstIterator;
public:
	class Iterator : 
		public iterator< random_access_iterator_tag, Double, 
							Int, Double *, Double & >
	{
	//	SampleBuffer is a friend of its Iterator, so
	//	that only SampleBuffer can access the private
	//	constructor:
		friend class SampleBuffer;
		
	//	ConstIterator is also a friend, so that they can
	//	be converted:
		friend class ConstIterator;
		
	//	Iterator keeps a reference to its subject 
	//	SampleBuffer (Aggregate), it uses only the 
	//	interface defined in SampleBuffer, so it 
	//	doesn't need to know the concrete class.
	//
	//	Need to store a pointer (rather than a
	//	rerference) because we need to allow 
	//	assignment, and therefore may need to 
	//	change the subject after initialization.
	//
		typedef SampleBuffer Buffer_Type;
		Buffer_Type & mSubject;
		
	//	Iterator stores its position in the 
	//	traversal:
		Int mPosition;
		
	//	The private constructor is available only to 
	//	SampleBuffer:
		Iterator( Buffer_Type & sb, Int pos ) :
			mSubject( sb ),
			mPosition( 0 )
		{
			setPosition( pos );
		}
		
	//	don't allow uninitialized construction:
	//	is this a problem for STL compliance?
	private:
		Iterator( void );
		
	//	anyone can copy an Iterator:
	public:
		Iterator( const Iterator & other ) :
			mSubject( other.mSubject ),
			mPosition( other.mPosition )
		{
		}
		
	//	assignment is okay too, as long as the 
	//	subject doesn't change:
		Iterator & operator= ( const Iterator & other )
		{
			//	check for identity:
			if ( &other != this )
			{
				checkSubject( other ); 
				mPosition = other.mPosition;
			}
			return *this;
		}
		
	public:
	//	access to Aggregate:
		value_type & operator * ( void ) 
			{ return mSubject.at( mPosition ); }
		
	//	increment:
	//	define operator ++() and implement all other
	//	increments and decrements in terms of that one.
		Iterator & operator += ( Int n )
			{ setPosition( mPosition + n ); return *this; }
		
		Iterator & operator ++( void ) 
			{ *this += 1; return *this; }
		
		Iterator operator ++( int ) 
			{ Iterator it( *this ); *this += 1; return it; }
		
		Iterator operator +( Int  n )
			{ return Iterator( *this ) += n; } 
		
	//	decrement:
		Iterator & operator -= ( Int n )
			{ *this += -n; return *this; }
		
		Iterator & operator --( void ) 
			{ *this += -1; return *this; }
		
		Iterator operator --( int ) 
			{ Iterator it( *this ); *this += -1; return it; }
			
		Iterator operator -( Int  n ) 
			{ return Iterator( *this ) -= n; }
		
	//	comparison:	
	//	note - STL iterators don't seem to check that they share
	//	the same subject before computing difference or comparing.
		Boolean operator == ( const Iterator & other ) const
			{ checkSubject( other ); return mPosition == other.mPosition;}
		
		Boolean operator < ( const Iterator & other ) const
			{ checkSubject( other ); return mPosition < other.mPosition; }
		
		Boolean operator > ( const Iterator & other ) const
			{ checkSubject( other ); return mPosition > other.mPosition; }
		
		Boolean operator != ( const Iterator & other ) const
			{ return ! operator == (other); }
			
		Boolean operator <= ( const Iterator & other ) const
			{ return ! operator > (other); }
			
		Boolean operator >= ( const Iterator & other ) const
			{ return ! operator < (other); }		
		
	//	distance:
	//	note - STL iterators don't seem to check that they share
	//	the same subject before computing difference or comparing.
		Int operator - ( const Iterator & other ) const
			{ checkSubject( other ); return mPosition - other.mPosition; }
		
	//	helper for setting position with boundary checking:
	//	allow only valid indices on the subject, and one index
	//	out-of-range at each end. The out-of-range values are
	//	used for comparisons and traversal boudary testing, but 
	//	cannot be dereferenced.
	private:
		void setPosition( Int k )
		{
			if ( k < -1 )
				mPosition = -1;
			else if ( k > mSubject.size() )
				mPosition = mSubject.size();
			else
				mPosition = k;
		}
		
	//	many operations involving two Iterators don't make 
	//	sense if the two have different subjects, sameSubject()
	//	is a helper for identifying this condition.
	//	Verify that the subjects refer to the same memory
	//	location:
	//	note - STL iterators don't seem to check that they share
	//	the same subject before computing difference or comparing.
		void checkSubject( const Iterator & other ) const 
		{
			if ( & mSubject != & other.mSubject )
				Throw( RuntimeException, "Operation requires SampleBuffer Iterators have same subject!" );
		} 

	};	//	end of class Iterator

//
//	ConstIterator definition
//
//	Almost exactly the same as Iterator. 
//
public:
	class ConstIterator : 
		public iterator< random_access_iterator_tag, const Double, 
							Int, const Double *, const Double & >
	{
	//	SampleBuffer is a friend of its Iterator, so
	//	that only SampleBuffer can access the private
	//	constructor:
		friend class SampleBuffer;
		
	//	Iterator keeps a reference to its subject 
	//	SampleBuffer (Aggregate), it uses only the 
	//	interface defined in SampleBuffer, so it 
	//	doesn't need to know the concrete class.
	//
	//	Need to store a pointer (rather than a
	//	rerference) because we need to allow 
	//	assignment, and therefore may need to 
	//	change the subject after initialization.
	//
		typedef const SampleBuffer Buffer_Type;
		Buffer_Type & mSubject;
		
	//	Iterator stores its position in the 
	//	traversal:
		Int mPosition;
		
	//	The private constructor is available only to 
	//	SampleBuffer:
		ConstIterator( Buffer_Type & sb, Int pos ) :
			mSubject( sb ),
			mPosition( 0 )
		{
			setPosition( pos );
		}
		
	//	don't allow uninitialized construction:
	//	is this a problem for STL compliance?
	private:
		ConstIterator( void );
		
	//	anyone can copy an ConstIterator:
	public:
		ConstIterator( const ConstIterator & other ) :
			mSubject( other.mSubject ),
			mPosition( other.mPosition )
		{
		}
		
	//	construction from a non-const Iterator:
	//	this is the principle (non-templatizable)
	//	difference between Iterator and ConstIterator.
		ConstIterator( const Iterator & other ) :
			mSubject( other.mSubject ),
			mPosition( other.mPosition )
		{
		}
		
	//	assignment is okay too, as long as the 
	//	subject doesn't change:
		ConstIterator & operator= ( const ConstIterator & other )
		{
			//	check for identity:
			if ( &other != this )
			{
				checkSubject( other ); 
				mPosition = other.mPosition;
			}
			return *this;
		}
		
	public:
	//	access to Aggregate:
		value_type & operator * ( void ) 
			{ return mSubject.at( mPosition ); }
		
	//	increment:
	//	define operator ++() and implement all other
	//	increments and decrements in terms of that one.
		ConstIterator & operator += ( Int n )
			{ setPosition( mPosition + n ); return *this; }
		
		ConstIterator & operator ++( void ) 
			{ *this += 1; return *this; }
		
		ConstIterator operator ++( int ) 
			{ ConstIterator it( *this ); *this += 1; return it; }
		
		ConstIterator operator +( Int  n )
			{ return ConstIterator( *this ) += n; } 
		
	//	decrement:
		ConstIterator & operator -= ( Int n )
			{ *this += -n; return *this; }
		
		ConstIterator & operator --( void ) 
			{ *this += -1; return *this; }
		
		ConstIterator operator --( int ) 
			{ ConstIterator it( *this ); *this += -1; return it; }
			
		ConstIterator operator -( Int  n ) 
			{ return ConstIterator( *this ) -= n; }
		
	//	comparison:	
	//	note - STL iterators don't seem to check that they share
	//	the same subject before computing difference or comparing.
		Boolean operator == ( const ConstIterator & other ) const
			{ checkSubject( other ); return mPosition == other.mPosition;}
		
		Boolean operator < ( const ConstIterator & other ) const
			{ checkSubject( other ); return mPosition < other.mPosition; }
		
		Boolean operator > ( const ConstIterator & other ) const
			{ checkSubject( other ); return mPosition > other.mPosition; }
		
		Boolean operator != ( const ConstIterator & other ) const
			{ return ! operator == (other); }
			
		Boolean operator <= ( const ConstIterator & other ) const
			{ return ! operator > (other); }
			
		Boolean operator >= ( const ConstIterator & other ) const
			{ return ! operator < (other); }		
		
	//	distance:
	//	note - STL iterators don't seem to check that they share
	//	the same subject before computing difference or comparing.
		Int operator - ( const ConstIterator & other ) const
			{ checkSubject( other ); return mPosition - other.mPosition; }
		
	//	helper for setting position with boundary checking:
	//	allow only valid indices on the subject, and one index
	//	out-of-range at each end. The out-of-range values are
	//	used for comparisons and traversal boudary testing, but 
	//	cannot be dereferenced.
	private:
		void setPosition( Int k )
		{
			if ( k < -1 )
				mPosition = -1;
			else if ( k > mSubject.size() )
				mPosition = mSubject.size();
			else
				mPosition = k;
		}
		
	//	many operations involving two Iterators don't make 
	//	sense if the two have different subjects, sameSubject()
	//	is a helper for identifying this condition.
	//	Verify that the subjects refer to the same memory
	//	location:
	//	note - STL iterators don't seem to check that they share
	//	the same subject before computing difference or comparing.
		void checkSubject( const ConstIterator & other ) const 
		{
			if ( & mSubject != & other.mSubject )
				Throw( RuntimeException, "Operation requires SampleBuffer Iterators have same subject!" );
		} 

	};	//	end of class ConstIterator

//	Iterator access:
public:
	Iterator begin( void ) { return Iterator( *this, 0 ); }
	Iterator end( void ) { return Iterator( *this, size() ); }

	ConstIterator begin( void ) const { return ConstIterator( *this, 0 ); }
	ConstIterator end( void ) const { return ConstIterator( *this, size() ); }

};	//	end of abstract class SampleBuffer

End_Namespace( Loris )


#endif	//	ndef __Abstract_Sample_Buffer__
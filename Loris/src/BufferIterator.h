#ifndef ___Sample_Buffer_Iterator___
#define ___Sample_Buffer_Iterator___

// ---------------------------------------------------------------------------
//	BufferIterator.h
//
//	--- Never include this file, only include SampleBuffer.h. ---
//
//	Definition of a class of Iterators for SampleBuffers. 
//	(see SampleBuffer.h)
//	These used to be designed in the SampleBuffer class definition, 
//	they have been isolated in this file to improve the clarity of that 
//	definition, which used to be consumed by these rarely-used 
//	iterator definitions. 
//
//	These iterators are templatized on the buffer type to resolve 
//	header inclusion order conflicts. They are designed specifically
//	to iterate SampleBuffers and derivatives, although they could 
//	probably be used to iterate anything with a similar interface,
//	like a regular old vector<>, but there's no good reason to use 
//	them that way, they are not fancy or special.
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
//	These iterators are _not_ claimed to be thread-safe.
//	
//	Kelly Fitz 
//	Feb 1999
// ---------------------------------------------------------------------------

#include "LorisLib.h"
#include "Exception.h"

#include <iterator>
using std::iterator;
using std::random_access_iterator_tag;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class ConstBufIterator_
//
//	ConstBufIterator_ definition
//
//	Define a random access iterator that can 
//	traverse any object derived from SampleBuffer. 
//
//	Could use a template to allow me to define const
//	and non-const types without typing everything 
//	twice. The drawback to that approach is that 
//	I can't provide a conversion from BufIterator_ to a 
//	ConstBufIterator_.
//
//	So, use the approach in the STL, two separate
//	definitions, mostly identical. 
//
template< class Buffer_Type, class Value_Type > class BufIterator_;	// fwd declaration, 
																	// for conversion
	
template< class Buffer_Type, class Value_Type >
class ConstBufIterator_ : 
	public iterator< random_access_iterator_tag, const Value_Type >
{
//	Buffer_Type is a friend of its BufIterator_, so
//	that only SampleBuffer can access the private
//	constructor:
	friend class Buffer_Type;
	
//	BufIterator_ keeps a reference to its subject 
//	SampleBuffer (Aggregate), it uses only the 
//	interface defined in SampleBuffer, so it 
//	doesn't need to know the concrete class.
	const Buffer_Type & mSubject;
	
//	BufIterator_ stores its position in the 
//	traversal:
	int mPosition;
	
//	The private constructor is available only to 
//	SampleBuffer:
	ConstBufIterator_( const Buffer_Type & sb, int pos ) :
		mSubject( sb ),
		mPosition( 0 )
	{
		setPosition( pos );
	}
	
//	don't allow uninitialized construction:
//	is this a problem for STL compliance?
private:
	ConstBufIterator_( void );
	
//	anyone can copy an ConstBufIterator_:
public:
	ConstBufIterator_( const ConstBufIterator_ & other ) :
		mSubject( other.mSubject ),
		mPosition( other.mPosition )
	{
	}
	
//	construction from a non-const BufIterator_:
//	this is the principle (non-templatizable)
//	difference between BufIterator_ and ConstBufIterator_.
	ConstBufIterator_( const BufIterator_< Buffer_Type, double > & other ) :
		mSubject( other.mSubject ),
		mPosition( other.mPosition )
	{
	}
	
//	assignment is okay too, as long as the 
//	subject doesn't change:
	ConstBufIterator_ & operator= ( const ConstBufIterator_ & other )
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
//	Should be able to use iterator::value_type, but it
//	won't compile here.
	Value_Type & operator * ( void ) 
		{ return mSubject.at( mPosition ); }
	
//	increment:
//	define operator ++() and implement all other
//	increments and decrements in terms of that one.
	ConstBufIterator_ & operator += ( int n )
		{ setPosition( mPosition + n ); return *this; }
	
	ConstBufIterator_ & operator ++( void ) 
		{ *this += 1; return *this; }
	
	ConstBufIterator_ operator ++( int ) 
		{ ConstBufIterator_ it( *this ); *this += 1; return it; }
	
	ConstBufIterator_ operator +( int  n )
		{ return ConstBufIterator_( *this ) += n; } 
	
//	decrement:
	ConstBufIterator_ & operator -= ( int n )
		{ *this += -n; return *this; }
	
	ConstBufIterator_ & operator --( void ) 
		{ *this += -1; return *this; }
	
	ConstBufIterator_ operator --( int ) 
		{ ConstBufIterator_ it( *this ); *this += -1; return it; }
		
	ConstBufIterator_ operator -( int  n ) 
		{ return ConstBufIterator_( *this ) -= n; }
	
//	comparison:	
//	note - STL iterators don't seem to check that they share
//	the same subject before computing difference or comparing.
	boolean operator == ( const ConstBufIterator_ & other ) const
		{ checkSubject( other ); return mPosition == other.mPosition;}
	
	boolean operator < ( const ConstBufIterator_ & other ) const
		{ checkSubject( other ); return mPosition < other.mPosition; }
	
	boolean operator > ( const ConstBufIterator_ & other ) const
		{ checkSubject( other ); return mPosition > other.mPosition; }
	
	boolean operator != ( const ConstBufIterator_ & other ) const
		{ return ! operator == (other); }
		
	boolean operator <= ( const ConstBufIterator_ & other ) const
		{ return ! operator > (other); }
		
	boolean operator >= ( const ConstBufIterator_ & other ) const
		{ return ! operator < (other); }		
	
//	distance:
//	note - STL iterators don't seem to check that they share
//	the same subject before computing difference or comparing.
	int operator - ( const ConstBufIterator_ & other ) const
		{ checkSubject( other ); return mPosition - other.mPosition; }
	
//	helper for setting position with boundary checking:
//	allow only valid indices on the subject, and one index
//	out-of-range at each end. The out-of-range values are
//	used for comparisons and traversal boudary testing, but 
//	cannot be dereferenced.
private:
	void setPosition( int k )
	{
		if ( k < -1 )
			mPosition = -1;
		else if ( k > mSubject.size() )
			mPosition = mSubject.size();
		else
			mPosition = k;
	}
	
//	many operations involving two Iterator_s don't make 
//	sense if the two have different subjects, sameSubject()
//	is a helper for identifying this condition.
//	Verify that the subjects refer to the same memory
//	location:
//	note - STL iterators don't seem to check that they share
//	the same subject before computing difference or comparing.
	void checkSubject( const ConstBufIterator_ & other ) const 
	{
		if ( & mSubject != & other.mSubject )
			Throw( InvalidIterator, "Operation requires Buffer Iterators have same subject!" );
	} 

};	//	end of class ConstBufIterator_
	
// ---------------------------------------------------------------------------
//	class BufIterator_
//
//	Almost exactly the same as ConstBufIterator_. 
//			
template< class Buffer_Type, class Value_Type >
class BufIterator_ : 
	public iterator< random_access_iterator_tag, Value_Type > 
{
//	Buffer_Type is a friend of its BufIterator_, so
//	that only SampleBuffer can access the private
//	constructor:
	friend class Buffer_Type;
	
//	ConstBufIterator_ is also a friend, so that they can
//	be converted:
	friend class ConstBufIterator_< Buffer_Type, Value_Type >;
	
//	BufIterator_ keeps a reference to its subject 
//	SampleBuffer (Aggregate), it uses only the 
//	interface defined in SampleBuffer, so it 
//	doesn't need to know the concrete class.
	Buffer_Type & mSubject;
	
//	BufIterator_ stores its position in the 
//	traversal:
	int mPosition;
	
//	The private constructor is available only to 
//	SampleBuffer:
	BufIterator_( Buffer_Type & sb, int pos ) :
		mSubject( sb ),
		mPosition( 0 )
	{
		setPosition( pos );
	}
	
//	don't allow uninitialized construction:
//	is this a problem for STL compliance?
private:
	BufIterator_( void );
	
//	anyone can copy an BufIterator_:
public:
	BufIterator_( const BufIterator_ & other ) :
		mSubject( other.mSubject ),
		mPosition( other.mPosition )
	{
	}
	
//	assignment is okay too, as long as the 
//	subject doesn't change:
	BufIterator_ & operator= ( const BufIterator_ & other )
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
//	Should be able to use iterator::value_type, but it
//	won't compile here.
	Value_Type & operator * ( void ) 
		{ return mSubject.at( mPosition ); }
	
//	increment:
//	define operator ++() and implement all other
//	increments and decrements in terms of that one.
	BufIterator_ & operator += ( int n )
		{ setPosition( mPosition + n ); return *this; }
	
	BufIterator_ & operator ++( void ) 
		{ *this += 1; return *this; }
	
	BufIterator_ operator ++( int ) 
		{ BufIterator_ it( *this ); *this += 1; return it; }
	
	BufIterator_ operator +( int  n )
		{ return BufIterator_( *this ) += n; } 
	
//	decrement:
	BufIterator_ & operator -= ( int n )
		{ *this += -n; return *this; }
	
	BufIterator_ & operator --( void ) 
		{ *this += -1; return *this; }
	
	BufIterator_ operator --( int ) 
		{ BufIterator_ it( *this ); *this += -1; return it; }
		
	BufIterator_ operator -( int  n ) 
		{ return BufIterator_( *this ) -= n; }
	
//	comparison:	
//	note - STL iterators don't seem to check that they share
//	the same subject before computing difference or comparing.
	boolean operator == ( const BufIterator_ & other ) const
		{ checkSubject( other ); return mPosition == other.mPosition;}
	
	boolean operator < ( const BufIterator_ & other ) const
		{ checkSubject( other ); return mPosition < other.mPosition; }
	
	boolean operator > ( const BufIterator_ & other ) const
		{ checkSubject( other ); return mPosition > other.mPosition; }
	
	boolean operator != ( const BufIterator_ & other ) const
		{ return ! operator == (other); }
		
	boolean operator <= ( const BufIterator_ & other ) const
		{ return ! operator > (other); }
		
	boolean operator >= ( const BufIterator_ & other ) const
		{ return ! operator < (other); }		
	
//	distance:
//	note - STL iterators don't seem to check that they share
//	the same subject before computing difference or comparing.
	int operator - ( const BufIterator_ & other ) const
		{ checkSubject( other ); return mPosition - other.mPosition; }
	
//	helper for setting position with boundary checking:
//	allow only valid indices on the subject, and one index
//	out-of-range at each end. The out-of-range values are
//	used for comparisons and traversal boudary testing, but 
//	cannot be dereferenced.
private:
	void setPosition( int k )
	{
		if ( k < -1 )
			mPosition = -1;
		else if ( k > mSubject.size() )
			mPosition = mSubject.size();
		else
			mPosition = k;
	}
	
//	many operations involving two Iterator_s don't make 
//	sense if the two have different subjects, sameSubject()
//	is a helper for identifying this condition.
//	Verify that the subjects refer to the same memory
//	location:
//	note - STL iterators don't seem to check that they share
//	the same subject before computing difference or comparing.
	void checkSubject( const BufIterator_ & other ) const 
	{
		if ( & mSubject != & other.mSubject )
			Throw( InvalidIterator, "Operation requires Buffer Iterators have same subject!" );
	} 

};	//	end of class BufIterator_

End_Namespace( Loris )

#endif	//	nedf ___Sample_Buffer_Iterator___

#ifndef __Loris_partial_iterator__
#define __Loris_partial_iterator__

// ===========================================================================
//	PartialIterator.h
//
//	Class definintion for Loris::PartialIterator, a const iterator over Partials.
//	PartialIterator and derived classes cannot be used to modify partials,
//	since the subject is a const reference, but they can be used to compute 
//	transformed Partial data on the fly, without creating a new Partial.
//
//	-kel 7 Oct 99
//
// ===========================================================================
#include "LorisLib.h"
#include "Partial.h"

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class PartialIterator
//
//	Dervied classes should override the Partial and Breakpoint access
//	members if they need to provide transformed Partial envelope data.
//	Derived classed may also override advance() and rewind() if they need
//	to update their state when the cursor moves.
//
//	reset() is provided to allow the same iterator to be applied to many
//	Partials, and also allow the iterator to be configured and referenced
//	before applying it to a particular Partial.
//
//	Dervied classes must provide a clone() member so that objects
//	composed of PartialIterators can be copied.
//
class PartialIterator
{
public:
	PartialIterator( void );
	PartialIterator( const Partial & pin );
	
	//	compiler-generated copy constructor is adequate:
	//PartialIterator( const PartialIterator & other );
	
	//	need to make this virtual as a base class:
	virtual ~PartialIterator( void ) {}
	
	//	HEY make this pure virtual when PartialIterator 
	//	becomes an abstract class:
	virtual PartialIterator * clone( void ) const 
			{ return new PartialIterator( *this ); }
	
	//	iterator interface:
	virtual void advance( void );			//	may be overridden
	virtual void reset( const Partial & p );//	may be overridden
	virtual void rewind( void );			//	may be overridden
	
	boolean atEnd( void ) const;
	boolean isHead( void ) const;
	boolean isTail( void ) const;
	
	const Breakpoint & current( void ) const;
	const Partial & subject( void ) const;
	
	//	Partial access:
	virtual double duration( void ) const;
	virtual double startTime( void ) const;
	virtual double endTime( void ) const;
	virtual double initialPhase( void ) const;
	virtual int label( void ) const;
	
	//	Breakpoint access:
	virtual double frequency( void ) const;
	virtual double amplitude( void ) const;
	virtual double bandwidth( void ) const;
	virtual double phase( void ) const;
	virtual double time( void ) const;

private:	
	const Partial * _p;
	Partial::const_iterator _cur;
	
};	//	end of class PartialIterator


End_Namespace( Loris )

#endif	// ndef __Loris_partial_iterator__

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
#include "Partial.h"

#include <memory>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif


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
//	protected construction 
//	(base class only, cannot instantiate)
protected:
	PartialIterator( void ) {}
	PartialIterator( const PartialIterator & ) {}
	
public:
	//	need to make this virtual as a base class:
	virtual ~PartialIterator( void ) {}
	
	//	derived classes must provide a virtual constructor:
	virtual PartialIterator * clone( void ) const = 0;
	
	//	reset applies the Iterator to a new Partial: 
	virtual void reset( const Partial & p ) = 0;
	
	//	iterator interface:
	virtual void advance( void ) = 0;
	virtual bool atEnd( void ) const = 0;
	
	//	Partial access:
	virtual double duration( void ) const = 0;
	virtual double startTime( void ) const = 0;
	virtual double endTime( void ) const = 0;
	virtual double initialPhase( void ) const = 0;
	virtual int label( void ) const = 0;
	
	//	Breakpoint access:
	virtual double frequency( void ) const = 0;
	virtual double amplitude( void ) const = 0;
	virtual double bandwidth( void ) const = 0;
	virtual double phase( void ) const = 0;
	virtual double time( void ) const = 0;

};	//	end of abstract base class PartialIterator

//	definition of auto-ptr type:
typedef std::auto_ptr< PartialIterator > PartialIteratorPtr;

// ---------------------------------------------------------------------------
//	class BasicPartialIterator
//
class BasicPartialIterator : public PartialIterator
{
public:
	BasicPartialIterator( void );
	BasicPartialIterator( const Partial & pin );
	BasicPartialIterator( const BasicPartialIterator & other );
	
	virtual ~BasicPartialIterator( void ) {}
	
	//	derived classes must provide a virtual constructor:
	//	In standard C++, an overriding member can return a type that
	//	is derived from the return type of the overidden member.
	//	But not in MIPSPro C++.
virtual
#if defined(__sgi) && ! defined(__GNUC__)
	PartialIterator * 
#else
	BasicPartialIterator *	
#endif
		clone( void ) const { return new BasicPartialIterator( *this ); }
	
	//	reset applies the Iterator to a new Partial: 
	virtual void reset( const Partial & p );
	
	//	iterator interface:
	virtual void advance( void );
	virtual bool atEnd( void ) const;
	
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
	
protected:
	const Breakpoint & current( void ) const;
	const Partial & subject( void ) const;
	
private:	
	const Partial * _p;
	Partial::const_iterator _cur;
	
};	//	end of class BasicPartialIterator

// ---------------------------------------------------------------------------
//	class PartialDecorIterator
//
class PartialDecorIterator : public PartialIterator
{
//	protected construction 
//	(base class only, cannot instantiate)
protected:
	PartialDecorIterator( void ) : 
		_iter( new BasicPartialIterator() ) {}
	PartialDecorIterator( const Partial & pin ) : 
		_iter( new BasicPartialIterator(pin) ) {}
	PartialDecorIterator( PartialIteratorPtr iter ) : 
		_iter( iter ) {}
		
	//	compiler-generated copy constructor is not adequate
	//	in this case, because auto_ptrs (the _iter member)
	//	cannot be copied directly:
	PartialDecorIterator( const PartialDecorIterator & other ) : 
		_iter( other._iter->clone() ),
		PartialIterator( other ) {}
	
public:
	virtual ~PartialDecorIterator( void ) {}
	
	//	reset applies the Iterator to a new Partial: 
	virtual void reset( const Partial & p ) { iterator()->reset( p ); }
	
	//	iterator interface:
	virtual void advance( void ) { iterator()->advance(); }
	virtual bool atEnd( void ) const { return iterator()->atEnd(); }
	
	//	Partial access:
	virtual double duration( void ) const	{ return iterator()->duration(); }
	virtual double startTime( void ) const	{ return iterator()->startTime(); }
	virtual double endTime( void ) const	{ return iterator()->endTime(); }
	virtual double initialPhase( void ) const { return iterator()->initialPhase(); }
	virtual int label( void ) const			{ return iterator()->label(); }
	
	//	Breakpoint access:
	virtual double frequency( void ) const	{ return iterator()->frequency(); }
	virtual double amplitude( void ) const	{ return iterator()->amplitude(); }
	virtual double bandwidth( void ) const	{ return iterator()->bandwidth(); }
	virtual double phase( void ) const 		{ return iterator()->phase(); }
	virtual double time( void ) const		{ return iterator()->time(); }
	
protected:
	const PartialIteratorPtr & iterator( void ) const	{ return _iter; }
	
private:	
	PartialIteratorPtr _iter;
	
};	//	end of abstract base class PartialDecorIterator

// ---------------------------------------------------------------------------
//	class PartialIteratorOwner
//
//	Mixin class.
/*
class PartialIteratorOwner
{
//	protected construction 
//	(base class only, cannot instantiate)
protected:
	PartialIteratorOwner( void ) : 
		_iter( new BasicPartialIterator() ) {}
	PartialIteratorOwner( PartialIteratorPtr iter ) : 
		_iter( iter ) {}
		
	//	compiler-generated copy constructor is not adequate
	//	in this case, because auto_ptrs (the _iter member)
	//	cannot be copied directly:
	PartialIteratorOwner( const PartialIteratorOwner & other ) : 
		_iter( other._iter->clone() ) {}
	
private:
	//	not defined:			
	PartialIteratorOwner & operator= ( const PartialIteratorOwner & );

public:
	virtual ~PartialIteratorOwner( void ) {}

	//	iterator access and mutation:
	const PartialIteratorPtr & iterator( void ) const { return _iter; }

	PartialIteratorPtr 
	setIterator( PartialIteratorPtr inIter = PartialIteratorPtr( new BasicPartialIterator() ) ) 
	{
		PartialIteratorPtr ret( _iter );
		_iter = inIter;
		return ret;
	}

private:	
	PartialIteratorPtr _iter;
		
};	//	end of mixin class PartialIteratorOwner
*/
#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef __Loris_partial_iterator__

#ifndef INCLUDE_PARTIALVIEW_H
#define INCLUDE_PARTIALVIEW_H
/*
 * Copyright (c) 1999-2000 Kelly Fitz and Lippold Haken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * PartialView.h
 *
 * Definitions of PartialView (abstract interface class), PartialViewIterator,
 * BasicPartialView (concrete), and PartialViewDecorator (abstract Decorator).
 * Together these classes can be used to provide a filtered or altered view
 * on a Partial's parameter envelope without altering the Partial itself.
 * Moreover, the filters and alterations can be added incrementally using the
 * Decorator pattern (GoF p. 175).
 *
 * Kelly Fitz, 15 May 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 
// ===========================================================================
//	Custom PartialView subclasses can be derived from PartialViewDecorator
//	and inserted in a chain of decorated views, or applied by objects that
//	can use views for interpreting Partial parameter envelopes.
//
//	In the old Lemur days, this sure seemed like the most important thing in
//	the world. It turns out not to be that useful anymore. In most applications
//	we just copy Partials and modify them in-place. The only PartialView client
//	is synthesizer, which can be assigned a PartialView to provide modified
//	synthesis without altering the Partials. In practice, the default
//	(BasicPartialView) is always used.
//
//	In light of this, and the fact that the encapsulation is lousy at this
// 	point, maybe they should just be decomissioned.
//
//	These classes do not perform any sanity checking to make sure that
//	they are not misused. Clients are responsible for making sure that
//	the iterators are valid and that the views are viewing a Partial.
//	Otherwise the results are undefined, and unlikely to be satisfying.
//
// ===========================================================================
#include "Partial.h"
#include <memory> 		//	for auto_ptr 

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

class PartialViewIterator;

// ---------------------------------------------------------------------------
//	class PartialView
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
//	composed of PartialViews can be copied.
//
class PartialView
{
//	-- public interface --
public:
	//	view applies the Iterator to a new Partial: 
	virtual void view( const Partial & p ) = 0;
	
	//	iterator creation:
	virtual PartialViewIterator begin( void ) const = 0;
	virtual PartialViewIterator end( void ) const = 0;
	
	//	iterator interface:
	virtual void advance( PartialConstIterator & iter, int n = 1 ) const = 0;
	virtual bool atEnd( const PartialConstIterator & iter ) const = 0;

	//	Breakpoint access:
	virtual double frequency( const PartialConstIterator & iter ) const = 0;
	virtual double amplitude( const PartialConstIterator & iter ) const = 0;
	virtual double bandwidth( const PartialConstIterator & iter ) const = 0;
	virtual double phase( const PartialConstIterator & iter ) const = 0;
	virtual double time( const PartialConstIterator & iter ) const = 0;
	
	//	derived classes must provide a virtual constructor:
	virtual PartialView * clone( void ) const = 0;
	
	//	need to make this virtual as a base class:
	virtual ~PartialView( void ) {}
	
	//	Partial access:
	//	These are implemented in terms of the
	//	abstract Breakpoint access members.
	double startTime( void ) const;
	double endTime( void ) const;
	double duration( void ) const;
	double initialPhase( void ) const;
	
//	protected construction 
//	(base class only, cannot instantiate)
protected:
	PartialView( void ) {}
	PartialView( const PartialView & ) {}
	
};	//	end of abstract base class PartialView

// ---------------------------------------------------------------------------
//	class PartialViewIterator
//
class PartialViewIterator
{
//	-- instance variables --
	const PartialView * _view; 
	PartialConstIterator _pos;	
	
//	-- public interface --
public:
	//	default (bogus) construction:
	PartialViewIterator( void );
	
	//	copy and assignment
	PartialViewIterator( const PartialViewIterator & rhs );
	PartialViewIterator & operator = ( const PartialViewIterator & rhs );
	
	//	initialized construction (by PartialView): 
	PartialViewIterator( const PartialView & v, const PartialConstIterator & piter );
	
	//	comparison:
	friend bool operator == ( const PartialViewIterator & lhs, 
							  const PartialViewIterator & rhs )
		{ return (lhs._view == rhs._view) && (lhs._pos == rhs._pos); }
		
	//	iterator interface:
	void advance( int n = 1 ) { _view->advance( _pos, n ); }
	bool atEnd( void ) { return _view->atEnd( _pos ); }
	
	//	Breakpoint access:
	double frequency( void ) 
		{ return _view->frequency( _pos ); }
	double amplitude( void ) 
		{ return _view->amplitude( _pos ); }
	double bandwidth( void ) 
		{ return _view->bandwidth( _pos ); }
	double phase( void ) 
		{ return _view->phase( _pos ); }
 	double time( void ) 
		{ return _view->time( _pos ); }

};	//	end of abstract base class PartialView


// ---------------------------------------------------------------------------
//	class BasicPartialView
//
//	Definition of a trivial PartialView that provides an unaltered view
//	of the subject Partial's parameter envelope.
//
class BasicPartialView : public PartialView
{
//	-- instance variables --
	const Partial * _partial;	//	the Partial being viewed
	
//	-- public interface --
public:
	BasicPartialView( void );
	BasicPartialView( const BasicPartialView & rhs );
	
	//	derived classes must provide a virtual constructor:
	virtual BasicPartialView * clone( void ) const;
	
	//	view applies the Iterator to a new Partial: 
	virtual void view( const Partial & p );
	
	//	iterator creation:
	virtual PartialViewIterator begin( void ) const;
	virtual PartialViewIterator end( void ) const;
	
	//	iterator interface:
	virtual void advance( PartialConstIterator & iter, int n = 1 ) const;
	virtual bool atEnd( const PartialConstIterator & iter ) const;
		
	//	Breakpoint access:
	virtual double frequency( const PartialConstIterator & iter ) const;
	virtual double amplitude( const PartialConstIterator & iter ) const;
	virtual double bandwidth( const PartialConstIterator & iter ) const;
	virtual double phase( const PartialConstIterator & iter ) const;
	virtual double time( const PartialConstIterator & iter ) const;
	
};	//	end of class BasicPartialView

// ---------------------------------------------------------------------------
//	class PartialViewDecorator
//
//	decorator class for PartialViews, owns its sub-view.
//
class PartialViewDecorator : public PartialView
{
//	-- instance variables --
	std::auto_ptr< PartialView > _subview;
	
//	-- public interface --
public:	
	//	assign a new view to be decorated:
	void decorate( const PartialView & v );
	 
	//	All other public members delegate, by default, to the subview.
	//
	//	view applies the Iterator to a new Partial: 
	virtual void view( const Partial & p ) { subview().view( p ); }
	
	//	iterator creation:
	virtual PartialViewIterator begin( void ) const
		{ return subview().begin(); }
	virtual PartialViewIterator end( void ) const
		{ return subview().end(); }
	
	//	iterator interface:
	virtual void advance( PartialConstIterator & iter, int n = 1 ) const
		{ subview().advance( iter, n ); }
	virtual bool atEnd( const PartialConstIterator & iter ) const 
		{ return subview().atEnd( iter ); }
	
	//	Breakpoint access:
	virtual double frequency( const PartialConstIterator & iter ) const
		 { return subview().frequency( iter ); }
	virtual double amplitude( const PartialConstIterator & iter ) const
		 { return subview().amplitude( iter ); }
	virtual double bandwidth( const PartialConstIterator & iter ) const
		 { return subview().bandwidth( iter ); }
	virtual double phase( const PartialConstIterator & iter ) const
		 { return subview().phase( iter ); }
	virtual double time( const PartialConstIterator & iter ) const
		 { return subview().time( iter ); }
	
//	-- subclass interface  --
protected:
	//	decorated view access (helper):
	const PartialView & subview( void ) const { return *_subview; }
	PartialView & subview( void ) { return *_subview; }
	
	//	protected construction and assignment:
	PartialViewDecorator( void );
	PartialViewDecorator( const PartialViewDecorator & rhs );
	PartialViewDecorator & operator = ( const PartialViewDecorator & rhs );

};	//	end of abstract base class PartialViewDecorator


#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_PARTIALVIEW_H

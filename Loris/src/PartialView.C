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
 * PartialView.C
 *
 * Implementation of PartialView (abstract interface class), PartialViewIterator,
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
#include "PartialView.h"
#include "Partial.h"

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	PartialView startTime
// ---------------------------------------------------------------------------
//	This can be implemented in terms of the abstract Breakpoint access
//	members.
//
double
PartialView::startTime( void ) const
{
	return begin().time();
}

// ---------------------------------------------------------------------------
//	PartialView endTime
// ---------------------------------------------------------------------------
//	This can be implemented in terms of the abstract Breakpoint access
//	members.
//
double
PartialView::endTime( void ) const
{
	PartialViewIterator last = end();
	last.advance(-1);
	return last.time();
}

// ---------------------------------------------------------------------------
//	PartialView duration
// ---------------------------------------------------------------------------
//	This can be implemented in terms of the abstract Breakpoint access
//	members.
//
double
PartialView::duration( void ) const
{
	return endTime() - startTime(); 
}

// ---------------------------------------------------------------------------
//	PartialView initialPhase
// ---------------------------------------------------------------------------
//	This can be implemented in terms of the abstract Breakpoint access
//	members.
//
double
PartialView::initialPhase( void ) const
{
	return begin().phase(); 
}

// ---------------------------------------------------------------------------
//	PartialViewIterator default constructor
// ---------------------------------------------------------------------------
//	Called only by PartialView to construct an initialized iterator.
//
PartialViewIterator::PartialViewIterator( void ) : 
	_view( NULL ) 
{
}

// ---------------------------------------------------------------------------
//	PartialViewIterator constructor
// ---------------------------------------------------------------------------
//	Called only by PartialView to construct an initialized iterator.
//
PartialViewIterator::PartialViewIterator( const PartialView & v, 
										  const PartialConstIterator & piter ) : 
	_view( &v ),
	_pos( piter ) 
{
}

// ---------------------------------------------------------------------------
//	PartialViewIterator copy constructor
// ---------------------------------------------------------------------------
//
PartialViewIterator::PartialViewIterator( const PartialViewIterator & rhs ) : 
	_view( rhs._view ),
	_pos( rhs._pos ) 
{
}

// ---------------------------------------------------------------------------
//	PartialViewIterator assignment
// ---------------------------------------------------------------------------
//
PartialViewIterator & 
PartialViewIterator::operator = ( const PartialViewIterator & rhs )
{
	if ( &rhs != this )
	{
		_view = rhs._view;
		_pos = rhs._pos;
	}
	return *this;
}

// ---------------------------------------------------------------------------
//	BasicPartialView default constructor
// ---------------------------------------------------------------------------
//
BasicPartialView::BasicPartialView( void ) : 
	_partial( NULL )
{
}

// ---------------------------------------------------------------------------
//	BasicPartialView copy constructor
// ---------------------------------------------------------------------------
//
BasicPartialView::BasicPartialView( const BasicPartialView & rhs ) : 
	_partial( rhs._partial ),
	PartialView( rhs )
{
}

// ---------------------------------------------------------------------------
//	BasicPartialView virtual (clone) constructor
// ---------------------------------------------------------------------------
//
BasicPartialView *	
BasicPartialView::clone( void ) const 
{ 
	return new BasicPartialView( *this ); 
}

// ---------------------------------------------------------------------------
//	(BasicPartialView) view
// ---------------------------------------------------------------------------
//	Assigns the View to a new Partial, invalidates all oustanding iterators
//	on the View.
//
void
BasicPartialView::view( const Partial & p )
{
	_partial = & p;
}

// ---------------------------------------------------------------------------
//	BasicPartialView begin
// ---------------------------------------------------------------------------
//
PartialViewIterator
BasicPartialView:: begin( void ) const
{
	return PartialViewIterator( *this, _partial->begin() );
}

// ---------------------------------------------------------------------------
//	BasicPartialView end
// ---------------------------------------------------------------------------
//
PartialViewIterator
BasicPartialView::end( void ) const
{
	return PartialViewIterator( *this, _partial->end() );
}

// ---------------------------------------------------------------------------
//	(BasicPartialView) advance
// ---------------------------------------------------------------------------
//	Advance the iterator.
//
void
BasicPartialView::advance( PartialConstIterator & iter, int n ) const
{
	if ( n > 0 )
	{
		PartialConstIterator end = _partial->end();
		while ( n > 0 && iter != end )
		{
			++iter;
			--n;
		}
	}
	else if (n < 0)
	{
		PartialConstIterator beg = _partial->begin();
		while ( n < 0 && iter != beg )
		{
			--iter;
			++n;
		}
	}
}

// ---------------------------------------------------------------------------
//	(BasicPartialView) atEnd
// ---------------------------------------------------------------------------
//
bool
BasicPartialView::atEnd( const PartialConstIterator & iter ) const
{
	return iter == _partial->end();
}

// ---------------------------------------------------------------------------
//	(BasicPartialView) frequency
// ---------------------------------------------------------------------------
//
double
BasicPartialView::frequency( const PartialConstIterator & iter ) const
{
	return iter->frequency();
}

// ---------------------------------------------------------------------------
//	(BasicPartialView) amplitude
// ---------------------------------------------------------------------------
//
double
BasicPartialView::amplitude( const PartialConstIterator & iter ) const
{
	return iter->amplitude();
}

// ---------------------------------------------------------------------------
//	(BasicPartialView) bandwidth
// ---------------------------------------------------------------------------
//
double
BasicPartialView::bandwidth( const PartialConstIterator & iter ) const
{
	return iter->bandwidth();
}

// ---------------------------------------------------------------------------
//	(BasicPartialView) phase
// ---------------------------------------------------------------------------
//
double
BasicPartialView::phase( const PartialConstIterator & iter ) const
{
	return iter->phase();
}

// ---------------------------------------------------------------------------
//	(BasicPartialView) time
// ---------------------------------------------------------------------------
//
double
BasicPartialView::time( const PartialConstIterator & iter ) const
{
	return iter.time();
}

// ---------------------------------------------------------------------------
//	PartialViewDecorator default constructor
// ---------------------------------------------------------------------------
//
PartialViewDecorator::PartialViewDecorator( void ) : 
	_subview( new BasicPartialView() ) 
{
}

// ---------------------------------------------------------------------------
//	PartialViewDecorator copy constructor
// ---------------------------------------------------------------------------
//	This could be called by the copy constructor of a derived class, needs
//	to make a deep copy of the subview. Compiler-generated copy constructor
//	cannot be used because the autoptr<> _subview cannot be copied trivially.
//
PartialViewDecorator::PartialViewDecorator( const PartialViewDecorator & rhs ) : 
	_subview( rhs._subview->clone() ),
	PartialView( rhs ) 
{
}

// ---------------------------------------------------------------------------
//	PartialViewDecorator assignment
// ---------------------------------------------------------------------------
//	This could be called by the assignment operator of a derived class, needs
//	to make a deep copy of the subview.
//
PartialViewDecorator & 
PartialViewDecorator::operator = ( const PartialViewDecorator & rhs )
{
	if ( &rhs != this ) 
	{
		_subview.reset( rhs._subview->clone() );
	}
	return *this;
}

// ---------------------------------------------------------------------------
//	PartialViewDecorator decorate
// ---------------------------------------------------------------------------
//	Decorate a different PartialView.
//
void 
PartialViewDecorator::decorate( const PartialView & v )
{
	_subview.reset( v.clone() );
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

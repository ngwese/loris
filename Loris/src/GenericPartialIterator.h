#ifndef INCLUDE_PARTIALITERATOR_H
#define INCLUDE_PARTIALITERATOR_H
/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
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
 * GenericPartialIterator.h
 *
 * Definition of template generic Partial iterator and const_iterator classes.
 * Nothing in this file is directly useful to clients, it is used by classes
 * defined in Partial.h.
 *
 * Kelly Fitz, 22 May 2003
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
//	begin namespace
namespace Loris {

template <class BreakpointContainerPolicy> class GenericPartialConstIterator;

// ---------------------------------------------------------------------------
//	template iterator accessor functions
//
//	These can be specialized differently for other kinds of BaseContatiners.
//
template< class BaseIterator >
double 
iterator_access_time( const BaseIterator & it ) { return it->first; }

template< class BaseIterator >
Breakpoint &
iterator_access_breakpoint( BaseIterator & it ) { return it->second; }

template< class BaseIterator >
const Breakpoint & 
iterator_access_const_breakpoint( const BaseIterator & it ) { return it->second; }

// ---------------------------------------------------------------------------
//	class GenericPartialIterator
//
//	Non-const iterator for the Loris::Partial Breakpoint map. Wraps
//	the non-const iterator for std::map< double, Breakpoint >. Implements
//	a bidirectional iterator interface, and additionally offers time
//	and Breakpoint (reference) access through time() and breakpoint()
//	memebrs.
//
template <class BreakpointContainerPolicy>
class GenericPartialIterator
{
//	-- instance variables --
	typedef typename BreakpointContainerPolicy::container_type BaseContainer;
	typedef typename BaseContainer::iterator BaseIterator;
	BaseIterator _iter;
	
//	-- public interface --
public:
//	-- bidirectional iterator interface --
	typedef typename BaseIterator::iterator_category	iterator_category;
	typedef Breakpoint     								value_type;
	typedef typename BaseIterator::difference_type  	difference_type;
	typedef Breakpoint *								pointer;
	typedef Breakpoint &								reference;

//	construction:
//	(allow compiler to generate copy, assignment, and destruction):
	GenericPartialIterator( void ) {}
	
//	pre-increment/decrement:
	GenericPartialIterator& operator ++ () { ++_iter; return *this; }
	GenericPartialIterator& operator -- () { --_iter; return *this; }

//	post-increment/decrement:
	GenericPartialIterator operator ++ ( int ) { return GenericPartialIterator( _iter++ ); } 
	GenericPartialIterator operator -- ( int ) { return GenericPartialIterator( _iter-- ); } 
	
//	dereference (for treating Partial like a 
//	STL collection of Breakpoints):
	const Breakpoint & operator * ( void ) const { return breakpoint(); }
	Breakpoint & operator * ( void ) { return breakpoint(); }
	const Breakpoint * operator -> ( void ) const  { return & breakpoint(); }
	Breakpoint * operator -> ( void )  { return & breakpoint(); }
		
//	comparison:
	friend bool operator == ( const GenericPartialIterator & lhs, 
							  const GenericPartialIterator & rhs )
		{ return lhs._iter == rhs._iter; }
	friend bool operator != ( const GenericPartialIterator & lhs, 
							  const GenericPartialIterator & rhs )
		{ return lhs._iter != rhs._iter; }
	
//	-- time and Breakpoint access --
	const Breakpoint & breakpoint( void ) const 
		{ return iterator_access_const_breakpoint(_iter); }
	Breakpoint & breakpoint( void ) 
		{ return iterator_access_breakpoint(_iter); }
	double time( void ) const  
		{ return iterator_access_time(_iter); }

//	-- BaseIterator conversions --
private:
	//	construction by GenericBreakpointContainer from a BaseIterator:
	explicit GenericPartialIterator( const BaseIterator & it ) :
		_iter(it) {}

	friend class GenericBreakpointContainer;
	
	//	befriend  GenericPartialConstIterator, 
	//	for const construction from non-const:
	friend class GenericPartialConstIterator< BreakpointContainerPolicy >;	
	
};	//	end of class GenericPartialIterator

// ---------------------------------------------------------------------------
//	class GenericPartialConstIterator
//
//	Const iterator for the Loris::Partial Breakpoint map. Wraps
//	the const iterator for std::map< double, Breakpoint >. Implements
//	a bidirectional iterator interface, and additionally offers time
//	and Breakpoint (reference) access through time() and breakpoint()
//	memebrs.
//
template <class BreakpointContainerPolicy>
class GenericPartialConstIterator
{
//	-- instance variables --
	typedef typename BreakpointContainerPolicy::container_type BaseContainer;
	typedef typename BaseContainer::const_iterator BaseIterator;
	BaseIterator _iter;
	
//	-- public interface --
public:
//	-- bidirectional iterator interface --
	typedef typename BaseIterator::iterator_category	iterator_category;
	typedef Breakpoint     								value_type;
	typedef typename BaseIterator::difference_type  	difference_type;
	typedef const Breakpoint *							pointer;
	typedef const Breakpoint &							reference;

//	construction:
//	(allow compiler to generate copy, assignment, and destruction):
	GenericPartialConstIterator( void ) {}
	GenericPartialConstIterator( const GenericPartialIterator< BreakpointContainerPolicy > & other ) :
		_iter( other._iter ) {}
	
//	pre-increment/decrement:
	GenericPartialConstIterator& operator ++ () { ++_iter; return *this; }
	GenericPartialConstIterator& operator -- () { --_iter; return *this; }

//	post-increment/decrement:
	GenericPartialConstIterator operator ++ ( int ) { return GenericPartialConstIterator( _iter++ ); } 
	GenericPartialConstIterator operator -- ( int ) { return GenericPartialConstIterator( _iter-- ); } 
	
//	dereference (for treating Partial like a 
//	STL collection of Breakpoints):
	const Breakpoint & operator * ( void ) const { return breakpoint(); }
	const Breakpoint * operator -> ( void ) const { return & breakpoint(); }
	
//	comparison:
	friend bool operator == ( const GenericPartialConstIterator & lhs, 
							  const GenericPartialConstIterator & rhs )
		{ return lhs._iter == rhs._iter; }
	friend bool operator != ( const GenericPartialConstIterator & lhs, 
							  const GenericPartialConstIterator & rhs )
		{ return lhs._iter != rhs._iter; }
	
//	-- time and Breakpoint access --
	const Breakpoint & breakpoint( void ) const 
		{ return iterator_access_const_breakpoint(_iter); }
	double time( void ) const  
		{ return iterator_access_time(_iter); }
	
//	-- BaseIterator conversions --
private:
	//	construction by GenericBreakpointContainer from a BaseIterator:
	explicit GenericPartialConstIterator( BaseIterator it ) :
		_iter(it) {}
	
	friend class GenericBreakpointContainer;

};	//	end of class GenericPartialConstIterator

// ---------------------------------------------------------------------------
//	class GenericBreakpointContainer
//
//	Generic class that can be a friend of the generic iterator classes, so
//	that base iterator access doesn't have to be public.
//
class GenericBreakpointContainer
{
protected:
	template< class Iterator >
	static typename Iterator::BaseIterator
	baseIterator( Iterator it ) { return it._iter; }
	
	template< class Iterator >
	static Iterator
	makeIterator( typename Iterator::BaseIterator it ) 
		{ return Iterator( it ); }

	// protect destruction so nobody can try to delete one of these,
	// then I don't need a virtual destructor.
	~GenericBreakpointContainer( void ) {}
};

}	//	end of namespace Loris

#endif /* ndef INCLUDE_PARTIALITERATOR_H */


#ifndef INCLUDE_REFCNTBP_H
#define INCLUDE_REFCNTBP_H
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
 * RefCountedBreakpointContainer.h
 *
 * Component of class Loris::Partial. The policy for storing Breakpoints has
 * been abstracted out of the Partial class itself. Partial inherits from a 
 * BreakpointContainer class, which specifies the storage implementation, and
 * the operations that are specific to that implementation. All other operations
 * are implemented in terms of a few BreakpointContainer pollicy operations. 
 * Ideally, Partial would be a template class (as shown in Alexandrescu, Modern
 * C++ Design, but that would necessitate implementing Partial entirely in the 
 * header, which is unacceptable. Ideally, the necessary operations for a 
 * Breakpoint container policy would be pure virtual members of an abstract 
 * interface class, but that would necessitate virtual functions, which is 
 * unacceptable overhead. The current implementation is all statically bound
 * so there is no performance penalty for factoring out the Breakpoint container
 * policy. There is no need for even a vitual destructor, because destruction is
 * protected in the base policy classes (a trick I learned from Alexandrescu).
 *
 * The no frills policy is the same as the old implementation, based on
 * std::map. the reference counted policy uses a reference counted pointer class
 * to manage a pointer to the std::map, and implements copy-on-write semantics. 
 * I thought this would speed things up by eliminating some copying of big 
 * Breakpoint maps, but in fact, it slowed things down! Must be that indirection
 * isn't free! So the no frills policy is the one we use. The reference 
 * counted one has been moved out to RefCountedBreakpointContainer.h.
 *
 * Kelly Fitz, 12 June 2003
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <Breakpoint.h>
#include <GenericPartialIterator.h>

#include <cstddef>
#include <map>
#include <stdexcept>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
// Template Ptr class from chapter 14 of Accelerated C++, by Koenig and 
// Moo, with very minor modifications. This smart pointer class supports
// copy-on-write, and was selected for that reason, hence the name.
//
template <class T> 
class COWPtr 
{
public:
	// define reference count type:
#ifdef _MSC_VER
	typedef size_t refcount_type;
#else
	typedef std::size_t refcount_type;
#endif

	// new member to copy the object conditionally when needed
	void make_unique() {
		if (*refptr != 1) {
			--*refptr;
			refptr = new refcount_type(1);
			p = p? clone(p): 0;
		}
	}

	// the rest of the class looks like `Ref_handle' except for its name
	COWPtr(): refptr(new refcount_type(1)), p(0) { }
	COWPtr(T* t): refptr(new refcount_type(1)), p(t) { }
	COWPtr(const COWPtr& h): refptr(h.refptr), p(h.p) { ++*refptr; }

	COWPtr& operator=(const COWPtr&);    // implemented analogously to 14.2/261
	~COWPtr();                        // implemented analogously to 14.2/262
	operator bool() const { return p; }
	T& operator*() const;          // implemented analogously to 14.2/261
	T* operator->() const;         // implemented analogously to 14.2/261

private:
	T* p;
	refcount_type* refptr;
};

template <class T> T* clone(const T* tp)
{
	return tp->clone();
}



template<class T>
T& COWPtr<T>::operator*() const { if (p) return *p; throw std::runtime_error("unbound Ptr"); }

template<class T>
T* COWPtr<T>::operator->() const { if (p) return p; throw std::runtime_error("unbound Ptr"); }


template<class T>
COWPtr<T>& 
COWPtr<T>::operator=(const COWPtr& rhs)
{
        ++*rhs.refptr;
        // free the lhs, destroying pointers if appropriate
        if (--*refptr == 0) 
		{
                delete refptr;
                delete p;
        }

        // copy in values from the right-hand side\
        refptr = rhs.refptr;
        p = rhs.p;
        return *this;
}

template<class T> 
COWPtr<T>::~COWPtr()
{
        if (--*refptr == 0) 
		{
                delete refptr;
                delete p;
        }
}

// ---------------------------------------------------------------------------
//	Breakpoint container policy
//
//	responsibilities:
//		default construction
//		copy (construction)
//		operator= (assign)
//		operator== (equivalence)
//		size
//		insert( pos, Breakpoint )
//		erase( b, e )
//		findAfter( time )
//		begin (const and non-const)
//		end (const and non-const)
//		iterator and const_iterator types, and size_type
//		
//	NOTE: any mutating operation, AND anything that
//	returns a non-const iterator has to make_unique!
//
//	This is invoked by make_unique to clone the map.
inline std::map< double, Breakpoint > * 
clone( const std::map< double, Breakpoint > * tp )
{
	return new std::map< double, Breakpoint >(*tp);
}

class RefCountedBreakpointContainer : public GenericBreakpointContainer
{
//	-- public interface --
public:
//	-- types --
	typedef std::map< double, Breakpoint > container_type;
	typedef GenericPartialIterator< RefCountedBreakpointContainer > iterator;
	typedef GenericPartialConstIterator< RefCountedBreakpointContainer > const_iterator;
	typedef container_type::size_type size_type;

//	-- construction --
	RefCountedBreakpointContainer( void ) : _ptr( new container_type() ) {}
	
//	-- compare --
	bool operator==( const RefCountedBreakpointContainer & rhs ) const
		{ return *_ptr == *(rhs._ptr); }

//	-- access opertions --
	const_iterator begin( void ) const { return makeIterator<const_iterator>( _ptr->begin() ); }
	iterator begin( void ) { _ptr.make_unique(); return makeIterator<iterator>( _ptr->begin() ); }
	const_iterator end( void ) const { return makeIterator<const_iterator>( _ptr->end() ); }
	iterator end( void ) { _ptr.make_unique(); return makeIterator<iterator>( _ptr->end() ); }
	size_type size( void ) const { 	return _ptr->size(); }
	
	const_iterator findAfter( double time ) const
	{
		return makeIterator<const_iterator>( _ptr->lower_bound( time ) );
	}
	iterator findAfter( double time ) 
	{
		_ptr.make_unique();
		return makeIterator<iterator>( _ptr->lower_bound( time ) );
	}

//	-- mutating operations --
	iterator insert( double time, const Breakpoint & bp )
	{
		// make this reference unique!
		_ptr.make_unique();
		
		std::pair< container_type::iterator, bool > result = 
			_ptr->insert( container_type::value_type(time, bp) );
		if ( ! result.second )
			result.first->second = bp;
		return makeIterator<iterator>( result.first );
	}
 
	iterator erase( iterator beg, iterator end )
	{
		// make this reference unique!
		_ptr.make_unique();
		
		//	This relies on the unsavory conversion from const_iterator 
		//	to container_type::const_iterator.
		_ptr->erase( baseIterator(beg), baseIterator(end) );
		return end;
	}

protected:
	//	protect destruction
	~RefCountedBreakpointContainer( void ) {}

	//	icky constructor for Partial to use:
	//	This relies on the unsavory conversion from const_iterator 
	//	to container_type::const_iterator.
	RefCountedBreakpointContainer( const_iterator beg, const_iterator end ) 
		: _ptr( new container_type( baseIterator(beg), baseIterator(end) ) ) {}

//	-- implementation core --
private:
	COWPtr< container_type > _ptr;	//	reference counted Breakpoint envelope
	
};

}	//	end of namespace Loris

#endif /* ndef INCLUDE_REFCNTBP_H */

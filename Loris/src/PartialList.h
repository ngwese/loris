#ifndef INCLUDE_PARTIALLIST_H
#define INCLUDE_PARTIALLIST_H
// ===========================================================================
//	PartialList.h
//	
//	Definition of a class representing an ordered colleciton of Partials.
//	Really, we use the STL std::list<Partial>, but templates make it difficult
//	to insulate clients, for instance, its a mess to declare a template
//	class type in a header file without defining it. You can't use:
//
//		class std::list<Partial>;
//
//	without declaring all four template arguments and their default types, etc.
//	With this wrapper class, you can put:
//
//		class PartialList;
//
//	in the header file for a class that uses lists of Partials in its 
//	interface, but does not rely on them in-size.
//
//	This class is not insulating, its okay for clients of PartalList to
//	include <list> and Partial.h and to see the implementation details.
//	Implemented entirely inline for efficiency.
//
//	-kel 21 July 2000
//
// ===========================================================================
#include "Partial.h"
#include <list>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

#define PartialListIsClass
#ifdef PartialListIsClass

// ---------------------------------------------------------------------------
//	class PartialList
//
//	Simplified wrapper for std::list<Partial>, public interface is 
//	similar to std::list<>, but not complete. Low-cost (splicing)
//	conversion to and from std::list<> are provided so that the 
//	complete std::list<> interface is available without copying.
//
class PartialList
{
//	-- instance variables --
	std::list< Partial > _l;
	
//	-- public interface --
public:
	//	type definitions:
	typedef std::list<Partial>::iterator iterator;
	typedef std::list<Partial>::const_iterator const_iterator;

	//	compiler can generate default constructor, destructor,
	//	copy and assignment.
	PartialList(void) {}

	//	construction from std::list<>:
	explicit PartialList( const std::list< Partial > & l ) : _l(l) {}
	
	//	construction from iterator range:
	PartialList( iterator b, iterator e ) : _l(b,e) {}
	PartialList( const_iterator b, const_iterator e ) : _l(b,e) {}

	//	iterator access:
	iterator begin(void) { return _l.begin(); }
	const_iterator begin(void) const { return _l.begin(); }

	iterator end(void) { return _l.end(); }
	const_iterator end(void) const { return _l.end(); }

	//	list<> interface, delegated:
	bool empty(void) const { return _l.empty(); }
	std::list<Partial>::size_type size(void) const { return _l.size(); }
	void clear(void) { _l.clear() ; }

	Partial & front(void) { return _l.front(); }
	const Partial & front(void) const { return _l.front(); }
	Partial & back(void) { return _l.back(); }
	const Partial & back(void) const { return _l.back(); }

	void push_front( const Partial & p ) { _l.push_front(p); }
	void push_back( const Partial & p ) { _l.push_back(p); }

	iterator insert( iterator pos, const Partial & p ) { return _l.insert(pos, p); }	
	iterator erase( iterator pos ) { return _l.erase(pos); }

	void splice( iterator pos, PartialList & other ) { _l.splice(pos, other._l); }
	
		
};	//	end of class PartialList

#else // not defined PartialListIsClass

typedef std::list<Partial> PartialList

#endif // def PartialListIsClass

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_PARTIALLIST_H

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
//	This class is not fully insulating, its okay for clients of PartalList to
//	include <list> and see the implementation details.
//
//	-kel 21 July 2000
//
// ===========================================================================

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class PartialList
//
//	Wrapper for std::list<Partial>, public interface should mostly 
//	look like std::list<>.
//
class PartialList
{
//	-- instance variables --
	std::list< Partial > _l;
	
//	-- public interface --
public:
	
};	//	end of class PartialList

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_PARTIALLIST_H
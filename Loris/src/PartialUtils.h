#ifndef INCLUDE_PARTIALUTILS_H
#define INCLUDE_PARTIALUTILS_H
// ===========================================================================
//	PartialUtils.h
//	
//	A group of Partial utility function objects for use with STL 
//	searching and sorting algorithms. PartialUtils is a namespace
//	within the Loris namespace, unless compiled with NO_LORIS_NAMESPACE,
//	in which case it is a class.
//
//	Lakos suggests protecting utility functions like this in a class 
//	definition, but I think a namespace is better, since that is really
//	what is being constructed.
//
//	-kel 6 July 2000
//
// ===========================================================================
#include "Partial.h"
#include <functional>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class/namespace PartialUtils
//
#if !defined( NO_LORIS_NAMESPACE )
//	PartialUtils is a namespace:
namespace PartialUtils {
#else
//	PartialUtils is a class:
class PartialUtils
{
//	unimplemented, cannot instantiate PartialUtils:
	PartialUtils(void);
	PartialUtils(const PartialUtils &);
	
//	-- public interface --
public:
#endif

//	predicates:
	struct label_equals : 
		public std::binary_function< const Partial, int, bool >
	{
		bool operator()( const Partial & p, int label ) const 
			{ return p.label() == label; }
	};
	
//	comparitors:
	struct label_less : 
		public std::binary_function< const Partial, const Partial, bool >
	{
		bool operator()( const Partial & lhs, const Partial & rhs ) const 
			{ return lhs.label() < rhs.label(); }
	};
	

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace PartialUtils
#else
};	//	end of class PartialUtils
#endif

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	//	ndef INCLUDE_PARTIALUTILS_H
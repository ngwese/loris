#ifndef INCLUDE_BREAKPOINTUTILS_H
#define INCLUDE_BREAKPOINTUTILS_H
// ===========================================================================
//	BreakpointUtils.h
//	
//	A group of Breakpoint utility function objects for use with STL 
//	searching and sorting algorithms. BreakpointUtils is a namespace
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
//	class/namespace BreakpointUtils
//
#if !defined( NO_LORIS_NAMESPACE )
//	BreakpointUtils is a namespace:
namespace BreakpointUtils {
#else
//	BreakpointUtils is a class:
class BreakpointUtils
{
//	unimplemented, cannot instantiate BreakpointUtils:
	BreakpointUtils(void);
	BreakpointUtils(const BreakpointUtils &);
	
//	-- public interface --
public:
#endif

//	predicates:
	struct frequency_between : 
		public std::unary_function< const Breakpoint, bool >
	{
		bool operator()( const Breakpoint & b )  const
			{ 
				return (b.frequency() > _fmin) && 
					   (b.frequency() < _fmax); 
			}
			
		//	constructor:
		frequency_between( double x, double y ) : 
			_fmin( x ), _fmax( y ) 
			{ if (x>y) std::swap(x,y); }
			
		//	bounds:
		private:
			double _fmin, _fmax;
	};

//	comparitors:
	struct less_frequency : 
		public std::binary_function< const Breakpoint, const Breakpoint, bool >
	{
		bool operator()( const Breakpoint & lhs, const Breakpoint & rhs ) const
			{ return lhs.frequency() < rhs.frequency(); }
	};

	struct greater_amplitude : 
		public std::binary_function< const Breakpoint, const Breakpoint, bool >
	{
		bool operator()( const Breakpoint & lhs, const Breakpoint & rhs ) const
			{ return lhs.amplitude() > rhs.amplitude(); }
	};	


#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace BreakpointUtils
#else
};	//	end of class BreakpointUtils
#endif

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	//	ndef INCLUDE_BREAKPOINTUTILS_H
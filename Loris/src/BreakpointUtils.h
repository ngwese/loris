#ifndef INCLUDE_BREAKPOINTUTILS_H
#define INCLUDE_BREAKPOINTUTILS_H
/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2004 by Kelly Fitz and Lippold Haken
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
 * BreakpointUtils.h
 *
 * Breakpoint utility functions collected in namespace BreakpointUtils.
 * Some out-of-line functions are defined in BreakpointUtils.C.
 *
 * Kelly Fitz, 6 July 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <Breakpoint.h>
#include <algorithm>
#include <functional>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	class/namespace BreakpointUtils
//	
//	BreakpointUtils represents a collection of utility functions and 
//	function objects for applying STL searching and sorting algorithms 
//	to collections of Loris Breakpoints. BreakpointUtils is a namespace
//	within the Loris namespace, unless compiled with NO_NESTED_NAMESPACE,
//	in which case it is a class (having only static members and private
//	constructors).
//
//	Lakos suggests protecting utility functions like this in a class 
//	definition, but I think a namespace is better, since that is really
//	what is being constructed.
//
//
#if !defined( NO_NESTED_NAMESPACE )
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

//	-- free functions --
#if defined( NO_NESTED_NAMESPACE )
	static
#endif
	inline void addNoiseEnergy( Breakpoint & bp, double enoise ) { bp.addNoiseEnergy(enoise); }
	/*	Add noise (bandwidth) energy to a Breakpoint by computing new 
		amplitude and bandwidth values. enoise may be negative, but 
		noise energy cannot be removed (negative energy added) in excess 
		of the current noise energy.
		
		This operation is now part of the Breakpoint interface.
	 */
	 
	#if defined( NO_NESTED_NAMESPACE )
	static
	#endif
	Breakpoint makeNullBefore( const Breakpoint & bp, double fadeTime ); // see BreakpointUtils.C
	/*	Return a null (zero-amplitude) Breakpoint to preceed the specified 
		Breakpoint, useful for fading in a Partial.
	 */
	
	#if defined( NO_NESTED_NAMESPACE )
	static
	#endif
	Breakpoint makeNullAfter( const Breakpoint & bp, double fadeTime ); // see BreakpointUtils.C
	/*	Return a null (zero-amplitude) Breakpoint to succeed the specified 
		Breakpoint, useful for fading out a Partial.
	 */


//	-- predicates --
	/*	Predicate functor returning true if its Breakpoint argument 
		has frequency between specified bounds, and false otherwise.
	 */
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

//	-- comparitors --
	/*	Comparitor (binary) functor returning true if its first Breakpoint
		argument has frequency less than that of its second Breakpoint argument,
		and false otherwise.
	 */
	struct less_frequency : 
		public std::binary_function< const Breakpoint, const Breakpoint, bool >
	{
		bool operator()( const Breakpoint & lhs, const Breakpoint & rhs ) const
			{ return lhs.frequency() < rhs.frequency(); }
	};

	/*	Comparitor (binary) functor returning true if its first Breakpoint
		argument has amplitude greater than that of its second Breakpoint argument,
		and false otherwise.
	 */
	struct greater_amplitude : 
		public std::binary_function< const Breakpoint, const Breakpoint, bool >
	{
		bool operator()( const Breakpoint & lhs, const Breakpoint & rhs ) const
			{ return lhs.amplitude() > rhs.amplitude(); }
	};	


#if !defined( NO_NESTED_NAMESPACE )
}	//	end of namespace BreakpointUtils
#else
};	//	end of class BreakpointUtils
#endif

}	//	end of namespace Loris

#endif /* ndef INCLUDE_BREAKPOINTUTILS_H */

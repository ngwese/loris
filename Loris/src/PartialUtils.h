#ifndef INCLUDE_PARTIALUTILS_H
#define INCLUDE_PARTIALUTILS_H
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
 * PartialUtils.h
 *
 * Partial utility functions collected in namespace PartialUtils.
 *
 * Kelly Fitz, 6 July 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <Partial.h>
#include <functional>
#include <utility>

//	begin namespace
namespace Loris {

using std::pair;

// ---------------------------------------------------------------------------
//	class/namespace PartialUtils
//	
//	A group of Partial utility function objects for use with STL 
//	searching and sorting algorithms. PartialUtils is a namespace
//	within the Loris namespace, unless compiled with NO_NESTED_NAMESPACE,
//	in which case it is a class.
//
//	Lakos suggests protecting utility functions like this in a class 
//	definition, but I think a namespace is better, since that is really
//	what is being constructed.
//
//
#if !defined( NO_NESTED_NAMESPACE )
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
	
	struct duration_greater : 
		public std::binary_function< const Partial, const Partial, bool >
	{
		bool operator()( const Partial & lhs, const Partial & rhs ) const 
			{ return lhs.duration() > rhs.duration(); }
	};
	
//	functions on ranges of Partials:
	template <typename Iterator>
#if defined( NO_NESTED_NAMESPACE )
	static
#endif
	inline 
	pair<double,double>
	timeSpan( Iterator begin, Iterator end ) 
	{
		double tmin = 0., tmax = 0.;
		if ( begin != end )
		{
			Iterator it = begin;
			tmin = it->startTime();
			tmax = it->endTime();
			while( it != end )
			{
				tmin = std::min( tmin, it->startTime() );
				tmax = std::max( tmax, it->endTime() );
				++it;
			}
		}
		return std::make_pair(tmin, tmax);
	}
	

#if !defined( NO_NESTED_NAMESPACE )
}	//	end of namespace PartialUtils
#else
};	//	end of class PartialUtils
#endif

}	//	end of namespace Loris

#endif /* ndef INCLUDE_PARTIALUTILS_H */

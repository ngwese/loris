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

#include <algorithm>
#include <functional>
#include <utility>

//	begin namespace
namespace Loris {

class Envelope;

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
public:
#endif
//	-- Partial mutating functors --

	/* 	Trim a Partial by removing Breakpoints outside a specified time span.
		Insert a Breakpoint at the boundary when cropping occurs.
	 */
	struct crop : public std::unary_function< Partial, void >
	{
		double minTime, maxTime;
		crop( double x, double y ) : minTime( x ), maxTime( y ) 
		{
			if ( minTime > maxTime )
				std::swap( minTime, maxTime );
		}
		
		void operator()( Partial & p ) const;	// see PartialUtils.C
	};

	/*	Scale the amplitude of the specified Partial according to
		an envelope representing a time-varying amplitude scale value.
	 */
	struct scale_amp : public std::unary_function< Partial, void >
	{
		Envelope & env;
		scale_amp( Envelope & e ) : env(e) {}
		
		void operator()( Partial & p ) const;	// see PartialUtils.C
	};

	/*	Scale the bandwidth of the specified Partial according to
		an envelope representing a time-varying bandwidth scale value.
	 */
	struct scale_bandwidth : public std::unary_function< Partial, void >
	{
		Envelope & env;
		scale_bandwidth( Envelope & e ) : env(e) {}
		
		void operator()( Partial & p ) const;	// see PartialUtils.C
	};

	/*	Scale the frequency of the specified Partial according to
		an envelope representing a time-varying frequency scale value.
	 */
	struct scale_frequency : public std::unary_function< Partial, void >
	{
		Envelope & env;
		scale_frequency( Envelope & e ) : env(e) {}
		
		void operator()( Partial & p ) const;	// see PartialUtils.C
	};

	/*	Scale the relative noise content of the specified Partial according 
		to an envelope representing a (time-varying) noise energy 
		scale value.
	 */
	struct scale_noise_ratio : public std::unary_function< Partial, void >
	{
		Envelope & env;
		scale_noise_ratio( Envelope & e ) : env(e) {}
		
		void operator()( Partial & p ) const;	// see PartialUtils.C
	};

	/*	Shift the pitch of the specified Partial according to
		the given pitch envelope. The pitch envelope is assumed to have 
		units of cents (1/100 of a halfstep).
	 */
	struct shift_pitch : public std::unary_function< Partial, void >
	{
		Envelope & env;
		shift_pitch( Envelope & e ) : env(e) {}
		
		void operator()( Partial & p ) const;	// see PartialUtils.C
	};	
	
	/*	Shift the time of all the Breakpoints in a Partial by a 
		constant amount.
	 */
	struct shift_time : public std::unary_function< Partial, void >
	{
		double offset;
		shift_time( double x ) : offset(x) {}
		
		void operator()( Partial & p ) const;	// see PartialUtils.C
	};

	/*	Return the time (in seconds) spanned by a specified half-open
		(STL-style) range of Partials as a std::pair composed of the earliest
		Partial start time and latest Partial end time in the range.
	 */
	template <typename Iterator>
	#if defined( NO_NESTED_NAMESPACE )
	static
	#endif
	inline 
	std::pair<double,double> timeSpan( Iterator begin, Iterator end ) 
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
	
//	-- predicates --
	/*	Predicate functor returning true if the label of its Partial argument is
		equal to the specified 32-bit label, and false otherwise.
	 */
	struct label_equals : 
		public std::unary_function< const Partial, bool >
	{
		int label;
		label_equals( int l ) : label(l) {}
		
		bool operator()( const Partial & p ) const 
			{ return p.label() == label; }
	};
	
	/*	Predicate functor returning true if the label of its Partial argument is
		greater than the specified 32-bit label, and false otherwise.
	 */
	struct label_greater : 
		public std::unary_function< const Partial, bool >
	{
		int label;
		label_greater( int l ) : label(l) {}
		
		bool operator()( const Partial & p ) const 
			{ return p.label() > label; }
	};
		
//	-- comparitors --
	/*	Comparitor (binary) functor returning true if its first Partial
		argument has a label whose 32-bit integer representation is less than
		that of the second Partial argument's label, and false otherwise.
	 */
	template< class Comparitor = std::less< Partial::label_type > >
	struct compare_label : 
		public std::binary_function< const Partial, const Partial, bool >
	{
		Comparitor comp;
		bool operator()( const Partial & lhs, const Partial & rhs ) const 
			{ return comp( lhs.label(), rhs.label() ); }
	};
	
/*
	Not sure yet whether I want to add these.
		
	template< class Comparitor >
	compare_label< Comparitor > 
	label_comparitor( Comparitor c = std::less< Partial::label_type >() )
		{ return compare_label< Comparitor >(); }
		
	compare_label< > 
	label_comparitor( void )
		{ return compare_label< >(); }
*/
	/*	Comparitor (binary) functor returning true if its first Partial
		argument has duration greater than that of the second Partial
		argument, and false otherwise.
	 */
	template< class Comparitor = std::less< double > >
	struct compare_duration : 
		public std::binary_function< const Partial, const Partial, bool >
	{
		Comparitor comp;
		bool operator()( const Partial & lhs, const Partial & rhs ) const 
			{ return comp( lhs.duration(), rhs.duration() ); }
	};
/*
	Not sure yet whether I want to add these.
		
	template< class Comparitor >
	compare_duration< Comparitor > 
	duration_comparitor( Comparitor c = std::less< double >() )
		{ return compare_duration< Comparitor >(); }
		
	compare_duration< > 
	duration_comparitor( void )
		{ return compare_duration< >(); }
	
*/	
#if !defined( NO_NESTED_NAMESPACE )
}	//	end of namespace PartialUtils
#else
//	unimplemented, cannot instantiate PartialUtils:
private:
	PartialUtils(void);
	PartialUtils(const PartialUtils &);
	
};	//	end of class PartialUtils
#endif

}	//	end of namespace Loris

#endif /* ndef INCLUDE_PARTIALUTILS_H */

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
//	-- free functions --

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

	/*	Apply crop to all partials in a half-open range.
	 */
	template <typename Iterator>
	#if defined( NO_NESTED_NAMESPACE )
	static
	#endif
	inline 
	void cropAll( Iterator begin, Iterator end, double t1, double t2 )
	{
		std::for_each( begin, end, crop( t1, t2 ) );	
	}

	/*	Scale the amplitude of the specified Partial according to
		an envelope representing a time-varying amplitude scale value.
	 */
	struct scale_amp : public std::unary_function< Partial, void >
	{
		Envelope & env;
		scale_amp( Envelope & e ) : env(e) {}
		
		void operator()( Partial & p ) const;	// see PartialUtils.C
	};

	/*	Apply scale_amp to all partials in a half-open range.
	 */
	template <typename Iterator>
	#if defined( NO_NESTED_NAMESPACE )
	static
	#endif
	inline 
	void scaleAmp( Iterator begin, Iterator end, Envelope & e )
	{
		std::for_each( begin, end, scale_amp( e ) );	
	}

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

	/*	Apply scale_noise_ratio to all partials in a half-open range.
	 */
	template <typename Iterator>
	#if defined( NO_NESTED_NAMESPACE )
	static
	#endif
	inline 
	void scaleNoiseRatio( Iterator begin, Iterator end, Envelope & e )
	{
		std::for_each( begin, end, scale_noise_ratio( e ) );	
	}

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

	/*	Apply shift_pitch to all partials in a half-open range.
	 */
	template <typename Iterator>
	#if defined( NO_NESTED_NAMESPACE )
	static
	#endif
	inline 
	void shiftPitch( Iterator begin, Iterator end, Envelope & e )
	{
		std::for_each( begin, end, shift_pitch( e ) );	
	}
	
	
	/*	Shift the time of all the Breakpoints in a Partial by a 
		constant amount.
	 */
	struct shift_time : public std::unary_function< Partial, void >
	{
		double offset;
		shift_time( double x ) : offset(x) {}
		
		void operator()( Partial & p ) const;	// see PartialUtils.C
	};

	/*	Apply shift_time to all partials in a half-open range.
	 */
	template <typename Iterator>
	#if defined( NO_NESTED_NAMESPACE )
	static
	#endif
	inline 
	void shiftTime( Iterator begin, Iterator end, double offset )
	{
		std::for_each( begin, end, shift_time( offset ) );	
	}
	
	
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
	
	/*	Template adapter for using PartialUtil predicates with 
		collections of pointers to Partials.
	 */
	template<typename Predicate>
	struct deref_predicate :
		public std::unary_function< const Partial *, bool >
	{
		Predicate pred;
		deref_predicate( Predicate p = Predicate() ) : pred(p) {}
		
		bool operator()( const Partial * p ) const 
			{ return pred(*p); }
	};
	
//	-- comparitors --
	/*	Comparitor (binary) functor returning true if its first Partial
		argument has a label whose 32-bit integer representation is less than
		that of the second Partial argument's label, and false otherwise.
	 */
	struct label_less : 
		public std::binary_function< const Partial, const Partial, bool >
	{
		bool operator()( const Partial & lhs, const Partial & rhs ) const 
			{ return lhs.label() < rhs.label(); }
	};
	
	/*	Comparitor (binary) functor returning true if its first Partial
		argument has duration greater than that of the second Partial
		argument, and false otherwise.
	 */
	struct duration_greater : 
		public std::binary_function< const Partial, const Partial, bool >
	{
		bool operator()( const Partial & lhs, const Partial & rhs ) const 
			{ return lhs.duration() > rhs.duration(); }
	};
	
	/*	Template adapter for using PartialUtil comparitors with 
		collections of pointers to Partials.
	 */
	template<typename Comparitor>
	struct deref_comparitor :
		public std::binary_function< const Partial *, const Partial *, bool >
	{
		Comparitor comp;
		deref_comparitor( Comparitor c = Comparitor() ) : comp(c) {}
		
		bool operator()( const Partial * lhs, const Partial * rhs ) const 
			{ return comp(*lhs, *rhs); }
	};

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

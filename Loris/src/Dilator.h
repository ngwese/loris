#ifndef INCLUDE_DILATOR_H
#define INCLUDE_DILATOR_H
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
 * Dilator.h
 *
 * Definition of class Dilator.
 *
 * Kelly Fitz, 26 Oct 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if defined(NO_TEMPLATE_MEMBERS)
#include "PartialList.h"
#endif

#include <vector>

//	begin namespace
namespace Loris {

class Partial;

// ---------------------------------------------------------------------------
//	class Dilator
//
//	Class Dilator represents an algorithm for non-uniformly expanding
//	and contracting the Partial parameter envelopes according to the initial
//	and target (desired) times of temporal features.
//	
//	It is frequently necessary to redistribute temporal events in this way
//	in preparation for a sound morph. For example, when morphing instrument
//	tones, it is common to align the attack, sustain, and release portions
//	of the source sounds by dilating or contracting those temporal regions.
//
class Dilator
{
//	-- instance variables --

	std::vector< double > _initial, _target;	//	time points
	
//	-- public interface --
public:
//	-- construction --

	//	Construct a new Dilator with no time points.
	Dilator( void );
	 	
	//	Construct a new Dilator using a range of initial time points
	//	and a range of target (desired) time points. The client must
	//	ensure that the target range has at least as many elements as
	//	the initial range.
#if ! defined(NO_TEMPLATE_MEMBERS)
	template<typename Iter1, typename Iter2>
	Dilator( Iter1 ibegin, Iter1 iend, Iter2 tbegin );
#else
	Dilator( const double * ibegin, const double * iend, const double * tbegin );
#endif

	//	Destroy this Dilator.
	~Dilator( void );
	
	//	Insert a pair of initial and target time points. 
	//	
	//	Specify a pair of initial and target time points to be used
	//	by this Dilator, corresponding, for example, to the initial
	//	and desired time of a particular temporal feature in an
	//	analyzed sound.
	//	
	//	i is an initial, or source, time point
	//	t is a target time point
	//	
	//	The time points will be sorted before they are used.
	//	If, in the sequences of initial and target time points, there are
	//	exactly the same number of initial time points preceding i as
	//	target time points preceding t, then time i will be warped to 
	//	time t in the dilation process.	
	void insert( double i, double t );
	
//	-- dilation --

	//	Replace the Partial envelope with a new envelope having the
	//	same Breakpoints at times computed to align temporal features
	//	in the sorted sequence of initial time points with their 
	//	counterparts the sorted sequence of target time points.
	//
	//	Depending on the specification of initial and target time 
	//	points, the dilated Partial may have Breakpoints at times
	//	less than 0, even if the original Partial did not.
	//
	//	It is possible to have duplicate time points in either sequence.
	//	Duplicate initial time points result in very localized stretching.
	//	Duplicate target time points result in very localized compression.
	//
	//	If all initial time points are greater than 0, then an implicit
	//	time point at 0 is assumed in both initial and target sequences, 
	//	so the onset of a sound can be stretched without explcitly specifying a 
	//	zero point in each vector. (This seems most intuitive, and only looks
	//	like an inconsistency if clients are using negative time points in 
	//	their Dilator, or Partials having Breakpoints before time 0, both 
	//	of which are probably unusual circumstances.)
	//
	//	p is the Partial to dilate.
	void dilate( Partial & p ) const;
	 
	//	Function call operator: same as dilate( Partial & p ).
	void operator() ( Partial & p ) const;
	 
	//	Non-uniformly expand and contract the parameter envelopes of the each
	//	Partial in the specified half-open range according to this Dilator's
	//	stored initial and target (desired) times.
#if ! defined(NO_TEMPLATE_MEMBERS)
	template<typename Iter>
	void dilate( Iter dilate_begin, Iter dilate_end  ) const;
#else
	void dilate( PartialList::iterator dilate_begin, 
				 PartialList::iterator dilate_end  ) const;
#endif
	 
	//	Function call operator: same as dilate( Iter dilate_begin, Iter dilate_end )
#if ! defined(NO_TEMPLATE_MEMBERS)
	template<typename Iter>
	void operator() ( Iter dilate_begin, Iter dilate_end  ) const;
#else
	void operator() ( PartialList::iterator dilate_begin, 
					  PartialList::iterator dilate_end ) const;
#endif
	 
	//	Return the dilated time value corresponding to the specified 
	//	initial time.
    double warpTime( double currentTime );

//	-- unimplemented until useful --
private:
	Dilator( const Dilator & );
	Dilator & operator= ( const Dilator & rhs );
	
};	//	end of class Dilator


// ---------------------------------------------------------------------------
//	constructor (sequences of time points)
// ---------------------------------------------------------------------------
//!	Construct a new Dilator using a range of initial time points
//!	and a range of target (desired) time points. The client must
//!	ensure that the target range has at least as many elements as
//!	the initial range.
//!	
//!	@param ibegin is the beginning of a sequence of initial, or source,
//!	time points.
//!	@param iend is (one-past) the end of a sequence of initial, or
//!	source, time points.
//!	@param tbegin is the beginning of a sequence of target time points; 
//!	this sequence must be as long as the sequence of initial time points
//!	described by @p ibegin and @p iend.
//!
//!	If compiled with NO_TEMPLATE_MEMBERS defined, this member accepts
//!	only <tt>const double *</tt> arguments.
//
#if ! defined(NO_TEMPLATE_MEMBERS)
template<typename Iter1, typename Iter2>
Dilator::Dilator( Iter1 ibegin, Iter1 iend, Iter2 tbegin )
#else
Dilator::Dilator( const double * ibegin, const double * iend, const double * tbegin )
#endif
{
	while ( ibegin != iend )
	{
		insert( *ibegin++, *tbegin++ );
	}
}

// ---------------------------------------------------------------------------
//	dilate (sequence of Partials)
// ---------------------------------------------------------------------------
//!	Non-uniformly expand and contract the parameter envelopes of the each
//!	Partial in the specified half-open range according to this Dilator's
//!	stored initial and target (desired) times. 
//!
//!	@param dilate_begin is the beginning of a sequence of Partials to dilate.
//!	@param dilate_end is (one-past) the end of a sequence of Partials to dilate.
//!
//!	If compiled with NO_TEMPLATE_MEMBERS defined, this member accepts
//!	only @c PartialList::const_iterator arguments.
//!	
//!	@sa Dilator::dilate( Partial & p ) const
//
#if ! defined(NO_TEMPLATE_MEMBERS)
template<typename Iter>
void Dilator::dilate( Iter dilate_begin, Iter dilate_end  ) const
#else
void Dilator::dilate( PartialList::iterator dilate_begin, 
					  PartialList::iterator dilate_end  ) const
#endif
{
	while ( dilate_begin != dilate_end )
	{
		dilate( *(dilate_begin++) );
	}
}

// ---------------------------------------------------------------------------
//	Function call operator (sequence of Partials)
// ---------------------------------------------------------------------------
//!	Function call operator: same as 
//!	<tt>dilate( Iter dilate_begin, Iter dilate_end )</tt>
//!
//!	If compiled with NO_TEMPLATE_MEMBERS defined, this member accepts
//!	only @c PartialList::const_iterator arguments.
//!	
//!	@sa Dilator::dilate( Partial & p ) const
//	 
#if ! defined(NO_TEMPLATE_MEMBERS)
template<typename Iter>
void Dilator::operator() ( Iter dilate_begin, Iter dilate_end  ) const
#else
void Dilator::operator() ( PartialList::iterator dilate_begin, 
						   PartialList::iterator dilate_end ) const
#endif
{ 
	dilate( dilate_begin, dilate_end ); 
}

// ---------------------------------------------------------------------------
//	Function call operator (single Partial)
// ---------------------------------------------------------------------------
//!	Function call operator: same as <tt>dilate( Partial & p )</tt>.
//!	
//!	@sa Dilator::dilate( Partial & p ) const
//
inline 
void Dilator::operator() ( Partial & p ) const
{ 
	dilate( p ); 
}

}	//	end of namespace Loris

#endif /* ndef INCLUDE_DILATOR_H */

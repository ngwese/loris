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
	Dilator( void );
	/*	Construct a new Dilator with no time points.
	 */
	 
	//Dilator( const double * ibegin, const double * iend, const double * tbegin );
	/*	Construct a new Dilator using a range of initial time points
		and a range of target (desired) time points. The client must
		ensure that the target range has at least as many elements as
		the initial range.
	 */
	
#if ! defined(NO_TEMPLATE_MEMBERS)
	template<typename Iter1, typename Iter2>
	Dilator( Iter1 ibegin, Iter1 iend, Iter2 tbegin )
#else
	Dilator( const double * ibegin, const double * iend, const double * tbegin )
#endif
	{
		while ( ibegin != iend )
			insert( *ibegin++, *tbegin++ );
	}
	/*	Construct a new Dilator using a range of initial time points
		and a range of target (desired) time points. The client must
		ensure that the target range has at least as many elements as
		the initial range.
	 */

	~Dilator( void );
	/*	Destroy this Dilator.
	 */
	
	void insert( double i, double t );
	/*	Specify a pair of initial and target time points to be used
		by this Dilator, corresponding, for example, to the initial
		and desired time of a particular temporal feature in an
		analyzed sound.
	 */
	
//	-- dilation --
	void dilate( Partial & p );
	/*	Non-uniformly expand and contract the parameter envelopes of the
		specified Partial according to this Dilator's stored initial and 
		target (desired) times.
	 */
	 
	void operator() ( Partial & p ) { dilate( p ); }
	/*	Function call operator: same as dilate( Partial & p ).
	 */
	 
#if ! defined(NO_TEMPLATE_MEMBERS)
	template<typename Iter>
	void dilate( Iter dilate_begin, Iter dilate_end  )
#else
	void dilate( PartialList::iterator dilate_begin, PartialList::iterator dilate_end  )
#endif
	{
		while ( dilate_begin != dilate_end )
			dilate( *(dilate_begin++) );
	}
	/*	Non-uniformly expand and contract the parameter envelopes of the each
		Partial in the specified half-open range according to this Dilator's
		stored initial and target (desired) times.
	 */
	 
#if ! defined(NO_TEMPLATE_MEMBERS)
	template<typename Iter>
	void operator() ( Iter dilate_begin, Iter dilate_end  )
#else
	void operator() ( PartialList::iterator dilate_begin, PartialList::iterator dilate_end )
#endif
		{ dilate( dilate_begin, dilate_end ); }
	/*	Function call operator: same as dilate( Iter dilate_begin, Iter dilate_end )
	 */
	 
    double warpTime( double currentTime );
    /*	Return the dilated time value corresponding to the specified 
        initial time.
     */

//	-- unimplemented until useful --
private:
	Dilator( const Dilator & );
	Dilator & operator= ( const Dilator & rhs );
	
};	//	end of class Dilator

}	//	end of namespace Loris

#endif /* ndef INCLUDE_DILATOR_H */

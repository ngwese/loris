#ifndef INCLUDE_SIEVE_H
#define INCLUDE_SIEVE_H
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
 * Sieve.h
 *
 * Definition of class Sieve.
 *
 * Lippold Haken, 20 Jan 2001
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#if defined(NO_TEMPLATE_MEMBERS)
#include "PartialList.h"
#endif

#include "PartialPtrs.h"
#include <memory>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	class Sieve
//
//	Class Sieve represents an algorithm for identifying channelized (see
//	Channelizer) Partials that overlap in time, and selecting the longer
//	one to represent the channel.
//	
//	In some cases, the energy redistribution effected by the distiller
//	(see Distiller) is undesirable. In such cases, the partials can be
//	sifted before distillation. The sifting process in Loris identifies
//	all the partials that would be rejected (and converted to noise
//	energy) by the distiller and assigns them a label of 0. These sifted
//	partials can then be identified and treated sepearately or removed
//	altogether, or they can be passed through the distiller unlabeled, and
//	crossfaded in the morphing process (see Morpher).
//
class Sieve
{
//	-- instance variables --
	double _fadeTime;
	
//	-- public interface --
public:
//	-- construction --
	explicit Sieve( double partialFadeTime = 0.001 /* 1 ms */ );
	/*	Construct a new Sieve using the specified partial fade
		time. If unspecified, the fade time defaults to one 
		millisecond (0.001 s).
	 */
	 
	~Sieve( void );
	/*	Destroy this Sieve.
	 */
	
//	-- sifting --
#if ! defined(NO_TEMPLATE_MEMBERS)
	template<typename Iter>
	void sift( Iter sift_begin, Iter sift_end  )
#else
	void sift( PartialList::iterator sift_begin, PartialList::iterator sift_end  )
#endif
	{
		PartialPtrs ptrs;
		fillPartialPtrs( sift_begin, sift_end, ptrs );
		sift_ptrs( ptrs );
	}
	/*	Sift labeled Partials on the specified half-open (STL-style)
		range. If any two Partials having same label overlap in time, keep
		only the longer of the two Partials. Set the label of the shorter
		duration partial to zero. No Partials are removed from the
		PartialList and the list order is unaltered. 
	 */
		 
private:
//	-- implementation --
	void sift_ptrs( PartialPtrs & ptrs );

//	-- unimplemented --
	Sieve( const Sieve & other );
	Sieve & operator= ( const Sieve & other );
	
};	//	end of class Sieve

}	//	end of namespace Loris

#endif /* ndef INCLUDE_SIEVE_H */

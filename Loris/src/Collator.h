#ifndef INCLUDE_COLLATOR_H
#define INCLUDE_COLLATOR_H
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
 * Collator.h
 *
 * Definition of class Collator.
 *
 * Kelly Fitz, 29 April 2005
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "Partial.h"
#include "PartialList.h"
#include "PartialUtils.h"

#include <algorithm>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	class Collator
//
//!	Class Collator represents an algorithm for reducing a collection
//!	of Partials into the smallest collection of "equivalent" Partials
//!   by joining non-overlapping Partials end to end.
//!	
//!	Partials that are not labeled, that is, Partials having label 0,
//!	are are "collated " into groups of non-overlapping (in time)
//!   Partials, assigned an unused label (greater than the label associated
//!   with any labeled Partial), and fused into a single Partial per
//!   group. "Collating" is a bit like "sifting" but non-overlapping
//!   Partials are grouped without regard to frequency proximity. This
//!   algorithm produces the smallest-possible number of collated Partials.
//!   Thanks to Ulrike Axen for providing this optimal algorithm.
//!	
//!	Collating modifies the Partial container (a PartialList). Only
//!	unlabeled (labeled 0) Partials are affected by the collating
//!	operation. Collated Partials are moved to the end of the 
//!   collection of Partials.
//
class Collator
{
//	-- instance variables --

	double _fadeTime, _gapTime;       
		
//	-- public interface --
public:
//	-- construction --

	//!	Construct a new Collator using the specified fade time
	//!	for gaps between Partials. When two non-overlapping Partials
	//!	are collated into a single Partial, the collated Partial
	//!	fades out at the end of the earlier Partial and back in again
	//!	at the onset of the later one. The fade time is the time over
	//!	which these fades occur. By default, use a 1 ms fade time.
	//!	The gap time is the additional time over which a Partial faded
	//!	out must remain at zero amplitude before it can fade back in.
	//!	By default, use a gap time of one tenth of a millisecond, to 
	//!	prevent a pair of arbitrarily close null Breakpoints being
	//!	inserted.
	//!
	//!   \param   partialFadeTime is the time (in seconds) over
	//!            which Partials joined by collating fade to
	//!            and from zero amplitude. Default is 0.001 (one
	//!            millisecond).
	//!   \param   partialSilentTime is the minimum duration (in seconds) 
	//!            of the silent (zero-amplitude) gap between two 
	//!            Partials joined by collating. (Default is
	//!            0.0001 (one tenth of a millisecond).
	explicit
	Collator( double partialFadeTime = 0.001    /* 1 ms */,
             double partialSilentTime = 0.0001 /* .1 ms */ );
	 
	//	Use compiler-generated copy, assign, and destroy.
	
//	-- collating --

	//!	Collate labeled Partials in a collection leaving only a single 
	//!	Partial per non-zero label. 
	//!
	//!	Unlabeled (zero-labeled) Partials are collated into the smallest-possible 
	//!	number of Partials that does not combine any overlapping Partials.
	//!	Collated Partials assigned labels higher than any label in the original 
	//!	list, and appear at the end of the collated Partials.
	//!
	//!	Return an iterator refering to the position of the first collated Partial,
	//!	of the end of the collated collection if there are no collated Partials.
	//!   Since collating is in-place, the Partials collection may be smaller
	//!   (fewer Partials) after collating, and any iterators on the collection
	//!   may be invalidated.
	//!
	//!   \post   All Partials in the collection are uniquely-labeled
	//!   \param  partials is the collection of Partials to distill in-place
	//!   \return the position of the end of the range of collated Partials,
	//!           which is either the end of the collection, oor the position
	//!           of the first collated Partial, composed of unlabeled Partials
	//!           in the original collection.
	//!
	//!	If compiled with NO_TEMPLATE_MEMBERS defined, then partials
	//!	must be a PartialList, otherwise it can be any container type
	//!	storing Partials that supports at least bidirectional iterators.
   //!
   //!  \sa Collator::collate( Container & partials )
#if ! defined(NO_TEMPLATE_MEMBERS)
	template< typename Container >
	typename Container::iterator collate( Container & partials );
#else
    inline
	PartialList::iterator collate( PartialList & partials );
#endif

	//!	Function call operator: same as collate( PartialList & partials ).
#if ! defined(NO_TEMPLATE_MEMBERS)
	template< typename Container >
	typename Container::iterator operator() ( Container & partials );
#else
	PartialList::iterator operator() ( PartialList & partials );
#endif
	
   //! Static member that constructs an instance and applies
   //! it to a sequence of Partials. 
   //! Construct a Collator using default parameters, 
   //! and use it to collate a sequence of Partials. 
   //!
   //! \post   All Partials in the collection are uniquely-labeled
   //! \param  partials is the collection of Partials to collate in-place
   //! \param   partialFadeTime is the time (in seconds) over
   //!          which Partials joined by collating fade to
   //!          and from zero amplitude.
   //! \param   partialSilentTime is the minimum duration (in seconds) 
   //!          of the silent (zero-amplitude) gap between two 
   //!          Partials joined by collating. (Default is
   //!          0.0001 (one tenth of a millisecond).
   //! \return the position of the end of the range of collated Partials,
   //!         which is either the end of the collection, oor the position
   //!         of the first collated Partial, composed of unlabeled Partials
   //!         in the original collection.
   //!
   //!	If compiled with NO_TEMPLATE_MEMBERS defined, then partials
   //!	must be a PartialList, otherwise it can be any container type
   //!	storing Partials that supports at least bidirectional iterators.
   #if ! defined(NO_TEMPLATE_MEMBERS)
	template< typename Container >
	static typename Container::iterator 
	collate( Container & partials, double partialFadeTime,
                                  double partialSilentTime = 0.0001 /* .1 ms */ );
#else
	static inline PartialList::iterator
	collate( PartialList & partials, double partialFadeTime,
                                    double partialSilentTime = 0.0001 /* .1 ms */ );
#endif


private:

//	-- helpers --

    //! Collate unlabeled (zero labeled) Partials into the smallest
    //! possible number of Partials that does not combine any temporally
    //! overlapping Partials. Give each collated Partial a label, starting
    //! with startlabel, and incrementing. The unlabeled Partials are
    //! collated in-place.
    void collateAux( PartialList & unlabled, Partial::label_type startLabel );
	
};	//	end of class Collator

// ---------------------------------------------------------------------------
//	collate
// ---------------------------------------------------------------------------
//!	Collate labeled Partials in a collection leaving only a single 
//!	Partial per non-zero label. 
//!
//!	Unlabeled (zero-labeled) Partials are collated into the smallest-possible 
//!	number of Partials that does not combine any overlapping Partials.
//!	Collated Partials assigned labels higher than any label in the original 
//!	list, and appear at the end of the collated Partials.
//!
//!	Return an iterator refering to the position of the first collated Partial,
//!	of the end of the collated collection if there are no collated Partials.
//!   Since collating is in-place, the Partials collection may be smaller
//!   (fewer Partials) after collating, and any iterators on the collection
//!   may be invalidated.
//!
//!   \post   All Partials in the collection are uniquely-labeled
//!   \param  partials is the collection of Partials to collate in-place
//!   \return the position of the end of the range of collated Partials,
//!           which is either the end of the collection, oor the position
//!           of the first collated Partial, composed of unlabeled Partials
//!           in the original collection.
//!
//!	If compiled with NO_TEMPLATE_MEMBERS defined, then partials
//!	must be a PartialList, otherwise it can be any container type
//!	storing Partials that supports at least bidirectional iterators.
//!
//
#if ! defined(NO_TEMPLATE_MEMBERS)
template< typename Container >
typename Container::iterator Collator::collate( Container & partials )
#else
inline
PartialList::iterator Collator::collate( PartialList & partials )
#endif
{
#if ! defined(NO_TEMPLATE_MEMBERS)
    typedef typename Container::iterator Iterator;
#else
    typedef PartialList::iterator Iterator;
#endif

    // Container::iterator dist_begin = partials.begin();
    // Container::iterator dist_end = partials.end();
    //
    //  Using these iterators (as arguments) only makes sense
    //  if collate is changed to return the end of the collated
    //  Partials, rather than the end of the collated Partials.
    //  In the current implementation, when there are collated
    //  Partials, there is no way to find the end of the range 
    //  of collated Partials after the call to collate, for 
    //  all containers. Making the collate function generic to
    //  all containers will work, but making it work on ranges
    //  requires an interface change.
    
    // The way to do this is to partition the Partials
    // into labeled and unlabeled, and collate the unlabeled
    // ones and replace the unlabeled range.
    // (This requires bidirectional iterator support.)
    Iterator begUnlabeled = 
       std::stable_partition( partials.begin(), partials.end(), 
                              std::not1( PartialUtils::isLabelEqual(0) ) );
    
    //  remember how many labeled Partials there are
    typename Iterator::difference_type numLabeled = 
        std::distance( partials.begin(), begUnlabeled );
    
    // cannot splice if this operation is to be generic
    // with respect to container, have to copy:
    PartialList collated( begUnlabeled, partials.end() );
    // collated.splice( collated.end(), begUnlabeled, partials.end() );
    
    Partial::label_type maxlabel = 
      std::max_element( partials.begin(), begUnlabeled, 
                        PartialUtils::compareLabelLess() )->label();

    //	collate unlabeled (zero-labeled) Partials:
    collateAux( collated, std::max( maxlabel+1, 1 ) );
    
    //  copy the collated Partials back into the source container
    //  after the range of labeled Partials		
    Iterator endcollated = 
        std::copy( collated.begin(), collated.end(), begUnlabeled );

    //  remove extra Partials from the end of the source container
    //  (endcollated is the end of the collated Partials):
    partials.erase( endcollated, partials.end() );

    //  what could be returned? Beginning of collated Partials
    //  would be nice, but it would take linear search to find it.
    //  Still, without that, how can the client possibly find 
    //  the first collated?
    begUnlabeled = partials.begin();
    std::advance( begUnlabeled, numLabeled );
    return begUnlabeled;
}

// ---------------------------------------------------------------------------
//	Function call operator 
// ---------------------------------------------------------------------------
//!	Function call operator: same as collate( PartialList & partials ).
//!	
//! \sa Collator::collate( Container & partials )
//
#if ! defined(NO_TEMPLATE_MEMBERS)
template< typename Container >
typename Container::iterator Collator::operator()( Container & partials )
#else
inline
PartialList::iterator Collator::operator()( PartialList & partials )
#endif
{ 
	return collate( partials );
}

// ---------------------------------------------------------------------------
//	collate
// ---------------------------------------------------------------------------
//! Static member that constructs an instance and applies
//! it to a sequence of Partials. 
//! Construct a Collator using default parameters, 
//! and use it to collate a sequence of Partials. 
//!
//! \post   All Partials in the collection are uniquely-labeled
//! \param  partials is the collection of Partials to collate in-place
//! \param   partialFadeTime is the time (in seconds) over
//!          which Partials joined by collating fade to
//!          and from zero amplitude.
//! \param   partialSilentTime is the minimum duration (in seconds) 
//!          of the silent (zero-amplitude) gap between two 
//!          Partials joined by collateation. (Default is
//!          0.0001 (one tenth of a millisecond).
//! \return the position of the end of the range of collated Partials,
//!         which is either the end of the collection, oor the position
//!         of the first collated Partial, composed of unlabeled Partials
//!         in the original collection.
//!
//!	If compiled with NO_TEMPLATE_MEMBERS defined, then partials
//!	must be a PartialList, otherwise it can be any container type
//!	storing Partials that supports at least bidirectional iterators.
//
#if ! defined(NO_TEMPLATE_MEMBERS)
template< typename Container >
typename Container::iterator 
Collator::collate( Container & partials, double partialFadeTime,
                                         double partialSilentTime )
#else
inline
PartialList::iterator 
Collator::collate( PartialList & partials, double partialFadeTime,
                                           double partialSilentTime )
#endif
{
    Collator instance( partialFadeTime, partialSilentTime );
    return instance.collate( partials );
}

}	//	end of namespace Loris

#endif /* ndef INCLUDE_COLLATOR_H */
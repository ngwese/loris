#ifndef INCLUDE_DISTILLER_H
#define INCLUDE_DISTILLER_H
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
 * Distiller.h
 *
 * Definition of class Distiller.
 *
 * Kelly Fitz, 20 Oct 1999
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
//	class Distiller
//
//!	Class Distiller represents an algorithm for "distilling" a group of
//!	Partials that logically represent a single component into a single
//!	Partial.
//!	
//!	The sound morphing algorithm in Loris requires that Partials in a
//!	given source be labeled uniquely, that is, no two Partials can have
//!	the same label. The Distiller enforces this condition. All Partials
//!	identified with a particular frequency channel (see Channelizer), and,
//!	therefore, having a common label, are distilled into a single Partial,
//!	leaving at most a single Partial per frequency channel and label.
//!	Channels that contain no Partials are not represented in the distilled
//!	data. Partials that are not labeled, that is, Partials having label 0,
//!	are are "collated " into groups of non-overlapping (in time)
//!   Partials, assigned an unused label (greater than the label associated
//!   with any frequency channel), and fused into a single Partial per
//!   group. "Collating" is a bit like "sifting" but non-overlapping
//!   Partials are grouped without regard to frequency proximity. This
//!   algorithm produces the smallest-possible number of collated Partials.
//!   Thanks to Ulrike Axen for providing this optimal algorithm.
//!	
//!	Distillation modifies the Partial container (a PartialList). All
//!	Partials in the distilled range having a common label are replaced by
//!	a single Partial in the distillation process.
//
class Distiller
{
//	-- instance variables --

	double _fadeTime, _gapTime;         // distillation parameters
		
//	-- public interface --
public:
//	-- construction --

	//!	Construct a new Distiller using the specified fade time
	//!	for gaps between Partials. When two non-overlapping Partials
	//!	are distilled into a single Partial, the distilled Partial
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
	//!            which Partials joined by distillation fade to
	//!            and from zero amplitude. Default is 0.001 (one
	//!            millisecond).
	//!   \param   partialSilentTime is the minimum duration (in seconds) 
	//!            of the silent (zero-amplitude) gap between two 
	//!            Partials joined by distillation. (Default is
	//!            0.0001 (one tenth of a millisecond).
	explicit
	Distiller( double partialFadeTime = 0.001    /* 1 ms */,
              double partialSilentTime = 0.0001 /* .1 ms */ );
	 
	//	Use compiler-generated copy, assign, and destroy.
	
//	-- distillation --

	//!	Distill labeled Partials in a collection leaving only a single 
	//!	Partial per non-zero label. 
	//!
	//!	Unlabeled (zero-labeled) Partials are collated into the smallest-possible 
	//!	number of Partials that does not combine any overlapping Partials.
	//!	Collated Partials assigned labels higher than any label in the original 
	//!	list, and appear at the end of the distilled Partials.
	//!
	//!	Return an iterator refering to the position of the first collated Partial,
	//!	of the end of the distilled collection if there are no collated Partials.
	//!   Since distillation is in-place, the Partials collection may be smaller
	//!   (fewer Partials) after distillation, and any iterators on the collection
	//!   may be invalidated.
	//!
	//!   \post   All Partials in the collection are uniquely-labeled
	//!   \param  partials is the collection of Partials to distill in-place
	//!   \return the position of the end of the range of distilled Partials,
	//!           which is either the end of the collection, oor the position
	//!           of the first collated Partial, composed of unlabeled Partials
	//!           in the original collection.
	//!
	//!	If compiled with NO_TEMPLATE_MEMBERS defined, then partials
	//!	must be a PartialList, otherwise it can be any container type
	//!	storing Partials that supports at least bidirectional iterators.
   //!
   //!  \sa Distiller::distill( Container & partials )
#if ! defined(NO_TEMPLATE_MEMBERS)
	template< typename Container >
	typename Container::iterator distill( Container & partials );
#else
    inline
	PartialList::iterator distill( PartialList & partials );
#endif

	//!	Function call operator: same as distill( PartialList & partials ).
#if ! defined(NO_TEMPLATE_MEMBERS)
	template< typename Container >
	typename Container::iterator operator() ( Container & partials );
#else
	PartialList::iterator operator() ( PartialList & partials );
#endif
	
   //! Static member that constructs an instance and applies
   //! it to a sequence of Partials. 
   //! Construct a Distiller using default parameters, 
   //! and use it to distill a sequence of Partials. 
   //!
   //! \post   All Partials in the collection are uniquely-labeled
   //! \param  partials is the collection of Partials to distill in-place
   //! \param   partialFadeTime is the time (in seconds) over
   //!          which Partials joined by distillation fade to
   //!          and from zero amplitude.
   //! \param   partialSilentTime is the minimum duration (in seconds) 
   //!          of the silent (zero-amplitude) gap between two 
   //!          Partials joined by distillation. (Default is
   //!          0.0001 (one tenth of a millisecond).
   //! \return the position of the end of the range of distilled Partials,
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
	distill( Container & partials, double partialFadeTime,
                                  double partialSilentTime = 0.0001 /* .1 ms */ );
#else
	static inline PartialList::iterator
	distill( PartialList & partials, double partialFadeTime,
                                    double partialSilentTime = 0.0001 /* .1 ms */ );
#endif


private:

//	-- helpers --

    //! Distill a list of Partials having a common label
    //! into a single Partial with that label, and append it
    //! to the distilled collection. If an empty list of Partials
    //! is passed, then an empty Partial having the specified
    //! label is appended.
    void distillOne( PartialList & partials, Partial::label_type label,
                     PartialList & distilled );

    //! Collate unlabeled (zero labeled) Partials into the smallest
    //! possible number of Partials that does not combine any temporally
    //! overlapping Partials. Give each collated Partial a label, starting
    //! with startlabel, and incrementing. The unlabeled Partials are
    //! collated in-place.
    void collateUnlabeled( PartialList & unlabled, Partial::label_type startLabel );
	
};	//	end of class Distiller

// ---------------------------------------------------------------------------
//	distill
// ---------------------------------------------------------------------------
//!	Distill labeled Partials in a collection leaving only a single 
//!	Partial per non-zero label. 
//!
//!	Unlabeled (zero-labeled) Partials are collated into the smallest-possible 
//!	number of Partials that does not combine any overlapping Partials.
//!	Collated Partials assigned labels higher than any label in the original 
//!	list, and appear at the end of the distilled Partials.
//!
//!	Return an iterator refering to the position of the first collated Partial,
//!	of the end of the distilled collection if there are no collated Partials.
//!   Since distillation is in-place, the Partials collection may be smaller
//!   (fewer Partials) after distillation, and any iterators on the collection
//!   may be invalidated.
//!
//!   \post   All Partials in the collection are uniquely-labeled
//!   \param  partials is the collection of Partials to distill in-place
//!   \return the position of the end of the range of distilled Partials,
//!           which is either the end of the collection, oor the position
//!           of the first collated Partial, composed of unlabeled Partials
//!           in the original collection.
//!
//!	If compiled with NO_TEMPLATE_MEMBERS defined, then partials
//!	must be a PartialList, otherwise it can be any container type
//!	storing Partials that supports at least bidirectional iterators.
//!
//!   \sa Distiller::distill( Container & partials )
//
#if ! defined(NO_TEMPLATE_MEMBERS)
template< typename Container >
typename Container::iterator Distiller::distill( Container & partials )
#else
inline
PartialList::iterator Distiller::distill( PartialList & partials )
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
    //  if distill is changed to return the end of the collated
    //  Partials, rather than the end of the distilled Partials.
    //  In the current implementation, when there are collated
    //  Partials, there is no way to find the end of the range 
    //  of collated Partials after the call to distill, for 
    //  all containers. Making the distill function generic to
    //  all containers will work, but making it work on ranges
    //  requires an interface change.
    
    
    PartialList distilled, savezeros; //  temporary containers of Partials
				  
    //  remember the largest label ever used
    //  in distilling, collated unlabeled Partials
    //  will be assigned labels greater than this:
	Partial::label_type maxlabel = 0;
	
	Iterator lower = partials.begin();
	while ( lower != partials.end() )
	{
		Partial::label_type label = lower->label();

        //  identify the range of Partials having the same label:
	    Iterator upper = 
	        std::partition( lower, partials.end(),
                            PartialUtils::isLabelEqual( label ) );
                            
        //  upper is the first Partial after lower whose label is not
        //  equal to that of lower.
        
		//	[lowerbound, upperbound) is a range of all the
		//	partials in dist_list labeled label.
		//
		//	distill labeled channel, unless label is 0
		//	(zero-labeled Partials will remain where they
		//	are, and wind up at the front of the list):	
		if ( label != 0 )
		{
		    maxlabel = std::max( label, maxlabel );
		    
			//	make a container of the Partials having the same 
			//	label, and distill them:
			PartialList samelabel( lower, upper );
			distillOne( samelabel, label, distilled );
		}
		else	//	label == 0
		{
			//	save zero-labeled Partials to splice
			// 	back into the original list
			savezeros.insert( savezeros.begin(), lower, upper );
		}

        lower = upper;
	}
        
    //  copy the distilled Partials back into the source container,
	//  enddistilled is the end of the range of distilled Partials
	//  (return this to caller):
	Iterator enddistilled =
	    std::copy( distilled.begin(), distilled.end(), partials.begin() );
	
	//  append collated unlabeled Partials, if any,
	//  and remove extra Partials from the end of the 
	//  source collection:
	if ( savezeros.empty() )
	{
	    //  remove extra Partials from the end of the source container,
	    //  update enddistilled, which may become invalid by
	    //  erasure:
	    enddistilled = partials.erase( enddistilled, partials.end() );
	}
	else
	{
	    //	collate unlabeled (zero-labeled) Partials:
		collateUnlabeled( savezeros, std::max( maxlabel+1, 1 ) );

        //  copy the collated Partials back into the source container
        //  after the range of distilled Partials		
	    //  (enddistilled is still the end of the distilled Partials):
		Iterator endcollated = 
		    std::copy( savezeros.begin(), savezeros.end(), enddistilled );

	    //  remove extra Partials from the end of the source container
	    //  (enddistilled is still the end of the distilled Partials,
	    //  because it is the position of the first collate Partial, and
	    //  there is at least one collated Partial):
		partials.erase( endcollated, partials.end() );
	}
	
	return enddistilled;
}

// ---------------------------------------------------------------------------
//	Function call operator 
// ---------------------------------------------------------------------------
//!	Function call operator: same as distill( PartialList & partials ).
//!	
//! \sa Distiller::distill( Container & partials )
//
#if ! defined(NO_TEMPLATE_MEMBERS)
template< typename Container >
typename Container::iterator Distiller::operator()( Container & partials )
#else
inline
PartialList::iterator Distiller::operator()( PartialList & partials )
#endif
{ 
	return distill( partials );
}

// ---------------------------------------------------------------------------
//	distill
// ---------------------------------------------------------------------------
//! Static member that constructs an instance and applies
//! it to a sequence of Partials. 
//! Construct a Distiller using default parameters, 
//! and use it to distill a sequence of Partials. 
//!
//! \post   All Partials in the collection are uniquely-labeled
//! \param  partials is the collection of Partials to distill in-place
//! \param   partialFadeTime is the time (in seconds) over
//!          which Partials joined by distillation fade to
//!          and from zero amplitude.
//! \param   partialSilentTime is the minimum duration (in seconds) 
//!          of the silent (zero-amplitude) gap between two 
//!          Partials joined by distillation. (Default is
//!          0.0001 (one tenth of a millisecond).
//! \return the position of the end of the range of distilled Partials,
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
Distiller::distill( Container & partials, double partialFadeTime,
                                          double partialSilentTime )
#else
inline
PartialList::iterator 
Distiller::distill( PartialList & partials, double partialFadeTime,
                                            double partialSilentTime )
#endif
{
    Distiller instance( partialFadeTime, partialSilentTime );
    return instance.distill( partials );
}

}	//	end of namespace Loris

#endif /* ndef INCLUDE_DISTILLER_H */

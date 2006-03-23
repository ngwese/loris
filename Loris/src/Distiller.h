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
//!	are are left unmodified at the end of the Partial sequence.
//!	
//!	Distillation modifies the Partial container (a PartialList). All
//!	Partials in the distilled range having a common label are replaced by
//!	a single Partial in the distillation process. Only labeled
//! Partials are affected by distillation. 
//
class Distiller
{
//	-- instance variables --

	double _fadeTime, _gapTime;         // distillation parameters
		
//	-- public interface --
public:

//	-- global defaults and constants --

    //! Default time (in seconds) over which Partials joined by
    //! distillation fade to and from zero amplitude.
    static const double DefaultFadeTime;
    
    //! Default minimum duration (in seconds) of the silent 
    //! (zero-amplitude) gap between two Partials joined by distillation.
    static const double DefaultSilentTime;

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
	//!            and from zero amplitude. (Default is 
	//!            Distiller::DefaultFadeTime).
	//!   \param   partialSilentTime is the minimum duration (in seconds) 
	//!            of the silent (zero-amplitude) gap between two 
	//!            Partials joined by distillation. (Default is
	//!            Distiller::DefaultSilentTime).
	explicit
	Distiller( double partialFadeTime = Distiller::DefaultFadeTime,
               double partialSilentTime = Distiller::DefaultSilentTime );
	 
	//	Use compiler-generated copy, assign, and destroy.
	
//	-- distillation --

    //!	Distill labeled Partials in a collection leaving only a single 
    //!	Partial per non-zero label. 
    //!
    //!	Unlabeled (zero-labeled) Partials are left unmodified at 
    //! the end of the distilled Partials.
    //!
    //!	Return an iterator refering to the position of the first unlabeled Partial,
    //!	or the end of the distilled collection if there are no unlabeled Partials.
    //! Since distillation is in-place, the Partials collection may be smaller
    //! (fewer Partials) after distillation, and any iterators on the collection
    //! may be invalidated.
    //!
    //! \post   All labeled Partials in the collection are uniquely-labeled,
    //!         and all unlabeled Partials have been moved to the end of the
    //!         sequence.
    //! \param  partials is the collection of Partials to distill in-place
    //! \return the position of the end of the range of distilled Partials,
    //!         which is either the end of the collection, or the position
    //!         or the first unlabeled Partial.
    //!
    //!	If compiled with NO_TEMPLATE_MEMBERS defined, then partials
    //!	must be a PartialList, otherwise it can be any container type
    //!	storing Partials that supports at least bidirectional iterators.
    //!
    //! \sa Distiller::distill( Container & partials )
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
    //!
    //! \post   All labeled Partials in the collection are uniquely-labeled,
    //!         and all unlabeled Partials have been moved to the end of the
    //!         sequence.
    //! \param  partials is the collection of Partials to distill in-place
    //! \param  partialFadeTime is the time (in seconds) over
    //!         which Partials joined by distillation fade to
    //!         and from zero amplitude.
    //! \param  partialSilentTime is the minimum duration (in seconds) 
    //!         of the silent (zero-amplitude) gap between two 
    //!         Partials joined by distillation. (Default is
    //!         Distiller::DefaultSilentTime).
    //! \return the position of the end of the range of distilled Partials,
    //!         which is either the end of the collection, or the position
    //!         or the first unlabeled Partial.
    //!
    //!	If compiled with NO_TEMPLATE_MEMBERS defined, then partials
    //!	must be a PartialList, otherwise it can be any container type
    //!	storing Partials that supports at least bidirectional iterators.
   #if ! defined(NO_TEMPLATE_MEMBERS)
	template< typename Container >
	static typename Container::iterator 
	distill( Container & partials, double partialFadeTime,
             double partialSilentTime = Distiller::DefaultSilentTime );
#else
	static inline PartialList::iterator
	distill( PartialList & partials, double partialFadeTime,
             double partialSilentTime = Distiller::DefaultSilentTime );
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
	
};	//	end of class Distiller

// ---------------------------------------------------------------------------
//	distill
// ---------------------------------------------------------------------------
//! Distill labeled Partials in a collection leaving only a single 
//!	Partial per non-zero label. 
//!
//!	Unlabeled (zero-labeled) Partials are left unmodified at 
//! the end of the distilled Partials.
//!
//!	Return an iterator refering to the position of the first unlabeled Partial,
//!	or the end of the distilled collection if there are no unlabeled Partials.
//! Since distillation is in-place, the Partials collection may be smaller
//! (fewer Partials) after distillation, and any iterators on the collection
//! may be invalidated.
//!
//!   \post   All labeled Partials in the collection are uniquely-labeled,
//!           and all unlabeled Partials have been moved to the end of the
//!           sequence.
//!   \param  partials is the collection of Partials to distill in-place
//!   \return the position of the end of the range of distilled Partials,
//!           which is either the end of the collection, or the position
//!           or the first unlabeled Partial.
//!
//!	If compiled with NO_TEMPLATE_MEMBERS defined, then partials
//!	must be a PartialList, otherwise it can be any container type
//!	storing Partials that supports at least bidirectional iterators.
//!
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
    // Partition the Partials into labeled and unlabeled, 
    // and distill the labeled ones and replace the 
    // labeled range.
    // (This requires bidirectional iterator support.)
    Iterator beginUnlabeled = 
        std::partition( partials.begin(), partials.end(), 
                        std::not1( PartialUtils::isLabelEqual(0) ) );
        //  this used to be a stable partition, which 
        //  is slower and seems unnecessary
        //
        //  HEY figure out why I get slightly different results
        //  when I use stable_partition.
        
    PartialList distilled; //  temporary container of Partials				  
	
	Iterator lower = partials.begin();
	while ( lower != beginUnlabeled )
	{
		Partial::label_type label = lower->label();

        //  arrange a sequence of Partials having the same label:
        //  (need to partition because the Partials are not
        //  sorted by label, and cannot be sorted easily,
        //  if we don't know the capabilities of the container)
	    Iterator upper = 
	        std::partition( lower, beginUnlabeled,
                            PartialUtils::isLabelEqual( label ) );
            //  this used to be a stable partition, which 
            //  is slower and seems unnecessary
            //
            //  HEY figure out why I get slightly different results
            //  when I use stable_partition.
        
                            
        //  upper is the first Partial after lower whose label is not
        //  equal to that of lower.
		//	[lower, upper) is a range of all the
		//	partials labeled `label'.

        Assert( label != 0 );  // unlabeled Partials are at end

        //	make a container of the Partials having the same 
        //	label, and distill them:
        PartialList samelabel( lower, upper );
        distillOne( samelabel, label, distilled );

        lower = upper;
    }
        
    //  Copy the distilled Partials back into the source container,
	//  endDistilled is the end of the range of distilled Partials.
	Iterator endDistilled =
	    std::copy( distilled.begin(), distilled.end(), partials.begin() );

    //  remove extra Partials from the source container
    if ( endDistilled != beginUnlabeled )
    {
        partials.erase( endDistilled, beginUnlabeled );
        
        // restore beginUnlabeled:    
        typename Iterator::difference_type numDistilled = distilled.size();
        beginUnlabeled = partials.begin();
        std::advance( beginUnlabeled, numDistilled );
    }

    return beginUnlabeled;
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
//!
//!   \post   All labeled Partials in the collection are uniquely-labeled,
//!           and all unlabeled Partials have been moved to the end of the
//!           sequence.
//!   \param  partials is the collection of Partials to distill in-place
//!   \param  partialFadeTime is the time (in seconds) over
//!           which Partials joined by distillation fade to
//!           and from zero amplitude.
//!   \param  partialSilentTime is the minimum duration (in seconds) 
//!           of the silent (zero-amplitude) gap between two 
//!           Partials joined by distillation. (Default is
//!           Distiller::DefaultSilentTime).
//!   \return the position of the end of the range of distilled Partials,
//!           which is either the end of the collection, or the position
//!           or the first unlabeled Partial.
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

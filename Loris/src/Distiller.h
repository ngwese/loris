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

#include "PartialList.h"

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	class Distiller
//
//	Class Distiller represents an algorithm for "distilling" a group of
//	Partials that logically represent a single component into a single
//	Partial.
//	
//	The sound morphing algorithm in Loris requires that Partials in a
//	given source be labeled uniquely, that is, no two Partials can have
//	the same label. The Distiller enforces this condition. All Partials
//	identified with a particular frequency channel (see Channelizer), and,
//	therefore, having a common label, are distilled into a single Partial,
//	leaving at most a single Partial per frequency channel and label.
//	Channels that contain no Partials are not represented in the distilled
//	data. Partials that are not labeled, that is, Partials having label 0,
//	are are "collated " into groups of non-overlapping (in time)
// 	Partials, assigned an unused label (greater than the label associated
// 	with any frequency channel), and fused into a single Partial per
// 	group. "Collating" is a bit like "sifting" but non-overlapping
// 	Partials are grouped without regard to frequency proximity. This
// 	algorithm produces the smallest-possible number of collated Partials.
// 	Thanks to Ulrike Axen for providing this optimal algorithm.
//	
//	Distillation modifies the Partial container (a PartialList). All
//	Partials in the distilled range having a common label are replaced by
//	a single Partial in the distillation process.
//
class Distiller
{
//	-- instance variables --
	double _fadeTime, _gapTime;
	
//	-- public interface --
public:
//	-- construction --

	//	Construct a new Distiller using the specified fade time
	//	for gaps between Partials. When two non-overlapping Partials
	//	are distilled into a single Partial, the distilled Partial
	//	fades out at the end of the earlier Partial and back in again
	//	at the onset of the later one. The fade time is the time over
	//	which these fades occur. By default, use a 1 ms fade time.
	//	The gap time is the additional time over which a Partial faded
	//	out must remain at zero amplitude before it can fade back in.
	//	By default, use a gap time of 0.
	Distiller( double partialFadeTime = 0.001 /* 1 ms */,
			  double partialSilentTime = 0. );
	 
	//	Destroy this Distiller.
	~Distiller( void );
	
//	-- distillation --

	//	Distill labeled Partials in a PartialList into a list containing a single 
	//	Partial per non-zero label. The distilled list will contain as many 
	//	Partials as there were non-zero labels in the original list.
	//
	//	Unlabeled (zero-labeled) Partials are collated into the smallest-possible 
	//	number of Partials that does not combine any overlapping Partials.
	//	Collated Partials assigned labels higher than any label in the original 
	//	list, and appear at the end of the distilled PartialList.
	//
	//	Return an iterator refering to the position of the first collated Partial,
	//	of the end of the distilled list if there are no collated Partials.
	PartialList::iterator distill( PartialList & container );

	//!	Function call operator: same as distill( PartialList & container ).
	PartialList::iterator operator() ( PartialList & container )
		{ return distill( container ); }

// -- deprecated distill-range members, don't use! --

	//	Distill Partial on the specified half-open (STL-style) range in the
	//	specified container (PartialList). 
	void distill( PartialList & container, PartialList::iterator dist_begin, 
				  PartialList::iterator dist_end )
		{ 
			// These are deprecated, don't use them! 
			// I am getting rid of them soon!
			PartialList l( dist_begin, dist_end );
			distill( l );
			container.erase( dist_begin, dist_end );
			container.splice( dist_end, l );
		} 

	//	Function call operator: same as distill( PartialList & container,
	//	PartialList::iterator dist_begin, PartialList::iterator dist_end ).
	void operator() ( PartialList & container, PartialList::iterator dist_begin, 
					  PartialList::iterator dist_end )
		{ distill( container, dist_begin, dist_end ); }

//	-- unimplemented --
private:
	Distiller( const Distiller & other );
	Distiller & operator= ( const Distiller & other );
	
};	//	end of class Distiller

}	//	end of namespace Loris

#endif /* ndef INCLUDE_DISTILLER_H */

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
 * Sieve.C
 *
 * Implementation of class Sieve.
 *
 * Lippold Haken, 20 Jan 2001
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include <config.h>
#endif

#include <Sieve.h>
#include <Partial.h>
#include <Breakpoint.h>
#include <Exception.h>
#include <PartialUtils.h>
#include <Notifier.h>
#include <algorithm>
#include <list>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	sieve_aux STATIC
// ---------------------------------------------------------------------------
//	iterate over all other Partials with same labeling.
//  if any other partial has time-overlap with this partial,
//  and if this partial is not the longer of the two,
//  then remove the label from this partial.
//
//  return 1 if we remove the partial, else return zero.
//	
static int sieve_aux( Partial &src, double minGapTime,
				   	  std::list< Partial >::const_iterator start,
				   	  std::list< Partial >::const_iterator end)
{
	std::list< Partial >::const_iterator it;
	for ( it = start; it != end; ++it ) 
	{
		//	skip if other partial is already sifted out.
		if ( it->label() == 0 )
			continue;

		//	skip the source Partial:
		//	(identity test: compare addresses)
		//	(this is a sanity check, should not happen since
		//	src should be at position end)
		if ( &(*it) == &src )
		{
			debugger << "wierd: sieve_aux found src Partial in its iteration range" << endl;
			continue;
		}
			
		//  skip if no overlap:
		if ( src.startTime() > it->endTime() + minGapTime ||
			 src.endTime() + minGapTime < it->startTime() )
		{
			continue;
		}
		
		//  are we longer duration?
		//	(this should never be true, since the Partials
		//	are sorted by duration)
		if ( src.duration() > it->duration() ) 
		{
			debugger << "wierd: sieve_aux found src Partial longer than a Partial in the iteration range" << endl;
			continue;
		}
		
		//  we overlap wth something longer; remove us from this label.
		/*
		debugger << "Partial starting " << src.startTime() << ", " 
				 << src.frequencyAt( src.startTime() ) << " ending " 
				 << src.endTime()  << ", " << src.frequencyAt( src.endTime() ) 
				 << " zapped by Partial starting " 
				 << it->startTime() << ", " << it->frequencyAt( it->startTime() )
				 << " ending " << it->endTime() << ", " 
				 << it->frequencyAt( it->endTime() ) << endl;
		*/
		src.setLabel( 0 );
		return 1;
	}	//	end iteration over Partial range
	
	return 0;
}

// ---------------------------------------------------------------------------
//	Sieve constructor
// ---------------------------------------------------------------------------
//	By default, use a gap time equal to 0.
//
Sieve::Sieve( double minGapTime ) :
	_minGapTime( minGapTime )
{
	Assert( _minGapTime >= 0.0 );
}

// ---------------------------------------------------------------------------
//	Sieve destructor
// ---------------------------------------------------------------------------
//
Sieve::~Sieve( void )
{
}

// ---------------------------------------------------------------------------
//	sift
// ---------------------------------------------------------------------------
//	Sift labeled Partials: 
//  If any two partials with same label overlap in time,
//  keep only the longer of the two partials.
//  Set the label of the shorter duration partial to zero.
//	This operation does not preserve the sort order or the list!
//
//	If iterator bounds aren't specified, then the whole list is processed.
//
void 
Sieve::sift( std::list<Partial> & container )
{
	sift( container, container.begin(), container.end() );
}

// ---------------------------------------------------------------------------
//	sift
// ---------------------------------------------------------------------------
//	Sift labeled Partials: 
//  If any two partials with same label overlap in time,
//  keep only the longer of the two partials.
//  Set the label of the shorter duration partial to zero.
//	This operation does not preserve the sort order or the list!
//
//	By sorting the Partials by duration first, we can speed
//	this algorithm up by reducing the number of Partials that
//	need to be scanned by sieve_aux() (see below) and more 
//	importantly, we can make its behavior consistent regardless
//	of the prior sorting of the Partials (previously, long 
//	Partials could cause others to get sifted out, only to be
//	themselves sifted out by an even longer Partial).
//
//	It is ugly to pass in the list itself, instead of the iterator
//	range only, but we cannot sort the Partials without having the
//	list, nor can we splice the range into a temporary list, and
//	copying (twice) would be too expensive for large data sets.
//	So we are stuck requiring the list reference.
//
void 
Sieve::sift( std::list<Partial> & container, 
			 std::list< Partial >::iterator sift_begin, 
			 std::list< Partial >::iterator sift_end  )
{
	int zapped = 0;

	//	make a new temporary list that can be sorted and
	//	distilled, since it isn't possible to sort a select
	//	range of position in a list:
	std::list<Partial> sift_list;
	sift_list.splice( sift_list.begin(), container, sift_begin, sift_end );
	
	//	sort the std::list< Partial > by length and label:
	sift_list.sort( PartialUtils::duration_greater() );
	sift_list.sort( PartialUtils::label_less() );
	
	// 	iterate over labels and sift each one:
	std::list<Partial>::iterator lowerbound = sift_list.begin();
	while ( lowerbound != sift_list.end() )
	{
		int label = lowerbound->label();
		
		//	first the first element in l after sieve_begin
		//	having a label not equal to 'label':
		std::list<Partial>::iterator upperbound = 
			std::find_if( lowerbound, sift_list.end(), 
						  std::not1( std::bind2nd( PartialUtils::label_equals(), label ) ) );

#ifdef Debug_Loris
		//	don't want to compute this iterator distance unless debugging:
		debugger << "sifting Partials labeled " << label << endl;
		debugger << "Sieve found " << std::distance( lowerbound, upperbound ) << 
					" Partials labeled " << label << endl;
#endif
		//  sift all partials with this label, unless the
		//	label is 0:
		if ( label != 0 )
		{
			std::list< Partial >::iterator it;
			for ( it = lowerbound; it != upperbound; ++it ) 
			{
				//	sieve_aux only needs to consider Partials on the
				//	half-open range [lowerbound, it), because all 
				//	Partials after it are shorter, thanks to the
				//	sorting above:
				zapped += sieve_aux( *it, _minGapTime, lowerbound, it );
			} 
		}
		
		//	advance Partial list iterator:
		lowerbound = upperbound;
	}

#ifdef Debug_Loris
	debugger << "Sifted out (relabeled) " << zapped << " of " << sift_list.size() << "." << endl;
#endif

	//	splice the Partials back into the original list:
	container.splice( sift_end, sift_list );

}

}	//	end of namespace Loris


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
#include <PartialList.h>
#include <Breakpoint.h>
#include <Exception.h>
#include <PartialUtils.h>
#include <Notifier.h>
#include <algorithm>
#include <list>
#include <set>

//	begin namespace
namespace Loris {

//	Definition of a comparitor for sorting a collection of pointers
//	to Partials by label (increasing) and duration (decreasing), so
//	that Partial ptrs are arranged by label, with the lowest labels
//	first, and then with the longest Partials having each label
//	before the shorter ones.
struct SortPartialPtrs :
	public std::binary_function< const Partial *, const Partial *, bool >
{
	bool operator()( const Partial * lhs, const Partial * rhs ) const 
		{ 
			return 	( lhs->label() != rhs->label() ) ?
					( lhs->label() < rhs->label() ) :
					( lhs->duration() > rhs->duration() );
		}
};

//	Definition of a collection sorted by that comparitor.
typedef std::set< Partial *, SortPartialPtrs > PartialPtrsSet;

//	Definition of predicate for finding the end of a Patial *
//	range having a common label.
struct PartialPtrLabelNE :
	public std::unary_function< const Partial *, bool >
{
	int label;
	PartialPtrLabelNE( int l ) : label(l) {}

	bool operator()( const Partial * p ) const
		{ return p->label() != label; }
};


// ---------------------------------------------------------------------------
//	sieve_aux STATIC
// ---------------------------------------------------------------------------
//	Iterate over all other Partials with same labeling.
//  If any other partial has time-overlap with this partial,
//  and if this partial is not the longer of the two,
//  then relabel this Partial with label 0.
//
//	Overlap is defined by the minimum time gap between Partials
//	(minGapTime), so Partials that have less then minGapTime
//	between them are considered overlapping.
//
//  Return 1 if we relabel the partial, else return zero.
//	
static int sieve_aux( Partial & src, double minGapTime,
				   	  std::list<Partial>::const_iterator start,
				   	  std::list<Partial>::const_iterator end)
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
		Assert( &(*it) != &src );
			
		//  skip if no overlap:
		if ( src.startTime() > it->endTime() + minGapTime ||
			 src.endTime() + minGapTime < it->startTime() )
		{
			continue;
		}
		
		//  are we longer duration?
		//	(this should never be true, since the Partials
		//	are sorted by duration)
		Assert( src.duration() <= it->duration() );
		
		//  we overlap wth something longer; remove us from this label.
#if Debug_Loris
		debugger << "Partial starting " << src.startTime() << ", " 
				 << src.frequencyAt( src.startTime() ) << " ending " 
				 << src.endTime()  << ", " << src.frequencyAt( src.endTime() ) 
				 << " zapped by Partial starting " 
				 << it->startTime() << ", " << it->frequencyAt( it->startTime() )
				 << " ending " << it->endTime() << ", " 
				 << it->frequencyAt( it->endTime() ) << endl;
#endif

		src.setLabel( 0 );
		return 1;
	}	//	end iteration over Partial range
	
	return 0;
}

template <typename Iter>	//	Iter must be the position of a Partial *
Iter
find_overlapping( Partial & p, double minGapTime, Iter start, Iter end)
{
	for ( Iter it = start; it != end; ++it ) 
	{
		//	skip if other partial is already sifted out.
		if ( (*it)->label() == 0 )
			continue;
		
		//	skip the source Partial:
		//	(identity test: compare addresses)
		//	(this is a sanity check, should not happen since
		//	src should be at position end)
		Assert( (*it) != &p );

		//  test for overlap:
		if ( p.startTime() < (*it)->endTime() + minGapTime &&
			 p.endTime() + minGapTime > (*it)->startTime() )
		{
			//  Does the overlapping Partial have longer duration?
			//	(this should never be true, since the Partials
			//	are sorted by duration)
			Assert( p.duration() <= (*it)->duration() );
			
#if Debug_Loris
			debugger << "Partial starting " << p.startTime() << ", " 
					 << p.begin().breakpoint().frequency() << " ending " 
					 << p.endTime()  << ", " << (--p.end()).breakpoint().frequency() 
					 << " zapped by overlapping Partial starting " 
					 << (*it)->startTime() << ", " << (*it)->begin().breakpoint().frequency()
					 << " ending " << (*it)->endTime() << ", " 
					 << (--(*it)->end()).breakpoint().frequency()  << endl;
#endif
			return it;
		}
	}
	
	//	it is now the position of an overlapping Partial, or end:
	return end;
}

// ---------------------------------------------------------------------------
//	Sieve constructor
// ---------------------------------------------------------------------------
//	By default, use a gap time equal to 0, i.e. use the normal definition 
//	of overlap to deermine which Partials get sifted out.
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
//	The only other alternative would be to make a collection
//	(could be a set, which automatically sorts itself) of
//	pointers to Partials. This wouldn't be as expensive, and
//	but would require some new definitions, like the comparitor.
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
	//	(why am I making a temporary copy of this list whose
	//	contents I am replacing anyway?)
	//std::list<Partial> sift_list;
	//sift_list.splice( sift_list.begin(), container, sift_begin, sift_end );
	
	//	sort the std::list< Partial > by length and label:
	//sift_list.sort( PartialUtils::duration_greater() );
	//sift_list.sort( PartialUtils::label_less() );
	
	PartialPtrsSet sift_set;
	while ( sift_begin != sift_end )
	{
		sift_set.insert( &(*sift_begin) );
		++sift_begin;
	}
	
	// 	iterate over labels and sift each one:
	PartialPtrsSet::iterator lowerbound = sift_set.begin();
	while ( lowerbound != sift_set.end() )
	{
		int label = (*lowerbound)->label();
		
		//	first the first element in l after sieve_begin
		//	having a label not equal to 'label':
		PartialPtrsSet::iterator upperbound = 
			std::find_if( lowerbound, sift_set.end(), PartialPtrLabelNE(label) );

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
			PartialPtrsSet::iterator it;
			for ( it = lowerbound; it != upperbound; ++it ) 
			{
				//	sieve_aux only needs to consider Partials on the
				//	half-open range [lowerbound, it), because all 
				//	Partials after it are shorter, thanks to the
				//	sorting of the sift_set:
				//zapped += sieve_aux( *it, _minGapTime, lowerbound, it );
				
				if( it != find_overlapping( **it, _minGapTime, lowerbound, it ) )
				{
					(*it)->setLabel(0);
					++zapped;
				}
			} 
		}
		
		//	advance Partial set iterator:
		lowerbound = upperbound;
	}

#ifdef Debug_Loris
	debugger << "Sifted out (relabeled) " << zapped << " of " << sift_set.size() << "." << endl;
#endif

	//	splice the Partials back into the original list:
	//container.splice( sift_end, sift_list );

}

}	//	end of namespace Loris


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
 * Distiller.C
 *
 * Implementation of class Distiller.
 *
 * Kelly Fitz, 20 Oct 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include <config.h>
#endif

#include <Distiller.h>
#include <Breakpoint.h>
#include <BreakpointUtils.h>
#include <Exception.h>
#include <Partial.h>
#include <PartialList.h>
#include <PartialUtils.h>
#include <Notifier.h>

#include <algorithm>
#include <cmath>

#if defined(HAVE_M_PI) && (HAVE_M_PI)
	const double Pi = M_PI;
#else
	const double Pi = 3.14159265358979324;
#endif

//	begin namespace
namespace Loris {

//	helper function prototypes:
static Partial distillOne( PartialList & partials, int label, double fadeTime, double gapTime = 0 );
static void collateUnlabeled( PartialList & partials, int startLabel, double fadeTime, double gapTime = 0 );

// ---------------------------------------------------------------------------
//	Distiller constructor
// ---------------------------------------------------------------------------
//	By default, use a Partial fade time equal to 1 ms. This is the time over
//	which a Partial that is the distillation of two non-overlapping Partials
//	fades out at the end of one Partial, and in again at the beginning of the
//	other.
//
Distiller::Distiller( double partialFadeTime ) :
	_fadeTime( partialFadeTime )
{
	if( _fadeTime < 0.0 )
		Throw( InvalidArgument, "Distiller fade time must be non-negative." );
}

// ---------------------------------------------------------------------------
//	Distiller destructor
// ---------------------------------------------------------------------------
//
Distiller::~Distiller( void )
{
}

// ---------------------------------------------------------------------------
//	distill
// ---------------------------------------------------------------------------
//	Distill labeled Partials in a list into a list containing a single 
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
//
PartialList::iterator 
Distiller::distill( PartialList & partials ) 
{
	// save this information for debugging:
	int howmanywerethere = partials.size();

	//	sort the PartialList by label:
	debugger << "Distiller sorting Partials by label..." << endl;
	partials.sort( PartialUtils::label_less() );

	// 	iterate over labels and distill each one:
	PartialList::iterator lowerbound = partials.begin();
	PartialList::iterator enddistilled = partials.begin();
	
	//	save zeros here so they can be stuck at the end:
	PartialList savezeros;

	// 	invariant:
	// 	Partials on the range [dist_list.begin(), lowerbound)
	// 	have been distilled already, so lowerbound is either
	// 	the end of the list (dist_list.end()) or it is the
	// 	position of the first Partial in the list having a
	// 	label corresponding to a channel that has not yet 
	//	been distilled.	
	int label = 0;				  
	while ( lowerbound != partials.end() )
	{
		label = lowerbound->label();
		
		//	find the first element in l after lowerbound
		//	having a label not equal to 'label':
		PartialList::iterator upperbound = 
			std::find_if( lowerbound, partials.end(), 
						  std::not1( PartialUtils::label_equals(label) ) );
		
		//	[lowerbound, upperbound) is a range of all the
		//	partials in dist_list labeled label.
		//
		//	distill labeled channel, unless label is 0
		//	(zero-labeled Partials will remain where they
		//	are, and wind up at the front of the list):	
		if ( label != 0 )
		{
			if ( std::distance( lowerbound, upperbound ) > 1 )
			{
				//	make a container of the Partials having the same 
				//	label, and sort it by duration:
				PartialList samelabel( lowerbound, upperbound );
				debugger << "Distiller found " << samelabel.size() 
						<< " Partials labeled " << label << endl;
				
				//	replace the Partial at enddistilled with 
				// 	the resulting distilled partial:
				*enddistilled = distillOne( samelabel, label, _fadeTime );
			}
			else
			{
				*enddistilled = *lowerbound;
			}
			++enddistilled;
		}
		else	//	label == 0
		{
			//	save zero-labeled Partials to splice
			// 	back into the original list; may have
			//	to advance enddistilled:
			if ( enddistilled == lowerbound )
			{
				enddistilled = upperbound;
			}
			savezeros.splice( savezeros.begin(), partials, lowerbound, upperbound );
			debugger << "Distiller found " << savezeros.size() 
					 << " unlabeled Partials, saving..." << endl;
		}

		//	advance Partial list iterator:
		lowerbound = upperbound;
	}

	//	erase leftover Partials in original list:
	partials.erase( enddistilled, partials.end() );

	PartialList::iterator ret = partials.end();
	//	collate unlabeled (zero-labeled) Partials:
	if ( savezeros.size() > 0 )
	{
		collateUnlabeled( savezeros, std::max(label+1, 1), _fadeTime );
		ret = savezeros.begin();
		partials.splice( partials.end(), savezeros );
	}
	debugger << "distilled " << partials.size() << " Partials from " << howmanywerethere << endl;
	
	return ret;
}

#pragma mark -- helpers --

// ---------------------------------------------------------------------------
//	makeNullBefore	(STATIC)
// ---------------------------------------------------------------------------
// return a null (zero-amplitude) Breakpoint
// to preceed the specified Breakpoint
static Breakpoint makeNullBefore( const Breakpoint & bp, double fadeTime )
{
	Breakpoint ret( bp );
	// adjust phase
	double dp = 2. * Pi * fadeTime * bp.frequency();
	ret.setPhase( std::fmod( ret.phase() - dp, 2. * Pi ) );
	ret.setAmplitude(0.);
	
	return ret;
}

// ---------------------------------------------------------------------------
//	makeNullAfter	(STATIC)
// ---------------------------------------------------------------------------
// return a null (zero-amplitude) Breakpoint
// to succeed the specified Breakpoint
static Breakpoint makeNullAfter( const Breakpoint & bp, double fadeTime )
{
	Breakpoint ret( bp );
	// adjust phase
	double dp = 2. * Pi * fadeTime * bp.frequency();
	ret.setPhase( std::fmod( ret.phase() + dp, 2. * Pi ) );
	ret.setAmplitude(0.);
	
	return ret;
}

// ---------------------------------------------------------------------------
//	merge	(STATIC)
// ---------------------------------------------------------------------------
//	Merge the Breakpoints in the specified iterator range into the
//	distilled Partial. The beginning of the range may overlap, and 
//	will replace, some non-zero-amplitude portion of the distilled
//	Partial. Assume that there is no such overlap at the end of the 
//	range (could check).
//
static void merge( Partial::const_iterator beg, 
				   Partial::const_iterator end, 
				   Partial & destPartial, double fadeTime, 
				   double gapTime = 0. )
{	
	//	absorb energy in distilled Partial that overlaps the
	//	range to merge:
	Partial toMerge( beg, end );
	toMerge.absorb( destPartial );
	
	// debugger << "merging in " << toMerge.startTime() << "," << toMerge.endTime() << endl;
	
	//	fade out and in at end of merge if nececssary:
	//	(assumes that insert and erase do not invalidate
	//	other iterators!)
	double clearance = fadeTime + gapTime;
	Partial::iterator removeEnd = destPartial.findAfter( toMerge.endTime() + clearance );
	if ( removeEnd != destPartial.end() )
	{
		if ( (--toMerge.end()).breakpoint().amplitude() > 0 )
		{
			toMerge.insert( toMerge.endTime() + fadeTime, 
							makeNullAfter((--toMerge.end()).breakpoint(), fadeTime) );
		}

		if ( removeEnd.breakpoint().amplitude() > 0 )
		{
			//	update removeEnd so that we don't remove this 
			//	null we are inserting:
			removeEnd = destPartial.insert( removeEnd.time() - fadeTime, 
											makeNullBefore( removeEnd.breakpoint(), fadeTime ) );
		}		
	}
	
	//	fade out and in at beginning of merge if necessary:
	//	(assumes that insert and erase do not invalidate
	//	other iterators!)
	Partial::iterator removeBegin = destPartial.findAfter( toMerge.startTime() - clearance );
	if ( removeBegin != destPartial.begin() )
	{
		if ( toMerge.begin().breakpoint().amplitude() > 0 )
		{
			toMerge.insert( toMerge.startTime() - fadeTime, 
							makeNullBefore( toMerge.begin().breakpoint(), fadeTime ) );
		}

		Partial::iterator beforeMerge = --Partial::iterator(removeBegin);
		if ( beforeMerge.breakpoint().amplitude() > 0 )
		{
			destPartial.insert( beforeMerge.time() + fadeTime, 
								makeNullAfter( beforeMerge.breakpoint(), fadeTime ) );
		}		
	}
	
	//	remove the Breakpoints in the merge range from destPartial:
	destPartial.erase( removeBegin, removeEnd );

	//	insert the Breakpoints in the range:
	for ( Partial::const_iterator insert = toMerge.begin(); insert != toMerge.end(); ++insert )
	{
		destPartial.insert( insert.time(), insert.breakpoint() );
	}
		
/*	debugger << "CHECK" << endl;
	for ( Partial::iterator distit = destPartial.begin(); distit != destPartial.end(); ++distit )
	{
		debugger << distit.time() << " " << distit.breakpoint().frequency()<< " " << distit.breakpoint().amplitude() << endl;
	}
*/
}

// ---------------------------------------------------------------------------
//	findContribution		(STATIC)
// ---------------------------------------------------------------------------
//	Find and return an iterator range delimiting the portion of pshort that
// 	should be spliced into the distilled Partial plong. If any Breakpoint 
//	falls in a zero-amplitude region of plong, then pshort should contribute,
//	and its onset should be retained. Therefore, if cbeg is not equal to cend, 
//	then cbeg is pshort.begin().
//
std::pair< Partial::iterator, Partial::iterator >
findContribution( Partial & pshort, const Partial & plong, 
				  double fadeTime, double gapTime = 0. )
{
	//	a Breakpoint can only fit in the gap if there's
	//	enough time to fade out pshort, introduce a
	//	space of length gapTime, and fade in the rest
	//	of plong:
	double clearance = fadeTime + gapTime;
	
	Partial::iterator cbeg = pshort.begin();
	while ( cbeg != pshort.end() && 
			( plong.amplitudeAt( cbeg.time() ) > 0 ||
			  plong.amplitudeAt( cbeg.time() + clearance ) > 0 ) )
	{
		++cbeg;
	}
	
	Partial::iterator cend = cbeg;
	
	// if a gap is found, find the end of the
	// range of Breakpoints that fit in that
	// gap:
	while ( cend != pshort.end() &&
			plong.amplitudeAt( cend.time() ) == 0 &&
			plong.amplitudeAt( cend.time() + clearance ) == 0 )
	{
		++cend;
	}

	// if a gap is found, and it is big enough for at
	// least one Breakpoint, then include the 
	// onset of the Partial:
	if ( cbeg != pshort.end()  )
	{
		cbeg = pshort.begin();
	}
	
	return std::make_pair( cbeg, cend );
}

// ---------------------------------------------------------------------------
//	distillOne		(STATIC)
// ---------------------------------------------------------------------------
//	Helper function for distilling Partials having a common label
// 	into a single Partial with that label.
//
static Partial distillOne( PartialList & partials, int label, 
						   double fadeTime, double gapTime )
{
	Partial newp;
	newp.setLabel( label );

	//	return empty Partial if there is nothing
	//	to distill:
	if ( partials.size() == 0 )
		return newp;
	
	//	sort Partials by duration, longer
	//	Partials will be prefered:
	partials.sort( PartialUtils::duration_greater() );
	
	// keep the longest Partial:
	PartialList::iterator it = partials.begin();
	newp = *it;
		
	//	iterate over remaining partials:
	for ( ++it; it != partials.end(); ++it )
	{
		std::pair< Partial::iterator, Partial::iterator > range = 
			findContribution( *it, newp, fadeTime );
		Partial::iterator cb = range.first, ce = range.second;
		
		//	merge Breakpoints into the new Partial, if
		//	there are any that contribute, otherwise
		//	just absorb the current Partial as noise:
		if ( cb != ce )
		{
			//	absorb the non-contributing part:
			if ( ce != it->end() )
			{
				Partial absorbMe( --Partial::iterator(ce), it->end() );
				newp.absorb( absorbMe );
			}

			// merge the contributing part:
			merge( cb, ce, newp, fadeTime );
		}
		else
		{
			//	no contribution, absorb the whole thing:
			newp.absorb( *it );
		}		
	}
	
	return newp;
}

// ---------------------------------------------------------------------------
//	helper predicates
// ---------------------------------------------------------------------------
static bool ends_earlier( const Partial & lhs, const Partial & rhs )
{
	return lhs.endTime() < rhs.endTime();
}

struct ends_before : public std::unary_function< const Partial, bool >
{
	double t;
	ends_before( double time ) : t( time ) {}
	
	bool operator() ( const Partial & p ) const 
		{ return p.endTime() < t; }
};

// ---------------------------------------------------------------------------
//	collateUnlabeled
// ---------------------------------------------------------------------------
//	Helper function for collating unlabeled Partials into the smallest
// 	possible number of Partials that does not combine any temporally
//	overlapping Partials. Give each collated Partial a label, starting
//	with startlabel, and incrementing.
//
static void collateUnlabeled( PartialList & partials, int startlabel, 
						 double fadeTime, double gapTime )
{
	debugger << "collating " << partials.size() << " Partials..." << endl;
	
	// 	sort Partials by end time:
	// 	thanks to Ulrike Axen for this optimal algorithm!
	partials.sort( ends_earlier );
	
	//	the first (earliest-ending) Partial will be
	//	the first collated Partial:
	PartialList::iterator endcollated = partials.begin();
	(endcollated++)->setLabel( startlabel++ );
	
	//	invariant:
	//	Partials in the range [partials.begin(), endcollated)
	//	are the collated Partials.
	while ( endcollated != partials.end() )
	{
		//	find a collated Partial that ends
		//	before this one begins.
		//	There must be a gap of at least
		//	twice the fadeTime, because this algorithm
		//	does not remove any null Breakpoints, and 
		//	because Partials joined in this way might
		//	be far apart in frequency.
		const double clearance = (2.*fadeTime) + gapTime;
		PartialList::iterator it = 
			std::find_if( partials.begin(), endcollated, 
						  ends_before( endcollated->startTime() - clearance) );
						  
		// 	if no such Partial exists, then this Partial
		//	becomes one of the collated ones, otherwise, 
		//	insert two null Breakpoints, and then all
		//	the Breakpoints in this Partial:
		if ( it == endcollated )
		{
			(endcollated++)->setLabel( startlabel++ );
		}
		else
		{	
			Partial & addme = *endcollated;
			Partial & collated = *it;
			Assert( &addme != &collated );
			
			//	insert a null at the (current) end
			//	of collated:
			double nulltime1 = collated.endTime() + fadeTime;
			Breakpoint null1( collated.frequencyAt(nulltime1), 0., 
							  collated.bandwidthAt(nulltime1), collated.phaseAt(nulltime1) );			
			collated.insert( nulltime1, null1 );

			//	insert a null at the beginning of
			//	of the current Partial:
			double nulltime2 = addme.startTime() - fadeTime;
			Assert( nulltime2 >= nulltime1 );
			Breakpoint null2( addme.frequencyAt(nulltime2), 0., 
							  addme.bandwidthAt(nulltime2), addme.phaseAt(nulltime2) );			
			collated.insert( nulltime2, null2 );
	
			//	insert all the Breakpoints in addme 
			//	into collated:
			Partial::iterator addme_it;
			for ( addme_it = addme.begin(); addme_it != addme.end(); ++addme_it )
			{
				collated.insert( addme_it.time(), addme_it.breakpoint() );
			}
			
			//	remove this Partial from the list:
			partials.erase( endcollated++ );
		}
	}
	
	debugger << "...now have " << partials.size() << endl;
}

}	//	end of namespace Loris

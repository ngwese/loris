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
static void distill_aux( PartialList &, int , Partial &, double );
static void collate_aux( PartialList & , int, double );

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
//	Distill the labeled Partials in a list into a list containing a  single 
//	Partial per non-zero label. The distilled list will contain as many 
//	Partials as there were non-zero labels in the original list, plus the
//	zero-labeled Partials. 
//
//	Formerly, zero-labeled Partials are eliminated. Now they remain at
//	the end of the list.
//
//	sort partials by label
//	for each non-zero label: (distill_aux)
//		sort partials having this label by length
//		initialize new partial
//		for each partial having this label:
//			if this partial overlaps with any longer partial:
//				skip
//			else
//				insert a zero-amplitude breakpoint before the first breakpoint
//				for each breakpoint in this partial:
//					add up all energy in shorter partials at the time
//						of this breakpoint
//					absorb all that energy as noise
//					add a copy of this breakpoint to the new partial
//				insert a zero-amplitude breakpoint after the last breakpoint
//							
//
void 
Distiller::distill( PartialList & partials ) 
{
	// save this information for debugging:
	int howmanywerethere = partials.size();

	//	sort the PartialList by label:
	debugger << "Distiller sorting Partials by label..." << endl;
	partials.sort( PartialUtils::label_less() );	//	this is a stable sort

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
			//	make a container of the Partials having the same 
			//	label, and sort it by duration:
			PartialList samelabel( lowerbound, upperbound );
			debugger << "Distiller found " << samelabel.size() 
					 << " Partials labeled " << label
					 << ", sorting  by duration..." << endl;
			samelabel.sort( PartialUtils::duration_greater() );

			//	create the resulting distilled partial, 
			//	replacing the Partial at enddistilled:
			Partial & newp = *(enddistilled++);
			distill_aux( samelabel, label, newp, _fadeTime );
		}
		else	//	label == 0
		{
			//	save zero-labeled Partials to splice
			// 	back into the original list; may have
			//	to advance enddistilled:
			if ( enddistilled == lowerbound )
				enddistilled = upperbound;
			savezeros.splice( savezeros.begin(), partials, lowerbound, upperbound );
			debugger << "Distiller found " << savezeros.size() 
					 << " unlabeled Partials, saving..." << endl;
		}

		//	advance Partial list iterator:
		lowerbound = upperbound;
	}

	//	erase leftover Partials in original list:
	partials.erase( enddistilled, partials.end() );

	//	collate unlabeled (zero-labeled) Partials:
	collate_aux( savezeros, std::max(label+1, 1), _fadeTime );
	partials.splice( partials.end(), savezeros );

	debugger << "distilled " << partials.size() << " Partials from " << howmanywerethere << endl;
}

#pragma mark -- helpers --

// ---------------------------------------------------------------------------
//	overlap	(STATIC)
// ---------------------------------------------------------------------------
//	Return true if the specified Partial overlaps temporally with any
//	Partial in the given iterator range.
//
template <typename Iterator>
static bool
overlap( Iterator partial, Iterator begin, Iterator end )
{
	for ( Iterator it = begin; it != end; ++it )
	{
		if ( partial->startTime() < it->endTime() && 
			 partial->endTime() > it->startTime() )
			return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
//	collectEnergy	(STATIC)
// ---------------------------------------------------------------------------
//	Return the energy represented by all the Partials in the specified
//	iterator range at the specified time.
//
template <typename Iterator>
static double
collectEnergy( double time, Iterator begin, Iterator end )
{
	double e = 0.;	
	for ( Iterator it = begin; it != end; ++it )
	{
		double a = it->amplitudeAt( time );
		e += a * a;
	}
	return e;
}

// ---------------------------------------------------------------------------
//	distill_aux
// ---------------------------------------------------------------------------
//	Helper function for distilling Partials having a common label
// 	into a single Partial with that label.
//
static void distill_aux( PartialList & partials, int label, 
						 Partial & newp, double fadeTime )
{
	//	create the resulting distilled partial:
	newp = Partial();
	newp.setLabel( label );
	
	//	iterate over partials:
	PartialList::iterator it;
	for ( it = partials.begin(); it != partials.end(); ++it )
	{
		//	skip this Partial if it overlaps with any longer Partial:
		//	Need only consider earlier Partials in the list, because
		// 	the list is sorted by Partial duration.
		if ( ! overlap( it, partials.begin(), it ) )
		{
			//	look for null Breakpoints in newp that may need
			//	to be removed. No Partial in the distillation range
			//	overlaps this one (it), so nulls will be either
			//	the Breakpoint right before or right after the
			//	start time for this Partial (or both?):
			//
			//	Find the earliest position in newp after the start time
			//	of the current Partial. If there is such a position, and
			//	if the Breakpoint in newp after it->startTime() is a null
			//	and is closer to it->endTime() than _fadeTime, get rid of it,
			//	it is too close:
			//	(note: this will work even if the current Partial has only
			//	a single Breakpoint, but only because of the overlap check
			//	above)
			Partial::iterator after = newp.findAfter( it->startTime() );
			if ( after != newp.end() && 
				 after.time() < it->startTime() + fadeTime &&
				 after.breakpoint().amplitude() == 0. )
			{
				//	remove it:
				//	post-increment so that after is still
				//	valid and is still the position of the
				//	first Breakpoint after it->startTime();
				//	always safe because list erasure only 
				//	invalidates the erased position, and 
				//	because after is not newp.end().
				newp.erase( after++ );
			}
			
			//	if the Breakpoint in newp before it->startTime() is a null
			//	and is closer than _fadeTime, get rid of it, its
			//	too close:
			//	(note: this will work even if the current Partial has only
			//	a single Breakpoint, but only because of the overlap check
			//	above)
			//
			Partial::iterator before = after;	//	don't decrement until we are sure that
												//	after is not newp.begin(); undefined behavior
			if ( after != newp.begin() && 
			     (--before).time() > it->startTime() - fadeTime )
			{	 
				//	there's a Breakpoint soon before this Partial,
				//	if its a null, remove it:
				// 	before is no longer valid after this!
				if ( before.breakpoint().amplitude() == 0. )
				{
					newp.erase( before );
				}
			}
			else
			{
				//	nothing earlier than this Partial, or everything 
				//	else (so far) is much earlier than this Partial,
				//	insert a zero-amplitude Breakpoint before the
				//	beginning of this Partial:
				Breakpoint zeroBp = it->begin().breakpoint();
				zeroBp.setAmplitude( 0. );
				newp.insert( std::max( it->startTime() - fadeTime, 0. ), zeroBp );
			}
			
			//	adding in this Partial:
			//
			//	for each breakpoint in this partial:
			//		add up all energy in shorter partials at the time
			//			of this breakpoint
			//		absorb all that energy as noise
			//		add a copy of this breakpoint to the new partial
			//
			Partial::iterator lastInsert;
			for ( Partial::const_iterator envpos = it->begin(); envpos != it->end(); ++envpos )
			{
				Breakpoint bp = envpos.breakpoint();
				double time = envpos.time();
				PartialList::iterator nextp( it );
				++nextp;
				double xse = collectEnergy( time, nextp, partials.end() );
				BreakpointUtils::addNoiseEnergy( bp, xse );
				lastInsert = newp.insert( time, bp );
			}
	
			//	the last Breakpoint inserted is at position lastInsert, 
			//	if the next Breakpoint in newp is == newp.end, or
			//	is more than Partial:FadeTime() away, then insert a 
			//	zero-amplitude Breakpoint:
			Partial::iterator next( lastInsert );
			++next;
			double tend = it->endTime() + fadeTime;
			if ( next != newp.end() && next.time() < tend )
			{
				//	the next Breakpoint after this Partial is near,
				//	if its a null, get rid of it:
				if ( next.breakpoint().amplitude() == 0 )
				{
					newp.erase( next );
				}
			}
			else
			{
				//	nothing after this Partial, or everything
				//	else (so far) after this Partial is long after,
				//	insert a null Breakpoint:
				Breakpoint otherzeroBp = (--(it->end())).breakpoint();
				otherzeroBp.setAmplitude( 0. );
				lastInsert = newp.insert( tend, otherzeroBp );
			}
		
		}	//	end of adding in this non-overlapping Partial
		
	}	//	end loop over partials to distill
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
//	collate_aux
// ---------------------------------------------------------------------------
//	Helper function for collating unlabeled Partials into the smallest
// 	possible number of Partials that does not combine any temporally
//	overlapping Partials. Give each collated Partial a label, starting
//	with startlabel, and incrementing.
//
static void collate_aux( PartialList & partials, int startlabel, double fadeTime )
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
		PartialList::iterator it = 
			std::find_if( partials.begin(), endcollated, 
						  ends_before( endcollated->startTime() - (2.*fadeTime) ) );
						  
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
			Breakpoint null1 = (--(collated.end())).breakpoint();
			null1.setAmplitude( 0. );
			collated.insert( nulltime1, null1 );

			//	insert a null at the beginning of
			//	of the current Partial:
			double nulltime2 = addme.startTime() - fadeTime;
			Assert( nulltime2 >= nulltime1 );
			Breakpoint null2 = addme.begin().breakpoint();
			null2.setAmplitude( 0. );
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

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
				distill_aux( samelabel, label, *enddistilled, _fadeTime );
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

	//	collate unlabeled (zero-labeled) Partials:
	if ( savezeros.size() > 0 )
	{
		collate_aux( savezeros, std::max(label+1, 1), _fadeTime );
		partials.splice( partials.end(), savezeros );
	}
	debugger << "distilled " << partials.size() << " Partials from " << howmanywerethere << endl;
}

#pragma mark -- helpers --

// ---------------------------------------------------------------------------
//	overlap	(STATIC)
// ---------------------------------------------------------------------------
//	Return true if the specified Partials overlap temporally.
//
static bool
overlap( const Partial & p1, const Partial & p2, double fadeTime )
{
	return (p1.startTime() - fadeTime) < (p2.endTime() + fadeTime) && 
		   (p1.endTime() + fadeTime) > (p2.startTime() - fadeTime);
}

// ---------------------------------------------------------------------------
//	overlap	(STATIC)
// ---------------------------------------------------------------------------
//	Return true if the specified Partial overlaps temporally with any
//	Partial in the given iterator range.
//
template <typename Iterator>
static bool
overlap( Iterator partial, Iterator begin, Iterator end, double fadeTime )
{
	for ( Iterator it = begin; it != end; ++it )
	{
		if ( overlap( *partial, *it, fadeTime ) )
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
//	mergeNonOverlapping	(STATIC)
// ---------------------------------------------------------------------------
//	Merge the src Partial into the dst Partial, by adding its Breakpoints.
//	src is assumed _not_ to overlap any part of the dst Partial having
//	non-zero amplitude.
//
//	Null (zero-amplitude) Breakpoints are inserted in gaps between non-zero
//	amplitude parts of dst, and the ends of src, but this function does _not_
//	enforce the condition that there must be a gap and a zero-amplitude 
//	Breakpoint before the merged-in Partial, nor does it insert null 
//	Breakpoints at the ends of the dst Partial.
//
//	Three steps:
//	- remove null (zero-amplitude) Breakpoints from dst that overlap src
//	- insert null (zero-amplitude) Breakpoints into dst, if necessary
//	- copy the src Breakpoints into dst
//
static void mergeNonOverlapping( const Partial & src, Partial & dst, double fadeTime )
{
	Partial::iterator after = dst.findAfter( src.startTime() );
	
	//	Remove null (zero-amplitude) Breakpoints in dst 
	//	that are less than fadeTime earlier than the 
	//	startTime of src:
	//
	//	INVARIANT: 
	//	after is the position of the earliest Breakpoint in dst
	//	after the startTime of src, or it is dst.end().
	if ( after != dst.end() && after != dst.begin() )
	{
		Partial::iterator it = after;
		
		//	LOOP INVARIANT:
		//	it == after and it != dst.end()
		//	
		//	if it has a predecessor, and that predecessor
		//	is closer than fadeTime and has zero amplitude, 
		//	remove it.
		while ( it != dst.begin() && 
				(--it).time() > src.startTime() - fadeTime &&
				it.breakpoint().amplitude() == 0. )
		{
			debugger << "removing null Breakpoint before src" << endl;
			it = dst.erase( it );
		}
	}

	// 	Find and remove all Breakpoints in dst between the start 
	//	and end times of src. The non-overlapping condition implies
	//	that all these Breakpoints must be nulls (0 amplitude), and
	//	they can be removed. 
	//
	//	LOOP INVARIANT (as above): 
	//	after is the position of the earliest Breakpoint in dst
	//	after the startTime of src, or it is dst.end().
	while ( after != dst.end() && !(after.time() > src.endTime()) )
	{
		if ( after.breakpoint().amplitude() != 0. )
		{
			Throw( InvalidArgument, 
				   "mergeNonOverlapping src Partial must not overlap non-empty part of dst Partial");
		}
		debugger << "removing null Breakpoint during src" << endl;
		after = dst.erase( after );
	}
	
	//	Remove null (zero-amplitude) Breakpoints in dst 
	//	that are less than fadeTime later than the 
	//	endTime of src:
	//
	//	LOOP INVARIANT (as above): 
	//	after is the position of the earliest Breakpoint in dst
	//	after the startTime of src, or it is dst.end().
	while ( after != dst.end() && 
			!(after.time() > src.endTime() + fadeTime) &&
			after.breakpoint().amplitude() == 0.)
	{
		//	after is a null (zero-amplitude) Breakpoint in
		// 	dst that is nearer than fadeTime to the endTime
		//	of src, remove it:
		debugger << "removing null Breakpoint after src" << endl;
		after = dst.erase( after );
	}

	//	insert null Breakpoint(s) in dst before the beginning 
	//	of src, if necessary:
	//
	//	INVARIANT: 
	//	after is the position of a Breakpoint in dst
	//	later than the endTime of src, or it is dst.end().
	//	If it is a null Breakpoint, then it is more
	//	than fadeTime later than the endTime of src.
	if ( after != dst.begin() )
	{
		Partial::iterator before = after;
		--before;
		Assert( before.time() < src.startTime() );
		
		//	if the Breakpoint in dst before the startTime
		//	of src is earlier by more than fadeTime, insert
		//	a null before the start of src:
		if ( before.time() <  src.startTime() - fadeTime )
		{
			double t = std::max( src.startTime() - fadeTime, 0. );
			debugger << "inserting null Breakpoint before src, time = " << t << endl;
			Breakpoint zeroBp( src.frequencyAt(t), 0., src.bandwidthAt(t), src.phaseAt(t) );			
			dst.insert( t, zeroBp );
		}
		
		//	if the Breakpoint in dst before the startTime
		//	of src is earlier by more than 2*fadeTime, and
		//	before is not a null Breakpoint, insert
		//	another null after that Breakpoint:
		if ( before.breakpoint().amplitude() > 0 &&
			 before.time() <  src.startTime() - (2 * fadeTime) )
		{
			double t = before.time() + fadeTime;
			debugger << "inserting null Breakpoint before src, time = " << t << endl;
			// freq and bw should be those of before, phase needs to be computed:
			double ph = std::fmod( before->phase() + (2. * Pi * fadeTime * before->frequency()), 2. * Pi );
			Breakpoint zeroBp( before->frequency(), 0., before->bandwidth(), ph );			
			dst.insert( t, zeroBp );
		}
	}	

	//	insert null Breakpoint(s) in dst after the end 
	//	of src, if necessary:
	//
	//	INVARIANT (as above): 
	//	after is the position of a Breakpoint in dst
	//	later than the endTime of src, or it is dst.end().
	//	If it is a null Breakpoint, then it is more
	//	than fadeTime later than the endTime of src.
	if ( after != dst.end() )
	{
		Assert( after.time() > src.endTime() );
		
		//	if the Breakpoint in dst after the endTime
		//	of src is later by more than fadeTime, insert
		//	a null after the end of src:
		if ( after.time() >  src.endTime() + fadeTime )
		{
			double t = src.endTime() + fadeTime;
			debugger << "inserting null Breakpoint after src, time = " << t << endl;
			Breakpoint zeroBp( src.frequencyAt(t), 0., src.bandwidthAt(t), src.phaseAt(t) );			
			dst.insert( t, zeroBp );
		}
		
		//	if the Breakpoint in dst after the endTime
		//	of src is later by more than 2*fadeTime, and
		//	after is not a null Breakpoint, insert
		//	another null before that Breakpoint:
		if ( after.breakpoint().amplitude() > 0 &&
			 after.time() >  src.endTime() + (2 * fadeTime) )
		{
			double t = after.time() - fadeTime;
			debugger << "inserting null Breakpoint after src, time = " << t << endl;
			// freq and bw should be those of after, phase needs to be computed:
			double ph = std::fmod( after->phase() - (2. * Pi * fadeTime * after->frequency()), 2. * Pi );
			Breakpoint zeroBp( after->frequency(), 0., after->bandwidth(), ph );			
			dst.insert( t, zeroBp );
		}
	}	
			
	//	finally, copy Breakpoints from src into dst:
	for ( Partial::const_iterator envpos = src.begin(); envpos != src.end(); ++envpos )
	{
		debugger << "copying Breakpoint from src, time = " << envpos.time() << endl;
		dst.insert( envpos.time(), envpos.breakpoint() );
	}
}

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
//	mergeNonOverlapping	(STATIC)
// ---------------------------------------------------------------------------
//	Merge the src Partial into the dst Partial, by adding its Breakpoints.
//	src is assumed _not_ to overlap any part of the dst Partial having
//	non-zero amplitude.
//
static void mergeNonOverlapping2( const Partial & src, Partial & distilled, double fadeTime )
{
	Partial::const_iterator cont_beg = src.begin();
	Partial::const_iterator cont_end = src.end();
	
	if ( cont_beg != cont_end )
	{
		// remove Breakpoints from distilled, if necessary:
		Partial::iterator it = distilled.findAfter( cont_beg.time() - (2.*fadeTime) );
		Partial::const_iterator cont_last = --Partial::const_iterator(cont_end);
		double endtime = cont_last.time();
		while ( it != distilled.end() && it.time() < endtime + (2.*fadeTime) )
		{
			it = distilled.erase( it );
		}

		// merge contributing Breakpoints:
		Partial::const_iterator insert = cont_beg;
		while ( insert != cont_end )
		{
			distilled.insert( insert.time(), insert.breakpoint() );
			++insert;
		}
	
		//
		// INSERT ZEROS HERE!!!!!
		//
		// need one, or possibly two before each onset
		// (only need to do this if there are contributing
		// Breakpoints)
		
		// before contributing region:
		if ( distilled.amplitudeAt( cont_beg.time() - fadeTime ) > 0. )
		{
			Partial::iterator inserted = 
				distilled.insert( cont_beg.time() - fadeTime, 
								makeNullBefore( cont_beg.breakpoint(), fadeTime ) );
			if ( inserted != distilled.begin() && (--inserted)->amplitude() > 0. )
			{
				Assert( inserted.time() < cont_beg.time() - (2*fadeTime) );
				distilled.insert( inserted.time() + fadeTime, makeNullAfter( inserted.breakpoint(), fadeTime ) );
			}
		}

		// after contributing region:
		if ( distilled.amplitudeAt( endtime + fadeTime ) > 0. )
		{
			Partial::iterator inserted = 
				distilled.insert( endtime + fadeTime, makeNullAfter( cont_last.breakpoint(), fadeTime ) );
			
			if ( (++inserted) != distilled.end() && inserted->amplitude() > 0. )
			{
				Assert( inserted.time() > endtime + (2*fadeTime) );
				distilled.insert( inserted.time() - fadeTime, makeNullBefore( inserted.breakpoint(), fadeTime ) );
			}

		}
	}					  
}

// ---------------------------------------------------------------------------
//	findContribution		(STATIC)
// ---------------------------------------------------------------------------
//
std::pair< Partial::iterator, Partial::iterator >
findContribution( Partial & pshort, const Partial & plong, double fadeTime )
{
	Partial::iterator cbeg = pshort.begin();
	while ( cbeg != pshort.end() && 
			plong.amplitudeAt( cbeg.time() ) > 0 )
	{
		++cbeg;
	}
	
	Partial::iterator cend = cbeg;
	
	// if a gap is found, find the end of the
	// range of Breakpoints that fit in that
	// gap:
	while ( cend != pshort.end() &&
			plong.amplitudeAt( cend.time() + (2*fadeTime) ) == 0 )
	{
		++cend;
	}

	// if a gap is found, and it is big enough for at
	// least one Breakpoint, then include the 
	// onset of the Partial:
	if ( cbeg != cend  )
	{
		cbeg = pshort.begin();
	}
	
	return std::make_pair( cbeg, cend );
}

// ---------------------------------------------------------------------------
//	distill_aux		(STATIC)
// ---------------------------------------------------------------------------
//	Helper function for distilling Partials having a common label
// 	into a single Partial with that label.
//
static void distill_aux( PartialList & partials, int label, 
						 Partial & newp, double fadeTime )
{
	if ( partials.size() == 0 )
	{
		newp = Partial();
		newp.setLabel( label );
		return;
	}
	
	//	sort Partials by duration, longer
	//	Partials will be prefered:
	partials.sort( PartialUtils::duration_greater() );
	
	// keep the longest Partial:
	PartialList::iterator it = partials.begin();
	newp = *it;
	newp.setLabel( label );
	
	//	iterate over remaining partials:
	for ( ++it; it != partials.end(); ++it )
	{
		std::pair< Partial::iterator, Partial::iterator > range = 
			findContribution( *it, newp, fadeTime );
		Partial::iterator cb = range.first, ce = range.second;
		
		if ( cb != ce )
		{
			mergeNonOverlapping2( Partial(cb, ce), newp, fadeTime );	
		}
		
		// abosrb non-contributing ends of shorter:
		if ( cb != it->begin() )
		{
			Partial absorbMe( it->begin(), cb );
			newp.absorb( absorbMe );
		}
		
		if ( ce != it->end() )
		{
			Partial absorbMe( ce, it->end()  );
			newp.absorb( absorbMe );
		}
		

/*		//	skip this Partial if it overlaps with any longer Partial:
		//	Need only consider earlier Partials in the list, because
		// 	the list is sorted by Partial duration.
		if ( ! overlap( it, partials.begin(), it, fadeTime ) )
		{
			//	absorb all shorter partials' energy as noise,
			//	add a copy of this partial to the new partial
			//	(note: energy will be absorbed only from 
			//	overlapping shorter Partials, which are never
			//	going to be considered inside this loop, so 
			//	`it' is never going to be a Partial whose
			//	energy was already absorbed. Moreover, no two
			//	Partials will absorb the same energy, because
			//	they would have to overlap.)
			PartialList::iterator nextp( it );
			while ( ++nextp != partials.end() )
			{	
				if ( overlap(*it, *nextp, fadeTime) )
				{
					debugger << "absorbing energy in Partial starting at time " << nextp->startTime() << endl;
					it->absorb( *nextp );
				}
			}
			mergeNonOverlapping2( *it, newp, fadeTime );			
		}		
*/
	}
	newp.setLabel( label );
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
		const double GapTime = 2.*fadeTime;
		PartialList::iterator it = 
			std::find_if( partials.begin(), endcollated, 
						  ends_before( endcollated->startTime() - GapTime) );
						  
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

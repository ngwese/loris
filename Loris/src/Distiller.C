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
#include "Distiller.h"
#include "Partial.h"
#include "Breakpoint.h"
#include "Exception.h"
#include "PartialUtils.h"
#include "notifier.h"
#include <algorithm>
#include <list>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	distill_aux STATIC
// ---------------------------------------------------------------------------
//	Mother of all helper functions, distillation core: 
//	Distill a single source Partial into the destination Partial, 
//	inserting Breakpoints only at times when the source Partial
//	is louder than every other Partial.
//
//	For every Breakpoint in every Partial in the list, determine whether 
//	that Breakpoint's Partial is the loudest (greatest total amplitude -or- 
//	greatest sine energy, which one?) Partial at the time of that Breakpoint.
//	If not, forget it and go on to the next Breakpoint. If so, copy that
//	Breakpoint, add all other Partials' energies as Bandwidth, and insert 
//	the copy in the new distilled Partial. If there are gaps, time when no
//	Partial in the list is active, ramp the Partial out and back in again at
//	the end of the gap.
//
//	This algorithm is neither particularly time- nor space-efficient.
//
//	(start, end) _must_ be a valid range in a list< Partial >...or else!
//	
static void distill_aux( const Partial & src, 
			   			 Partial & dest, 
				   		 std::list< Partial >::const_iterator start,
				   		 std::list< Partial >::const_iterator end  )
{
	//	iterate over the source Partial:
	for ( PartialConstIterator pIter = src.begin(); pIter != src.end(); ++pIter )
	{
		//	iterate over all Partials in the range and compute the
		//	bandwidth energy contribution at the time of bp
		//	due to all the other Partials:
		double xse = 0.;
		std::list< Partial >::const_iterator it;
		for ( it = start; it != end; ++it ) 
		{
			//	skip the source Partial:
			//	(identity test: compare addresses)
			if ( &(*it) == &src )
				continue;

			//	accumulate energy to be added as bandwidth:
			//	(this should use the same algorithm as the energy
			//	redistribution in the analysis)
			//	If this Partial is louder than is represented
			//	by bp at the time of bp, then break out of
			//	this loop, this Breakpoint will not be part of 
			//	the distilled Partial.
			double a = it->amplitudeAt( pIter.time() );
			if ( a > pIter->amplitude() ) {
				break;	
			}
			else {
				xse += a*a;
			}
		}	//	end iteration over Partial range
		
		//	if all Partials were examined, then
		//	src is the loudest Partial at the time of bp,
		//	and the bandwidth energy contributed by the 
		//	other Partials is xse.
		//	Create a new Breakpoint and add it to dest:
		if ( it == end ) 
		{
			//	create and insert the new Breakpoint:
			Breakpoint newBp( *pIter );
			newBp.addNoise( xse );
			dest.insert( pIter.time(), newBp );
			
		}	//	end if all other Partials are quieter at time of pIter
		
	}	//	end iteration over source Partial
}

// ---------------------------------------------------------------------------
//	fixGaps STATIC
// ---------------------------------------------------------------------------
//	Look for gaps between Partials in the half-open interval [start,end)
//	and make sure that the new distilled Partial (dest) ramps to zero
//	amplitude during those gaps.
//
static void fixGaps( Partial & dest, 
		  			 std::list< Partial >::const_iterator start,
		  			 std::list< Partial >::const_iterator end )
{
	//	make a list of segments:
	std::list< std::pair<double, double> > segments;
	for ( std::list< Partial >::const_iterator p = start; p != end; ++p )
	{	
		segments.push_back( std::make_pair( p->startTime(), p->endTime() ) );
	}
	//debugger << "found " << segments.size() << " segments." << endl;
	
	//	sort the list and collapse overlapping segments
	//	to leave a list of non-overlapping segments:
	segments.sort(); 	//	uses op < ( pair<>, pair<> ), does what you think
	std::list< std::pair<double, double> >::iterator curseg;
	for ( curseg = segments.begin(); curseg != segments.end(); ++curseg )
	{
		//	curseg absorbs all succeeding segments that
		//	begin before it ends:
		std::list< std::pair<double, double> >::iterator nextseg = curseg;
		for ( ++nextseg; nextseg != segments.end(); ++nextseg ) 
		{
			if ( nextseg->first > curseg->second )
				break;
			//	else absorb:
			curseg->second = std::max( curseg->second, nextseg->second );
		}
		
		//	nextseg now begins after curseg ends,
		//	remove all segments after curseg and before
		//	nextseg (list<> erasure is guaranteed not to 
		//	invalidate curseg or any iterators refering to
		//	elements that aren't deleted):
		std::list< std::pair<double, double> >::iterator del = curseg;
		++del;
		segments.erase( del, nextseg );
	}
	//debugger << "collapsed to " << segments.size() << " segments" << endl;
	
	//	fill in gaps between segments:
	if ( segments.size() > 1 )
	{
		std::list< std::pair<double, double> >::iterator seg = segments.begin();
		std::list< std::pair<double, double> >::iterator nextseg = seg;	
		++nextseg;
		while ( nextseg != segments.end() )
		{	
			double gap = nextseg->first - seg->second;
			
			//	sanity check:
			Assert( gap > 0. );
			
			//	fill in the gap if its big enough:
			if ( gap > 2. * Partial::FadeTime() )
			{
				//
				//	HEY should we use zero BW or dest BW?
				//
				double time = seg->second + Partial::FadeTime();
				double freq = dest.frequencyAt( seg->second );
				double phase = dest.phaseAt(seg->second) +  (freq * Partial::FadeTime()); 
				double bw = dest.bandwidthAt( seg->second );
				dest.insert( time, Breakpoint( freq, 0., bw, phase ) );
				
				time = nextseg->first - Partial::FadeTime();
				freq = dest.frequencyAt( nextseg->first );
				phase = dest.phaseAt(nextseg->first) - (freq * Partial::FadeTime());
				bw = dest.bandwidthAt( nextseg->first );
				dest.insert( time, Breakpoint( freq, 0., bw, phase ) );
			} 
			
			//	advance iterators:
			++seg;
			++nextseg;
		}
	}
	
	//	make the dest Partial have 0 amplitude
	//	Breakpoints at its head and tail (VERY 
	//	important for morphing):
	if ( dest.amplitudeAt( dest.startTime() ) > 0. )
	{
		double time = dest.startTime() - Partial::FadeTime();
		double freq = dest.frequencyAt(time);
		double phase = dest.phaseAt(time);
		double bw = dest.bandwidthAt(time);
		dest.insert( time, Breakpoint( freq, 0., bw, phase ) );		
	}
	
	if ( dest.amplitudeAt( dest.endTime() ) > 0. )
	{
		double time = dest.endTime() + Partial::FadeTime();
		double freq = dest.frequencyAt(time);
		double phase = dest.phaseAt(time);
		double bw = dest.bandwidthAt(time);
		dest.insert( time, Breakpoint( freq, 0., bw, phase ) );		
	}
}

// ---------------------------------------------------------------------------
//	Distiller constructor
// ---------------------------------------------------------------------------
//
Distiller::Distiller( void )
{
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
void 
Distiller::distill( std::list<Partial> & l )
{
	int howmanywerethere = l.size();

	//	sort the std::list< Partial > by label:
	debugger << "Distiller sorting Partials by label..." << endl;
	l.sort( PartialUtils::label_less() );
	
	
	// 	iterate over labels and distill each one:
	std::list<Partial>::iterator dist_begin = l.begin();
					  
	while ( dist_begin != l.end() )
	{
		int label = dist_begin->label();
		debugger << "distilling Partials labeled " << label << endl;
		
		//	first the first element in l after dist_begin
		//	having a label not equal to 'label':
		std::list<Partial>::iterator dist_end = 
			std::find_if( dist_begin, l.end(), 
						  std::not1( std::bind2nd( PartialUtils::label_equals(), label ) ) );
#ifdef Debug_Loris
		//	don't want to compute this iterator distance unless debugging:
		debugger << "Distiller found " << std::distance( dist_begin, dist_end ) << 
					" Partials labeled " << label << endl;
#endif
		
		//	distill label, unless label is 0
		//	(zero-labeled Partials will remain where they
		//	are, and wind up at the end of the list):	
		if ( label != 0 )
		{
			//	create the resulting distilled partial:
			Partial newp;
			newp.setLabel( label );
			
			//	iterate over range:
			for ( std::list< Partial >::const_iterator it = dist_begin; it != dist_end; ++it )
			{
				distill_aux( *it, newp, dist_begin, dist_end );
			}
			
			//	fill in gaps:
			fixGaps( newp, dist_begin, dist_end );
			
			//	insert the new Partial at the beginning of
			//	the list, and erase the Partials in the
			//	distilled range:
			l.erase( dist_begin, dist_end );
			l.insert( l.begin(), newp );
		}

		//	advance Partial list iterator:
		dist_begin = dist_end;
	}

	debugger << "distilled " << l.size() << " Partials from " << howmanywerethere << endl;
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

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
#include <Partial.h>
#include <PartialList.h>
#include <Breakpoint.h>
#include <Exception.h>
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
//	If iterator bounds aren't specified, then the whole list is processed.
//
void 
Distiller::distill( PartialList & container  )
{
	distill( container, container.begin(), container.end() );
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
//	sort partials by length
//	stable_sort by label (preserve length sorting for partials
//		of same label, can I do this? Is list::sort() stable?)
//	for each label:
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
Distiller::distill( PartialList & container, 
					PartialList::iterator dist_begin, 
					PartialList::iterator dist_end )
{
	int howmanywerethere = container.size();

	//	make a new temporary list that can be sorted and
	//	distilled, since it isn't possible to sort a select
	//	range of position in a list:
	PartialList dist_list;
	dist_list.splice( dist_list.begin(), container, dist_begin, dist_end );
	
	//	sort the PartialList by duration and label:
	debugger << "Distiller sorting Partials by duration..." << endl;
	dist_list.sort( PartialUtils::duration_greater() );
	debugger << "Distiller sorting Partials by label..." << endl;
	dist_list.sort( PartialUtils::label_less() );	//	this is a stable sort

	// 	iterate over labels and distill each one:
	PartialList::iterator lowerbound = dist_list.begin();
					  
	while ( lowerbound != dist_list.end() )
	{
		int label = lowerbound->label();
		
		//	find the first element in l after lowerbound
		//	having a label not equal to 'label':
		PartialList::iterator upperbound = 
			std::find_if( lowerbound, dist_list.end(), 
						  std::not1( std::bind2nd( PartialUtils::label_equals(), label ) ) );
#ifdef Debug_Loris
		//	don't want to compute this iterator distance unless debugging:
		debugger << "distilling Partials labeled " << label << endl;
		debugger << "Distiller found " << std::distance( lowerbound, upperbound ) << 
					" Partials labeled " << label << endl;
#endif
		
		//	distill label, unless label is 0
		//	(zero-labeled Partials will remain where they
		//	are, and wind up at the front of the list):	
		if ( label != 0 )
		{
			//	create the resulting distilled partial:
			Partial newp;
			newp.setLabel( label );
			
			//	iterate over range:
			for ( PartialList::iterator it = lowerbound; it != upperbound; ++it )
			{
				//	skip this Partial if it overlaps with any longer Partial:
				if ( ! overlap( it, lowerbound, it ) )
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
					//	and is closer than Partial::FadeTime, get rid of it, its
					//	too close:
					//	(note: this will work even if the current Partial has only
					//	a single Breakpoint, but only because of the overlap check
					//	above)
					Partial::iterator after = newp.findAfter( it->startTime() );
					Partial::iterator before = after;	//	don't decrement until we are sure that
														//	after is not newp.begin(); undefined behavior
					if ( after != newp.end() && 
						 after.time() < it->startTime() + Partial::FadeTime() &&
						 after.breakpoint().amplitude() == 0. )
					{
						//	remove it:
						newp.erase( after );
					}
					
					//	if the Breakpoint in newp before it->startTime() is a null
					//	and is closer than Partial::FadeTime, get rid of it, its
					//	too close:
					//	(note: this will work even if the current Partial has only
					//	a single Breakpoint, but only because of the overlap check
					//	above)
					//
					if ( after != newp.begin() && 
						 (--before).time() > it->startTime() - Partial::FadeTime() )
					{	 
						//	there's a Breakpoint soon before this Partial,
						//	if its a null, remove it:
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
						newp.insert( std::max( it->startTime() - Partial::FadeTime(), 0. ), zeroBp );
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
						double xse = collectEnergy( time, nextp, upperbound );
						bp.addNoise( xse );
						lastInsert = newp.insert( time, bp );
					}
			
					//	the last Breakpoint inserted is at position lastInsert, 
					//	if the next Breakpoint in newp is == newp.end, or
					//	is more than Partial:FadeTime() away, then insert a 
					//	zero-amplitude Breakpoint:
					Partial::iterator next( lastInsert );
					++next;
					double tend = it->endTime() + Partial::FadeTime();
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
				
			}	//	end loop over distillation range
			
			//	insert the new Partial at the end of the
			//	specified distillation range in the original
			//	container:			
			//putEmHere = container.insert( putEmHere, newp );
			container.insert( dist_end, newp );
		}
		else	//	label == 0
		{
			//	splice zero-labeled Partials back in the original list:
			container.splice( dist_end, dist_list, lowerbound, upperbound );
		}

		//	advance Partial list iterator:
		lowerbound = upperbound;
	}

	debugger << "distilled " << container.size() << " Partials from " << howmanywerethere << endl;
}

}	//	end of namespace Loris

// ===========================================================================
//	Distiller.C
//	
//	A group of Partials that logically represent a single component
//	can be distilled into a single Partial using a Distiller. 
//
//	-kel 20 Oct 99
//
// ===========================================================================
#include "Distiller.h"
#include "Partial.h"
#include "Breakpoint.h"
#include "Map.h"
#include "notifier.h"
#include <cmath>
#include <list>
#include <vector>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	Distiller constructor
// ---------------------------------------------------------------------------
//
Distiller::Distiller( void )
{
}



// ---------------------------------------------------------------------------
//	distill
// ---------------------------------------------------------------------------
//	Distill the Partials in a range into a single Partial, 
//	add the result to the collection of distilled Partials.
//
//	(start, end) _must_ be a valid range in a list< Partial >...or else!
//
void 
Distiller::distill( PartialList::const_iterator start,
				 	PartialList::const_iterator end, 
				 	int assignLabel )
{
	if ( assignLabel <= 0 )
		Throw( InvalidArgument, "distillation label must be positive" );

	//	create the resulting distilled partial:
	Partial newp;
	newp.setLabel( assignLabel );
	
	//	iterate over range:
	for ( PartialList::const_iterator it = start; it != end; ++it )
	{
		distillOne( *it, newp, start, end );
	}
	
	//	fill in gaps:
	fixGaps( newp, start, end );

	//	add the newly-distilled partial to the collection:
	partials().push_back( newp );
}

#pragma mark -
#pragma mark helpers
// ---------------------------------------------------------------------------
//	distillOne
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
void
Distiller::distillOne( const Partial & src, 
					   Partial & dest, 
					   PartialList::const_iterator start,
					   PartialList::const_iterator end  )
{
	//	iterate over the source Partial:
	for ( PartialConstIterator pIter = src.begin(); pIter != src.end(); ++pIter )
	{
		//	iterate over all Partials in the range and compute the
		//	bandwidth energy contribution at the time of bp
		//	due to all the other Partials:
		double xse = 0.;
		PartialList::const_iterator it;
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


/*
// ---------------------------------------------------------------------------
//	gapAt
// ---------------------------------------------------------------------------
//	Return true if none of the Partials in the range has any energy at time, 
//	otherwise return false.
//
bool 
Distiller::gapAt( double time, 
				  PartialList::const_iterator start,
				  PartialList::const_iterator end ) const
{
	while ( start != end ) {
		if ( (*start).amplitudeAt( time ) > 0. ) {
			return false;
		}
		++start;
	}
	return true;
}
*/

// ---------------------------------------------------------------------------
//	fixGaps
// ---------------------------------------------------------------------------
//	Look for gaps between Partials in the half-open interval [start,end)
//	and make sure that the new distilled Partial (dest) ramps to zero
//	amplitude during those gaps.
//
void 
Distiller::fixGaps( Partial & dest, 
				  	PartialList::const_iterator start,
				  	PartialList::const_iterator end )
{
	//	make a list of segments:
	std::list< std::pair<double, double> > segments;
	for ( PartialList::const_iterator p = start; p != end; ++p )
	{	
		segments.push_back( std::make_pair( p->startTime(), p->endTime() ) );
	}
	debugger << "found " << segments.size() << " segments." << endl;
	
	//	sort the list and collapse overlapping segments
	//	to leave a list of non-overlapping segments:
	segments.sort(); 	//	uses op < ( pair<>, pair<> ), does what you think
	std::list< std::pair<double, double> >::iterator curseg;
	for ( curseg = segments.begin(); curseg != segments.end(); ++curseg )
	{
		//	curseg absorbs all succeeding segments that
		//	begin before it ends:
		std::list< std::pair<double, double> >::iterator nextseg = curseg;
		while ( (++nextseg)->first < curseg->second )
		{
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
	debugger << "collapsed to " << segments.size() << " segments" << endl;
	
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
				dest.insert( time, Breakpoint( freq, 0., 0., phase ) );
				
				time = nextseg->first - Partial::FadeTime();
				freq = dest.frequencyAt( nextseg->first );
				phase = dest.phaseAt(nextseg->first) - (freq * Partial::FadeTime());
				dest.insert( time, Breakpoint( freq, 0., 0., phase ) );
			} 
			
			//	advance iterators:
			++seg;
			++nextseg;
		}
	}
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

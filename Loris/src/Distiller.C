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

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
#else
	#include <math.h>
#endif

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	distill
// ---------------------------------------------------------------------------
//	Distill the Partials in all in a range into a single Partial, 
//	add the result to the collection of distilled Partials.
//
//	(start, end) _must_ be a valid range in a list< Partial >...or else!
//
const Partial & 
Distiller::distill( PartialList::const_iterator start,
				 	PartialList::const_iterator end, 
				 	int assignLabel /* default = 0 */ )
{
	//	create the resulting distilled partial:
	Partial newp;
	newp.setLabel( assignLabel );
	
	//	iterate over range:
	for ( PartialList::const_iterator it = start; it != end; ++it )
	{
		distillOne( *it, newp, start, end );
	}

	//	add the newly-distilled partial to the collection:
	partials().push_back( newp );
	return partials().back();
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
	const double FADE_TIME = 0.001;	//	1 ms
	
	//	iterate over the source Partial:
	for ( PartialConstIterator pIter = src.begin(); pIter != src.end(); ++pIter )
	{
		//	iterate over all Partials in the range and compute the
		//	bandwidth energy contribution at the time of bp
		//	due to all the other Partials:
		double xse = 0.;
		PartialList::const_iterator it;
		for ( it = start; it != end; ++it ) {
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
		if ( it == end ) {
			//	compute the original Breakpoint energy:
			double etot = pIter->amplitude() * pIter->amplitude();
			double ebw = etot * pIter->bandwidth();
			
			//	add some bandwith energy:
			etot += xse;
			ebw += xse;
			double bw;
			if ( etot > 0. ) 
				bw = ebw / etot;
			else 
				bw = 0.;
			
			//	create and insert the new Breakpoint:
			Breakpoint newBp( pIter->frequency(), std::sqrt( etot ), bw, pIter->phase() );
			dest.insert( pIter.time(), newBp );
			
			//	if there is a gap after this Breakpoint, 
			//	fade out:
			if ( pIter == src.end() && 
				 gapAt( pIter.time() + FADE_TIME, start, end ) ) {
				Breakpoint zeroPt( pIter->frequency(), 0., pIter->bandwidth(), 
								   src.phaseAt( pIter.time() + FADE_TIME ) );
				dest.insert( pIter.time() + FADE_TIME, zeroPt );
			}
			
			//	if there is a gap before this Breakpoint,
			//	fade in, but don't insert Breakpoints at
			//	times before zero:
			if ( pIter == src.begin() && 
				 pIter.time() > FADE_TIME && 
				 gapAt( pIter.time() - FADE_TIME, start, end ) ) {
				Breakpoint zeroPt( pIter->frequency(), 0., pIter->bandwidth(), 
								   src.phaseAt( pIter.time() - FADE_TIME ) );
				dest.insert( pIter.time() - FADE_TIME, zeroPt );
			}
		}	//	end if all other Partials are quieter at time of pIter
	}	//	end iteration over source Partial
}

// ---------------------------------------------------------------------------
//	gapAt
// ---------------------------------------------------------------------------
//	Return true if none of the Partials in l has any energy at time, 
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


#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

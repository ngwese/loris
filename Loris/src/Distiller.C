// ===========================================================================
//	Distiller.C
//	
//	A group of Partials that logically represent a single component
//	can be distilled into a single Partial using a Distiller. 
//
//	-kel 20 Oct 99
//
// ===========================================================================

#include "LorisLib.h"
#include "Distiller.h"
#include "Partial.h"
#include "PartialIterator.h"
#include "Breakpoint.h"

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
#else
	#include <math.h>
#endif

#include <algorithm>

using namespace std;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	distill
// ---------------------------------------------------------------------------
//	Distill the Partials in all in a range into a single Partial, 
//	add the result to the collection of distilled Partials.
//
//	(start, end) _must_ be a valid range in a list< Partial >...or else!
//
const Partial & 
Distiller::distill( const list<Partial>::const_iterator & start,
				 	const list<Partial>::const_iterator & end, 
				 	int assignLabel /* default = 0 */ )
{
	//	create the resulting distilled partial:
	Partial newp;
	newp.setLabel( assignLabel );
	
	//	iterate over range:
	for ( list<Partial>::const_iterator it = start; it != end; ++it ) {
		distillOne( *it, newp, start, end );
	}
	
	//	add the newly-distilled partial to the collection:
	_partials.push_back( newp );
	return _partials.back();
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
Distiller::distillOne( const Partial & src, Partial & dest, 
					   const list<Partial>::const_iterator & start,
					   const list<Partial>::const_iterator & end  )
{
	//	iterate over the source Partial:
	for ( iterator()->reset( src ); ! iterator()->atEnd(); iterator()->advance() ) { 
		//	iterate over all Partials in the range and compute the
		//	bandwidth energy contribution at the time of bp
		//	due to all the other Partials:
		double xse = 0.;
		list<Partial>::const_iterator it;
		for ( it = start; it != end; ++it ) {
			//	skip the source Partial:
			//	(identity test: compare addresses)
			if ( &(*start) == &src )
				continue;

			//	accumulate energy to be added as bandwidth:
			//	(this should use the same algorithm as the energy
			//	redistribution in the analysis)
			//	If this Partial is louder than is represented
			//	by bp at the time of bp, then break out of
			//	this loop, this Breakpoint will not be part of 
			//	the distilled Partial.
			double a = (*start).amplitudeAt( iterator()->time() );
			if ( a > iterator()->amplitude() ) {
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
			double etot = iterator()->amplitude() * iterator()->amplitude();
			double ebw = etot * iterator()->bandwidth();
			
			//	add some bandwith energy:
			etot += xse;
			ebw += xse;
			
			//	create and insert the new Breakpoint:
			Breakpoint newBp( iterator()->frequency(), sqrt( etot ), ebw / etot, iterator()->phase() );
			dest.insert( iterator()->time(), newBp );
			
			//	if there is a gap after this Breakpoint, 
			//	fade out:
			if ( iterator()->time() == iterator()->endTime() && 
				 gapAt( iterator()->time() + _fadeTime, start, end ) ) {
				Breakpoint zeroPt( iterator()->frequency(), 0., iterator()->bandwidth(), 
								   src.phaseAt( iterator()->time() + _fadeTime ) );
				dest.insert( iterator()->time() + _fadeTime, zeroPt );
			}
			
			//	if there is a gap before this Breakpoint,
			//	fade in, but don't insert Breakpoints at
			//	times before zero:
			if ( iterator()->time() == iterator()->startTime() && 
				 iterator()->time() > 0.001 && 
				 gapAt( iterator()->time() - _fadeTime, start, end ) ) {
				Breakpoint zeroPt( iterator()->frequency(), 0., iterator()->bandwidth(), 
								   src.phaseAt( iterator()->time() - _fadeTime ) );
				dest.insert( iterator()->time() - _fadeTime, zeroPt );
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
boolean 
Distiller::gapAt( double time, list<Partial>::const_iterator start,
				  list<Partial>::const_iterator end ) const
{
	while ( start != end ) {
		if ( (*start).amplitudeAt( time ) > 0. ) {
			return false;
		}
		++start;
	}
	return true;
}


End_Namespace( Loris )

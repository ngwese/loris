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
//	Distiller constructor
// ---------------------------------------------------------------------------
//
Distiller::Distiller( double x = 0.001 ) : 
	_fadeTime( x ) 
{
}

// ---------------------------------------------------------------------------
//	Distiller copy constructor
// ---------------------------------------------------------------------------
//
Distiller::Distiller( const Distiller & other ) : 
	_fadeTime( other._fadeTime ),
	PartialCollector( other )
{
}

// ---------------------------------------------------------------------------
//	Distiller assignment operator 
// ---------------------------------------------------------------------------
//
Distiller &
Distiller::operator= ( const Distiller & other )
{
	if ( &other != this ) {
		//
		//	remove LowMemException
		//
		//try {
			//	first try duplicating the list of 
			//	Partials in the base class:
			_partials = other._partials;
			_fadeTime = other._fadeTime;
		//}
		//catch( LowMemException & ex ) {
		//	ex.append( "failed to assign Distiller" );
		//	throw;
		//}
	}
	return *this;
}

// ---------------------------------------------------------------------------
//	distill
// ---------------------------------------------------------------------------
//	Distill the Partials in all in a range into a single Partial, 
//	add the result to the collection of distilled Partials.
//
//	(start, end) _must_ be a valid range in a list< Partial >...or else!
//
const Partial & 
Distiller::distill( const list<Partial>::const_iterator start,
				 	const list<Partial>::const_iterator end, 
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
					   const list<Partial>::const_iterator start,
					   const list<Partial>::const_iterator end  )
{
	//	iterate over the source Partial:
	for ( BasicPartialIterator pIter( src ); ! pIter.atEnd(); pIter.advance() ) { 
		//	iterate over all Partials in the range and compute the
		//	bandwidth energy contribution at the time of bp
		//	due to all the other Partials:
		double xse = 0.;
		list<Partial>::const_iterator it;
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
			if ( a > pIter.amplitude() ) {
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
			double etot = pIter.amplitude() * pIter.amplitude();
			double ebw = etot * pIter.bandwidth();
			
			//	add some bandwith energy:
			etot += xse;
			ebw += xse;
			
			//	create and insert the new Breakpoint:
			Breakpoint newBp( pIter.frequency(), sqrt( etot ), ebw / etot, pIter.phase() );
			dest.insert( pIter.time(), newBp );
			
			//	if there is a gap after this Breakpoint, 
			//	fade out:
			if ( pIter.time() == pIter.endTime() && 
				 gapAt( pIter.time() + _fadeTime, start, end ) ) {
				Breakpoint zeroPt( pIter.frequency(), 0., pIter.bandwidth(), 
								   src.phaseAt( pIter.time() + _fadeTime ) );
				dest.insert( pIter.time() + _fadeTime, zeroPt );
			}
			
			//	if there is a gap before this Breakpoint,
			//	fade in, but don't insert Breakpoints at
			//	times before zero:
			if ( pIter.time() == pIter.startTime() && 
				 pIter.time() > _fadeTime && 
				 gapAt( pIter.time() - _fadeTime, start, end ) ) {
				Breakpoint zeroPt( pIter.frequency(), 0., pIter.bandwidth(), 
								   src.phaseAt( pIter.time() - _fadeTime ) );
				dest.insert( pIter.time() - _fadeTime, zeroPt );
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
Distiller::gapAt( double time, 
				  list<Partial>::const_iterator start,
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

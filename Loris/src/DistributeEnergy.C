/*
 * Copyright (c) 1999-2000 Kelly Fitz and Lippold Haken
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
 * DistributeEnergy.C
 *
 * Implementation of Loris function object DistributeEnergy.
 *
 * Kelly Fitz, 23 Jan 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "DistributeEnergy.h"
#include "Partial.h"
#include "Breakpoint.h"
#include <algorithm>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	constructor
// ---------------------------------------------------------------------------
//
DistributeEnergy::DistributeEnergy( double distanceHz ) : 
	_maxdist( distanceHz ) 
{
}

// ---------------------------------------------------------------------------
//	destructor
// ---------------------------------------------------------------------------
//
DistributeEnergy::~DistributeEnergy( void )
{
}

// ---------------------------------------------------------------------------
//	computeAlpha
// ---------------------------------------------------------------------------
//
//	Return the relative proximity of x to below and above, 0 if
//	x == below, 1 if x == above, etc.
//
//	Could do lots of checking, but since computeAlpha is called only 
//	by another member, just assume that below <= x <= above, and
//	go for efficiency.
//
//	static local function
//
static double computeAlpha( double x, double below, double above ) 
{
	return ( x - below ) / ( above - below );
/*
	//	lots of checking:
	if ( above < below ) {
		swap( above, below );
	}
	
	if ( above <  x ) {
		return 1.;
	}
	else if ( below > x ) {
		return 0.;
	}
	else {
		return ( x - below ) / ( above - below );
	}
*/
}

// ---------------------------------------------------------------------------
//	addNoiseEnergy
// ---------------------------------------------------------------------------
//	Distribute energy to p as bandwidth over the time (lower,upper).
//
//	static local function
//
static void addNoiseEnergy( double energy, Partial & p, 
							double lower, double upper ) 
{
	if ( lower > upper ) {
		std::swap( lower, upper );
	}
	
	//	loop over Breakpoints in p in the range (lower,upper):
	for ( PartialIterator it = p.findPos( lower ); 
		  it != p.end() && it.time() < upper;
		  ++it ) 
	{
		//	don't transfer noise energy to 
		//	zero-amplitude breakpoints, 
		//	sounds bad:
		if ( it->amplitude() > 0. )
			it->addNoise( energy );
	}
}

// ---------------------------------------------------------------------------
//	addNoiseEnergy
// ---------------------------------------------------------------------------
//
void 
DistributeEnergy::distribute( const Partial & p, 
							  PartialList::iterator begin, 
							  PartialList::iterator end ) const
{
	//	loop over Breakpoints in p:
	PartialConstIterator envIter;
	double tUpperBound = p.startTime();	// initialize:
	for ( envIter = p.begin(); envIter != p.end(); ++envIter ) {
		double time = envIter.time();
		const Breakpoint & bp = * envIter;
		
		//	find nearest Partial in (begin,end) above
		//	and below (in frequency) to bp at time:
		PartialList::iterator above = end, below = end;
		double freqAbove = 0., freqBelow = 0.;
		for ( PartialList::iterator it = begin; it != end; ++it ) {
			//	cannot distribute energy to a Partial
			//	that does not exist at time:
			if ( it->startTime() > time || it->endTime() < time ) {
				continue;
			}
			
			//	could also check to make sure that p isn't in
			//	(begin, end), so as not to distribute p's
			//	energy to itself:
			if ( &(*it) == &p ) {
				continue;
			}

			//	compare to other candidates:
			double f = it->frequencyAt( time );
			if ( f < bp.frequency() ) {
				//	candidate below:
				if ( below == end || f > freqBelow ) {
					//	better than previous candidate below:
					below = it;
					freqBelow = f;
				}
			}
			else {
				//	candidate above:
				if ( above == end || f < freqAbove ) {
					//	better than previous candidate above:
					above = it;
					freqAbove = f;
				}
			}
		}	//	end of loop over partials in (begin,end)
		
		//	compute time bounds for energy distribution:
		//	(halfway between this breakpoint and its neighbors)
		double tLowerBound = tUpperBound;
		PartialConstIterator next( envIter );
		if ( ++next != p.end() ) {
			tUpperBound = ( next.time() + time ) * 0.5;
		}
		else {
			tUpperBound = time;
		}
		
		//	make sure the candidates aren't too far
		if ( freqAbove - bp.frequency() > _maxdist ) {
			above = end;
		}
		if ( bp.frequency() - freqBelow > _maxdist ) {
			below = end;
		} 
		
		
		//	four cases: either, neither, or both could
		//	be end (i.e. no candidate exists):
		double e = bp.amplitude() * bp.amplitude();
		if ( above != end && below != end ) {
			double alpha = computeAlpha( bp.frequency(), freqBelow, freqAbove );
			addNoiseEnergy( e * (1. - alpha), *below, tLowerBound, tUpperBound );
			addNoiseEnergy( e * alpha, *above, tLowerBound, tUpperBound );
		}
		else if ( above != end ) {
			addNoiseEnergy( e, *above, tLowerBound, tUpperBound );
		}
		else if ( below != end ) {
			addNoiseEnergy( e, *below, tLowerBound, tUpperBound );
		}
		//	else no Partials in (begin,end) are eligible

	}	//	end loop over partial envelope
};


#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

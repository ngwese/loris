// ===========================================================================
//	DistributeEnergy.C
//	
//	Implementation of non-template mambers of Loris function object 
//	DistributeEnergy.
//
//	-kel 23 Jan 2000
//
// ===========================================================================
#include "LorisLib.h"
#include "DistributeEnergy.h"
#include "Partial.h"
#include "Breakpoint.h"
#include <algorithm>

Begin_Namespace( Loris )

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
double 
DistributeEnergy::computeAlpha( double x, double below, double above ) const
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
void
DistributeEnergy::addNoiseEnergy( double energy, Partial & p, 
								  double lower, double upper ) const
{
	if ( lower > upper ) {
		std::swap( lower, upper );
	}
	
	//	loop over Breakpoints in p in the range (lower,upper):
	for ( Partial::iterator it = p.findPos( lower ); 
		  it != p.end() && it->first < upper;
		  ++it ) {
		//	don't transfer noise energy to 
		//	zero-amplitude breakpoints, 
		//	sounds bad?
		if ( it->second.amplitude() > 0. )
			it->second.addNoise( energy );
	}
}

End_Namespace( Loris )

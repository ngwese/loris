// ===========================================================================
//	AssociateBandwidth.C
//	
//	Implementation of Loris analysis strategy class AssociateBandwidth.
//
//	-kel 20 Jan 2000
//
// ===========================================================================
#include "AssociateBandwidth.h"
#include "Exception.h"
#include "Notifier.h"
#include "bark.h"
#include <algorithm>

using namespace std;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	AssociateBandwidth constructor
// ---------------------------------------------------------------------------
//
AssociateBandwidth::AssociateBandwidth( void ) :
	_spectralEnergy( 24 ),
	_sinusoidalEnergy( 24 ),
	_weights( 24 ),
	_windowFactor( 1. )
{
}	

// ---------------------------------------------------------------------------
//	AssociateBandwidth destructor
// ---------------------------------------------------------------------------
//
AssociateBandwidth::~AssociateBandwidth( void )
{
}

// ---------------------------------------------------------------------------
//	computeSurplusEnergy
// ---------------------------------------------------------------------------
//	In each region, compute the difference between the loudness (cube root 
//	of energy) due to spectral energy and the loudness due to sinusoidal 
//	energy, and from that difference compute the excess spectral energy 
//	in each region. 
//
//	This is the only place we use loudness in this algorithm, probably 
//	doesn't matter at all...
//
void
AssociateBandwidth::computeSurplusEnergy( vector<double> & surplus )
{
	for ( int i = 0; i < surplus.size(); ++i ) {
		//	spectral energy has to be scaled to account
		//	for windowing and transform length:
		double spec = pow( _spectralEnergy[i], 1. / 3. );
		double sin = pow( _sinusoidalEnergy[i], 1. / 3. );
		
		//	excess cannot be negative:
		double diff = max( 0., spec - sin );
		/*
		double diff = spec - sin;
		if ( diff < 0. ) {
			debugger << "clamping surplus energy at zero in region " << i << endl;
			diff = 0.;
		}
		else {
			debugger << "surplus energy " << diff << " in region " << i << endl;
		}
		*/
		surplus[i] = diff * diff * diff;	//	cheaper than pow()
	}
}

// ---------------------------------------------------------------------------
//	computeNoiseEnergy
// ---------------------------------------------------------------------------
//	Return the noise energy to be associated with a component at freqHz.
//	surplus contains the surplus spectral energy in each region, which is,
//	by defintion, non-negative.
//
double 
AssociateBandwidth::computeNoiseEnergy( double freqHz, 
										const vector<double> & surplus )
{
	double barks = bark( freqHz );
	
	//	contribute x to two regions having center
	//	frequencies less and greater than freqHz:
	int posBelow = findRegionBelow( barks );
	int posAbove = posBelow + 1;

	double alpha = computeAlpha( barks );

	double noise = 0.;
	if ( posAbove < surplus.size() )					
		noise += surplus[posAbove] * alpha / _weights[posAbove];
	
	if ( posBelow >= 0 )
		noise += surplus[posBelow] * (1. - alpha) / _weights[posBelow];
				
	return noise;
}

// ---------------------------------------------------------------------------
//	distribute
// ---------------------------------------------------------------------------
//
void 
AssociateBandwidth::distribute( double freqHz, double x, vector<double> & regions )
{
	double barks = bark( freqHz );
	
	//	contribute x to two regions having center
	//	frequencies less and greater than freqHz:
	int posBelow = findRegionBelow( barks );
	int posAbove = posBelow + 1;
	
	double alpha = computeAlpha( barks );
	
	if ( posAbove < regions.size() )					
		regions[posAbove] += alpha * x;
	
	if ( posBelow >= 0 )
		regions[posBelow] += (1. - alpha) * x;
}

// ---------------------------------------------------------------------------
//	findRegionBelow
// ---------------------------------------------------------------------------
//	Return the index of the last region having center frequency less than
//	or equal to barks, or -1 if no region is low enough. 
//
//	This will be different when regions are not all centered on 
//	every integer bark frequency.
//
int 
AssociateBandwidth::findRegionBelow( double barks )
{
	//	sanity check:
	Assert( barks >= 0. );
	
	//	the lowest region is centered at 1 bark:
	if ( barks < 1. ) {
		return -1;
	}
	else {
		return min( (int)floor( barks ) - 1, (int)_spectralEnergy.size() - 1 );
	}
}

// ---------------------------------------------------------------------------
//	computeAlpha
// ---------------------------------------------------------------------------
//	Return the relative contribution of a component at barks to the
//	surrounding regions. Return 0. if barks is equal to a region frequency,
//	0.5 if it is halfway between center frequencies, almost 1 if it is
//	almost equal to the next higher center frequency.
//
//	In other words, as used, return 0 if barks is equal to the center
//	frequency of the region indexed by the return value of findRegionBelow(),
//	1 if it is equal to 1 more than that.
//
//	What about boundaries?
//	barks should be non-negative, so that boundary just tapers off to zero,
//	which is good, because we don't want DC to show up as noise.
//	If barks is greater than the center freqeuency of the highest region,
//	then we probably want to lump it in with the highest region, which 
//	is the last region centered at or below barks, so return 0.
//	
//
double
AssociateBandwidth::computeAlpha( double barks )
{
	//	sanity check:
	Assert( barks >= 0. );
	
	const double maxfreq = _spectralEnergy.size();
	if ( barks > maxfreq ) {
		return 0.;
	}
	else {
		return barks - floor(barks);
	}
}

// ---------------------------------------------------------------------------
//	reset
// ---------------------------------------------------------------------------
//	This is called after each distribution of bandwidth energy.
//
void
AssociateBandwidth::reset( void )
{
	fill( _spectralEnergy.begin(), _spectralEnergy.end(), 0. );
	fill( _sinusoidalEnergy.begin(), _sinusoidalEnergy.end(), 0. );
	fill( _weights.begin(), _weights.end(), 0. );
}

End_Namespace( Loris )


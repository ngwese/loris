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
#include "notifier.h"
#include "Breakpoint.h"
#include "bark.h"
#include <algorithm>
#include <cmath>

using namespace Loris;

// ---------------------------------------------------------------------------
//	AssociateBandwidth constructor
// ---------------------------------------------------------------------------
//	Association regions are centered on all integer bin frequencies, regionWidth
//	is the total width (in Hz) of the overlapping bandwidth association regions, 
//	the region centers are spaced at half this width.
//
AssociateBandwidth::AssociateBandwidth( double regionWidth, double srate ) :
	_weights( int(srate/regionWidth) ),
	_surplus( int(srate/regionWidth) ),
	_regionRate( 2./regionWidth )
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
//	computeNoiseEnergy
// ---------------------------------------------------------------------------
//	Return the noise energy to be associated with a component at freqHz.
//	_surplus contains the surplus spectral energy in each region, which is,
//	by defintion, non-negative.
//
double 
AssociateBandwidth::computeNoiseEnergy( double freq, double amp )
{
	//	don't mess with negative frequencies:
	if ( freq < 0. )
		return 0.;
	
	//	compute the fractional bin frequency 
	//	corresponding to freqHz:
	double bin = binFrequency( freq );
	
	//	contribute x to two regions having center
	//	frequencies less and greater than freqHz:
	int posBelow = findRegionBelow( bin );
	int posAbove = posBelow + 1;

	double alpha = computeAlpha( bin );

	double noise = 0.;
	//	Have to check for alpha == 0, because 
	//	the weights will be zero (see computeAlpha()):
	//	(ignore lowest regions)
	const int LowestRegion = 2;
	/*
	if ( posAbove < _surplus.size() && alpha != 0. && posAbove >= LowestRegion )
		noise += _surplus[posAbove] * alpha / _weights[posAbove];
	
	if ( posBelow >= LowestRegion )
		noise += _surplus[posBelow] * (1. - alpha) / _weights[posBelow];
	*/
	//	new idea, weight Partials by amplitude:	
	if ( posAbove < _surplus.size() && alpha != 0. && posAbove >= LowestRegion )
		noise += _surplus[posAbove] * alpha * amp / _weights[posAbove];
	
	if ( posBelow >= LowestRegion )
		noise += _surplus[posBelow] * (1. - alpha) * amp / _weights[posBelow];
		
	return noise;
}

// ---------------------------------------------------------------------------
//	distribute
// ---------------------------------------------------------------------------
//
void 
AssociateBandwidth::distribute( double freqHz, double x, std::vector<double> & regions )
{
	//	don't mess with negative frequencies:
	if ( freqHz < 0. )
		return;
	
	//	compute the warped, fractional bin frequency
	//	corresponding to freqHz:	
	double bin = binFrequency( freqHz );
	
	//	contribute x to two regions having center
	//	frequencies less and greater than freqHz:
	int posBelow = findRegionBelow( bin );
	int posAbove = posBelow + 1;
	
	double alpha = computeAlpha( bin );
	
	if ( posAbove < regions.size() )
		regions[posAbove] += alpha * x;
	
	if ( posBelow >= 0 )
		regions[posBelow] += (1. - alpha) * x;
}

// ---------------------------------------------------------------------------
//	findRegionBelow
// ---------------------------------------------------------------------------
//	Return the index of the last region having center frequency less than
//	or equal to freq, or -1 if no region is low enough. 
//
//	Note: the zeroeth region is centered at bin frequency 1 and tapers
//	to zero at bin frequency 0! (when booger is 1.)
//
int 
AssociateBandwidth::findRegionBelow( double binfreq )
{
	const double booger = 0.;
	if ( binfreq < booger ) {
		return -1;
	}
	else {
		return std::min( std::floor(binfreq - booger), _surplus.size() - 1. );
	}
}

// ---------------------------------------------------------------------------
//	computeAlpha
// ---------------------------------------------------------------------------
//	binfreq is a warped, fractional bin frequency, and bin frequencies 
//	are integers. Return the relative contribution of a component at
//	binfreq to the bins (bw association regions) below and above
//	binfreq. 
//
double
AssociateBandwidth::computeAlpha( double binfreq )
{
	//	everything above the center of the highest
	//	bin is lumped into that bin; i.e it does
	//	not taper off at higher frequencies:	
	if ( binfreq > _surplus.size() ) {
		return 0.;
	}
	else {
		return binfreq - std::floor( binfreq );
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
	std::fill( _weights.begin(), _weights.end(), 0. );
	std::fill( _surplus.begin(), _surplus.end(), 0. );
}

// ---------------------------------------------------------------------------
//	accumulateSinusoid
// ---------------------------------------------------------------------------
//	Accumulate sinusoidal energy at frequency f and amplitude a.
//	The amplitude isn't used for anything.
//	
void
AssociateBandwidth::accumulateSinusoid( double freq, double amp )
{
	//	don't mess with negative frequencies:
	if ( freq < 0. )
		return;

	//	distribute weight at the peak frequency:
	//distribute( freq, 1., _weights );\
	//	new idea: weight Partials by amplitude:
	distribute( freq, amp, _weights );
}

// ---------------------------------------------------------------------------
//	accumulateNoise
// ---------------------------------------------------------------------------
//	Accumulate a rejected spectral peak as surplus (noise) energy.
//
void
AssociateBandwidth::accumulateNoise( double freq, double amp )
{
	//	compute energy contribution and distribute 
	//	at frequency f, don't mess with negative 
	//	frequencies:
	if ( freq > 0. )
		distribute( freq, amp * amp, _surplus  );
}

// ---------------------------------------------------------------------------
//	associate
// ---------------------------------------------------------------------------
//	Associate bandwidth with a single Breakpoint.
//	Both strategies call this.
//
void 
AssociateBandwidth::associate( Breakpoint & bp )
{		
	bp.addNoise( computeNoiseEnergy( bp.frequency(), bp.amplitude() ) );
}



// ---------------------------------------------------------------------------
//	binFrequency
// ---------------------------------------------------------------------------
//	Compute the warped fractional bin/region frequency corresponding to 
//	freqHz. (_regionRate is the number of regions per hertz.)
//
//	Once, we used bark frequency scale warping here, but there seems to be
//	no reason to do so. The best results seem to be indistinguishable from
// 	plain 'ol 1k bins, and some results are much worse.
//	
inline double
AssociateBandwidth::binFrequency( double freqHz )
{
//#define Use_Barks
#ifndef Use_Barks
	return freqHz * _regionRate;
#else
	//	ignore region rate when using barks
	return bark(freqHz);
#endif
	
}


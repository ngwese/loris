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
#include "FourierTransform.h"
#include "ReassignedSpectrum.h"
#include <algorithm>

//	would you believe all this for a round() function?
#if !defined( __GNUC__ )
	#include <cmath>
	using std::round;
#else
	#define round(x) ((x)>0.)?(trunc((x)+0.5)):(trunc((x)-0.5))
#endif


using namespace std;
using namespace Loris;

// ---------------------------------------------------------------------------
//	AssociateBandwidth constructor
// ---------------------------------------------------------------------------
//	Association regions are centered on all integer bin frequencies, there 
//	are numBins of them, starting at bin frequency 1. The lowest frequency
//	region is always ignored, its surplus energy is set to zero. regionWidth
//	is the total width (in Hz) of the overlapping bandwidth association regions, 
//	the region centers are spaced at half this width.
//
AssociateBandwidth::AssociateBandwidth( const ReassignedSpectrum & spec, 
										double srate,
										double regionWidth,
										double crop ) :
	_spectrum( spec ),
	_spectralEnergy( int(srate/regionWidth) ),
	_sinusoidalEnergy( int(srate/regionWidth) ),
	_weights( int(srate/regionWidth) ),
	_surplus( int(srate/regionWidth) ),
	_regionRate( 2./regionWidth ),
	_hzPerSamp( srate / spec.size() ),
	_cropSamps( crop * srate )
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
//
void
AssociateBandwidth::computeSurplusEnergy( void )
{
	//	the lowest-frequency region (0) is always ignored, 
	//	DC should never show up as noise: 
	for ( int i = 1; i < _surplus.size(); ++i ) {
		_surplus[i] = 
			max(0., _spectralEnergy[i]-_sinusoidalEnergy[i]) * _spectrum.energyScale();
	}
}

// ---------------------------------------------------------------------------
//	computeNoiseEnergy
// ---------------------------------------------------------------------------
//	Return the noise energy to be associated with a component at freqHz.
//	_surplus contains the surplus spectral energy in each region, which is,
//	by defintion, non-negative.
//
double 
AssociateBandwidth::computeNoiseEnergy( double freqHz )
{
	//	don't mess with negative frequencies:
	if ( freqHz < 0. )
		return 0.;
	
	//	compute the fractional bin frequency 
	//	corresponding to freqHz:
	double bin = binFrequency( freqHz );
	
	//	contribute x to two regions having center
	//	frequencies less and greater than freqHz:
	int posBelow = findRegionBelow( bin );
	int posAbove = posBelow + 1;

	double alpha = computeAlpha( bin );

	double noise = 0.;
	//	have to check for alpha == 0, because 
	//	the weights will be zero:
	if ( posAbove < _surplus.size() && alpha > 0. )
		noise += _surplus[posAbove] * alpha / _weights[posAbove];
	
	if ( posBelow >= 0 )
		noise += _surplus[posBelow] * (1. - alpha) / _weights[posBelow];
				
	return noise;
}

// ---------------------------------------------------------------------------
//	distribute
// ---------------------------------------------------------------------------
//
void 
AssociateBandwidth::distribute( double freqHz, double x, vector<double> & regions )
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
int 
AssociateBandwidth::findRegionBelow( double binfreq )
{
	if ( binfreq < 1. ) {
		return -1;
	}
	else {
		return min( floor(binfreq - 1.), _spectralEnergy.size() - 1. );
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
	if ( binfreq > _spectralEnergy.size() ) {
		return 0.;
	}
	else {
		return binfreq - floor( binfreq );
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
	fill( _surplus.begin(), _surplus.end(), 0. );
}

// ---------------------------------------------------------------------------
//	accumulateSpectrum
// ---------------------------------------------------------------------------
//	Spectral energy accumulation. Spectral energy is scaled by the inverse of 
//	the energy scale, so that sinusoidal energy (in accumulateSinusoid) and 
//	surplus energy (in computeSurplusEnergy) need not be scaled.
//
void 
AssociateBandwidth::accumulateSpectrum( void )
{
	//	how we gonna taper off time-corrected components?
	const double start_taper = 0.5;
	const double end_taper = 1.;
	
	const int max_idx = _spectrum.size() / 2;
	for ( int i = 0; i < max_idx; ++i ) {
		//	taper is a number near zero for small time corrections,
		//	one for large correcitons:
		double tcratio = std::abs(_spectrum.timeCorrection(i)) / _cropSamps;
		double taper;
		if ( tcratio < start_taper )
			taper = 1.;
		else if ( tcratio < end_taper )
			taper = (end_taper - tcratio) / (end_taper - start_taper);
		else //	time correction is past end_taper
			taper = 0.;
			
		double m = std::abs( _spectrum[i] ) * _spectrum.magnitudeScale();
		m *= taper;
		
		distribute( i * _hzPerSamp, m * m, _spectralEnergy );
	}
}

// ---------------------------------------------------------------------------
//	accumulateSinusoid
// ---------------------------------------------------------------------------
//	Accumulate sinusoidal energy at frequency f and (nominal) amplitude a.
//	Now using more refined amplitude estimates obtained from ReassignedSpectrum
//	using samples of the main lobe of the analysis window spectrum.
//	
void
AssociateBandwidth::accumulateSinusoid( double f, double a )
{
	//	don't mess with negative frequencies:
	if ( f < 0. )
		return;

	//	distribute weight at the peak frequency:
	distribute( f, 1., _weights );
	
	//	compute energy contribution and distribute 
	//	at frequency f (scale to look like spectral
	//	energy):
	double esine = a * a / _spectrum.energyScale();
	distribute( f, esine, _sinusoidalEnergy  );
}

// ---------------------------------------------------------------------------
//	binFrequency
// ---------------------------------------------------------------------------
//	Compute the warped fractional bin/region frequency corresponding to 
//	freqHz. (_regionRate is the number of regions per hertz.)
//
//	Once, we used bark frequency scale warping here, but there seems to be
//	no reason to do so. The best results seem to be indistinguishable from
// 	plain 'ol 1k bins, and most results are much worse.
//	
inline double
AssociateBandwidth::binFrequency( double freqHz )
{
	return freqHz * _regionRate;
}


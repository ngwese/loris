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
#include "FourierTransform.h"
#include "ReassignedSpectrum.h"

using namespace std;
using namespace Loris;

// ---------------------------------------------------------------------------
//	AssociateBandwidth constructor
// ---------------------------------------------------------------------------
//	Association regions are centered on all integer bin frequencies, there 
//	are numBins of them, starting at bin frequency 1. The lowest frequency
//	region is always ignored, its surplus energy is set to zero. resolution
//	is the frequency separation of the region centers.
//
AssociateBandwidth::AssociateBandwidth( const ReassignedSpectrum & spec, 
										double srate,
										double resolution /* = 1000 */) :
	_spectrum( spec ),
	_spectralEnergy( 24 ), // int(0.5 * srate/resolution) ),
	_sinusoidalEnergy( 24 ), // int(0.5 * srate/resolution) ),
	_weights( 24 ), // int(0.5 * srate/resolution) ),
	_surplus( 24 ), // int(0.5 * srate/resolution) ),
	_regionRate( 1. / resolution ),
	_hzPerSamp( srate / spec.size() )
{
	computeWindowSpectrum( _spectrum.window() );
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
			max(0., _spectralEnergy[i]-_sinusoidalEnergy[i]) / _windowFactor;
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
//	computeWindowSpectrum
// ---------------------------------------------------------------------------
//	
static const long WinSpecOversample = 64;
void
AssociateBandwidth::computeWindowSpectrum( const vector< double > & v )
{
	FourierTransform ft( _spectrum.size() * WinSpecOversample );
	ft( v );
	
	double peakScale = 1. / abs( ft[0] );
	for( long i = 0; i < ft.size(); ++i ) {
		double x = peakScale * abs( ft[i] );
		if ( i > 0 && x > _winspec[i-1] ) {
			break;
		}
		_winspec.push_back( x );
	}

	//	compute the window scale:
	_windowFactor = _winspec[0] * _winspec[0];
	for ( long j = WinSpecOversample; j < _winspec.size(); j += WinSpecOversample ) {
		_windowFactor += 2. * _winspec[j] * _winspec[j];
	}
}

// ---------------------------------------------------------------------------
//	accumulateSpectrum
// ---------------------------------------------------------------------------
//	Spectral energy accumulation.
//
void 
AssociateBandwidth::accumulateSpectrum( void )
{
	const int max_idx = _spectrum.size() / 2;
	for ( int i = 0; i < max_idx; ++i ) {
		double m = _spectrum.magnitude(i);
		distribute( i * _hzPerSamp, m * m, _spectralEnergy );
	}
	
#ifdef Debug_Loris
	_specCopy = std::vector<double>( _spectrum.size(), 0. );
	_sinSpec = std::vector<double>( _spectrum.size(), 0. );
	for ( int j = 0; j < _spectrum.size(); ++j ) {
		_specCopy[j] = _spectrum.magnitude(j);
	}
	_residue = _specCopy;
#endif
}

// ---------------------------------------------------------------------------
//	accumulateSinusoid
// ---------------------------------------------------------------------------
//	Accumulate sinusoidal energy at frequency f and (nominal) amplitude a.
//	First need to compute a more accurate amplitude, and use that to 
//	scale the window spectrum when distributing energy.
//
//	3 Feb:
//	- new simpler offset really gives the smallest residual
//	- only need offset for amplitude correction, can use exact
//		window samples for distribution by distributing around
//		f instead of intBinNumber
//
//	Return corrected amp, for fun.
//	
double
AssociateBandwidth::accumulateSinusoid( double f, double a )
{
	//	don't mess with negative frequencies:
	if ( f < 0. )
		return a;

	//	distribute weight at the peak frequency:
	distribute( f, 1., _weights );
	
	//	compute the offset in the oversampled window spectrum:
	double fracBinNum = f / _hzPerSamp;
	long intBinNumber = round(fracBinNum);
	long offset = round( WinSpecOversample * (intBinNumber - fracBinNum) );
	
	//	compute the more accurate peak amplitude:
	double correctAmp = a / _winspec[abs(offset)];
	
	//	find the best rate to step through the 
	//	window spectrum:
	long step = WinSpecOversample;
	const long minStep = 1;
	double leastRes = -1.;
	const long Q = 4;
	for ( ; step > minStep; --step ) {
		//	compute the residue at this step:
		//	(use offset here because we are comparing
		//	with the actual spectral samples, and 
		//	the offset will give better measurements
		//	of the window spectrum at those samples)
		double specSamp = _spectrum.magnitude(intBinNumber);
		double res = (specSamp * specSamp) - (correctAmp * correctAmp);
		for ( int j = 1; (step * j) + abs(offset) < _winspec.size()/Q; ++j ) {
			//	j FT bins above:
			double z = correctAmp * _winspec[(step * j) + offset];
			specSamp = _spectrum.magnitude(intBinNumber + j);
			res += (specSamp * specSamp) - (z * z);
			
			//	j FT bins below, 
			//	don't index bins below 0:
			if ( intBinNumber >= j ) {
				z = correctAmp * _winspec[(step * j) - offset];
				specSamp = _spectrum.magnitude(intBinNumber - j);
				res += (specSamp * specSamp) - (z * z);
			}
		}	//	end for j
		
		//	res is now the energy residue for
		//	the current step, if it is worse than
		//	the previous one, use the previous step
		//	as the best one (assumes monotony):
		if ( leastRes > -1. && abs(res) > leastRes ) {
			++step;
			break;
		}
		//	otherwise, this is the smallest residue yet:
		leastRes = abs(res);
	}	//	end for step
	
	
	//	if we don't like a smaller step, maybe a bigger one would
	//	be nice:
	if ( step == WinSpecOversample ) {
		const long maxStep = 2 * WinSpecOversample;
		for ( ++step; step < maxStep; ++step ) {
			//	compute the residue at this step:
			//	(use offset here because we are comparing
			//	with the actual spectral samples, and 
			//	the offset will give better measurements
			//	of the window spectrum at those samples)
			double specSamp = _spectrum.magnitude(intBinNumber);
			double res = (specSamp * specSamp) - (correctAmp * correctAmp);
			for ( int j = 1; (step * j) + abs(offset) < _winspec.size()/Q; ++j ) {
				//	j FT bins above:
				double z = correctAmp * _winspec[(step * j) + offset];
				specSamp = _spectrum.magnitude(intBinNumber + j);
				res += (specSamp * specSamp) - (z * z);
				
				//	j FT bins below, 
				//	don't index bins below 0:
				if ( intBinNumber >= j ) {
					z = correctAmp * _winspec[(step * j) - offset];
					specSamp = _spectrum.magnitude(intBinNumber - j);
					res += (specSamp * specSamp) - (z * z);
				}
			}	//	end for j
			
			//	res is now the energy residue for
			//	the current step, if it is worse than
			//	the previous one, use the previous step
			//	as the best one (assumes monotony):
			if ( abs(res) > leastRes ) {
				--step;
				break;
			}
			//	otherwise, this is the smallest residue yet:
			leastRes = abs(res);
		}	//	end for step
	}	//	end if we didn't like a smaller step
	
	//	distribute the peak amplitude: 
	double z = correctAmp;
	#define YOYO
	#ifndef YOYO
	distribute( f, z * z, _sinusoidalEnergy  );
	#else
	double YO = z * z;
	#endif
	
#ifdef Debug_Loris
	double zz = correctAmp * _winspec[abs(offset)];
	_sinSpec[ intBinNumber ] += zz;
	_residue[ intBinNumber ] -= zz;
#endif

	//	distribute samples of the oversampled window spectrum:
	for ( int i = 1; (step * i) + abs(offset) < _winspec.size(); ++i ) {
		z = correctAmp * _winspec[step * i];
		#ifndef YOYO
		distribute( f + (i * _hzPerSamp), z * z, _sinusoidalEnergy  );
		#else
		YO += z * z;
		#endif
		
#ifdef Debug_Loris
		zz = correctAmp * _winspec[(step * i) + offset];
		_sinSpec[ intBinNumber + i ] += zz;
		_residue[ intBinNumber + i ] -= zz;
#endif
		
		//	don't index bins below 0:
		if ( intBinNumber >= i ) {
			z = correctAmp * _winspec[step * i];
			#ifndef YOYO
			distribute( f - (i * _hzPerSamp), z * z, _sinusoidalEnergy  );
			#else
			YO += z * z;
			#endif
			
#ifdef Debug_Loris
			zz = correctAmp * _winspec[(step * i) - offset];
			_sinSpec[ intBinNumber - i ] += zz;
			_residue[ intBinNumber - i ] -= zz;
#endif
		}
	}
	
	#ifdef YOYO
	distribute( f, YO, _sinusoidalEnergy  );
	#endif
	
	//	compute a better amplitude estimate 
	//	from the step and the window function:
	double ampRatio = 1. - (0.5 * (1. - ((double)step / WinSpecOversample)));
	return correctAmp / ampRatio;
}

// ---------------------------------------------------------------------------
//	binFrequency
// ---------------------------------------------------------------------------
//	Compute the warped fractional bin/region frequency corresponding to 
//	freqHz. (_regionRate is the number of regions per hertz.)
//
//	Once, we used bark frequency scale warping here, but there seems to be
//	no reason to do so. The best results seem to be indistinguishable from
// 	plain 'ol 1k bins.
//	
inline double
AssociateBandwidth::binFrequency( double freqHz )
{
	return freqHz * _regionRate;
	//return bark( freqHz );
}


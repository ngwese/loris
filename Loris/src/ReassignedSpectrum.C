/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
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
 * ReassignedSpectrum.C
 *
 * Implementation of class Loris::ReassignedSpectrum.
 *
 * Kelly Fitz, 9 Dec 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include <config.h>
#endif

#include<ReassignedSpectrum.h>
#include<Notifier.h>
#include<Exception.h>
#include <algorithm>
#include <cstdlib>	//	for std::abs()


#if HAVE_M_PI
	#include <cmath>	//	for M_PI (except on Mac)
	const double Pi = M_PI;
#else
	const double Pi = 3.14159265358979324;
#endif

#if defined(Debug_Loris)
#include <stdio.h>
#endif


#if defined(Debug_Loris)
#include <stdio.h>
#endif

//	there's a freakin' ton of std in here, 
//	just import the whole namespace
using namespace std;

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// 	static function for building fancy reassignment window:
template < typename RealWinIter, typename CplxWinIter >
static void 
buildReassignmentWindow( RealWinIter winbegin, RealWinIter winend, 
						 CplxWinIter rawinbegin, long transformSize );

// ---------------------------------------------------------------------------
//	ReassignedSpectrum constructor
// ---------------------------------------------------------------------------
//	Transform lengths are the smallest power of two greater than twice the
//	window length. The noise floor is specified in dB (negative).
//
ReassignedSpectrum::ReassignedSpectrum( const std::vector< double > & window ) :
	_transform( 1 << long( 1 + ceil( log(window.size()) / log(2.)) ) ),
	_ratransform( 1 << long( 1 + ceil( log(window.size()) / log(2.)) ) ),
	_window( window ),
	_rawindow( window.size(), 0. )
{
	buildReassignmentWindow( _window.begin(), _window.end(), _rawindow.begin(), _transform.size() );
	computeWindowSpectrum( _window );
}

// ---------------------------------------------------------------------------
//	ReassignedSpectrum destructor
// ---------------------------------------------------------------------------
//
ReassignedSpectrum::~ReassignedSpectrum( void )
{
}

// ---------------------------------------------------------------------------
//	transform
// ---------------------------------------------------------------------------
//	Compute the reassigned Fourier transform of the samples on the half open
//	range [sampsBegin, sampsEnd), aligning sampCenter with the center of
//	the analysis window.
//
//	Clients must ensure that the samples beyond the window boundaries are not 
//	requested. Note that it is not sufficient to check that sampsEnd - sampsBegin
//	is no greater than the window length, since sampCenter might not be in the
//	center of that range.
//
void
ReassignedSpectrum::transform( const double * sampsBegin, const double * sampCenter, const double * sampsEnd )
{
#ifdef Debug_Loris
	Assert( sampCenter >= sampsBegin );
	Assert( sampCenter < sampsEnd );
	const long firstHalfWinLength = window().size() / 2;
	const long secondHalfWinLength = (window().size() - 1) / 2;
	Assert( sampCenter - sampsBegin <= firstHalfWinLength );
	Assert( sampsEnd - sampCenter <= secondHalfWinLength + 1 );
#endif
		
	//	we will skip the beginning of the window
	//	only if pos is too close to the start of 
	//	the buffer:
	long winBeginOffset = 0; 
	if ( sampCenter - sampsBegin < (window().size() / 2) )
	{
		winBeginOffset = (window().size() / 2) - ( sampCenter - sampsBegin );
	}
			
		
	//	to get phase right, we will rotate the Fourier transform 
	//	input by pos - sampsBegin samples:
	long rotateBy = sampCenter - sampsBegin;
		
	//	window and rotate input and compute normal transform:
	//	window the samples into the FT buffer:
	FourierTransform::iterator it = 
		std::transform( sampsBegin, sampsEnd, _window.begin() + winBeginOffset, 
						_transform.begin(), std::multiplies<double>() );
	//	fill the rest with zeros:
	std::fill( it, _transform.end(), 0. );
	//	rotate to align phase:
	std::rotate( _transform.begin(), _transform.begin() + rotateBy, _transform.end() );

	//	compute transform:
	_transform.transform();

	//	compute the dual reassignment transform:
	//	window the samples into the reassignment FT buffer,
	//	using the complex-valued reassignment window:
	it = std::transform( sampsBegin, sampsEnd, _rawindow.begin() + winBeginOffset, 
						_ratransform.begin(), std::multiplies< std::complex<double> >() );
	
	//	fill the rest with zeros:
	std::fill( it, _ratransform.end(), 0. );
	//	rotate to align phase:
	std::rotate( _ratransform.begin(), _ratransform.begin() + rotateBy, _ratransform.end() );
	//	compute the transform:
	_ratransform.transform();
}

// ---------------------------------------------------------------------------
//	computeWindowSpectrum
// ---------------------------------------------------------------------------
//	Compute the spectrum of the (normal) analysis window, used to correct
//	component magnitudes.
//	
static const long OVERSAMPLE_WINDOW_SPECTRUM = 16;
void
ReassignedSpectrum::computeWindowSpectrum( const vector< double > & v )
{
	//	compute the appropriate scale factor
	//	to report correct component magnitudes:
	double winsum = 0;
	for ( int i = 0; i < _window.size(); ++i ) {
		winsum += _window[i];
	}
	_windowMagnitudeScale = 2. / winsum;
	
	debugger << "ReassignedSpectrum: length is " << _transform.size()
			 << " mag scale is " << magnitudeScale() << endl;

	//	now do the energy scale:
	debugger << "AssociateBandwidth oversampling window spectrum by " << OVERSAMPLE_WINDOW_SPECTRUM << endl;
	
	FourierTransform ft( size() * OVERSAMPLE_WINDOW_SPECTRUM );
	FourierTransform::iterator it = 
		std::copy( v.begin(), v.end(), ft.begin() );
	std::fill( it, ft.end(), 0. );
	ft.transform();	

	double peakScale = 1. / abs( ft[0] );
	for( long i = 0; i < ft.size(); ++i ) {
		double x = peakScale * abs( ft[i] );
		if ( i > 0 && x > _mainlobe[i-1] ) {
			break;
		}
		_mainlobe.push_back( x );
	}

	//	compute the window scale by summing the main
	//	lobe samples (but not oversampling):
	double sqrSum = _mainlobe[0] * _mainlobe[0];
	for ( long j = OVERSAMPLE_WINDOW_SPECTRUM; j < _mainlobe.size(); j += OVERSAMPLE_WINDOW_SPECTRUM ) {
		//	twice, because _mainlobe has only one side of
		//	the mainlobe, and all samples but the center
		//	(DC) sample are reflected on the other side:
		sqrSum += 2. * _mainlobe[j] * _mainlobe[j];
	}
	_windowEnergyScale = 1. / sqrSum;
}

// ---------------------------------------------------------------------------
//	frequencyCorrection
// ---------------------------------------------------------------------------
//	Equation 14 from the Trans ASP correspondence.
//	Correction is computed in fractional frequency samples, because
//	that's the kind of frequency domain ramp we used on our window.
//	sample is the frequency sample index, the nominal component 
//	frequency in samples. 
//
//	Parabolic interpolation can be tried too, but it appears to give
//	slightly worse results for the square wave.
//
double
ReassignedSpectrum::frequencyCorrection( long idx ) const
{
	long flip_idx;
	if (idx>0)
		flip_idx = _ratransform.size() - idx;
	else
		flip_idx = idx;
		
	//	the freq RA FT is the circular even part
	//	of the reassignment transform:
	std::complex<double> fra_part = 0.5*( _ratransform[idx] + std::conj( _ratransform[flip_idx] ) );
	
	double num = _transform[idx].real() * fra_part.imag() -
				 _transform[idx].imag() * fra_part.real();
	
	double magSquared = std::norm( _transform[idx] );

	return - num / magSquared;
}

// ---------------------------------------------------------------------------
//	timeCorrection
// ---------------------------------------------------------------------------
//	Equation ??? from the Trans ASP correspondence.
//	Correction is computed in fractional samples, because
//	that's the kind of ramp we used on our window.
//
//	sample is the frequency sample index, the nominal component 
//	frequency in samples. 
//
double
ReassignedSpectrum::timeCorrection( long idx ) const
{
	long flip_idx;
	if (idx>0)
		flip_idx = _ratransform.size() - idx;
	else
		flip_idx = idx;
		
	//	the time RA FT is the circular odd part 
	//	of the reassignment transform divided by j:
	/*
	const std::complex<double> minus_j(0,-1);
	std::complex<double> tra_part = minus_j * 0.5 * 
									( _ratransform[idx] - std::conj( _ratransform[flip_idx] ) );
	*/
	//	can compute this without complex multiplies:
	std::complex<double> tmp = _ratransform[idx] - std::conj( _ratransform[flip_idx] );
	std::complex<double> tra_part( 0.5*tmp.imag(), -0.5*tmp.real() );

	double num = _transform[idx].real() * tra_part.real() +
		  		 _transform[idx].imag() * tra_part.imag();
	double magSquared = norm( _transform[idx] );
	
	return num / magSquared;
}

// ---------------------------------------------------------------------------
//	reassignedFrequency
// ---------------------------------------------------------------------------
//	Return the reassigned, fractional frequency sample 
//	for the specified index.
//
double
ReassignedSpectrum::reassignedFrequency( unsigned long idx ) const
{
#if defined(Like_Lemur)
#define SMITHS_BRILLIANT_PARABOLAS
#endif

#if ! defined(SMITHS_BRILLIANT_PARABOLAS)
	return double(idx) + frequencyCorrection( idx );
	
#else // defined(SMITHS_BRILLIANT_PARABOLAS)
	double dbLeft = 20. * log10( abs( _transform[idx-1] ) );
	double dbCandidate = 20. * log10( abs( _transform[idx] ) );
	double dbRight = 20. * log10( abs( _transform[idx+1] ) );
	
	double peakXOffset = 0.5 * (dbLeft - dbRight) /
						 (dbLeft - 2.0 * dbCandidate + dbRight);

	return idx + peakXOffset;
	
#endif	//	defined SMITHS_BRILLIANT_PARABOLAS
}

// ---------------------------------------------------------------------------
//	reassignedTime
// ---------------------------------------------------------------------------
//	The nominal time is 0 (samples) since no other temporal information about 
//	the transformed buffer is available.
//
double
ReassignedSpectrum::reassignedTime( unsigned long idx ) const
{
	return timeCorrection( idx );
}

// ---------------------------------------------------------------------------
//	reassignedMagnitude
// ---------------------------------------------------------------------------
//	The magnitude is "reassigned" only in the sense that it is corrected to
//	account for the frequency reassignment used to obtain the fractional
//	bin frequency estimate, fracBinNum.
//
//	Two magnitude corrections are performed. First, the oversampled window
//	spectrum is used to estimate the amplitude based on the difference between
//	fracBinNum and the peak bin frequency, peakBinNumber. Next, the shape of the
//	spectrum in the vicinity of the peak is examined for signs of stretching
//	or squishing (which can be caused by non-stationary frequency components).
//	An add-hoc scale factor (found empirically by examining chirp analyses) is 
//	applied to arrive at the final amplitude estimate. Formerly, we allowed
//	main lobe squishing (narrowing, with an amplitude overestimation), but 
//	lacking any explanation for that kind of behavior, that part of the algorithm
// 	has been eliminated. 
//
//	peakBinNumber may not (often is not, except for very well-behaved sounds)
//	be the nearest integer bin number to fracBinNum, so it has to be passed 
//	in separately. Large frequency corrections may cause other problems too,
//	see below.
//	
double
ReassignedSpectrum::reassignedMagnitude( double fracBinNum, long peakBinNumber ) const
{
#if Debug_Loris
	//	sanity:
	//	we are all screwed up if peakBinNumber isn't a peak:
	Assert( abs(_transform[ peakBinNumber ]) > abs(_transform[ peakBinNumber+1 ]) &&
			abs(_transform[ peakBinNumber ]) > abs(_transform[ peakBinNumber-1 ]) );
#endif

	Assert( fracBinNum >= 0. );

#if defined(Like_Lemur)
#define SMITHS_INGENEOUS_PARABOLAS
#endif

#if ! defined(SMITHS_INGENEOUS_PARABOLAS)
	
	//	compute the nominal spectral amplitude by scaling
	//	the peak spectral sample:
	double a = magnitudeScale() * abs( _transform[ peakBinNumber ] );
	
	//	compute the offset in the oversampled window spectrum:
	//	(cheapo rounding)
	long offset = long((OVERSAMPLE_WINDOW_SPECTRUM * (peakBinNumber - fracBinNum)) + 0.5);
	
	//	if the offset is very large, corresponding to 
	//	a very large frequency correction (larger than,
	//	say, half the main lobe width), clamp the 
	//	amplitude rather than letting it get huge:
	if ( abs(offset) >= _mainlobe.size() * 0.5 )
	{
		return a / _mainlobe[_mainlobe.size() / 2];
	}

	//	compute a more accurate peak amplitude from
	//	samples of the window's main lobe:
	//	(main lobe spectrum has been normalized so
	//	that the zeroeth sample is 1.0)
	double correctAmp = a / _mainlobe[abs(offset)];
	
	//	estimate main lobe stretching by finding the step 
	//	rate that gives the best match (least residue) for 
	//	the main lobe, starting at the step corresponding 
	//	to no stretching and assuming monotonicity:
	//
	//	(this only makes sense when the offset (and frequency
	//	correction) are small)
	if ( abs(offset) > OVERSAMPLE_WINDOW_SPECTRUM )
	{
		return correctAmp;
	}
	
	long step = OVERSAMPLE_WINDOW_SPECTRUM;
	//	step must be at least 1:	
	const long minStep = 1;	
	//	only examine the middle of the main lobe:
	const long maxMainlobeIndex = _mainlobe.size() / 2;	
	
	//	initialize residue:
	double leastRes = -1.;
	for ( ; step > minStep; --step ) 
	{
		//	compute the residue at this step:
		//	(use offset here because we are comparing
		//	with the actual spectral samples, and 
		//	the offset will give better measurements
		//	of the window spectrum at those samples)
		double specSamp = magnitudeScale() * abs( _transform[ peakBinNumber ] );
		double res = (specSamp * specSamp) - (correctAmp * correctAmp);
		for ( int j = 1; (step * j) + abs(offset) < maxMainlobeIndex; ++j ) 
		{
			//	j FT bins above, 
			//	don't index bins above Nyquist:
			if ( peakBinNumber+j < size() / 2 ) 
			{
				double z = correctAmp * _mainlobe[(step * j) + offset];
				specSamp = magnitudeScale() * abs( _transform[ peakBinNumber+j ] );
				res += (specSamp * specSamp) - (z * z);
			}
			
			//	j FT bins below, 
			//	don't index bins below 0:
			if ( peakBinNumber >= j ) 
			{
				double z = correctAmp * _mainlobe[(step * j) - offset];
				specSamp = magnitudeScale() * abs( _transform[ peakBinNumber-j ] );
				res += (specSamp * specSamp) - (z * z);
			}
		}	//	end for j
		
		//	res is now the energy residue for
		//	the current step, if it is worse than
		//	the previous one, use the previous step
		//	as the best one (assumes monotony):
		if ( leastRes > -1. && abs(res) > leastRes ) 
		{
			++step;
			break;
		}
		//	otherwise, this is the smallest residue yet:
		leastRes = abs(res);
	}	//	end for step
	
	//	massage the amplitude as a (ad hoc) function
	//	of the ratio between the optimal step and the
	//	oversampling function (this was found to be 
	//	pretty good for chirps):
	double ampRatio = 1. - (0.5 * (1. - ((double)step / OVERSAMPLE_WINDOW_SPECTRUM)));
	correctAmp /= ampRatio;	
	return correctAmp;

#else // defined(SMITHS_INGENEOUS_PARABOLAS)
	
	//	keep this parabolic interpolation computation around
	//	only for sake of comparison, it is unlikely to yield
	//	good results with bandwidth association:
	double dbLeft = 20. * log10( abs( _transform[peakBinNumber-1] ) );
	double dbCandidate = 20. * log10( abs( _transform[peakBinNumber] ) );
	double dbRight = 20. * log10( abs( _transform[peakBinNumber+1] ) );
	
	double peakXOffset = 0.5 * (dbLeft - dbRight) /
						 (dbLeft - 2.0 * dbCandidate + dbRight);
	double dbmag = dbCandidate - 0.25 * (dbLeft - dbRight) * peakXOffset;
	double x = magnitudeScale() * pow( 10., 0.05 * dbmag );
	
	return x;
	
#endif	//	defined SMITHS_INGENEOUS_PARABOLAS
}

// ---------------------------------------------------------------------------
//	reassignedPhase
// ---------------------------------------------------------------------------
//	The reassigned phase is shifted to account for the time
//	correction (in fractional time samples) according to the 
//	corrected frequency (in fractional frequency samples).
//
double
ReassignedSpectrum::reassignedPhase( long idx,
									 double fracFreqSample, 
									 double timeCorrection ) const
{
	double phase = arg( _transform[ idx ] );
	phase += timeCorrection * fracFreqSample * 2. * Pi / _transform.size();
	
	return fmod( phase, 2. * Pi );
}


// ---------------------------------------------------------------------------
//	make_complex
// ---------------------------------------------------------------------------
//	Function object for building complex numbers.
//
template <class T>
struct make_complex
	: binary_function< T, T, std::complex<T> >
{
	std::complex<T> operator()(const T& re, const T& im) const;
};

template <class T>
inline
std::complex<T>
make_complex<T>::operator()(const T& re, const T& im) const
{
	return std::complex<T>( re, im );
}

// ---------------------------------------------------------------------------
//	applyFreqRamp
// ---------------------------------------------------------------------------
//	Adapted from the FrequencyReassignment constructor in Lemur 5.
//
static inline void applyFreqRamp( vector< double > & w, long transformSize  )
{
	//	we're going to do the frequency-domain ramp 
	//	by Fourier transforming the window, ramping,
	//	then transforming again. 
	//	Use a transform exactly as long as the window.
	//	load, w/out rotation, and transform.
	FourierTransform temp( w.size() );
	FourierTransform::iterator it = 
		std::copy( w.begin(), w.end(), temp.begin() );
	std::fill( it, temp.end(), 0. );
	temp.transform();
	
	//	extract complex transform and multiply by
	//	a frequency (sample) ramp:
	//	(the frequency ramp goes from 0 to N/2
	//	over the first half, then -N/2 to 0 over 
	//	the second (aliased) half of the transform,
	//	and has to be scaled by the ratio of the 
	//	transform lengths, so that k spans the length
	//	of the padded transforms, N)
	double lenRatio = (double)transformSize / temp.size();
	for ( int k = 0 ; k < temp.size(); ++k ) {
		if ( k < temp.size() / 2 ) {
			temp[ k ] *= k * lenRatio;
		}
		else {
			temp[ k ] *= (k - temp.size()) * lenRatio;
		}
	}
	
	//	invert the transform:
	temp.transform();
	
	//	the DFT of a DFT gives the scaled and INDEX REVERSED
	//	sequence. See p. 539 of O and S.
	//	DFT( X[n] ) -- DFT --> Nx[ -k mod N ] 
	//
	//	seems that I want the imaginary part of the index-reversed
	//	transform scaled by the size of the transform:
	std::reverse( temp.begin() + 1, temp.end() );
	for ( int i = 0; i < w.size(); ++i ) {
		w[i] = - imag( temp[i] ) / temp.size();
	}
}

// ---------------------------------------------------------------------------
//	applyTimeRamp
// ---------------------------------------------------------------------------
//	Make a copy of _window scaled by a ramp from -N/2 to N/2 for computing
//	time corrections in samples.
//
static inline void applyTimeRamp( vector< double > & w )
{
	//	the very center of the window should be scaled by 0.,
	//	need a fractional value for even-length windows, a
	//	whole number for odd-length windows:
	double offset = 0.5 * ( w.size() - 1 );
	for ( int k = 0 ; k < w.size(); ++k ) {
		w[ k ] *= ( k - offset );
	}
}

// ---------------------------------------------------------------------------
//	buildReassignmentWindow
// ---------------------------------------------------------------------------
//	Build a complex-valued window with the frequency-ramp window in
//	the real part and the time-ramp window in the imagnary part.
//
template < typename RealWinIter, typename CplxWinIter >
static void 
buildReassignmentWindow( RealWinIter winbegin, RealWinIter winend, 
						 CplxWinIter rawinbegin, long transformSize )
{
	std::vector< double > tramp( winbegin, winend );
	applyTimeRamp( tramp );
	
	std::vector< double > framp( winbegin, winend );
	applyFreqRamp( framp, transformSize );
	
	std::transform( framp.begin(), framp.end(), tramp.begin(),
					rawinbegin, make_complex< double >() );	
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

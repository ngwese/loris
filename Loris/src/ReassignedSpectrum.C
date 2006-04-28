/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2004 by Kelly Fitz and Lippold Haken
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
	#include "config.h"
#endif

#include "ReassignedSpectrum.h"
#include "Notifier.h"
#include "Exception.h"
#include <algorithm>	//	for std::transform(), others
#include <functional>	//	for bind1st, multiplies, etc.
#include <cstdlib>	    //	for std::abs()
#include <numeric>	    //	for std::accumulate()

#include <cmath>	//	for M_PI (except when its not there), fmod, fabs
#if defined(HAVE_M_PI) && (HAVE_M_PI)
	const double Pi = M_PI;
#else
	const double Pi = 3.14159265358979324;
#endif

// old quadratic interpolation code is still around, in case
// we ever want o use it for comparison, Lemur used to use that.
#if defined(Like_Lemur)
#define SMITHS_BRILLIANT_PARABOLAS
#endif

//  Could compute the mixed partial derivative of
//  phase, for use in bandwidth computation, using
//  the extra, unused half-transform in here, but
//  it might corrupt the magnitude transform
//  slightly, so don't do it until we are actually
//  going to use it.
#undef COMPUTE_MIXED_DERIVATIVE

//	there's a freakin' ton of std in here, 
//	just import the whole namespace
using namespace std;

//	begin namespace
namespace Loris {

// 	static function for building fancy reassignment window:
template < typename RealWinIter, typename CplxWinIter >
static void 
buildReassignmentWindows( RealWinIter winbegin, RealWinIter winend, 
						  CplxWinIter rawinbegin, CplxWinIter raw2inbegin );

// ---------------------------------------------------------------------------
//	ReassignedSpectrum constructor
// ---------------------------------------------------------------------------
//! Construct a new instance using the specified short-time window.
//!	Transform lengths are the smallest power of two greater than twice the
//!	window length.
//
ReassignedSpectrum::ReassignedSpectrum( const std::vector< double > & window ) :
	mMagnitudeTransform( 1 << long( 1 + ceil( log((double)window.size()) / log(2.)) ) ),
	mCorrectionTransform( 1 << long( 1 + ceil( log((double)window.size()) / log(2.)) ) ),
	mWindow( window.begin(), window.end() ),
	mMagnitudeTransformWindow( window.size(), 0. ),
	mCorrectionTransformWindow( window.size(), 0. )
{
	// scale the window so that the reported magnitudes
	// are correct:
	double winsum = std::accumulate( mWindow.begin(), mWindow.end(), 0. );
	std::transform( mWindow.begin(), mWindow.end(), mWindow.begin(), 
			        std::bind1st( std::multiplies<double>(), 2/winsum ) );
	
	buildReassignmentWindows( mWindow.begin(), mWindow.end(), 
	                          mMagnitudeTransformWindow.begin(), 
	                          mCorrectionTransformWindow.begin() );

	debugger << "ReassignedSpectrum: length is " << mMagnitudeTransform.size() << endl;
}

// ---------------------------------------------------------------------------
//	transform
// ---------------------------------------------------------------------------
//!	Compute the reassigned Fourier transform of the samples on the half open
//!	range [sampsBegin, sampsEnd), aligning sampCenter with the center of
//!	the analysis window.
//!
//! \param  sampsBegin pointer representing the beginning of 
//!         the (half-open) range of samples to transform
//! \param  sampCenter the sample in the range that is to be 
//!         aligned with the center of the analysis window
//! \param  sampsEnd pointer representing the end of 
//!         the (half-open) range of samples to transform
//!
//! \pre    sampsBegin must not be past sampCenter
//! \pre    sampsEnd must be past sampCenter
//! \post   the transform buffers store the reassigned 
//!         short-time transform data for the specified 
//!         samples
//
void
ReassignedSpectrum::transform( const double * sampsBegin, 
                               const double * sampCenter, 
                               const double * sampsEnd )
{
    if ( sampCenter < sampsBegin ||  sampCenter >= sampsEnd )
    {
        Throw( InvalidArgument, "Invalid sample range boundaries." );
    }

	const long firstHalfWinLength = window().size() / 2;
	const long secondHalfWinLength = (window().size() - 1) / 2;
	    
    //  ensure that samples outside the window are not used:
    sampsBegin = std::max( sampsBegin, sampCenter - firstHalfWinLength );
    sampsEnd = std::min( sampsEnd, sampCenter + secondHalfWinLength + 1 );
		
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
		std::transform( sampsBegin, sampsEnd, mMagnitudeTransformWindow.begin() + winBeginOffset, 
						mMagnitudeTransform.begin(), std::multiplies< std::complex< double > >() );
	//	fill the rest with zeros:
	std::fill( it, mMagnitudeTransform.end(), 0. );
	//	rotate to align phase:
	std::rotate( mMagnitudeTransform.begin(), mMagnitudeTransform.begin() + rotateBy, mMagnitudeTransform.end() );

	//	compute transform:
	mMagnitudeTransform.transform();

	//	compute the dual reassignment transform:
	//	window the samples into the reassignment FT buffer,
	//	using the complex-valued reassignment window:
	it = std::transform( sampsBegin, sampsEnd, mCorrectionTransformWindow.begin() + winBeginOffset, 
						mCorrectionTransform.begin(), std::multiplies< std::complex<double> >() );
	
	//	fill the rest with zeros:
	std::fill( it, mCorrectionTransform.end(), 0. );
	//	rotate to align phase:
	std::rotate( mCorrectionTransform.begin(), mCorrectionTransform.begin() + rotateBy, mCorrectionTransform.end() );
	//	compute the transform:
	mCorrectionTransform.transform();
}

// ---------------------------------------------------------------------------
//	size
// ---------------------------------------------------------------------------
//! Return the length of the Fourier transforms.
//
ReassignedSpectrum::size_type 
ReassignedSpectrum::size( void ) const 
{ 
    return mMagnitudeTransform.size(); 
}

// ---------------------------------------------------------------------------
//	window
// ---------------------------------------------------------------------------
//! Return read access to the short-time window samples.
//!	(Peers may need to know about the analysis window
//!	or about the scale factors in introduces.)
//
const std::vector< double > &
ReassignedSpectrum::window( void ) const 
{ 
    return mWindow; 
}

// ---------------------------------------------------------------------------
//	circEvenPartAt - helper
// ---------------------------------------------------------------------------
// Extract the circular even part from Fourier transform data.
// Used for computing two real transforms using a single complex transform.
//
template< class TransformData >
static std::complex<double>
circEvenPartAt( const TransformData & td, long idx )
{
    const long N = td.size();
    while( idx < 0 )
    {
        idx += N;
    }
    while( idx >= N )
    {
        idx -= N;
    }

 	long flip_idx;
	if ( idx != 0 )
	{
		flip_idx = N - idx;
	}
	else
    {
		flip_idx = idx;
	}
		
	return 0.5*( td[idx] + std::conj( td[flip_idx] ) );
}   

// ---------------------------------------------------------------------------
//	circOddPartAt - helper
// ---------------------------------------------------------------------------
// Extract the circular odd part divided by j from Fourier transform data.
// Used for computing two real transforms using a single complex transform.
//
template< class TransformData >
static std::complex<double>
circOddPartAt( const TransformData & td, long idx )
{
    const long N = td.size();
    while( idx < 0 )
    {
        idx += N;
    }
    while( idx >= N )
    {
        idx -= N;
    }

 	long flip_idx;
	if ( idx != 0 )
	{
		flip_idx = N - idx;
	}
	else
    {
		flip_idx = idx;
	}

	/*
	const std::complex<double> minus_j(0,-1);
	std::complex<double> tra_part = minus_j * 0.5 * 
									( td[idx] - std::conj( td[flip_idx] ) );
	*/
	//	can compute this without complex multiplies:
	std::complex<double> tmp = td[idx] - std::conj( td[flip_idx] );
	return std::complex<double>( 0.5*tmp.imag(), -0.5*tmp.real() );
}   

// ---------------------------------------------------------------------------
//	frequencyCorrection
// ---------------------------------------------------------------------------
//!	Compute the frequency correction at the specified frequency sample
//! using the method of Auger and Flandrin to evaluate the partial
//! derivative of spectrum phase w.r.t. time.
//!
//!	Correction is computed in fractional frequency samples, because
//!	that's the kind of frequency domain ramp we used on our window.
//!	sample is the frequency sample index, the nominal component 
//!	frequency in samples. 
//
//	Parabolic interpolation can be tried too (see reassignedFrequency()) 
//	but it appears to give slightly worse results, for example, with 
//	a square wave.
//
double
ReassignedSpectrum::frequencyCorrection( long idx ) const
{
	std::complex<double> X_h = circEvenPartAt( mMagnitudeTransform, idx );
    std::complex<double> X_Dh = circEvenPartAt( mCorrectionTransform, idx );
	
	double num = X_h.real() * X_Dh.imag() -
				 X_h.imag() * X_Dh.real();
	
	double magSquared = std::norm( X_h );

	//	need to scale by the oversampling factor?
	double oversampling = (double)mCorrectionTransform.size() / mMagnitudeTransformWindow.size();
	return - oversampling * num / magSquared;
}

// ---------------------------------------------------------------------------
//	timeCorrection
// ---------------------------------------------------------------------------
//!	Compute the time correction at the specified frequency sample
//! using the method of Auger and Flandrin to evaluate the partial
//! derivative of spectrum phase w.r.t. frequency.
//!
//!	Correction is computed in fractional samples, because
//!	that's the kind of ramp we used on our window.
//
double
ReassignedSpectrum::timeCorrection( long idx ) const
{
	std::complex<double> X_h = circEvenPartAt( mMagnitudeTransform, idx );
	std::complex<double> X_Th = circOddPartAt( mCorrectionTransform, idx ); 

	double num = X_h.real() * X_Th.real() +
		  		 X_h.imag() * X_Th.imag();
	double magSquared = norm( X_h );
	
	//	need to scale by the oversampling factor?
	//	No, seems to sound bad, why?
	//	(try alienthreat)
	// double oversampling = (double)mCorrectionTransform.size() / mMagnitudeTransformWindow.size();
	return num / magSquared;
}

// ---------------------------------------------------------------------------
//	reassignedFrequency
// ---------------------------------------------------------------------------
//! Return the reassigned frequency in fractional frequency 
//! samples computed at the specified transform index.
//!
//! \param  idx the frequency sample at which to evaluate the
//!         transform
//
double
ReassignedSpectrum::reassignedFrequency( long idx ) const
{
#if ! defined(SMITHS_BRILLIANT_PARABOLAS)

	return double(idx) + frequencyCorrection( idx );
	
#else // defined(SMITHS_BRILLIANT_PARABOLAS)

	double dbLeft = 20. * log10( abs( circEvenPartAt( mMagnitudeTransform, idx-1 ) ) );
	double dbCandidate = 20. * log10( abs( circEvenPartAt( mMagnitudeTransform, idx ) ) );
	double dbRight = 20. * log10( abs( circEvenPartAt( mMagnitudeTransform, idx+1 ) ) );
	
	double peakXOffset = 0.5 * (dbLeft - dbRight) /
						 (dbLeft - 2.0 * dbCandidate + dbRight);

	return idx + peakXOffset;
	
#endif	//	defined SMITHS_BRILLIANT_PARABOLAS
}

// ---------------------------------------------------------------------------
//	reassignedTime
// ---------------------------------------------------------------------------
//! Return the reassigned time in fractional samples
//! computed at the specified transform index.
//!
//! \param  idx the frequency sample at which to evaluate the
//!         transform
//
double
ReassignedSpectrum::reassignedTime( long idx ) const
{
	return timeCorrection( idx );
}

// ---------------------------------------------------------------------------
//	reassignedMagnitude
// ---------------------------------------------------------------------------
//! Return the spectrum magnitude (absolute)
//! computed at the specified transform index.
//!
//! \param  idx the frequency sample at which to evaluate the
//!         transform
//
double
ReassignedSpectrum::reassignedMagnitude( long idx ) const
{
#if ! defined(SMITHS_BRILLIANT_PARABOLAS)
	
	//	compute the nominal spectral amplitude by scaling
	//	the peak spectral sample:
	return abs( circEvenPartAt( mMagnitudeTransform, idx ) );
	
#else // defined(SMITHS_BRILLIANT_PARABOLAS)
	
	//	keep this parabolic interpolation computation around
	//	only for sake of comparison, it is unlikely to yield
	//	good results with bandwidth association:
	double dbLeft = 20. * log10( abs( circEvenPartAt( mMagnitudeTransform, idx-1 ) ) );
	double dbCandidate = 20. * log10( abs( circEvenPartAt( mMagnitudeTransform, idx ) ) );
	double dbRight = 20. * log10( abs( circEvenPartAt( mMagnitudeTransform, idx+1 ) ) );
	
	double peakXOffset = 0.5 * (dbLeft - dbRight) /
						 (dbLeft - 2.0 * dbCandidate + dbRight);
	double dbmag = dbCandidate - 0.25 * (dbLeft - dbRight) * peakXOffset;
	double x = pow( 10., 0.05 * dbmag );
	
	return x;
	
#endif	//	defined SMITHS_BRILLIANT_PARABOLAS
}

// ---------------------------------------------------------------------------
//	reassignedPhase
// ---------------------------------------------------------------------------
//! Return the phase in radians computed at the specified transform index.
//!	The reassigned phase is shifted to account for the time
//!	correction according to the corrected frequency.
//!
//! \param  idx the frequency sample at which to evaluate the
//!         transform
//
double
ReassignedSpectrum::reassignedPhase( long idx ) const
{
	double phase = arg( circEvenPartAt( mMagnitudeTransform, idx ) );
	
	const double offsetTime = timeCorrection( idx );
	const double offsetFreq = frequencyCorrection( idx );
	
	//	adjust phase according to the frequency correction:
	//	first compute H(1):
	//
	//  this seems like it would be a good idea, but in practice,
	//	it screws the phases up badly.
	//  Am I just correcting in the wrong direction? No, its 
	//	something else.
	//
	//  Seems like I had the slope way too big. Changed to compute 
	//	the slope from H(1) of a rotated window, and now the slope
	//	is so small that it seems like there will never be any phase
	//	correction.
	//
	//  Phase ought to be linear anyway, so I should just be
	//  able to use dumb old linear interpolation.
	double slope = (offsetFreq > 0) ? 
	      ( arg( circEvenPartAt( mMagnitudeTransform, idx+1 ) ) - phase ) : 
	      ( phase - arg( circEvenPartAt( mMagnitudeTransform, idx-1 ) ) );
	phase += offsetFreq * slope;
		
	//	adjust phase according to the time correction:
	const double fracFreqSample = idx + offsetFreq; 
	phase += offsetTime * fracFreqSample * 2. * Pi / mMagnitudeTransform.size();
	
	return fmod( phase, 2. * Pi );
}

// ---------------------------------------------------------------------------
//	reassignedBandwidth
// ---------------------------------------------------------------------------
//! Return the bandwidth factor computed at 
//! the specified transform index. (Experimental,
//! computed from the mixed partial derivative of
//! spectrum phase, not used in BW enhanced analysis.)
//!
//! \param  idx the frequency sample at which to evaluate the
//!         transform
//
double
ReassignedSpectrum::reassignedBandwidth( long idx ) const
{
#if defined(COMPUTE_MIXED_DERIVATIVE) && COMPUTE_MIXED_DERIVATIVE

  	std::complex<double> X_h = circEvenPartAt( mMagnitudeTransform, idx );
	std::complex<double> X_Th = circOddPartAt( mCorrectionTransform, idx ); 
    std::complex<double> X_Dh = circEvenPartAt( mCorrectionTransform, idx );
    std::complex<double> X_TDh = circOddPartAt( mMagnitudeTransform, idx );

	double term1 = (X_TDh * conj(X_h)).real() / norm( X_h );
	double term2 = ((X_Th * X_Dh) / (X_h * X_h)).real();
		  		  
	double scaleBy = 2. * Pi / mMagnitudeTransformWindow.size();

    double bw = fabs( 1.0 + (scaleBy * (term1 - term2)) );
    bw = min( 1.0, bw );

#else
    idx = idx;
    double bw = 0;
#endif
     
    return bw;  
}

// ---------------------------------------------------------------------------
//	subscript operator (deprecated)
// ---------------------------------------------------------------------------
//  Included to support old code.
//  The signature has changed, can no longer return a reference,
//  but since the reference returned was const, this version should 
//  keep most old code working, if not all.
//
std::complex< double >
ReassignedSpectrum::operator[]( unsigned long idx ) const
{
    return circEvenPartAt( mMagnitudeTransform, idx );
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
	std::complex<T> operator()(const T& re, const T& im) const
    {
    	return std::complex<T>( re, im );
    }
};

// ---------------------------------------------------------------------------
//	applyFreqRamp
// ---------------------------------------------------------------------------
//	Adapted from the FrequencyReassignment constructor in Lemur 5.
//
static inline void applyFreqRamp( vector< double > & w  )
{
	//	we're going to do the frequency-domain ramp 
	//	by Fourier transforming the window, ramping,
	//	then transforming again. 
	//	Use a transform exactly as long as the window.
	//	load, w/out rotation, and transform.
	FourierTransform temp( w.size() );
	FourierTransform::iterator it =  std::copy( w.begin(), w.end(), temp.begin() );
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
	for ( int k = 0 ; k < temp.size(); ++k ) 
	{
	   double x = (double)k;   // to get type promotion right
		if ( k < temp.size() / 2 ) 
		{
			temp[ k ] *= x;
		}
		else 
		{
			temp[ k ] *= ( x - temp.size() );
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
	for ( int i = 0; i < w.size(); ++i ) 
	{
		w[i] = - imag( temp[i] ) / temp.size();
	}
}

// ---------------------------------------------------------------------------
//	applyTimeRamp
// ---------------------------------------------------------------------------
//	Make a copy of mWindow scaled by a ramp from -N/2 to N/2 for computing
//	time corrections in samples.
//
static inline void applyTimeRamp( vector< double > & w )
{
	//	the very center of the window should be scaled by 0.,
	//	need a fractional value for even-length windows, a
	//	whole number for odd-length windows:
	double offset = 0.5 * ( w.size() - 1 );
	for ( int k = 0 ; k < w.size(); ++k ) 
	{
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
buildReassignmentWindows( RealWinIter winbegin, RealWinIter winend, 
						  CplxWinIter rawinbegin, CplxWinIter raw2inbegin )
{
	std::vector< double > tramp( winbegin, winend );
	applyTimeRamp( tramp );
	
	std::vector< double > framp( winbegin, winend );
	applyFreqRamp( framp );

	std::vector< double > tframp( framp.size(), 0. );
	
#if defined(COMPUTE_MIXED_DERIVATIVE) && COMPUTE_MIXED_DERIVATIVE	
    //  Do this only if we are computing the mixed 
    //  partial derivative of phase, otherwise, leave
    //  that vector empty.
	tframp = framp;
	applyTimeRamp( tframp );
#endif
	
	std::transform( framp.begin(), framp.end(), tramp.begin(),
					raw2inbegin, make_complex< double >() );	
    
	std::transform( winbegin, winend, tframp.begin(),
					rawinbegin, make_complex< double >() );	
}

}	//	end of namespace Loris

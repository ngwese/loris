// ===========================================================================
//	ReassignedSpectrum.C
//	
//	Implementation of Loris::ReassignedSpectrum.
//
//	-kel 9 Dec 99
//
// ===========================================================================
#include "ReassignedSpectrum.h"
#include "Notifier.h"

#include "AiffFile.h"	//	for debugging only

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	ReassignedSpectrum constructor
// ---------------------------------------------------------------------------
//	Transform lengths are the smallest power of two greater than twice the
//	window length. The noise floor is specified in dB (negative).
//
ReassignedSpectrum::ReassignedSpectrum( const vector< double > & window ) :
	_transform( 1 << long( 1 + ceil( log(window.size()) / log(2.)) ) ),
	_tfreqramp( 1 << long( 1 + ceil( log(window.size()) / log(2.)) ) ),
	_ttimeramp( 1 << long( 1 + ceil( log(window.size()) / log(2.)) ) ),
	_window( window ),
	_winfreqramp( window ),
	_wintimeramp( window )
{
	applyFreqRamp( _winfreqramp );
	applyTimeRamp( _wintimeramp );
	
	//	compute the appropriate scale factor
	//	to report correct component magnitudes:
	double winsum = 0;
	for ( int i = 0; i < _window.size(); ++i ) {
		winsum += _window[i];
	}
	_magScale = 2. / winsum;
	
	notifier << "RA: ft length is " << _transform.size() << " mag scale is " << _magScale << endl;
}

// ---------------------------------------------------------------------------
//	ReassignedSpectrum constructor
// ---------------------------------------------------------------------------
//
ReassignedSpectrum::~ReassignedSpectrum( void )
{
}

// ---------------------------------------------------------------------------
//	transform
// ---------------------------------------------------------------------------
//	idxCenter is the index in buf with which the center of the analysis window
//	should be aligned. It may be outside the extent of the buffer (that is, 
//	negative or greater than the size of the buffer).
//
//	The offset computation is confusing. boffset and eoffset are indices into
//	the sample buffer, woffset is an index into the window. The window and 
//	temporary transform buffer must have the same offset because the center 
//	of the window must always be at the center of the transform buffer (the
//	FourierTransform's loadAndRotate() assumes this).
//
//	I don't like the tempoarary buffer. I think that FT needs an interface that
//	does what I am using std::transform() to do. I also need to get rid of that
//	name collision (transform).
//
void
ReassignedSpectrum::transform( const vector< double > & buf, long idxCenter )
{
//	allocate a temporary sample buffer:
	vector< double > tmp( _window.size(), 0. );
	
//	determine sample boundaries from center index, 
//	buffer length, and window length:
	long boffset = idxCenter - ( _window.size() / 2 );
	long woffset = 0;
	if ( boffset < 0 ) {
		woffset = -boffset;
		boffset = 0;
	}
	long eoffset = min( buf.size(), boffset - woffset + _window.size() );
	if ( eoffset < boffset ) {
		eoffset = boffset;
	}
	
//	window and rotate input and compute normal transform:
	std::transform( buf.begin() + boffset, buf.begin() + eoffset, 
					_window.begin() + woffset, 
					tmp.begin() + woffset, multiplies< double >() );
	_transform.loadAndRotate( tmp );
	_transform.transform();

//	window and rotate input using frequency-ramped window
//	and compute frequency correction transform:
	std::transform( buf.begin() + boffset, buf.begin() + eoffset, 
					_winfreqramp.begin() + woffset, 
					tmp.begin() + woffset, multiplies< double >() );
	_tfreqramp.loadAndRotate( tmp );
	_tfreqramp.transform();

//	window and rotate input using time-ramped window and
//	compute time correction transform:
	std::transform( buf.begin() + boffset, buf.begin() + eoffset, 
					_wintimeramp.begin() + woffset, 
					tmp.begin() + woffset, multiplies< double >() );
	_ttimeramp.loadAndRotate( tmp );
	_ttimeramp.transform();
}

// ---------------------------------------------------------------------------
//	findPeaks
// ---------------------------------------------------------------------------
//	Detect and collect peaks in this short-time time-frequency spectrum
//	that exceed a specified magnitude threshold (defaults to -240 dB).
//
ReassignedSpectrum::Peaks
ReassignedSpectrum::findPeaks( double threshold_dB ) const
{
	double threshold = pow( 10., 0.05 * threshold_dB );	//	absolute magnitude threshold
	Peaks peaks;	//	empty collection
	
	for ( int j = 1; j < (_transform.size() / 2) - 1; ++j ) {
		if ( abs(_transform[j]) > abs(_transform[j-1]) && 
			 abs(_transform[j]) > abs(_transform[j+1])) {
			//	itsa magnitude peak:
			double f = j + frequencyCorrection( j );	//	fractional sample
			//double m = reassignedMagnitude( f );		//	from fractional sample
			double m = _magScale * abs(_transform[j]);
			 
			//	only retain data above the magnitude threshold:
			if ( m > threshold ) {
				peaks.insert( Peak( f, m ) );
				
				/*	Hopefully, this doesn't happen much.
				if ( abs(f-j) > 1 ) {
					debugger << "\Found frequency correction of " << abs(f-j) <<
								" for frequency sample " << j << 
								" having magnitude " << 20. * log10(m) << endl;
				}
				*/
			}
		}
	}
	
	return peaks;
}

// ---------------------------------------------------------------------------
//	applyFreqRamp
// ---------------------------------------------------------------------------
//	Adapted from the FrequencyReassignment constructor in Lemur 5.
//
void
ReassignedSpectrum::applyFreqRamp( vector< double > & w )
{
	//	we're going to do the frequency-domain ramp 
	//	by Fourier transforming the window, ramping,
	//	then transforming again. Can reuse the FT
	//	we already have.
	//	load, w/out rotation, and transform.
	_transform.load( w );
	_transform.transform();
	
	//	now that I have the window spectrum, I should be able
	//	to compute its slope near the peak, and use that later
	//	to correct the peak magnitudes:
	//mWinTransformSlope = 1. - abs( winFT.complexAt(1) ) / abs( winFT.complexAt(0) );
	
	//	extract complex transform and multiply by
	//	a frequency (sample) ramp:
	//	(the frequency ramp goes from 0 to N/2
	//	over the first half, then -N/2 to 0 over 
	//	the second (aliased) half of the transform)
	for ( int k = 0 ; k < _transform.size(); ++k ) {
		if ( k < _transform.size() / 2 ) {
			_transform[ k ] *= k;
		}
		else {
			_transform[ k ] *= k - _transform.size();
		}
	}
	
	//	invert the transform:
	_transform.transform();
	
	//	the DFT of a DFT gives the scaled and INDEX REVERSED
	//	sequence. See p. 539 of O and S.
	//	DFT( X[n] ) -- DFT --> Nx[ -k mod N ] 
	//
	//	seems that I want the imaginary part of the index-reversed
	//	transform scaled by the size of the transform:
	rotate( _transform.begin(), _transform.begin() + 1, _transform.end() );
	reverse( _transform.begin(), _transform.end() );
	/*
	std::transform( winFT.begin(), 
					winFT.end(), 
					_freqRampWin.begin(), 
					imag<double> );
	for_each( _freqRampWin.begin(), _freqRampWin.end(), 
				bind2nd( divides<double>(), winFT.size() ) );
	*/
	for ( int i = 0; i < w.size(); ++i ) {
		w[i] = - imag( _transform[i] ) / _transform.size();
	}
}

// ---------------------------------------------------------------------------
//	applyTimeRamp
// ---------------------------------------------------------------------------
//	Make a copy of _window scaled by a ramp from -N/2 to N/2 for computing
//	time corrections in samples.
//
void
ReassignedSpectrum::applyTimeRamp( vector< double > & w )
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
ReassignedSpectrum::frequencyCorrection( long sample ) const
{
//#define __parab
#ifndef __parab
	double num = _transform[sample].real() * _tfreqramp[sample].imag() -
					_transform[sample].imag() * _tfreqramp[sample].real();
	double magSquared = norm( _transform[sample] );
						
	return - num / magSquared;

#else

//	use parabolic interpolation until
//	we figure out why (whether?) freq reassignment sucks:
	double dbLeft = 20. * log10( abs( _transform[sample-1] ) );
	double dbCandidate = 20. * log10( abs( _transform[sample] ) );
	double dbRight = 20. * log10( abs( _transform[sample+1] ) );

	return	0.5 * (dbLeft - dbRight) /
			(dbLeft - (2. * dbCandidate) + dbRight);

#endif

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
ReassignedSpectrum::timeCorrection( long sample ) const
{
	double num = _transform[sample].real() * _ttimeramp[sample].real() +
		  		 _transform[sample].imag() * _ttimeramp[sample].imag();
	double magSquared = norm( _transform[sample] );
	
	return  num / magSquared;
}

// ---------------------------------------------------------------------------
//	reassignedFrequency
// ---------------------------------------------------------------------------
//	Compute the normalized (to sample rate 1.0) reassigned frequency 
//	from the fractional frequency sample.
//
double
ReassignedSpectrum::reassignedFrequency( double fracFreqSample ) const
{
	return fracFreqSample / _transform.size();
}

// ---------------------------------------------------------------------------
//	reassignedTime
// ---------------------------------------------------------------------------
//	Return the value of the time correction corresponding to the corrected
//	(fractional) frequency sample, interpolating the time corrections for 
//	neighboring samples.
//
//	The nominal time is 0 (samples) since no other temporal information about 
//	the transformed buffer is available.
//
double
ReassignedSpectrum::reassignedTime( double fracFreqSample ) const
{
	double alpha = fracFreqSample - floor( fracFreqSample );
	return (alpha * timeCorrection( ceil(fracFreqSample) )) + 
		   (( 1.-alpha ) * timeCorrection( floor(fracFreqSample) ));
}

// ---------------------------------------------------------------------------
//	reassignedMagnitude
// ---------------------------------------------------------------------------
//	This algorithm assumes that the frequency correction is small, so that
//	the magnitude spectrum is evaulated near the peak sample. This may be a bad 
//	assumption. In fact, it is a terrible assumption. Small peaks with large
//	frequency corrections evaulated at their corrected frequencies will 
//	report artificially large magnitudes. This is just because frequency 
//	reassignment is correctly reassigning the spectral samples to the 
//	centers of spectral gravity, where the strong components are.
//
double
ReassignedSpectrum::reassignedMagnitude( double fracFreqSample ) const
{
	return _magScale * abs( _transform[ round(fracFreqSample) ] );
/*
	//	parabola thing:
	long sample = round(fracFreqSample);
	double dbLeft = 20. * log10( abs( _transform[sample-1] ) );
	double dbCandidate = 20. * log10( abs( _transform[sample] ) );
	double dbRight = 20. * log10( abs( _transform[sample+1] ) );
	
	double polynomialPeakXOffset =	0.5 * (dbLeft - dbRight) /
						(dbLeft - 2.0 * dbCandidate + dbRight);
	
	return dbCandidate - 0.25 * (dbLeft - dbRight) * polynomialPeakXOffset;
*/
}

// ---------------------------------------------------------------------------
//	reassignedPhase
// ---------------------------------------------------------------------------
//	The reassigned phase is interpolated according to the corrected frequency,
//	the fractional frequency sample, and shifted to account for the time
//	correction (in fractional time samples).
//
//	This algorithm assumes that the frequency correction is small, so that
//	the phase spectrum is evaulated near the peak sample. This may be a bad 
//	assumption.
//
double
ReassignedSpectrum::reassignedPhase( double fracFreqSample, 
									 double timeCorrection ) const
{
	//	compute (interpolate) the phase at the reassigned frequency:
	double alpha = fracFreqSample - floor( fracFreqSample );

	double phaseBelow = arg( _transform[ floor( fracFreqSample ) ] );
	double phaseAbove = arg( _transform[ ceil( fracFreqSample ) ] );

	//	unwrap, assume that adjacent phase measurements near a
	//	spectral peak cannot differ by more than Pi:
	if ( fabs(phaseBelow - phaseAbove) > Pi )
	{
		if ( phaseBelow > phaseAbove ) {
			phaseAbove += TwoPi;
		}
		else {
			phaseAbove -= TwoPi;
		}
	}

	double phase = ( alpha * phaseAbove ) + (( 1. - alpha ) * phaseBelow );
				
	//	correct for time offset:
	phase += timeCorrection * fracFreqSample * TwoPi / _transform.size();
	
	return fmod( phase, TwoPi );
}


End_Namespace( Loris )

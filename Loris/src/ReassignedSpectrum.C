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

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	ReassignedSpectrum constructor
// ---------------------------------------------------------------------------
//	Transform lengths are the smallest power of two greater than twice the
//	window length. The noise floor is specified in dB (negative).
//
ReassignedSpectrum::ReassignedSpectrum( const vector< double > & window, double noiseFloor ) :
	_transform( 1 << long( 1 + ceil( log(window.size()) / log(2.)) ) ),
	_tfreqramp( 1 << long( 1 + ceil( log(window.size()) / log(2.)) ) ),
	_ttimeramp( 1 << long( 1 + ceil( log(window.size()) / log(2.)) ) ),
	_window( window ),
	_winfreqramp( window ),
	_wintimeramp( window ),
	_threshold( pow(10., 0.05 * noiseFloor) )
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
//	Use three iterators: begin, end, and middle, align middle with the 
//	center of the window. Can I do this? It limits the way I can lay 
//	windows over a buffer, because I can't position the center of the 
//	window outside the buffer, but maybe that's okay?
//
void
ReassignedSpectrum::transform( const vector< double > & buf )
{
//	clear the ridge data collection:
	_ridges.clear();
	
//	allocate a temporary sample buffer:
	vector< double > tmp( buf.size() );
	
//	window and rotate input and compute normal transform:
	std::transform( buf.begin(), buf.end(), _window.begin(), 
					tmp.begin(), multiplies< double >() );
	_transform.loadAndRotate( tmp );
	_transform.transform();

//	window and rotate input using frequency-ramped window
//	and compute frequency correction transform:
	std::transform( buf.begin(), buf.end(), _winfreqramp.begin(), 
					tmp.begin(), multiplies< double >() );
	_tfreqramp.loadAndRotate( tmp );
	_tfreqramp.transform();

//	window and rotate input using time-ramped window and
//	compute time correction transform:
	std::transform( buf.begin(), buf.end(), _wintimeramp.begin(), 
					tmp.begin(), multiplies< double >() );
	_ttimeramp.loadAndRotate( tmp );
	_ttimeramp.transform();

//	detect and collect time-frequency ridges:
	detectRidges();
}

// ---------------------------------------------------------------------------
//	detectRidges
// ---------------------------------------------------------------------------
//	Detect and collect peaks in this short-time time-frequency spectrum.
//
void
ReassignedSpectrum::detectRidges( void )
{
	for ( int j = 1; j < _transform.size() / 2; ++j ) {
		double mag = abs(_transform[j]); 
		if ( mag > abs(_transform[j-1]) && mag > abs(_transform[j+1])) {
			//	itsa magnitude peak:
			double f = j + frequencyCorrection( j );	//	fractional sample
			double m = reassignedMagnitude( f );		//	from fractional sample
			 
			//	only retain data above the magnitude threshold:
			if ( m > _threshold ) {
				//_ridges.push_back( Datum( t, f, m, p ) ); 
				_ridges.push_back( f );
				
				//	Hopefully, this doesn't happen much.
				if ( abs(f-j) > 0.5 ) {
					debugger << "\Found frequency correction of " << abs(f-j) <<
								" for frequency sample " << j << 
								" having magnitude " << 20. * log10(m) << endl;
				}
			}
		}
	}
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
double
ReassignedSpectrum::frequencyCorrection( long sample ) const
{
	double num = _transform[sample].real() * _tfreqramp[sample].imag() -
					_transform[sample].imag() * _tfreqramp[sample].real();
	double magSquared = abs( _transform[sample] ) * abs( _transform[sample] );
						
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
//	assumption.
//
double
ReassignedSpectrum::reassignedMagnitude( double fracFreqSample ) const
{
	return _magScale * abs( _transform[ round(fracFreqSample) ] );
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
	double alpha =  fracFreqSample - floor( fracFreqSample );

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
	phase += ( timeCorrection * fracFreqSample * TwoPi / _transform.size() );
	
	return fmod( phase, TwoPi );
}


End_Namespace( Loris )

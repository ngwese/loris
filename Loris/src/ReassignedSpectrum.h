#ifndef __reassigned_spectrum_analyzer__
#define __reassigned_spectrum_analyzer__
// ===========================================================================
//	ReassignedSpectrum.h
//	
//	Class definition for Loris::ReassignedSpectrum.
//
//	-kel 7 Dec 99
//
// ===========================================================================
#include "LorisLib.h"
#include "FourierTransform.h"

#include <vector>

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class ReassignedSpectrum
//
//	Computes a reassigned short-time Fourier spectrum and identifies
//	(interpolates) ridges. 
//	
class ReassignedSpectrum
{
//	-- public interface --
public:
//	construction:
	ReassignedSpectrum( const vector< double > & window, double noiseFloor = -90. );
	~ReassignedSpectrum( void );

//	spectrum computation:	
	void transform( const std::vector< double > & buf );

//	length of the three Fourier transforms:
	long size( void ) const { return _transform.size(); }
	
//	time-frequency ridge datum type:	
	struct Datum 
	{
		//	construction:
		Datum( void ) : time( 0. ), frequency( 0. ), magnitude( 0. ), phase( 0. ) {}
		Datum( double t, double f, double mag, double theta ) : 
			time( t ), frequency( f ), magnitude( mag ), phase( theta ) {}
		//	comparison (by frequency):
		boolean operator< ( const Datum & rhs ) const 
			{ return frequency < rhs.frequency; }
		//	instance variables:
		double time;		//	offset in samples from the center of the window
		double frequency;	//	normalized, fraction of sample rate
		double magnitude;	//	absolute
		double phase;		//	radians, corrected for time offset
	};
		
//	ridge (const) iterator access:
//	(is this good?)
	//typedef std::vector< Datum >::const_iterator ridge_iterator;
	typedef std::vector< double >::const_iterator ridge_iterator;
	ridge_iterator ridgesBegin( void ) const { return _ridges.begin(); }
	ridge_iterator ridgesEnd( void ) const { return _ridges.end(); }

//	reassigned data access:		
	double reassignedFrequency( double fracFreqSample ) const;
	double reassignedTime( double fracFreqSample ) const;
	double reassignedMagnitude( double fracFreqSample ) const;
	double reassignedPhase( double fracFreqSample, double timeCorrection ) const;
	
//	-- internal helpers --
private:
	void applyFreqRamp( std::vector< double > & w );
	void applyTimeRamp( std::vector< double > & w );
	
	void detectRidges( void );

	//	compute time and frequency corrections 
	//	at transform sample indices:
	double frequencyCorrection( long sample ) const;
	double timeCorrection( long sample ) const;
	
//	-- instance variables --
private:
	//	transforms:
	FourierTransform _transform, _tfreqramp, _ttimeramp;
	
	//	windows:
	std::vector< double > _window, _winfreqramp, _wintimeramp;
	
	//	time-frequency ridge data:
	//std::vector< Datum > _ridges;
	std::vector< double > _ridges;

	//	absolute threshold for ridge detection:
	const double _threshold;
	
	//	scale factor for correcting magnitudes:
	double _magScale;	
};	//	end of class ReassignedSpectrum

End_Namespace( Loris )

#endif	// ndef __reassigned_spectrum_analyzer__

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
//	(interpolates) short-time magnitude-spectral peaks.. 
//	
class ReassignedSpectrum
{
//	-- public interface --
public:
//	construction:
	ReassignedSpectrum( const vector< double > & window );
	~ReassignedSpectrum( void );

//	spectrum computation:	
	void transform( const std::vector< double > & buf, long idxCenter );

//	length of the three Fourier transforms:
	long size( void ) const { return _transform.size(); }
	
//	reassigned spectral data access:		
	double reassignedFrequency( double fracFreqSample ) const;
	double reassignedTime( double fracFreqSample ) const;
	double reassignedMagnitude( double fracFreqSample ) const;
	double reassignedPhase( double fracFreqSample, double timeCorrection ) const;
	
//	short-time magnitude spectral peak access:
	//	Peak type
	//	frequency is fractional samples, magnitude is absolute
	class Peak
	{
	public:
		//	construction:
		Peak( double f, double m ) : _p( f, m ) {}
		Peak( void ) : _p( 0., 0. ) {}
		//	access:
		double frequency( void ) const { return _p.first; }
		double magnitude( void ) const { return _p.second; }
		//	comparison (by frequency):
		boolean operator< ( const Peak & rhs ) const 
			{ return frequency() < rhs.frequency(); }
		//	frequency-magnitude pair:
	private:
		std::pair< double, double > _p;
	};	//	end of class Peak
	
	//	Peak collection type
	typedef std::set< Peak > Peaks;
	
	//	Peak collection access:
	Peaks findPeaks( double threshold_dB = -240. ) const;
	
//	-- internal helpers --
private:
	//	make the special window functions needed for
	//	frequency and time reassignment:
	void applyFreqRamp( std::vector< double > & w );
	void applyTimeRamp( std::vector< double > & w );
	
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
	
	//	scale factor for correcting magnitudes:
	double _magScale;	
};	//	end of class ReassignedSpectrum

End_Namespace( Loris )

#endif	// ndef __reassigned_spectrum_analyzer__

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
	ReassignedSpectrum( const std::vector< double > & window );
	~ReassignedSpectrum( void );

//	spectrum computation:	
	void transform( const std::vector< double > & buf, long idxCenter );

//	length of the three Fourier transforms:
	long size( void ) const { return _transform.size(); }
	
//	peers may need to know about the analysis window
//	or about the scale factors in introduces:
	const std::vector< double > & window( void ) const { return _window; }
	double magnitudeScale(void) const { return _windowMagnitudeScale; }
	double energyScale(void) const { return _windowEnergyScale; }
	
//	reassigned spectral data access:		
	double magnitude( unsigned long idx ) const;
	double reassignedFrequency( unsigned long idx ) const;
	double reassignedTime( double fracFreqSample ) const;
	double reassignedPhase( double fracFreqSample, double timeCorrection ) const;
	
	double correctedMagnitude( double fracBinNum, long bin ) const;
	
	const std::complex< double > & operator[]( unsigned long idx ) const 
		{ return _transform[idx]; }
	
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
	
	//	compute the window spectrum used to correct
	//	spectral component magnitudes:
	void computeWindowSpectrum( const std::vector< double > & v );

//	-- instance variables --
private:
	//	transforms:
	FourierTransform _transform, _tfreqramp, _ttimeramp;
	
	//	windows:
	std::vector< double > _window, _winfreqramp, _wintimeramp;
	
	//	oversampled window spectrum for correcting magnitudes:
	std::vector< double > _mainlobe;

	//	scale factors for correcting magnitudes:
	double _windowMagnitudeScale;	
	double _windowEnergyScale;	
};	//	end of class ReassignedSpectrum

End_Namespace( Loris )

#endif	// ndef __reassigned_spectrum_analyzer__

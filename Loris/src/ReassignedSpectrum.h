#ifndef INCLUDE_REASSIGNEDSPECTRUM_H
#define INCLUDE_REASSIGNEDSPECTRUM_H
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
 * ReassignedSpectrum.h
 *
 * Definition of class Loris::ReassignedSpectrum.
 *
 * Kelly Fitz, 7 Dec 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include <Loris_prefix.h>
#include "FourierTransform.h"
#include <vector>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif


// ---------------------------------------------------------------------------
//	class ReassignedSpectrum
//
//	Computes a reassigned short-time Fourier spectrum and identifies
//	(interpolates) short-time magnitude-spectral peaks. 
//	
class ReassignedSpectrum
{
//	-- public interface --
public:
//	construction:
	ReassignedSpectrum( const std::vector< double > & window );
	~ReassignedSpectrum( void );

//	spectrum computation:	
	void transform( const double * bufBegin, const double * pos, const double * bufEnd );

//	length of the three Fourier transforms:
	long size( void ) const { return _transform.size(); }
	
//	peers may need to know about the analysis window
//	or about the scale factors in introduces:
	const std::vector< double > & window( void ) const { return _window; }
	double magnitudeScale(void) const { return _windowMagnitudeScale; }
	double energyScale(void) const { return _windowEnergyScale; }
	
//	reassigned spectral data access:		
	double reassignedFrequency( unsigned long idx ) const;
	double reassignedTime( unsigned long idx ) const;
	double reassignedPhase( long idx, double fracFreqSample, double timeCorrection ) const;	
	double reassignedMagnitude( double fracBinNum, long intBinNumber ) const;
	
	const std::complex< double > & operator[]( unsigned long idx ) const 
		{ return _transform[idx]; }

//	time and frequency corrections 
//	at transform sample indices:
	double frequencyCorrection( long sample ) const;
	double timeCorrection( long sample ) const;
	
//	-- internal helpers --
private:
	//	make the special window functions needed for
	//	frequency and time reassignment:
	void applyFreqRamp( std::vector< double > & w );
	void applyTimeRamp( std::vector< double > & w );
	
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

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif /* ndef INCLUDE_REASSIGNEDSPECTRUM_H */

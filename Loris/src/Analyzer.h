#ifndef INCLUDE_ANALYZER_H
#define INCLUDE_ANALYZER_H
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
 * Analyzer.h
 *
 * Definition of class Loris::Analyzer.
 *
 * Kelly Fitz, 5 Dec 99
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include <memory>
#include <vector>
#include "Partial.h"
#include "PartialList.h"

//	begin namespace
namespace Loris {

class Analyzer_imp;
class Envelope;

// ---------------------------------------------------------------------------
//	class Analyzer
//
//	Class Analyzer represents a configuration of parameters for
//	performing Reassigned Bandwidth-Enhanced Additive Analysis
//	of sampled sounds. The analysis process yields a collection 
//	of Partials, each having a trio of synchronous, non-uniformly-
//	sampled breakpoint envelopes representing the time-varying 
//	frequency, amplitude, and noisiness of a single bandwidth-
//	enhanced sinusoid. These Partials are accumulated in the
// 	Analyzer.
//
//	The core analysis parameter is the frequency resolution, the minimum
//	instantaneous frequency spacing between partials. All other
//	parameters are initially configured according to this parameter
//	(and the analysis window width, if specified).
//	Subsequent parameter mutations are independent.
//	
//	For more information about Reassigned Bandwidth-Enhanced 
//	Analysis and the Reassigned Bandwidth-Enhanced Additive Sound 
//	Model, refer to the Loris website: www.cerlsoundgroup.org/Loris/.
//
class Analyzer
{
//	-- instance variables --
	std::auto_ptr< Analyzer_imp > _imp; 	//	insulating implementation class
				
//	-- public interface --
public:
//	-- construction --

	//	Construct a new Analyzer configured with the given	
	//	frequency resolution (minimum instantaneous frequency	
	//	difference between Partials). All other Analyzer parameters 	
	//	are computed from the specified frequency resolution. 	
	//	
	//	resolutionHz is the frequency resolution in Hz.
	explicit Analyzer( double resolutionHz );
	
	//	Construct a new Analyzer configured with the given	
	//	frequency resolution (minimum instantaneous frequency	
	//	difference between Partials) and analysis window width
	//	(main lobe, zero-to-zero). All other Analyzer parameters 	
	//	are computed from the specified resolution and window width. 	
	//	
	//	resolutionHz is the frequency resolution in Hz.
	//	windowWidthHz is the main lobe width of the Kaiser
	//	analysis window in Hz.
	Analyzer( double resolutionHz, double windowWidthHz );

	//	Construct  a new Analyzer having identical
	//	parameter configuration to another Analyzer. 
	//	The list of collected Partials is not copied. 		
	//	
	//	other is the Analyzer to copy.	
	Analyzer( const Analyzer & other );

	//	Destroy this Analyzer. 								
	~Analyzer( void );

	//	Change this Analyzer's parameter configuration to 
	//	be identical to that of another Analyzer. The list of 
	//	collected Partials is not copied or otherwise modified. 
	//	
	//	rhs is the Analyzer from which to copy parameters.			
	Analyzer & operator=( const Analyzer & rhs );

//	-- configuration --

	//	Configure this Analyzer with the given frequency resolution 
	//	(minimum instantaneous frequency difference between Partials)
	//	and analysis window width (main lobe, zero-to-zero, in Hz). 
	//	All other Analyzer parameters are (re-)computed from the 
	//	frequency resolution and window width. 		
	//	
	//	resolutionHz is the frequency resolution in Hz.
	//	windowWidthHz is the main lobe width of the Kaiser
	//	analysis window in Hz.
	//
	//	There are three categories of analysis parameters:
	//	- the resolution, and params that are usually related to (or
	//	identical to) the resolution (frequency floor and drift)
	//	- the window width and params that are usually related to (or
	//	identical to) the window width (hop and crop times)
	//	- independent parameters (bw region width and amp floor)
	void configure( double resolutionHz, double windowWidthHz );

//	-- analysis --

	//	Analyze a range of (mono) samples at the given sample rate 	  	
	//	(in Hz) and collect the resulting Partials.	
	//	
	//	bufBegin is a pointer to a buffer of floating point samples
	//	bufEnd is (one-past) the end of a buffer of floating point 
	//	samples
	//	srate is the sample rate of the samples in the buffer
	void analyze( const double * bufBegin, const double * bufEnd, double srate );
	
	//	Analyze a vector of (mono) samples at the given sample rate 	  	
	//	(in Hz) and append the extracted Partials to Analyzer's 
	//	PartialList (std::list of Partials).	
	//	
	//	vec is a vector of floating point samples
	//	srate is the sample rate of the samples in the vector
	void analyze( const std::vector<double> & vec, double srate );
	
//	-- tracking analysis --

	//	Analyze a range of (mono) samples at the given sample rate 	  	
	//	(in Hz) and append the extracted Partials to Analyzer's 
	//	PartialList (std::list of Partials). Use the specified envelope
	//	as a frequency reference for Partial tracking.
	//	
	//	bufBegin is a pointer to a buffer of floating point samples
	//	bufEnd is (one-past) the end of a buffer of floating point 
	//	samples
	//	srate is the sample rate of the samples in the buffer
	//	reference is an Envelope having the approximate
	//	frequency contour expected of the resulting Partials.
	void analyze( const double * bufBegin, const double * bufEnd, double srate,
				  const Envelope & reference );
	
	//	Analyze a vector of (mono) samples at the given sample rate 	  	
	//	(in Hz) and append the extracted Partials to Analyzer's 
	//	PartialList (std::list of Partials). Use the specified envelope
	//	as a frequency reference for Partial tracking.
	//	
	//	vec is a vector of floating point samples
	//	srate is the sample rate of the samples in the vector
	//	reference is an Envelope having the approximate
	//	frequency contour expected of the resulting Partials.
	void analyze( const std::vector<double> & vec, double srate, 
				  const Envelope & reference );
	
//	-- parameter access --
	//	Return the amplitude floor (lowest detected spectral amplitude),  			
	//	in (negative) dB, for this Analyzer. 				
	double ampFloor( void ) const;

	//	Return true if this Analyzer is configured to peform bandwidth
	//   association to distribute noise energy among extracted Partials, 
	//	and false if noise energy will be collected in noise Partials,
	//	labeled -1 in this Analyzer's PartialList.
	bool associateBandwidth( void ) const;

	//	Return the width (in Hz) of the Bandwidth Association regions
	//	used by this Analyzer. If zero, bandwidth enhancement is
	//	disabled.
	double bwRegionWidth( void ) const;

	//	Return the crop time (maximum temporal displacement of a time-
	//	frequency data point from the time-domain center of the analysis
	//	window, beyond which data points are considered "unreliable")
	//	for this Analyzer.
 	double cropTime( void ) const;

	//	Return the maximum allowable frequency difference between 					
	//	consecutive Breakpoints in a Partial envelope for this Analyzer. 				
  	double freqDrift( void ) const;

	//	Return the frequency floor (minimum instantaneous Partial  				
	//	frequency), in Hz, for this Analyzer. 				
	double freqFloor( void ) const;

	//	Return the frequency resolution (minimum instantaneous frequency  		
	//	difference between Partials) for this Analyzer. 	
	double freqResolution( void ) const;

	//	Return the hop time (which corresponds approximately to the 
	//	average density of Partial envelope Breakpoint data) for this 
	//	Analyzer.
	double hopTime( void ) const;

	//	Return the sidelobe attenutation level for the Kaiser analysis window in
	//	positive dB. Larger numbers (e.g. 90) give very good sidelobe 
	//	rejection but cause the window to be longer in time. Smaller numbers 
	//	(like 60) raise the level of the sidelobes, increasing the likelihood
	//	of frequency-domain interference, but allow the window to be shorter
	//	in time.
	double sidelobeLevel( void ) const;

	//	Return the frequency-domain main lobe width (measured between 
	//	zero-crossings) of the analysis window used by this Analyzer. 				
 	double windowWidth( void ) const;
	 
//	-- parameter mutation --

	//	Set the amplitude floor (lowest detected spectral amplitude), in  			
	//	(negative) dB, for this Analyzer. 
	//	
	//	x is the new value of this parameter.			
  	void setAmpFloor( double x );

	//	Set the width (in Hz) of the Bandwidth Association regions
	//	used by this Analyzer. If zero, bandwidth enhancement is 
	//	disabled.
	//	
	//	x is the new value of this parameter.			
	void setBwRegionWidth( double x );

	//	Set the crop time (maximum temporal displacement of a time-
	//	frequency data point from the time-domain center of the analysis
	//	window, beyond which data points are considered "unreliable")
	//	for this Analyzer.
	//	
	//	x is the new value of this parameter.			
 	void setCropTime( double x );

	//	Set the maximum allowable frequency difference between 					
	//	consecutive Breakpoints in a Partial envelope for this Analyzer. 				
	//	
	//	x is the new value of this parameter.			
	void setFreqDrift( double x );

	//	Set the frequency floor (minimum instantaneous Partial  				
	//	frequency), in Hz, for this Analyzer.
	//	
	//	x is the new value of this parameter.			
	void setFreqFloor( double x );

	//	Set the frequency resolution (minimum instantaneous frequency  		
	//	difference between Partials) for this Analyzer. (Does not cause 	
	//	other parameters to be recomputed.) 									
	//	
	//	x is the new value of this parameter.			
	void setFreqResolution( double x );

	//	Set the hop time (which corresponds approximately to the average
	//	density of Partial envelope Breakpoint data) for this Analyzer.
	//	
	//	x is the new value of this parameter.			
  	void setHopTime( double x );

	//	Set the sidelobe attenutation level for the Kaiser analysis window in
	//	positive dB. More negative numbers (e.g. -90) give very good sidelobe 
	//	rejection but cause the window to be longer in time. Less negative 
	//	numbers raise the level of the sidelobes, increasing the likelihood
	//	of frequency-domain interference, but allow the window to be shorter
	//	in time.
	//	
	//	x is the new value of this parameter.			
	void setSidelobeLevel( double x );

	//	Set the frequency-domain main lobe width (measured between 
	//	zero-crossings) of the analysis window used by this Analyzer. 	
	//	
	//	x is the new value of this parameter.			
	void setWindowWidth( double x );

//	-- PartialList access --

	//	Return a mutable reference to this Analyzer's list of 
	//	analyzed Partials. 
	PartialList & partials( void );

	//	Return an immutable (const) reference to this Analyzer's 
	//	list of analyzed Partials. 
	const PartialList & partials( void ) const;
	
};	//	end of class Analyzer

}	//	end of namespace Loris

#endif /* ndef INCLUDE_ANALYZER_H */

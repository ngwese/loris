#ifndef INCLUDE_ANALYZER_H
#define INCLUDE_ANALYZER_H
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
#include <vector>
#include <PartialList.h>

//	begin namespace
namespace Loris {

class Analyzer_imp;

// ---------------------------------------------------------------------------
//	class Analyzer
//	
//	Class Analyzer represents a configuration of parameters for
//	performing Reassigned Bandwidth-Enhanced Additive Analysis
//	of sampled waveforms. This analysis process yields a collection 
//	of Partials, each having a trio of synchronous, non-uniformly-
//	sampled breakpoint envelopes representing the time-varying 
//	frequency, amplitude, and noisiness of a single bandwidth-
//	enhanced sinusoid. 
//	
//	For more information about Reassigned Bandwidth-Enhanced 
//	Analysis and the Reassigned Bandwidth-Enhanced Additive Sound 
//	Model, refer to the Loris website: www.cerlsoundgroup.org/Loris/.
//
class Analyzer
{
//	-- instance variables --
	Analyzer_imp * _imp; 	//	insulating implementation class
				
//	-- public interface --
public:
//	-- construction --
	explicit Analyzer( double resolutionHz );
	/*	Construct and return a new Analyzer configured with the given	
		frequency resolution (minimum instantaneous frequency	
		difference between Partials). All other Analyzer parameters 	
		are computed from the specified frequency resolution. 			
	 */
	Analyzer( double resolutionHz, double windowWidthHz );
	/*	Construct and return a new Analyzer configured with the given	
		frequency resolution (minimum instantaneous frequency	
		difference between Partials) and analysis window width
		(main lobe, zero-to-zero). All other Analyzer parameters 	
		are computed from the specified resolution and window width. 			
	 */
	Analyzer( const Analyzer & other );
	/*	Construct and return a new Analyzer having identical
		parameter configuration to another Analyzer. The list of 
		collected Partials is not copied. 			
	 */
	~Analyzer( void );
	/*	Destroy this Analyzer. 								
	 */

//	-- configuration --
	void configure( double resolutionHz, double windowWidthHz );
	/*	Configure this Analyzer with the given frequency resolution 
		(minimum instantaneous frequency difference between Partials)
		and analysis window width (main lobe, zero-to-zero, in Hz). 
		All other Analyzer parameters are (re-)computed from the 
		frequency resolution and window width. 		
	 */
	Analyzer & operator = ( const Analyzer & rhs );
	/*	Change this Analyzer's parameter configuration to 
		be identical to that of another Analyzer. The list of 
		collected Partials is not copied or otherwise modified. 			
	 */

//	-- analysis --
	void analyze( const double * bufBegin, const double * bufEnd, double srate );
	/*	Analyze a range of (mono) samples at the given sample rate 	  	
		(in Hz) and append the extracted Partials to Analyzer's 
		PartialList (std::list of Partials).	
	 */
	
	void analyze( const std::vector<double> & buffer, double srate )
		{ analyze( &(buffer[0]),  &(buffer[0]) + buffer.size(), srate ); }
	/*	Analyze a vector of (mono) samples at the given sample rate 	  	
		(in Hz) and append the extracted Partials to Analyzer's 
		PartialList (std::list of Partials).	
	 */
	
//	-- parameter access --
	double ampFloor( void ) const;
	/*	Return the amplitude floor (lowest detected spectral amplitude),  			
		in (negative) dB, for this Analyzer. 				
	 */
	bool associateBandwidth( void ) const;
	/*	Return true if this Analyzer is configured to peform bandwidth
        association to distribute noise energy among extracted Partials, 
        and false if noise energy will be collected in noise Partials,
        labeled -1 in this Analyzer's PartialList.
	 */
	double bwRegionWidth( void ) const;
	/*	Return the width (in Hz) of the Bandwidth Association regions
		used by this Analyzer.
		
		This parameter is deprecated and not generally useful. It will be
		removed in a future release.
	 */
 	double cropTime( void ) const;
	/*	Return the crop time (maximum temporal displacement of a time-
		frequency data point from the time-domain center of the analysis
		window, beyond which data points are considered "unreliable")
		for this Analyzer.
		
		This parameter is deprecated and not generally useful. It will be
		removed in a future release.
	 */	
 	double freqDrift( void ) const;
	/*	Return the maximum allowable frequency difference between 					
		consecutive Breakpoints in a Partial envelope for this Analyzer. 				
	 */
 	double freqFloor( void ) const;
	/*	Return the frequency floor (minimum instantaneous Partial  				
		frequency), in Hz, for this Analyzer. 				
	 */
	double freqResolution( void ) const;
	/*	Return the frequency resolution (minimum instantaneous frequency  		
		difference between Partials) for this Analyzer. 	
	 */
	double hopTime( void ) const;
	/*	Return the hop time (which corresponds approximately to the 
		average density of Partial envelope Breakpoint data) for this 
		Analyzer.
	 */
	double sidelobeLevel( void ) const;
	/*	Return the sidelobe attenutation level for the Kaiser analysis window in
		positive dB. More negative numbers (e.g. -90) give very good sidelobe 
		rejection but cause the window to be longer in time. Less negative 
		numbers raise the level of the sidelobes, increasing the likelihood
		of frequency-domain interference, but allow the window to be shorter
		in time.
	 */
 	double windowWidth( void ) const;
	/*	Return the frequency-domain main lobe width (measured between 
		zero-crossings) of the analysis window used by this Analyzer. 				
	 */
	 
//	-- parameter mutation --
 	void setAmpFloor( double x );
	/*	Set the amplitude floor (lowest detected spectral amplitude), in  			
		(negative) dB, for this Analyzer. 				
	 */
 	void setBwRegionWidth( double x );
	/*	Set the width (in Hz) of the Bandwidth Association regions
		used by this Analyzer.
		
		This parameter is deprecated and not generally useful. It will be
		removed in a future release.
	 */
 	void setCropTime( double x );
	/*	Set the crop time (maximum temporal displacement of a time-
		frequency data point from the time-domain center of the analysis
		window, beyond which data points are considered "unreliable")
		for this Analyzer.
		
		This parameter is deprecated and not generally useful. It will be
		removed in a future release.
	 */
	void setFreqDrift( double x );
	/*	Set the maximum allowable frequency difference between 					
		consecutive Breakpoints in a Partial envelope for this Analyzer. 				
	 */
	void setFreqFloor( double x );
	/*	Set the amplitude floor (minimum instantaneous Partial  				
		frequency), in Hz, for this Analyzer.
	 */
	void setFreqResolution( double x );
	/*	Set the frequency resolution (minimum instantaneous frequency  		
		difference between Partials) for this Analyzer. (Does not cause 	
		other parameters to be recomputed.) 									
	 */
 	void setHopTime( double x );
	/*	Set the hop time (which corresponds approximately to the average
		density of Partial envelope Breakpoint data) for this Analyzer.
	 */
 	void setSidelobeLevel( double x );
	/*	Set the sidelobe attenutation level for the Kaiser analysis window in
		positive dB. More negative numbers (e.g. -90) give very good sidelobe 
		rejection but cause the window to be longer in time. Less negative 
		numbers raise the level of the sidelobes, increasing the likelihood
		of frequency-domain interference, but allow the window to be shorter
		in time.
	 */
	void setWindowWidth( double x );
	/*	Set the frequency-domain main lobe width (measured between 
		zero-crossings) of the analysis window used by this Analyzer. 				
	 */

//	-- PartialList access --
	PartialList & partials( void );
	/*	Return a mutable reference to this Analyzer's list of 
		analyzed Partials. 
	 */
	const PartialList & partials( void ) const;
	/*	Return an immutable (const) reference to this Analyzer's 
		list of analyzed Partials. 
	 */
	
};	//	end of class Analyzer

}	//	end of namespace Loris

#endif /* ndef INCLUDE_ANALYZER_H */

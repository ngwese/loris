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

#include <PartialList.h>

//	begin namespace
namespace Loris {

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
	double _resolution;	//	in Hz, minimum instantaneous frequency distance;
						//	this is the core parameter, others are, by default,
						//	computed from this one
	double _floor;		//	dB, relative to full amplitude sine wave, absolute
						//	amplitude threshold
	double _windowWidth;//	in Hz, width of main lobe; this might be more
						//	conveniently presented as window length, but
						//	the main lobe width more explicitly highlights
						//	the critical interaction with _resolution
	double _minFrequency;	//	lowest frequency (Hz) component extracted
							//	in spectral analysis
	double _drift;		//	the maximum frequency (Hz) difference between two 
						//	consecutive Breakpoints that will be linked to
						//	form a Partial
	double _hop;		//	in seconds, time between analysis windows in
						//	successive spectral analyses
	double _cropTime;	//	in seconds, maximum time correction for a spectral
						//	component to be considered reliable, and to be eligible
						//	for extraction and for Breakpoint formation
	double _bwRegionWidth;	//	width in Hz of overlapping bandwidth 
							//	association regions
							
	PartialList _partials;	//	collect Partials here
			
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
	
//	-- parameter access --
	double ampFloor( void ) const { return _floor; }
	/*	Return the amplitude floor (lowest detected spectral amplitude),  			
		in (negative) dB, for this Analyzer. 				
	 */
	double bwRegionWidth( void ) const { return _bwRegionWidth; }
	/*	Return the width (in Hz) of the Bandwidth Association regions
		used by this Analyzer.
		
		This parameter is deprecated and not generally useful. It will be
		removed in a future release.
	 */
 	double cropTime( void ) const { return _cropTime; }
	/*	Return the crop time (maximum temporal displacement of a time-
		frequency data point from the time-domain center of the analysis
		window, beyond which data points are considered "unreliable")
		for this Analyzer.
		
		This parameter is deprecated and not generally useful. It will be
		removed in a future release.
	 */	
 	double freqDrift( void ) const { return _drift;}
	/*	Return the maximum allowable frequency difference between 					
		consecutive Breakpoints in a Partial envelope for this Analyzer. 				
	 */
 	double freqFloor( void ) const { return _minFrequency; }
	/*	Return the frequency floor (minimum instantaneous Partial  				
		frequency), in Hz, for this Analyzer. 				
	 */
	double freqResolution( void ) const { return _resolution; }
	/*	Return the frequency resolution (minimum instantaneous frequency  		
		difference between Partials) for this Analyzer. 	
	 */
	double hopTime( void ) const { return _hop; }
	/*	Return the hop time (which corresponds approximately to the 
		average density of Partial envelope Breakpoint data) for this 
		Analyzer.
	 */
 	double windowWidth( void ) const { return _windowWidth; }
	/*	Return the frequency-domain main lobe width (measured between 
		zero-crossings) of the analysis window used by this Analyzer. 				
	 */
	 
//	-- parameter mutation --
 	void setAmpFloor( double x ) { _floor = x; }
	/*	Set the amplitude floor (lowest detected spectral amplitude), in  			
		(negative) dB, for this Analyzer. 				
	 */
 	void setBwRegionWidth( double x ) { _bwRegionWidth = x; }	
	/*	Set the width (in Hz) of the Bandwidth Association regions
		used by this Analyzer.
		
		This parameter is deprecated and not generally useful. It will be
		removed in a future release.
	 */
 	void setCropTime( double x ) { _cropTime = x; }
	/*	Set the crop time (maximum temporal displacement of a time-
		frequency data point from the time-domain center of the analysis
		window, beyond which data points are considered "unreliable")
		for this Analyzer.
		
		This parameter is deprecated and not generally useful. It will be
		removed in a future release.
	 */
	void setFreqDrift( double x ) { _drift = x; }
	/*	Set the maximum allowable frequency difference between 					
		consecutive Breakpoints in a Partial envelope for this Analyzer. 				
	 */
	void setFreqFloor( double x ) { _minFrequency = x; }
	/*	Set the amplitude floor (minimum instantaneous Partial  				
		frequency), in Hz, for this Analyzer.
	 */
	void setFreqResolution( double x ) { _resolution = x; }
	/*	Set the frequency resolution (minimum instantaneous frequency  		
		difference between Partials) for this Analyzer. (Does not cause 	
		other parameters to be recomputed.) 									
	 */
 	void setHopTime( double x ) { _hop = x; }
	/*	Set the hop time (which corresponds approximately to the average
		density of Partial envelope Breakpoint data) for this Analyzer.
	 */
	void setWindowWidth( double x ) { _windowWidth = x; }
	/*	Set the frequency-domain main lobe width (measured between 
		zero-crossings) of the analysis window used by this Analyzer. 				
	 */

//	-- PartialList access --
	PartialList & partials( void ) { return _partials; }
	/*	Return a mutable reference to this Analyzer's list of 
		analyzed Partials. 
	 */
	const PartialList & partials( void ) const { return _partials; }
	/*	Return an immutable (const) reference to this Analyzer's 
		list of analyzed Partials. 
	 */
	
};	//	end of class Analyzer

}	//	end of namespace Loris

#endif /* ndef INCLUDE_ANALYZER_H */

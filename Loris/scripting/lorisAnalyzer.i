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
 *	lorisAnalyzer.i
 *
 *	SWIG interface file describing the Loris::Analyzer class.
 *	Include this file in loris.i to include the Analyzer class
 *	interface in the scripting module. (Can be used with the 
 *	-shadow option to SWIG to build an Analyzer class in the 
 *	Python interface.) This file does not support exactly the 
 *	public interface of the C++ class, but has been modified to
 *	better support SWIG and scripting languages.
 *
 *
 * Kelly Fitz, 17 Nov 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
%{
#include "Analyzer.h"
#include "Partial.h"

//	for procedural interface construction and 
//	destruction, see comment below:
#define LORIS_OPAQUE_POINTERS 0
#include "loris.h"
%}


// ---------------------------------------------------------------------------
//	class Analyzer
//	
/*	An Analyzer represents a configuration of parameters for
	performing Reassigned Bandwidth-Enhanced Additive Analysis
	of sampled waveforms. This analysis process yields a collection 
	of Partials, each having a trio of synchronous, non-uniformly-
	sampled breakpoint envelopes representing the time-varying 
	frequency, amplitude, and noisiness of a single bandwidth-
	enhanced sinusoid. 

	For more information about Reassigned Bandwidth-Enhanced 
	Analysis and the Reassigned Bandwidth-Enhanced Additive Sound 
	Model, refer to the Loris website: www.cerlsoundgroup.org/Loris/
*/
class Analyzer
{
public:
//	construction:
	Analyzer( double resolutionHz );
	/*	Construct and return a new Analyzer configured with the given	
		frequency resolution (minimum instantaneous frequency	
		difference between Partials). All other Analyzer parameters 	
		are computed from the specified frequency resolution. 			
	 */
		 
	~Analyzer( void );
	/*	Destroy this Analyzer. 								
	 */	
	 
%addmethods 
{
//	copying:
	%new Analyzer * copy( void )
	{
		return new Analyzer( self->freqResolution() );
	}
	/*	Construct and return a new Analyzer having identical
		parameter configuration to another Analyzer.			
	 */

//	analysis:
	%new 
	PartialList * analyze( const SampleVector * vec, double srate )
	{
		PartialList * partials = new PartialList();
		self->analyze( vec->begin(), vec->end(), srate );
		partials->splice( partials->end(), self->partials() );
		return partials;
	}
	/*	Analyze a SampleVector of (mono) samples at the given sample rate 	  	
		(in Hz) and return the resulting Partials in a PartialList. 												
	 */
}
	
//	configuration:
	void configure( double resolutionHz );
	/*	Configure this Analyzer with the given frequency resolution 
		(minimum instantaneous frequency difference between Partials). 
		All other Analyzer parameters are (re-)computed from the 
		frequency resolution. 		
	 */
	
//	parameter access:
	double freqResolution( void ) const { return _resolution; }
	/*	Return the frequency resolution (minimum instantaneous frequency  		
		difference between Partials) for this Analyzer. 	
	 */
	double ampFloor( void ) const { return _floor; }
	/*	Return the amplitude floor (lowest detected spectral amplitude),  			
		in (negative) dB, for this Analyzer. 				
	 */
 	double windowWidth( void ) const { return _windowWidth; }
	/*	Return the frequency-domain main lobe width (measured between 
		zero-crossings) of the analysis window used by this Analyzer. 				
	 */
 	double freqFloor( void ) const { return _minFrequency; }
	/*	Return the frequency floor (minimum instantaneous Partial  				
		frequency), in Hz, for this Analyzer. 				
	 */
	double hopTime( void ) const { return _hop; }
	/*	Return the hop time (which corresponds approximately to the 
		average density of Partial envelope Breakpoint data) for this 
		Analyzer.
	 */
 	double freqDrift( void ) const { return _drift;}
	/*	Return the maximum allowable frequency difference between 					
		consecutive Breakpoints in a Partial envelope for this Analyzer. 				
	 */
 	double cropTime( void ) const { return _cropTime; }
	/*	Return the crop time (maximum temporal displacement of a time-
		frequency data point from the time-domain center of the analysis
		window, beyond which data points are considered "unreliable")
		for this Analyzer.
	 */
	double bwRegionWidth( void ) const { return _bwRegionWidth; }
	/*	Return the width (in Hz) of the Bandwidth Association regions
		used by this Analyzer.
	 */
	
//	parameter mutation:
	void setFreqResolution( double x ) { _resolution = x; }
	/*	Set the frequency resolution (minimum instantaneous frequency  		
		difference between Partials) for this Analyzer. (Does not cause 	
		other parameters to be recomputed.) 									
	 */
	 	void setAmpFloor( double x ) { _floor = x; }
	/*	Set the amplitude floor (lowest detected spectral amplitude), in  			
		(negative) dB, for this Analyzer. 				
	 */
	void setWindowWidth( double x ) { _windowWidth = x; }
	/*	Set the frequency-domain main lobe width (measured between 
		zero-crossings) of the analysis window used by this Analyzer. 				
	 */
	void setFreqFloor( double x ) { _minFrequency = x; }
	/*	Set the amplitude floor (minimum instantaneous Partial  				
		frequency), in Hz, for this Analyzer.
	 */
	void setFreqDrift( double x ) { _drift = x; }
	/*	Set the maximum allowable frequency difference between 					
		consecutive Breakpoints in a Partial envelope for this Analyzer. 				
	 */
 	void setHopTime( double x ) { _hop = x; }
	/*	Set the hop time (which corresponds approximately to the average
		density of Partial envelope Breakpoint data) for this Analyzer.
	 */
 	void setCropTime( double x ) { _cropTime = x; }
	/*	Set the crop time (maximum temporal displacement of a time-
		frequency data point from the time-domain center of the analysis
		window, beyond which data points are considered "unreliable")
		for this Analyzer.
	 */
 	void setBwRegionWidth( double x ) { _bwRegionWidth = x; }	
	/*	Set the width (in Hz) of the Bandwidth Association regions
		used by this Analyzer.
	 */

};	//	end of class Analyzer

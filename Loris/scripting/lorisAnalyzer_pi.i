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
 *	lorisAnalyzer_pi.i
 *
 *	SWIG interface file supporting the Loris procedural inteface
 *	declared in loris.h. Specifically, this interface file describes
 *	the procedural interface for the Analyzer class, implemented in
 *	lorisAnalyzer_pi.C. Include this file in loris.i to include
 *	the Analyzer procedural interface in the scripting module.
 *
 *	The SWIG interface (loris.i) uses the class interface in 
 *	lorisAnalyzer.i rather than this more cumbersome procedural 
 *	interface, so this file is no longer used.
 *
 *
 * Kelly Fitz, 13 Nov 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

/* ---------------------------------------------------------------- */
/*		Analyzer object interface
/*
 */
%section "Analyzer object interface"
%text %{
	An Analyzer represents a configuration of parameters for
	performing Reassigned Bandwidth-Enhanced Additive Analysis
	of sampled waveforms. This analysis process yields a collection 
	of Partials, each having a trio of synchronous, non-uniformly-
	sampled breakpoint envelopes representing the time-varying 
	frequency, amplitude, and noisiness of a single bandwidth-
	enhanced sinusoid. 

	For more information about Reassigned Bandwidth-Enhanced 
	Analysis and the Reassigned Bandwidth-Enhanced Additive Sound 
	Model, refer to the Loris website: www.cerlsoundgroup.org/Loris/
%}

Analyzer * createAnalyzer( double resolution );
/*	Construct and return a new Analyzer configured with the given	
	frequency resolution (minimum instantaneous frequency	
	difference between Partials). All other Analyzer parameters 	
	are computed from the specified frequency resolution. 			
 */
void destroyAnalyzer( Analyzer * ptr_this );
/*	Destroy this Analyzer. 								
 */
void analyzer_analyze( Analyzer * ptr_this, 
					   const SampleVector * vec, double srate,
					   PartialList * partials );
/*	Analyze a vector of (mono) samples at the given sample rate 	  	
	(in Hz) and append the extracted Partials to the given 
	PartialList. 												
 */
	
void analyzer_configure( Analyzer * ptr_this, double resolution );
/*	Configure this Analyzer with the given frequency resolution 
	(minimum instantaneous frequency difference between Partials). 
	All other Analyzer parameters are (re-)computed from the 
	frequency resolution. 		
 */
double analyzer_getFreqResolution( const Analyzer * ptr_this );
/*	Return the frequency resolution (minimum instantaneous frequency  		
	difference between Partials) for this Analyzer. 	
 */
void analyzer_setFreqResolution( Analyzer * ptr_this, double x );
/*	Set the frequency resolution (minimum instantaneous frequency  		
	difference between Partials) for this Analyzer. (Does not cause 	
	other parameters to be recomputed.) 									
 */
double analyzer_getAmpFloor( const Analyzer * ptr_this );
/*	Return the amplitude floor (lowest detected spectral amplitude),  			
	in (negative) dB, for this Analyzer. 				
 */
void analyzer_setAmpFloor( Analyzer * ptr_this, double x );
/*	Set the amplitude floor (lowest detected spectral amplitude), in  			
	(negative) dB, for this Analyzer. 				
 */
double analyzer_getWindowWidth( const Analyzer * ptr_this );
/*	Return the frequency-domain main lobe width (measured between 
	zero-crossings) of the analysis window used by this Analyzer. 				
 */
void analyzer_setWindowWidth( Analyzer * ptr_this, double x );
/*	Set the frequency-domain main lobe width (measured between 
	zero-crossings) of the analysis window used by this Analyzer. 				
 */
double analyzer_getFreqFloor( const Analyzer * ptr_this );
/*	Return the frequency floor (minimum instantaneous Partial  				
	frequency), in Hz, for this Analyzer. 				
 */
void analyzer_setFreqFloor( Analyzer * ptr_this, double x );
/*	Set the amplitude floor (minimum instantaneous Partial  				
	frequency), in Hz, for this Analyzer.
 */
double analyzer_getFreqDrift( const Analyzer * ptr_this );
/*	Return the maximum allowable frequency difference between 					
	consecutive Breakpoints in a Partial envelope for this Analyzer. 				
 */
void analyzer_setFreqDrift( Analyzer * ptr_this, double x );
/*	Set the maximum allowable frequency difference between 					
	consecutive Breakpoints in a Partial envelope for this Analyzer. 				
 */
double analyzer_getHopTime( const Analyzer * ptr_this );
/*	Return the hop time (which corresponds approximately to the 
	average density of Partial envelope Breakpoint data) for this 
	Analyzer.
 */
void analyzer_setHopTime( Analyzer * ptr_this, double x );
/*	Set the hop time (which corresponds approximately to the average
	density of Partial envelope Breakpoint data) for this Analyzer.
 */
double analyzer_getCropTime( const Analyzer * ptr_this );
/*	Return the crop time (maximum temporal displacement of a time-
	frequency data point from the time-domain center of the analysis
	window, beyond which data points are considered "unreliable")
	for this Analyzer.
 */
void analyzer_setCropTime( Analyzer * ptr_this, double x );
/*	Set the crop time (maximum temporal displacement of a time-
	frequency data point from the time-domain center of the analysis
	window, beyond which data points are considered "unreliable")
	for this Analyzer.
 */
double analyzer_getBwRegionWidth( const Analyzer * ptr_this );
/*	Return the width (in Hz) of the Bandwidth Association regions
	used by this Analyzer.
 */
void analyzer_setBwRegionWidth( Analyzer * ptr_this, double x );
/*	Set the width (in Hz) of the Bandwidth Association regions
	used by this Analyzer.
 */


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
 * Analyzer.C
 *
 * Implementation of class Loris::Analyzer.
 *
 * Kelly Fitz, 5 Dec 99
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include "config.h"
#endif

#include "Analyzer.h"

#include "AssociateBandwidth.h"
#include "Breakpoint.h"
#include "BreakpointEnvelope.h"
#include "Envelope.h"
#include "Exception.h"
#include "KaiserWindow.h"
#include "Notifier.h"
#include "Partial.h"
#include "PartialPtrs.h"
#include "ReassignedSpectrum.h"
#include "SpectralPeakSelector.h"
#include "PartialBuilder.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>
#include <vector>

using namespace std;

#if defined(HAVE_M_PI) && (HAVE_M_PI)
	const double Pi = M_PI;
#else
	const double Pi = 3.14159265358979324;
#endif

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	class Analyzer
//
//!	@class Analyzer Analyzer.h loris/Analyzer.h
//!	
//!	Class Analyzer represents a configuration of parameters for
//!	performing Reassigned Bandwidth-Enhanced Additive Analysis
//!	of sampled sounds. The analysis process yields a collection 
//!	of Partials, each having a trio of synchronous, non-uniformly-
//!	sampled breakpoint envelopes representing the time-varying 
//!	frequency, amplitude, and noisiness of a single bandwidth-
//!	enhanced sinusoid. These Partials are accumulated in the
//! Analyzer.
//!
//!	The core analysis parameter is the frequency resolution, the minimum
//!	instantaneous frequency spacing between partials. All other
//!	parameters are initially configured according to this parameter
//!	(and the analysis window width, if specified).
//!	Subsequent parameter mutations are independent.
//!	
//!	For more information about Reassigned Bandwidth-Enhanced 
//!	Analysis and the Reassigned Bandwidth-Enhanced Additive Sound 
//!	Model, refer to the Loris website: www.cerlsoundgroup.org/Loris/.

// ---------------------------------------------------------------------------
//	Analyzer_imp definition
// ---------------------------------------------------------------------------
//	Definition of an implementation class that insulates clients from
//	the implementation and representation details of Analyzer parameters.
//
struct Analyzer_imp
{
	double freqResolution;	//	in Hz, minimum instantaneous frequency distance;
							//	this is the core parameter, others are, by default,
							//	computed from this one
	
	double ampFloor;		//	dB, relative to full amplitude sine wave, absolute
							//	amplitude threshold (negative)
	
	double windowWidth;		//	in Hz, width of main lobe; this might be more
							//	conveniently presented as window length, but
							//	the main lobe width more explicitly highlights
							//	the critical interaction with resolution
	
	double freqFloor;		//	lowest frequency (Hz) component extracted
							//	in spectral analysis
	
	double freqDrift;		//	the maximum frequency (Hz) difference between two 
							//	consecutive Breakpoints that will be linked to
							//	form a Partial
	
	double hopTime;			//	in seconds, time between analysis windows in
							//	successive spectral analyses
	
	double cropTime;		//	in seconds, maximum time correction for a spectral
							//	component to be considered reliable, and to be eligible
							//	for extraction and for Breakpoint formation
	
	double bwRegionWidth;	//	width in Hz of overlapping bandwidth 
							//	association regions, or zero if bandwidth association
							//	is disabled
                            							
    double sidelobeLevel;	//	sidelobe attenutation level for the Kaiser analysis 
    				 		//	window, in positive dB
							
	PartialList partials;	//	collect Partials here
	
	//	parameter verification, except if parameters are invalid:
	void verify_params( void ) const;

};

// ---------------------------------------------------------------------------
//	Analyzer constructor - frequency resolution only
// ---------------------------------------------------------------------------
//!	Construct a new Analyzer configured with the given	
//!	frequency resolution (minimum instantaneous frequency	
//!	difference between Partials). All other Analyzer parameters 	
//!	are computed from the specified frequency resolution. 	
//!	
//!	@param resolutionHz is the frequency resolution in Hz.
//
Analyzer::Analyzer( double resolutionHz ) :
	_imp( new Analyzer_imp )
{
	configure( resolutionHz, resolutionHz );
}

// ---------------------------------------------------------------------------
//	Analyzer constructor
// ---------------------------------------------------------------------------
//!	Construct a new Analyzer configured with the given	
//!	frequency resolution (minimum instantaneous frequency	
//!	difference between Partials) and analysis window width
//!	(main lobe, zero-to-zero). All other Analyzer parameters 	
//!	are computed from the specified resolution and window width. 	
//!	
//!	@param resolutionHz is the frequency resolution in Hz.
//!	@param windowWidthHz is the main lobe width of the Kaiser
//!	analysis window in Hz.
//
Analyzer::Analyzer( double resolutionHz, double windowWidthHz ) :
	_imp( new Analyzer_imp )
{
	configure( resolutionHz, windowWidthHz );
}

// ---------------------------------------------------------------------------
//	Analyzer copy constructor
// ---------------------------------------------------------------------------
//!	Construct  a new Analyzer having identical
//!	parameter configuration to another Analyzer. 
//!	The list of collected Partials is not copied. 		
//!	
//!	@param other is the Analyzer to copy.	
//
Analyzer::Analyzer( const Analyzer & other ) :
	_imp( new Analyzer_imp( *other._imp ) )
{
}

// ---------------------------------------------------------------------------
//	Analyzer assignment
// ---------------------------------------------------------------------------
//!	Construct  a new Analyzer having identical
//!	parameter configuration to another Analyzer. 
//!	The list of collected Partials is not copied. 		
//!	
//!	@param rhs is the Analyzer to copy.	
//
Analyzer & 
Analyzer::operator=( const Analyzer & rhs )
{
	if ( this != & rhs ) 
	{
		*_imp = *rhs._imp;
	}
	return *this;
}

// ---------------------------------------------------------------------------
//	Analyzer destructor
// ---------------------------------------------------------------------------
//!	Destroy this Analyzer.
//
Analyzer::~Analyzer( void )
{
}

#pragma mark -- configuration --

// ---------------------------------------------------------------------------
//	configure
// ---------------------------------------------------------------------------
//!	Configure this Analyzer with the given frequency resolution 
//!	(minimum instantaneous frequency difference between Partials)
//!	and analysis window width (main lobe, zero-to-zero, in Hz). 
//!	All other Analyzer parameters are (re-)computed from the 
//!	frequency resolution and window width. 		
//!	
//!	@param resolutionHz is the frequency resolution in Hz.
//!	@param windowWidthHz is the main lobe width of the Kaiser
//!	analysis window in Hz.
//!		
//!	There are three categories of analysis parameters:
//!	- the resolution, and params that are usually related to (or
//!	identical to) the resolution (frequency floor and drift)
//!	- the window width and params that are usually related to (or
//!	identical to) the window width (hop and crop times)
//!	- independent parameters (bw region width and amp floor)
//
void
Analyzer::configure( double resolutionHz, double windowWidthHz )
{
	Assert( _imp.get() != 0 );
		
	//	use specified resolution:
	setFreqResolution( resolutionHz );
	
	//	floor defaults to -90 dB:
	setAmpFloor( -90. );
	
	//	window width should generally be approximately 
	//	equal to, and never more than twice the 
	//	frequency resolution:
	setWindowWidth( windowWidthHz );
	
	//	the Kaiser window sidelobe level can be the same
	//	as the amplitude floor (except in positive dB):
	setSidelobeLevel( - _imp->ampFloor );
	
	//	for the minimum frequency, below which no data is kept,
	//	use the frequency resolution by default (this makes 
	//	Lip happy, and is always safe?) and allow the client 
	//	to change it to anything at all.
	setFreqFloor( _imp->freqResolution );
	
	//	frequency drift in Hz is the maximum difference
	//	in frequency between consecutive Breakpoints in
	//	a Partial, by default, make it equal to one half
	//	the frequency resolution:
	setFreqDrift( .5 * _imp->freqResolution );
	
	//	hop time (in seconds) is the inverse of the
	//	window width....really. Smith and Serra (1990) cite 
	//	Allen (1977) saying: a good choice of hop is the window 
	//	length divided by the main lobe width in frequency samples,
	//	which turns out to be just the inverse of the width.
	setHopTime( 1. / _imp->windowWidth );
	
	//	crop time (in seconds) is the maximum allowable time
	//	correction, beyond which a reassigned spectral component
	//	is considered unreliable, and not considered eligible for
	//	Breakpoint formation in extractPeaks(). By default, use
	//	the hop time (should it be half that?):
	setCropTime( _imp->hopTime );
	
	//	bandwidth association region width 
	//	defaults to 2 kHz, corresponding to 
	//	1 kHz region center spacing:
	setBwRegionWidth( 2000. );
}

#pragma mark -- analysis --
// ---------------------------------------------------------------------------
//	analyze
// ---------------------------------------------------------------------------
//!	Analyze a vector of (mono) samples at the given sample rate 	  	
//!	(in Hz) and append the extracted Partials to Analyzer's 
//!	PartialList (std::list of Partials).	
//!	
//!	@param vec is a vector of floating point samples
//!	@param srate is the sample rate of the samples in the vector 
//
void 
Analyzer::analyze( const std::vector<double> & vec, double srate )		
{ 
	BreakpointEnvelope reference( 1.0 );
	analyze( &(vec[0]),  &(vec[0]) + vec.size(), srate, reference ); 
}

// ---------------------------------------------------------------------------
//	analyze
// ---------------------------------------------------------------------------
//!	Analyze a range of (mono) samples at the given sample rate 	  	
//!	(in Hz) and collect the resulting Partials.	
//!	
//!	@param bufBegin is a pointer to a buffer of floating point samples
//!	@param bufEnd is (one-past) the end of a buffer of floating point 
//!	samples
//!	@param srate is the sample rate of the samples in the buffer
//
void 
Analyzer::analyze( const double * bufBegin, const double * bufEnd, double srate )
{ 
	BreakpointEnvelope reference( 1.0 );
	analyze( bufBegin,  bufEnd, srate, reference ); 
}

// ---------------------------------------------------------------------------
//	analyze
// ---------------------------------------------------------------------------
//!	Analyze a vector of (mono) samples at the given sample rate 	  	
//!	(in Hz) and append the extracted Partials to Analyzer's 
//!	PartialList (std::list of Partials). Use the specified envelope
//!	as a frequency reference for Partial tracking.
//!
//!	@param vec is a vector of floating point samples
//!	@param srate is the sample rate of the samples in the vector
//!	@param reference is an Envelope having the approximate
//!	frequency contour expected of the resulting Partials.
//
void 
Analyzer::analyze( const std::vector<double> & vec, double srate, 
				   const Envelope & reference )		
{ 
	analyze( &(vec[0]),  &(vec[0]) + vec.size(), srate, reference ); 
}

// ---------------------------------------------------------------------------
//	analyze
// ---------------------------------------------------------------------------
//!	Analyze a range of (mono) samples at the given sample rate 	  	
//!	(in Hz) and append the extracted Partials to Analyzer's 
//!	PartialList (std::list of Partials). Use the specified envelope
//!	as a frequency reference for Partial tracking.
//!	
//!	@param bufBegin is a pointer to a buffer of floating point samples
//!	@param bufEnd is (one-past) the end of a buffer of floating point 
//!	samples
//!	@param srate is the sample rate of the samples in the buffer
//!	@param reference is an Envelope having the approximate
//!	frequency contour expected of the resulting Partials.
//
void 
Analyzer::analyze( const double * bufBegin, const double * bufEnd, double srate,
				   const Envelope & reference )
{ 
 	//	configure the reassigned spectral analyzer, 
    //	always use odd-length windows:

    /*	Kaiser window */
    double winshape = KaiserWindow::computeShape( sidelobeLevel() );
    long winlen = KaiserWindow::computeLength( windowWidth() / srate, winshape );    
    if (! (winlen % 2)) 
        ++winlen;
    debugger << "Using Kaiser window of length " << winlen << endl;
    
    std::vector< double > window( winlen );
    KaiserWindow::create( window, winshape );
   
 	ReassignedSpectrum spectrum( window );
	
    //	configure the peak selection and partial formation policies:
    SpectralPeakSelector selector( srate, _imp->freqResolution );
    PartialBuilder builder( _imp->freqDrift, reference );
 	
	//	configure bw association policy, unless
	//	bandwidth association is disabled:
	std::auto_ptr< AssociateBandwidth > bwAssociator;
	if( associateBandwidth() )
	{
		debugger << "Using bandwidth association regions of width " 
				 << bwRegionWidth() << " Hz" << endl;
		bwAssociator.reset( new AssociateBandwidth( bwRegionWidth(), srate ) );
	}
	else
	{
		debugger << "Bandwidth association disabled" << endl;
	}

	try 
	{ 
		//	loop over short-time analysis frames:
		const double * winMiddle = bufBegin; 
		while ( winMiddle < bufEnd )
		{
			//	compute the time of this analysis frame:
			double currentFrameTime = long(winMiddle - bufBegin) / srate;
			 
			//	compute reassigned spectrum:
			//  sampsBegin is the position of the first sample to be transformed,
			//	sampsEnd is the position after the last sample to be transformed.
			//	(these computations work for odd length windows only)
			const double * sampsBegin = std::max( winMiddle - (winlen / 2), bufBegin );
			const double * sampsEnd = std::min( winMiddle + (winlen / 2) + 1, bufEnd );
			spectrum.transform( sampsBegin, winMiddle, sampsEnd );
			
			//	extract peaks from the spectrum, thin and 
			//	fade quiet peaks out over 10 dB:
			#define FADE 10.
			Peaks & peaks = selector.extractPeaks( spectrum, _imp->freqFloor, _imp->cropTime );	
			Peaks::iterator rejected = selector.thinPeaks( _imp->ampFloor, FADE, currentFrameTime );

			if ( associateBandwidth() )
			{
				bwAssociator->associateBandwidth( peaks.begin(), rejected, peaks.end() );
			}
			
			//	remove rejected Breakpoints:
			peaks.erase( rejected, peaks.end() );

			//	form Partials from the extracted Breakpoints:
			builder.formPartials( peaks, currentFrameTime );
			
			//	slide the analysis window:
			winMiddle += long( _imp->hopTime * srate ); //	hop in samples, truncated

		}	//	end of loop over short-time frames
		
		//	unwarp the Partial frequency envelopes:
		builder.fixPartialFrequencies();
		_imp->partials.splice( _imp->partials.end(), builder.partials() );
	}
	catch ( Exception & ex ) 
	{
		ex.append( "analysis failed." );
		throw;
	}
}

#pragma mark -- parameter access --

// ---------------------------------------------------------------------------
//	ampFloor
// ---------------------------------------------------------------------------
//!	Return the amplitude floor (lowest detected spectral amplitude),  			
//!	in (negative) dB, for this Analyzer. 				
//
double 
Analyzer::ampFloor( void ) const 
{ 
    return _imp->ampFloor; 
}

// ---------------------------------------------------------------------------
//	associateBandwidth
// ---------------------------------------------------------------------------
//!	Return true if this Analyzer is configured to peform bandwidth
//!	association to distribute noise energy among extracted Partials, 
//!	and false if noise energy will be collected in noise Partials,
//!	labeled -1 in this Analyzer's PartialList.
//
bool 
Analyzer::associateBandwidth( void ) const 
{ 
    return _imp->bwRegionWidth > 0.; 
}

// ---------------------------------------------------------------------------
//	bwRegionWidth
// ---------------------------------------------------------------------------
//!	Return the width (in Hz) of the Bandwidth Association regions
//!	used by this Analyzer.
//
double 
Analyzer::bwRegionWidth( void ) const
{ 
    // debugger << "Analyzer::bwRegionWidth() is a deprecated member, and will be removed in a future Loris release." << endl;
    return _imp->bwRegionWidth; 
}

// ---------------------------------------------------------------------------
//	cropTime
// ---------------------------------------------------------------------------
//!	Return the crop time (maximum temporal displacement of a time-
//!	frequency data point from the time-domain center of the analysis
//!	window, beyond which data points are considered "unreliable")
//!	for this Analyzer.
//
double 
Analyzer::cropTime( void ) const 
{ 
    // debugger << "Analyzer::cropTime() is a deprecated member, and will be removed in a future Loris release." << endl;
    return _imp->cropTime; 
}

// ---------------------------------------------------------------------------
//	freqDrift
// ---------------------------------------------------------------------------
//!	Return the maximum allowable frequency difference 
//!	consecutive Breakpoints in a Partial envelope for this Analyzer. 				
//
double 
Analyzer::freqDrift( void ) const 
{ 
	return _imp->freqDrift;
}

// ---------------------------------------------------------------------------
//	freqFloor
// ---------------------------------------------------------------------------
//!	Return the frequency floor (minimum instantaneous Partial  				
//!	frequency), in Hz, for this Analyzer. 				
//
double 
Analyzer::freqFloor( void ) const 
{ 
	return _imp->freqFloor; 
}

// ---------------------------------------------------------------------------
//	freqResolution
// ---------------------------------------------------------------------------
//!	Return the frequency resolution (minimum instantaneous frequency  		
//!	difference between Partials) for this Analyzer.
//
double 
Analyzer::freqResolution( void ) const 
{ 
	return _imp->freqResolution; 
}

// ---------------------------------------------------------------------------
//	hopTime
// ---------------------------------------------------------------------------
//!	Return the hop time (which corresponds approximately to the 
//!	average density of Partial envelope Breakpoint data) for this 
//!	Analyzer.
//
double 
Analyzer::hopTime( void ) const 
{ 
	return _imp->hopTime; 
}

// ---------------------------------------------------------------------------
//	sidelobeLevel
// ---------------------------------------------------------------------------
//!	Return the sidelobe attenutation level for the Kaiser analysis window in
//!	positive dB. Larger numbers (e.g. 90) give very good sidelobe 
//!	rejection but cause the window to be longer in time. Smaller numbers 
//!	(like 60) raise the level of the sidelobes, increasing the likelihood
//!	of frequency-domain interference, but allow the window to be shorter
//!	in time.
//
double 
Analyzer::sidelobeLevel( void ) const 
{ 
	return _imp->sidelobeLevel; 
}

// ---------------------------------------------------------------------------
//	windowWidth
// ---------------------------------------------------------------------------
//!	Return the frequency-domain main lobe width (measured between 
//!	zero-crossings) of the analysis window used by this Analyzer. 				
//
double 
Analyzer::windowWidth( void ) const 
{ 
	return _imp->windowWidth; 
}

#pragma mark -- parameter mutation --

#define VERIFY_ARG(func, test)											\
	do {																\
		if (!(test)) 													\
			Throw( Loris::InvalidArgument, #func ": " #test  );			\
	} while (false)


// ---------------------------------------------------------------------------
//	setAmpFloor
// ---------------------------------------------------------------------------
//!	Set the amplitude floor (lowest detected spectral amplitude), in  			
//!	(negative) dB, for this Analyzer. 
//!	
//!	@param x is the new value of this parameter. 				
//
void 
Analyzer::setAmpFloor( double x ) 
{ 
	VERIFY_ARG( setAmpFloor, x < 0 );
	_imp->ampFloor = x; 
}

// ---------------------------------------------------------------------------
//	setBwRegionWidth
// ---------------------------------------------------------------------------
//!	Set the width (in Hz) of the Bandwidth Association regions
//!	used by this Analyzer. If zero, bandwidth enhancement is 
//!	disabled.
//!	
//!	@param x is the new value of this parameter.
//
void 
Analyzer::setBwRegionWidth( double x ) 
{ 
	VERIFY_ARG( setBwRegionWidth, x >= 0 );
   // debugger << "Analyzer::setBwRegionWidth() is a deprecated member, and will be removed in a future Loris release." << endl;
    _imp->bwRegionWidth = x; 
}	

// ---------------------------------------------------------------------------
//	setCropTime
// ---------------------------------------------------------------------------
//!	Set the crop time (maximum temporal displacement of a time-
//!	frequency data point from the time-domain center of the analysis
//!	window, beyond which data points are considered "unreliable")
//!	for this Analyzer.
//!	
//!	@param x is the new value of this parameter.
//
void 
Analyzer::setCropTime( double x ) 
{ 
 	VERIFY_ARG( setCropTime, x > 0 );
   // debugger << "Analyzer::setCropTime() is a deprecated member, and will be removed in a future Loris release." << endl;
    _imp->cropTime = x; 
}

// ---------------------------------------------------------------------------
//	setFreqDrift
// ---------------------------------------------------------------------------
//!	Set the maximum allowable frequency difference between 					
//!	consecutive Breakpoints in a Partial envelope for this Analyzer. 				
//!	
//!	@param x is the new value of this parameter.			
//
void 
Analyzer::setFreqDrift( double x ) 
{ 
	VERIFY_ARG( setFreqDrift, x > 0 );
	_imp->freqDrift = x; 
}

// ---------------------------------------------------------------------------
//	setFreqFloor
// ---------------------------------------------------------------------------
//!	Set the frequency floor (minimum instantaneous Partial  				
//!	frequency), in Hz, for this Analyzer.
//!	
//!	@param x is the new value of this parameter.					
//
void 
Analyzer::setFreqFloor( double x ) 
{ 
	VERIFY_ARG( setFreqFloor, x >= 0 );
	_imp->freqFloor = x; 
}

// ---------------------------------------------------------------------------
//	setFreqResolution
// ---------------------------------------------------------------------------
//!	Set the frequency resolution (minimum instantaneous frequency  		
//!	difference between Partials) for this Analyzer. (Does not cause 	
//!	other parameters to be recomputed.) 									
//!	
//!	@param x is the new value of this parameter.										
//
void 
Analyzer::setFreqResolution( double x ) 
{ 
	VERIFY_ARG( setFreqResolution, x > 0 );
	_imp->freqResolution = x; 
}

// ---------------------------------------------------------------------------
//	setSidelobeLevel
// ---------------------------------------------------------------------------
//!	Set the sidelobe attenutation level for the Kaiser analysis window in
//!	positive dB. More negative numbers (e.g. -90) give very good sidelobe 
//!	rejection but cause the window to be longer in time. Less negative 
//!	numbers raise the level of the sidelobes, increasing the likelihood
//!	of frequency-domain interference, but allow the window to be shorter
//!	in time.
//!	
//!	@param x is the new value of this parameter.	
//
void 
Analyzer::setSidelobeLevel( double x ) 
{ 
	VERIFY_ARG( setSidelobeLevel, x > 0 );
	_imp->sidelobeLevel = x; 
}

// ---------------------------------------------------------------------------
//	setHopTime
// ---------------------------------------------------------------------------
//!	Set the hop time (which corresponds approximately to the average
//!	density of Partial envelope Breakpoint data) for this Analyzer.
//!	
//!	@param x is the new value of this parameter.
//
void 
Analyzer::setHopTime( double x ) 
{ 
	VERIFY_ARG( setHopTime, x > 0 );
	_imp->hopTime = x; 
}

// ---------------------------------------------------------------------------
//	setWindowWidth
// ---------------------------------------------------------------------------
//!	Set the frequency-domain main lobe width (measured between 
//!	zero-crossings) of the analysis window used by this Analyzer. 	
//!	
//!	@param x is the new value of this parameter.			
//
void 
Analyzer::setWindowWidth( double x ) 
{ 
	VERIFY_ARG( setWindowWidth, x > 0 );
	_imp->windowWidth = x; 
}

#pragma mark -- PartialList access --

// ---------------------------------------------------------------------------
//	partials
// ---------------------------------------------------------------------------
//!	Return a mutable reference to this Analyzer's list of 
//!	analyzed Partials. 
//
PartialList & 
Analyzer::partials( void ) 
{ 
	return _imp->partials; 
}

// ---------------------------------------------------------------------------
//	partials
// ---------------------------------------------------------------------------
//!	Return an immutable (const) reference to this Analyzer's 
//!	list of analyzed Partials. 
//
const PartialList & 
Analyzer::partials( void ) const
{ 
	return _imp->partials; 
}

}	//	end of namespace Loris

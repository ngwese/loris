/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2008 by Kelly Fitz and Lippold Haken
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
 *	lorisFundamental.i
 *
 *  SWIG interface file describing FundamentalEstimator classes
 *	Include this file in loris.i to include the FundamentalFromSamples
 *	and FundamentalFromPartials classes interface in the scripting module. 
 *
 * Kelly Fitz, 13 Jan 2009
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

/* ***************** inserted C++ code ***************** */
%{

#include <Fundamental.h>

using Loris::F0Estimate;
using Loris::FundamentalFromPartials;

%}
/* ***************** end of inserted C++ code ***************** */


// ---------------------------------------------------------------------------
//  class F0Estimate
//
//	This class is included only because it is the return type for
//	the FundamentalEstimator classes valueAt members.

%feature("docstring",
"Represents a configuration of an iterative alrogithm for computing an 
estimate of fundamental frequency from a sequence of sinusoidal
frequencies and amplitudes using a likelihood estimator adapted
from Quatieri's Speech Signal Processing text.") F0Estimate;

class F0Estimate
{
public:
    //  --- lifecycle ---

%feature("docstring",
"Construct from parameters of the iterative F0 estimation 
algorithm. Find candidate F0 estimates as integer divisors
of the peak frequencies, pick the highest frequency of the
most likely candidates, and refine that estiamte using the
secant method. 

Store the frequency and the normalized value of the 
likelihood function at that frequency (1.0 indicates that
all the peaks are perfect harmonics of the estimated
frequency).") F0Estimate;

    F0Estimate( const std::vector<double> & amps, 
                const std::vector<double> & freqs, 
                double fmin, double fmax,
                double resolution );
                

    //  --- accessors ---
    
    
%feature("docstring",
"Return the F0 frequency estimate, in Hz, for this estimate.") frequency;
        
    double frequency( void ) const;
    
        
%feature("docstring",
"Return the normalized confidence for this estimate, 
equal to 1.0 when all frequencies are perfect
harmonics of this estimate's frequency.") confidence;
        
    double confidence( void ) const;
    
                    
};  //  end of class F0Estimate


// ---------------------------------------------------------------------------
//  class FundamentalEstimator
//

%nodefaultctor FundamentalEstimator;  // Disable the default constructor for FundamentalEstimator.


%feature("docstring",
"Base class for fundamental estimation, common storage for member
variable parameters, type definitions, and constants.") FundamentalEstimator;


class FundamentalEstimator
{
public:

//  -- types --

    typedef F0Estimate value_type;
    
//  -- constants --    

    enum {
    
        DefaultAmpFloor = -60,          //! the default absolute amplitude threshold in dB
        
        DefaultAmpRange = 30,           //! the default floating amplitude threshold in dB
    
        DefaultFreqCeiling = 4000,      //! the default frequency threshold in Hz
    
        DefaultPrecisionOver100 = 10,   //! the default frequency precision in 1/100 Hz

        DefaultMinConfidencePct = 90    //! the default required percent confidence to
                                        //! return an estimate (100 is absolute confidence)
    };    

};  //  end of base class FundamentalEstimator



// ---------------------------------------------------------------------------
//  class FundamentalFromPartials
//

%feature("docstring",
"Class FundamentalFromPartials represents an algorithm for 
time-varying fundamental frequency estimation from instantaneous
Partial amplitudes and frequencies based on a likelihood
estimator adapted from Quatieri's Speech Signal Processing text.") FundamentalFromPartials;

class FundamentalFromPartials : public FundamentalEstimator
{
public:

//  -- lifecycle --

%feature("docstring",
"Construct a new estimator.

The specified precision is used to terminate the iterative
estimation procedure. If unspecified, the default value,
DefaultPrecisionOver100 * 100 is used. 

precisionHz is the precision in Hz with which the 
fundamental estimates will be made.") FundamentalFromPartials;    

	FundamentalFromPartials( double precisionHz = DefaultPrecisionOver100 * 0.01 );


//  -- fundamental frequency estimation --



%feature("docstring",
"Construct a linear envelope from fundamental frequency 
estimates taken at the specified interval in seconds 
starting at tbeg (seconds) and ending before tend (seconds).

	partials is the sequence of Partials
	
	tbeg is the beginning of the time interval (in seconds)
	
	tend is the end of the time interval (in seconds)
	
	interval is the time between breakpoints in the
	fundamental frequency envelope (in seconds)
	
	lowerFreqBound is the lower bound on the fundamental
	frequency estimate (in Hz)
	
	upperFreqBound is the lower bound on the fundamental
	frequency estimate (in Hz)
	
	confidenceThreshold is the minimum confidence level
	resuired for a fundamental frequency estimate to be
	added to the envelope. Lower confidence estimates are
	not added, the envelope returned will not contain
	breakpoints at times associated with low confidence 
	estimates
	
	Returns a LinearEnvelope composed of breakpoints corresponding to
	the fundamental frequency estimates at samples of the span
	tbeg to tend at the specified sampling interval, only estimates
	having confidence level exceeding the specified confidence
	threshold are added to the envelope
") buildEnvelope;
        
     
    LinearEnvelope buildEnvelope( const PartialList & partials, 
                                  double tbeg, double tend, 
                                  double interval,
                                  double lowerFreqBound, double upperFreqBound, 
                                  double confidenceThreshold );

%feature("docstring",
"Return an estimate of the fundamental frequency computed 
at the specified time. The F0Estimate returned stores the
estimate of the fundamental frequency (in Hz) and the 
relative confidence (from 0 to 1) associated with that
estimate.

	partials is the sequence of Partials
	
	time is the time in seconds at which to attempt to estimate
	the fundamental frequency
	
	lowerFreqBound is the lower bound on the fundamental
	frequency estimate (in Hz)

	upperFreqBound is the lower bound on the fundamental
	frequency estimate (in Hz)
	
	Return the estimate of fundamental frequency in Hz and the 
	confidence associated with that estimate (see 
	F0Estimate.h)
" ) estimateAt;
    
    value_type estimateAt( const PartialList & partials, 
                           double time,
                           double lowerFreqBound, double upperFreqBound );


};   //  end of class FundamentalFromPartials



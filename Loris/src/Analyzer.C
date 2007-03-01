/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2006 by Kelly Fitz and Lippold Haken
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
#include "LorisExceptions.h"
#include "KaiserWindow.h"
#include "Notifier.h"
#include "Partial.h"
#include "PartialPtrs.h"
#include "ReassignedSpectrum.h"
#include "SpectralPeakSelector.h"
#include "PartialBuilder.h"

#include "phasefix.h"   //  HEY LOOKIE HERE - for new frequency/phase fixing at end of analysis

#include "estimateF0.h"

#include <algorithm>
#include <cmath>
#include <functional>   //  for std::plus
#include <memory>
#include <numeric>      //  for std::inner_product
#include <utility>
#include <vector>

using namespace std;

#if defined(HAVE_M_PI) && (HAVE_M_PI)
    const double Pi = M_PI;
#else
    const double Pi = 3.14159265358979324;
#endif

//  begin namespace
namespace Loris {


// ---------------------------------------------------------------------------
//  LinearEnvelopeBuilder
// ---------------------------------------------------------------------------
//  Base class for envelope builders that add a point (possibly) at each
//  analysis frame. 
//
//  TODO: make a dictionary of these things and allow clients to add their
//  own envelope builders and builder functions, and retrieve them after
//  analysis.
class LinearEnvelopeBuilder
{
public:
    virtual ~LinearEnvelopeBuilder( void ) {}
    virtual LinearEnvelopeBuilder * clone( void ) const = 0;
    virtual void build( const Peaks & peaks, double frameTime, LinearEnvelope & env ) = 0;
};

// ---------------------------------------------------------------------------
//  FundamentalBuilder - for constructing an F0 envelope during analysis
// ---------------------------------------------------------------------------
class FundamentalBuilder : public LinearEnvelopeBuilder
{
    double mFmin, mFmax, mAmpThresh, mFreqThresh;
    std::vector< double > amplitudes, frequencies;
    
    const double mMinConfidence;    // 0.9, this could be made a parameter, 
                                    // or raised to make estimates smoother
    
public:
    FundamentalBuilder( double fmin, double fmax, double threshDb = -60, double threshHz = 8000 ) :
        mFmin( fmin ), 
        mFmax( fmax ), 
        mAmpThresh( std::pow( 10., 0.05*(threshDb) ) ),
        mFreqThresh( threshHz ),
        mMinConfidence( 0.9 )
        {}
		
	FundamentalBuilder * clone( void ) const { return new FundamentalBuilder(*this); }
	
    void build( const Peaks & peaks, double frameTime, LinearEnvelope & env );
};

// ---------------------------------------------------------------------------
//  FundamentalBuilder::build
// ---------------------------------------------------------------------------
//
void FundamentalBuilder::build( const Peaks & peaks, double frameTime, 
	 							LinearEnvelope & env )
{
    amplitudes.clear();
    frequencies.clear();
    for ( Peaks::const_iterator spkpos = peaks.begin(); spkpos != peaks.end(); ++spkpos )
    {
        if ( spkpos->second.amplitude() > mAmpThresh &&
             spkpos->second.frequency() < mFreqThresh )
        {
            amplitudes.push_back( spkpos->second.amplitude() );
            frequencies.push_back( spkpos->second.frequency() );
        }
    }
    if ( ! amplitudes.empty() )
    {
        //  estimate f0
        F0estimate est = iterative_estimate( amplitudes, frequencies, 
                                             mFmin,
                                             mFmax,
                                             0.1 );
        
        if ( est.confidence >= mMinConfidence &&
             est.frequency > mFmin && est.frequency < mFmax  )
        {
            // notifier << "f0 is " << est.frequency << endl;
            //  add breakpoint to fundamental envelope
            env.insert( frameTime, est.frequency );
        }
    }
    
}

// ---------------------------------------------------------------------------
//  AmpEnvBuilder - for constructing an amplitude envelope during analysis
// ---------------------------------------------------------------------------
class AmpEnvBuilder : public LinearEnvelopeBuilder
{
public:
    AmpEnvBuilder( void ) {}
		
	AmpEnvBuilder * clone( void ) const { return new AmpEnvBuilder(*this); }
	
    void build( const Peaks & peaks, double frameTime, LinearEnvelope & env );

    //  helper
    static double 
    accumPeakSquaredAmps( double init, 
                          const Peaks::const_iterator::value_type & timeBpPair );
};

// ---------------------------------------------------------------------------
//  AmpEnvBuilder::build
// ---------------------------------------------------------------------------
//
void AmpEnvBuilder::build( const Peaks & peaks, double frameTime, 
                           LinearEnvelope & env )
{
    double x = std::accumulate( peaks.begin(), peaks.end(), 0.0, accumPeakSquaredAmps );
    env.insert( frameTime, std::sqrt( x ) );
}

// ---------------------------------------------------------------------------
//  accumPeakSquaredAmps
// ---------------------------------------------------------------------------
//  static helper used to construct an amplitude envelope.
//
double 
AmpEnvBuilder::accumPeakSquaredAmps( double init, 
                                     const Peaks::const_iterator::value_type & timeBpPair )
{
    return init + (timeBpPair.second.amplitude() * timeBpPair.second.amplitude());
}


// ---------------------------------------------------------------------------
//  Analyzer constructor - frequency resolution only
// ---------------------------------------------------------------------------
//! Construct a new Analyzer configured with the given  
//! frequency resolution (minimum instantaneous frequency   
//! difference between Partials). All other Analyzer parameters     
//! are computed from the specified frequency resolution.   
//! 
//! \param resolutionHz is the frequency resolution in Hz.
//
Analyzer::Analyzer( double resolutionHz )
{
    configure( resolutionHz, resolutionHz );
}

// ---------------------------------------------------------------------------
//  Analyzer constructor
// ---------------------------------------------------------------------------
//! Construct a new Analyzer configured with the given  
//! frequency resolution (minimum instantaneous frequency   
//! difference between Partials) and analysis window width
//! (main lobe, zero-to-zero). All other Analyzer parameters    
//! are computed from the specified resolution and window width.    
//! 
//! \param resolutionHz is the frequency resolution in Hz.
//! \param windowWidthHz is the main lobe width of the Kaiser
//! analysis window in Hz.
//
Analyzer::Analyzer( double resolutionHz, double windowWidthHz )
{
    configure( resolutionHz, windowWidthHz );
}

// ---------------------------------------------------------------------------
//  Analyzer copy constructor
// ---------------------------------------------------------------------------
//! Construct  a new Analyzer having identical
//! parameter configuration to another Analyzer. 
//! The list of collected Partials is not copied.       
//! 
//! \param other is the Analyzer to copy.   
//
Analyzer::Analyzer( const Analyzer & other ) :
    mF0Env( other.mF0Env ),
    mAmpEnv( other.mAmpEnv ),
    m_freqResolution( other.m_freqResolution ),
    m_ampFloor( other.m_ampFloor ),
    m_windowWidth( other.m_windowWidth ),
    m_freqFloor( other.m_freqFloor ),
    m_freqDrift( other.m_freqDrift ),
    m_hopTime( other.m_hopTime ),
    m_cropTime( other.m_cropTime ),
    m_bwRegionWidth( other.m_bwRegionWidth ),
    m_sidelobeLevel( other.m_sidelobeLevel ),
    m_phaseCorrect( other.m_phaseCorrect ),
    m_partials( other.m_partials )
{
    if ( 0 != other.mF0Builder.get() )
    {
        mF0Builder.reset( other.mF0Builder->clone() );
    }

    if ( 0 != other.mAmpEnvBuilder.get() )
    {
        mAmpEnvBuilder.reset( other.mAmpEnvBuilder->clone() );
    }
}

// ---------------------------------------------------------------------------
//  Analyzer assignment
// ---------------------------------------------------------------------------
//! Construct  a new Analyzer having identical
//! parameter configuration to another Analyzer. 
//! The list of collected Partials is not copied.       
//! 
//! \param rhs is the Analyzer to copy. 
//
Analyzer & 
Analyzer::operator=( const Analyzer & rhs )
{
    if ( this != & rhs ) 
    {
        m_freqResolution = rhs.m_freqResolution;
        m_ampFloor = rhs.m_ampFloor;
        m_windowWidth = rhs.m_windowWidth;
        m_freqFloor = rhs.m_freqFloor;  
        m_freqDrift = rhs.m_freqDrift;
        m_hopTime = rhs.m_hopTime;
        m_cropTime = rhs.m_cropTime;
        m_bwRegionWidth = rhs.m_bwRegionWidth;
        m_sidelobeLevel = rhs.m_sidelobeLevel;
        m_phaseCorrect = rhs.m_phaseCorrect;
        m_partials = rhs.m_partials;

        mF0Env = rhs.mF0Env;
        if ( 0 != rhs.mF0Builder.get() )
        {
            mF0Builder.reset( rhs.mF0Builder->clone() );
        }

        mAmpEnv = rhs.mAmpEnv;
        if ( 0 != rhs.mAmpEnvBuilder.get() )
        {
            mAmpEnvBuilder.reset( rhs.mAmpEnvBuilder->clone() );
        }
                
    }
    return *this;
}

// ---------------------------------------------------------------------------
//  Analyzer destructor
// ---------------------------------------------------------------------------
//! Destroy this Analyzer.
//
Analyzer::~Analyzer( void )
{
}

// -- configuration --

// ---------------------------------------------------------------------------
//  configure
// ---------------------------------------------------------------------------
//! Configure this Analyzer with the given frequency resolution 
//! (minimum instantaneous frequency difference between Partials)
//! and analysis window width (main lobe, zero-to-zero, in Hz). 
//! All other Analyzer parameters are (re-)computed from the 
//! frequency resolution and window width.      
//! 
//! \param resolutionHz is the frequency resolution in Hz.
//! \param windowWidthHz is the main lobe width of the Kaiser
//! analysis window in Hz.
//!     
//! There are three categories of analysis parameters:
//! - the resolution, and params that are usually related to (or
//! identical to) the resolution (frequency floor and drift)
//! - the window width and params that are usually related to (or
//! identical to) the window width (hop and crop times)
//! - independent parameters (bw region width and amp floor)
//
void
Analyzer::configure( double resolutionHz, double windowWidthHz )
{
    //  use specified resolution:
    setFreqResolution( resolutionHz );
    
    //  floor defaults to -90 dB:
    setAmpFloor( -90. );
    
    //  window width should generally be approximately 
    //  equal to, and never more than twice the 
    //  frequency resolution:
    setWindowWidth( windowWidthHz );
    
    //  the Kaiser window sidelobe level can be the same
    //  as the amplitude floor (except in positive dB):
    setSidelobeLevel( - m_ampFloor );
    
    //  for the minimum frequency, below which no data is kept,
    //  use the frequency resolution by default (this makes 
    //  Lip happy, and is always safe?) and allow the client 
    //  to change it to anything at all.
    setFreqFloor( m_freqResolution );
    
    //  frequency drift in Hz is the maximum difference
    //  in frequency between consecutive Breakpoints in
    //  a Partial, by default, make it equal to one fifth
    //  the frequency resolution:
    setFreqDrift( .2 * m_freqResolution );
    
    //  hop time (in seconds) is the inverse of the
    //  window width....really. Smith and Serra (1990) cite 
    //  Allen (1977) saying: a good choice of hop is the window 
    //  length divided by the main lobe width in frequency samples,
    //  which turns out to be just the inverse of the width.
    setHopTime( 1. / m_windowWidth );
    
    //  crop time (in seconds) is the maximum allowable time
    //  correction, beyond which a reassigned spectral component
    //  is considered unreliable, and not considered eligible for
    //  Breakpoint formation in extractPeaks(). By default, use
    //  the hop time (should it be half that?):
    setCropTime( m_hopTime );
    
    //  bandwidth association region width 
    //  defaults to 2 kHz, corresponding to 
    //  1 kHz region center spacing:
    setBwRegionWidth( 2000. );

    if ( 0 != mF0Builder.get() )
    {
        //  (re)configure the fundamental tracker using default 
        //  parameters:
        buildFundamentalEnv( true );
    }
    
    //  enable phase-correct Partial construction:
    m_phaseCorrect = true;
}

// -- analysis --
// ---------------------------------------------------------------------------
//  analyze
// ---------------------------------------------------------------------------
//! Analyze a vector of (mono) samples at the given sample rate         
//! (in Hz) and append the extracted Partials to Analyzer's 
//! PartialList (std::list of Partials).    
//! 
//! \param vec is a vector of floating point samples
//! \param srate is the sample rate of the samples in the vector 
//
void 
Analyzer::analyze( const std::vector<double> & vec, double srate )      
{ 
    BreakpointEnvelope reference( 1.0 );
    analyze( &(vec[0]),  &(vec[0]) + vec.size(), srate, reference ); 
}

// ---------------------------------------------------------------------------
//  analyze
// ---------------------------------------------------------------------------
//! Analyze a range of (mono) samples at the given sample rate      
//! (in Hz) and collect the resulting Partials. 
//! 
//! \param bufBegin is a pointer to a buffer of floating point samples
//! \param bufEnd is (one-past) the end of a buffer of floating point 
//! samples
//! \param srate is the sample rate of the samples in the buffer
//
void 
Analyzer::analyze( const double * bufBegin, const double * bufEnd, double srate )
{ 
    BreakpointEnvelope reference( 1.0 );
    analyze( bufBegin,  bufEnd, srate, reference ); 
}

// ---------------------------------------------------------------------------
//  analyze
// ---------------------------------------------------------------------------
//! Analyze a vector of (mono) samples at the given sample rate         
//! (in Hz) and append the extracted Partials to Analyzer's 
//! PartialList (std::list of Partials). Use the specified envelope
//! as a frequency reference for Partial tracking.
//!
//! \param vec is a vector of floating point samples
//! \param srate is the sample rate of the samples in the vector
//! \param reference is an Envelope having the approximate
//! frequency contour expected of the resulting Partials.
//
void 
Analyzer::analyze( const std::vector<double> & vec, double srate, 
                   const Envelope & reference )     
{ 
    analyze( &(vec[0]),  &(vec[0]) + vec.size(), srate, reference ); 
}

// ---------------------------------------------------------------------------
//  helpers
// ---------------------------------------------------------------------------
static double accumPeakSquaredAmps( double init, 
                                    const Peaks::const_iterator::value_type & timeBpPair )
{
    return init + (timeBpPair.second.amplitude() * timeBpPair.second.amplitude());
}

template < class Pair >
static double compare2nd( const Pair & p1, const Pair & p2 )
{
    return p1.second < p2.second;
}

// ---------------------------------------------------------------------------
//  analyze
// ---------------------------------------------------------------------------
//! Analyze a range of (mono) samples at the given sample rate      
//! (in Hz) and append the extracted Partials to Analyzer's 
//! PartialList (std::list of Partials). Use the specified envelope
//! as a frequency reference for Partial tracking.
//! 
//! \param bufBegin is a pointer to a buffer of floating point samples
//! \param bufEnd is (one-past) the end of a buffer of floating point 
//! samples
//! \param srate is the sample rate of the samples in the buffer
//! \param reference is an Envelope having the approximate
//! frequency contour expected of the resulting Partials.
//
void 
Analyzer::analyze( const double * bufBegin, const double * bufEnd, double srate,
                   const Envelope & reference )
{ 
    //  configure the reassigned spectral analyzer, 
    //  always use odd-length windows:

    //  Kaiser window
    double winshape = KaiserWindow::computeShape( sidelobeLevel() );
    long winlen = KaiserWindow::computeLength( windowWidth() / srate, winshape );    
    if (! (winlen % 2)) 
        ++winlen;
    debugger << "Using Kaiser window of length " << winlen << endl;
    
    std::vector< double > window( winlen );
    KaiserWindow::create( window, winshape );
   
    ReassignedSpectrum spectrum( window );
    
    //  configure the peak selection and partial formation policies:
    SpectralPeakSelector selector( srate, m_freqResolution );
    PartialBuilder builder( m_freqDrift, reference );
    
    //  configure bw association policy, unless
    //  bandwidth association is disabled:
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

    //  reset envelopes:
    mAmpEnv.clear();
    mF0Env.clear();
        
    try 
    { 
        //  loop over short-time analysis frames:
        const double * winMiddle = bufBegin; 

        while ( winMiddle < bufEnd )
        {
            //  compute the time of this analysis frame:
            double currentFrameTime = long(winMiddle - bufBegin) / srate;
            
            //  compute reassigned spectrum:
            //  sampsBegin is the position of the first sample to be transformed,
            //  sampsEnd is the position after the last sample to be transformed.
            //  (these computations work for odd length windows only)
            const double * sampsBegin = std::max( winMiddle - (winlen / 2), bufBegin );
            const double * sampsEnd = std::min( winMiddle + (winlen / 2) + 1, bufEnd );
            spectrum.transform( sampsBegin, winMiddle, sampsEnd );
            
             
            //  extract peaks from the spectrum, thin and 
            //  fade quiet peaks out over 10 dB:
            #define FADE 10.
            Peaks & peaks = selector.extractPeaks( spectrum, m_freqFloor, m_cropTime ); 
            Peaks::iterator rejected = selector.thinPeaks( m_ampFloor, FADE, currentFrameTime );

            if ( associateBandwidth() )
            {
                bwAssociator->associateBandwidth( peaks.begin(), rejected, peaks.end() );
            }
            
            //  remove rejected Breakpoints:
            peaks.erase( rejected, peaks.end() );
            
            //  estimate the amplitude in this frame:
            if ( 0 != mAmpEnvBuilder.get() )
            {
                mAmpEnvBuilder->build( peaks, currentFrameTime, mAmpEnv );
            }
            
            //  collect amplitudes and frequencies and try to 
            //  estimate the fundamental
            if ( 0 != mF0Builder.get() )
            {
                mF0Builder->build( peaks, currentFrameTime, mF0Env );
            }

            //  form Partials from the extracted Breakpoints:
            builder.formPartials( peaks, currentFrameTime );
            
            //  slide the analysis window:
            winMiddle += long( m_hopTime * srate ); //  hop in samples, truncated

        }   //  end of loop over short-time frames
        
        //  unwarp the Partial frequency envelopes:
        builder.fixPartialFrequencies();
        
        //  fix the frequencies and phases to be consistent.
        if ( m_phaseCorrect )
        {
            fixFrequency( builder.partials().begin(), builder.partials().end() );
        }
        
        
        //  for debugging:
        if ( ! mAmpEnv.empty() )
        {
            LinearEnvelope::iterator peakpos = 
                std::max_element( mAmpEnv.begin(), mAmpEnv.end(), 
                                  compare2nd<LinearEnvelope::iterator::value_type> );
            notifier << "HEY analyzer found amp peak at time : " << peakpos->first
                     << " value: " << peakpos->second << endl;
        }
        
        m_partials.splice( m_partials.end(), builder.partials() );
    }
    catch ( Exception & ex ) 
    {
        ex.append( "analysis failed." );
        throw;
    }
}

// -- parameter access --

// ---------------------------------------------------------------------------
//  ampFloor
// ---------------------------------------------------------------------------
//! Return the amplitude floor (lowest detected spectral amplitude),            
//! in (negative) dB, for this Analyzer.                
//
double 
Analyzer::ampFloor( void ) const 
{ 
    return m_ampFloor; 
}

// ---------------------------------------------------------------------------
//  associateBandwidth
// ---------------------------------------------------------------------------
//! Return true if this Analyzer is configured to peform bandwidth
//! association to distribute noise energy among extracted Partials, 
//! and false if noise energy will be collected in noise Partials,
//! labeled -1 in this Analyzer's PartialList.
//
bool 
Analyzer::associateBandwidth( void ) const 
{ 
    return m_bwRegionWidth > 0.; 
}

// ---------------------------------------------------------------------------
//  bwRegionWidth
// ---------------------------------------------------------------------------
//! Return the width (in Hz) of the Bandwidth Association regions
//! used by this Analyzer.
//
double 
Analyzer::bwRegionWidth( void ) const
{ 
    // debugger << "Analyzer::bwRegionWidth() is a deprecated member, and will be removed in a future Loris release." << endl;
    return m_bwRegionWidth; 
}

// ---------------------------------------------------------------------------
//  cropTime
// ---------------------------------------------------------------------------
//! Return the crop time (maximum temporal displacement of a time-
//! frequency data point from the time-domain center of the analysis
//! window, beyond which data points are considered "unreliable")
//! for this Analyzer.
//
double 
Analyzer::cropTime( void ) const 
{ 
    // debugger << "Analyzer::cropTime() is a deprecated member, and will be removed in a future Loris release." << endl;
    return m_cropTime; 
}

// ---------------------------------------------------------------------------
//  freqDrift
// ---------------------------------------------------------------------------
//! Return the maximum allowable frequency difference 
//! consecutive Breakpoints in a Partial envelope for this Analyzer.                
//
double 
Analyzer::freqDrift( void ) const 
{ 
    return m_freqDrift;
}

// ---------------------------------------------------------------------------
//  freqFloor
// ---------------------------------------------------------------------------
//! Return the frequency floor (minimum instantaneous Partial               
//! frequency), in Hz, for this Analyzer.               
//
double 
Analyzer::freqFloor( void ) const 
{ 
    return m_freqFloor; 
}

// ---------------------------------------------------------------------------
//  freqResolution
// ---------------------------------------------------------------------------
//! Return the frequency resolution (minimum instantaneous frequency        
//! difference between Partials) for this Analyzer.
//
double 
Analyzer::freqResolution( void ) const 
{ 
    return m_freqResolution; 
}

// ---------------------------------------------------------------------------
//  hopTime
// ---------------------------------------------------------------------------
//! Return the hop time (which corresponds approximately to the 
//! average density of Partial envelope Breakpoint data) for this 
//! Analyzer.
//
double 
Analyzer::hopTime( void ) const 
{ 
    return m_hopTime; 
}

// ---------------------------------------------------------------------------
//  sidelobeLevel
// ---------------------------------------------------------------------------
//! Return the sidelobe attenutation level for the Kaiser analysis window in
//! positive dB. Larger numbers (e.g. 90) give very good sidelobe 
//! rejection but cause the window to be longer in time. Smaller numbers 
//! (like 60) raise the level of the sidelobes, increasing the likelihood
//! of frequency-domain interference, but allow the window to be shorter
//! in time.
//
double 
Analyzer::sidelobeLevel( void ) const 
{ 
    return m_sidelobeLevel; 
}

// ---------------------------------------------------------------------------
//  windowWidth
// ---------------------------------------------------------------------------
//! Return the frequency-domain main lobe width (measured between 
//! zero-crossings) of the analysis window used by this Analyzer.               
//
double 
Analyzer::windowWidth( void ) const 
{ 
    return m_windowWidth; 
}

// ---------------------------------------------------------------------------
//  windowWidth
// ---------------------------------------------------------------------------
//! Return true if the phases and frequencies of the constructed
//! partials should be modified to be consistent at the end of the
//! analysis, and false otherwise. (Default is true.)
//!
//! \param  TF is a flag indicating whether or not to construct
//!         phase-corrected Partials
bool 
Analyzer::phaseCorrect( void ) const
{
    return m_phaseCorrect;
}

// -- parameter mutation --

#define VERIFY_ARG(func, test)                                          \
    do {                                                                \
        if (!(test))                                                    \
            Throw( Loris::InvalidArgument, #func ": " #test  );         \
    } while (false)


// ---------------------------------------------------------------------------
//  setAmpFloor
// ---------------------------------------------------------------------------
//! Set the amplitude floor (lowest detected spectral amplitude), in            
//! (negative) dB, for this Analyzer. 
//! 
//! \param x is the new value of this parameter.                
//
void 
Analyzer::setAmpFloor( double x ) 
{ 
    VERIFY_ARG( setAmpFloor, x < 0 );
    m_ampFloor = x; 
}

// ---------------------------------------------------------------------------
//  setBwRegionWidth
// ---------------------------------------------------------------------------
//! Set the width (in Hz) of the Bandwidth Association regions
//! used by this Analyzer. If zero, bandwidth enhancement is 
//! disabled.
//! 
//! \param x is the new value of this parameter.
//
void 
Analyzer::setBwRegionWidth( double x ) 
{ 
    VERIFY_ARG( setBwRegionWidth, x >= 0 );
   // debugger << "Analyzer::setBwRegionWidth() is a deprecated member, and will be removed in a future Loris release." << endl;
    m_bwRegionWidth = x; 
}   

// ---------------------------------------------------------------------------
//  setCropTime
// ---------------------------------------------------------------------------
//! Set the crop time (maximum temporal displacement of a time-
//! frequency data point from the time-domain center of the analysis
//! window, beyond which data points are considered "unreliable")
//! for this Analyzer.
//! 
//! \param x is the new value of this parameter.
//
void 
Analyzer::setCropTime( double x ) 
{ 
    VERIFY_ARG( setCropTime, x > 0 );
   // debugger << "Analyzer::setCropTime() is a deprecated member, and will be removed in a future Loris release." << endl;
    m_cropTime = x; 
}

// ---------------------------------------------------------------------------
//  setFreqDrift
// ---------------------------------------------------------------------------
//! Set the maximum allowable frequency difference between                  
//! consecutive Breakpoints in a Partial envelope for this Analyzer.                
//! 
//! \param x is the new value of this parameter.            
//
void 
Analyzer::setFreqDrift( double x ) 
{ 
    VERIFY_ARG( setFreqDrift, x > 0 );
    m_freqDrift = x; 
}

// ---------------------------------------------------------------------------
//  setFreqFloor
// ---------------------------------------------------------------------------
//! Set the frequency floor (minimum instantaneous Partial                  
//! frequency), in Hz, for this Analyzer.
//! 
//! \param x is the new value of this parameter.                    
//
void 
Analyzer::setFreqFloor( double x ) 
{ 
    VERIFY_ARG( setFreqFloor, x >= 0 );
    m_freqFloor = x; 
}

// ---------------------------------------------------------------------------
//  setFreqResolution
// ---------------------------------------------------------------------------
//! Set the frequency resolution (minimum instantaneous frequency       
//! difference between Partials) for this Analyzer. (Does not cause     
//! other parameters to be recomputed.)                                     
//! 
//! \param x is the new value of this parameter.                                        
//
void 
Analyzer::setFreqResolution( double x ) 
{ 
    VERIFY_ARG( setFreqResolution, x > 0 );
    m_freqResolution = x; 
}

// ---------------------------------------------------------------------------
//  setSidelobeLevel
// ---------------------------------------------------------------------------
//! Set the sidelobe attenutation level for the Kaiser analysis window in
//! positive dB. Higher numbers (e.g. 90) give very good sidelobe 
//! rejection but cause the window to be longer in time. Lower 
//! numbers raise the level of the sidelobes, increasing the likelihood
//! of frequency-domain interference, but allow the window to be shorter
//! in time.
//! 
//! \param x is the new value of this parameter.    
//
void 
Analyzer::setSidelobeLevel( double x ) 
{ 
    VERIFY_ARG( setSidelobeLevel, x > 0 );
    m_sidelobeLevel = x; 
}

// ---------------------------------------------------------------------------
//  setHopTime
// ---------------------------------------------------------------------------
//! Set the hop time (which corresponds approximately to the average
//! density of Partial envelope Breakpoint data) for this Analyzer.
//! 
//! \param x is the new value of this parameter.
//
void 
Analyzer::setHopTime( double x ) 
{ 
    VERIFY_ARG( setHopTime, x > 0 );
    m_hopTime = x; 
}

// ---------------------------------------------------------------------------
//  setWindowWidth
// ---------------------------------------------------------------------------
//! Set the frequency-domain main lobe width (measured between 
//! zero-crossings) of the analysis window used by this Analyzer.   
//! 
//! \param x is the new value of this parameter.            
//
void 
Analyzer::setWindowWidth( double x ) 
{ 
    VERIFY_ARG( setWindowWidth, x > 0 );
    m_windowWidth = x; 
}

// ---------------------------------------------------------------------------
//  setPhaseCorrect
// ---------------------------------------------------------------------------
//! Indicate whether the phases and frequencies of the constructed
//! partials should be modified to be consistent at the end of the
//! analysis. (Default is true.)
//!
//! \param  TF is a flag indicating whether or not to construct
//!         phase-corrected Partials
void 
Analyzer::setPhaseCorrect( bool TF )
{
    m_phaseCorrect = TF;
}

// -- PartialList access --

// ---------------------------------------------------------------------------
//  partials
// ---------------------------------------------------------------------------
//! Return a mutable reference to this Analyzer's list of 
//! analyzed Partials. 
//
PartialList & 
Analyzer::partials( void ) 
{ 
    return m_partials; 
}

// ---------------------------------------------------------------------------
//  partials
// ---------------------------------------------------------------------------
//! Return an immutable (const) reference to this Analyzer's 
//! list of analyzed Partials. 
//
const PartialList & 
Analyzer::partials( void ) const
{ 
    return m_partials; 
}

// ---------------------------------------------------------------------------
//  buildFundamentalEnv
// ---------------------------------------------------------------------------
//! Indicate whether the fundamental frequency envelope of the analyzed
//! sound should be estimated during analysis. If true (the
//! default), then the fundamental frequency estimate can be accessed by
//! fundamentalEnv() after the analysis is complete. Default
//! parameters for fundamental estimation are used. To set those
//! parameters, use buildFundamentalEnv( fmin, fmax, threshDb, threshHz )
//! instead.
//!
//! \param  TF is a flag indicating whether or not to construct
//!         the fundamental frequency envelope during analysis
//
void Analyzer::buildFundamentalEnv( bool TF )
{
    if ( TF )
    {
        //  configure with default parameters
        buildFundamentalEnv( m_freqResolution,
    			             2 * m_freqResolution,
    			             -60, 
    			             8000 );
    }
    else
    {
        // disable
        mF0Builder.reset( 0 );
    }
}

// ---------------------------------------------------------------------------
//  buildFundamentalEnv
// ---------------------------------------------------------------------------
//! Specify parameters for constructing a fundamental frequency 
//! envelope for the analyzed sound during analysis. The fundamental 
//! frequency estimate can be accessed by fundamentalEnv() after the 
//! analysis is complete. 
//!
//! \param  fmin is the lower bound on the fundamental frequency estimate
//! \param  fmax is the upper bound on the fundamental frequency estimate
//! \param  threshDb is the lower bound on the amplitude of a spectral peak
//!         that will constribute to the fundamental frequency estimate (very
//!         low amplitude peaks tend to have less reliable frequency estimates).
//!         Default is -60 dB.
//! \param  threshHz is the upper bound on the frequency of a spectral
//!         peak that will constribute to the fundamental frequency estimate.
//!         Default is 8 kHz.
//
void Analyzer::buildFundamentalEnv( double fmin, double fmax, 
                                    double threshDb, double threshHz )
{
    mF0Builder.reset( 
        new FundamentalBuilder( fmin, fmax, threshDb, threshHz ) );
}

// ---------------------------------------------------------------------------
//  fundamentalEnv
// ---------------------------------------------------------------------------
//! Return the fundamental frequency estimate envelope constructed
//! during the most recent analysis performed by this Analyzer.
//! Will be empty unless buildFundamentalEnv was invoked to enable the
//! construction of this envelope during analysis.
//
const LinearEnvelope & 
Analyzer::fundamentalEnv( void ) const
{   
    /*
    //  raise an exception if the fundamental estimate was not built,
    //  nothing good can come of returning the empty envelope:
    //  but on the other hand, checking the state of the builder
    //  does not tell anything about whether an envelope has been 
    //  constructed, or whether an analysis has even occurred.
    if ( 0 == mF0Builder.get() )
    {
        Throw( InvalidObject, "No fundamental envelope was built." );
    }
    */
    return mF0Env; 
}


// ---------------------------------------------------------------------------
//  buildAmpEnv
// ---------------------------------------------------------------------------
//! Indicate whether the amplitude envelope of the analyzed
//! sound should be estimated during analysis. If true (the
//! default), then the amplitude estimate can be accessed by
//! ampEnv() after the analysis is complete.
//!
//! \param  TF is a flag indicating whether or not to construct
//!         the amplitude envelope during analysis
//
void Analyzer::buildAmpEnv( bool TF )
{
    if ( TF )
    {
        mAmpEnvBuilder.reset( new AmpEnvBuilder );
    }
    else
    {
        // disable
        mAmpEnvBuilder.reset( 0 );
    }
    
}

// ---------------------------------------------------------------------------
//  ampEnv
// ---------------------------------------------------------------------------
//! Return the overall amplitude estimate envelope constructed
//! during the most recent analysis performed by this Analyzer.
//! Will be empty unless buildAmpEnv was invoked to enable the
//! construction of this envelope during analysis.
//
const LinearEnvelope & Analyzer::ampEnv( void ) const
{ 
    /*
    //  raise an exception if the amplitude estimate was not built,
    //  nothing good can come of returning the empty envelope,
    //  but on the other hand, checking the state of the builder
    //  does not tell anything about whether an envelope has been 
    //  constructed, or whether an analysis has even occurred.
    if ( 0 == mAmpEnvBuilder.get() )
    {
        Throw( InvalidObject, "No amplitude envelope was built." );
    }
    */
    return mAmpEnv; 
}

}   //  end of namespace Loris

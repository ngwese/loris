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
 * FundamentalEstimator.h
 *
 * Implementation of class FundamentalEstimator for computing an estimate of 
 * time-varying fundamental frequency from a sequence of samples using a
 * frequency domain maximum likelihood algorithm adapted from Quatieri's 
 * speech signal processing textbook. The analysis and spectral peak 
 * selection mechanisms are taken directly from the Loris Analyzer (see
 * Analyzer.C).
 *
 * Kelly Fitz, 3 July 2006
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include "config.h"
#endif

#include "FundamentalEstimator.h"

#include "LorisExceptions.h"
#include "KaiserWindow.h"
#include "LinearEnvelope.h"
#include "PartialUtils.h"
#include "ReassignedSpectrum.h"
#include "SpectralPeakSelector.h"

#include "estimateF0.h"

#include <algorithm>
#include <cmath>
#include <vector>

using namespace std;

#if defined(HAVE_M_PI) && (HAVE_M_PI)
	const double Pi = M_PI;
#else
	const double Pi = 3.14159265358979324;
#endif

//	begin namespace
namespace Loris {

#define VERIFY_ARG(func, test)											\
	do {																\
		if (!(test)) 													\
			Throw( Loris::InvalidArgument, #func ": " #test  );			\
	} while (false)

//	default parameters:
const double FundamentalEstimator::DefaultAmpFloor = -60;
const double FundamentalEstimator::DefaultPrecision = .1;
const double FundamentalEstimator::DefaultFreqCeiling = 10000;

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//!	Construct a new FundamentalEstimator configured with the 	
//!	specified parameters. 	
//!	
//!	\param lowerBoundHz is the minimum instantaneous fundamental 
//! frequency in Hz.
//!	\param upperBoundHz is the maximum instantaneous fundamental 
//! frequency in Hz.
FundamentalEstimator::FundamentalEstimator( double lowerBoundHz, double upperBoundHz )
{
	configure( lowerBoundHz, upperBoundHz );
}

// ---------------------------------------------------------------------------
//	FundamentalEstimator copy constructor
// ---------------------------------------------------------------------------
//!	Construct  a new FundamentalEstimator having identical
//!	parameter configuration to another FundamentalEstimator. 
//!	
//!	\param other is the FundamentalEstimator to copy.	
FundamentalEstimator::FundamentalEstimator( const FundamentalEstimator & other ):
    m_lowerBound( other.m_lowerBound ),
    m_upperBound( other.m_upperBound ),
    m_precision( other.m_precision ),
    m_ampFloor( other.m_ampFloor ),
    m_freqCeiling( other.m_freqCeiling ),
    m_hopTime( other.m_hopTime ),
    m_f0Env( other.m_f0Env ),
    m_energyEnv( other.m_energyEnv )
{
}

// ---------------------------------------------------------------------------
//	assignment
// ---------------------------------------------------------------------------
//!	Construct  a new FundamentalEstimator having identical
//!	parameter configuration to another FundamentalEstimator. 
//!	
//!	\param rhs is the FundamentalEstimator to copy.	
FundamentalEstimator & 
FundamentalEstimator::operator=( const FundamentalEstimator & rhs )
{
	if ( this != & rhs ) 
	{
        m_lowerBound = rhs.m_lowerBound;
        m_upperBound = rhs.m_upperBound;
        m_precision = rhs.m_precision;
        m_ampFloor = rhs.m_ampFloor;
        m_freqCeiling = rhs.m_freqCeiling;  
        m_hopTime = rhs.m_hopTime;
        m_f0Env = rhs.m_f0Env;
        m_energyEnv = rhs.m_energyEnv;
	}
	return *this;
}

// ---------------------------------------------------------------------------
//	destructor
// ---------------------------------------------------------------------------
//!	Destroy this FundamentalEstimator.
//
FundamentalEstimator::~FundamentalEstimator( void )
{
}

// -- configuration --

// ---------------------------------------------------------------------------
//	configure
// ---------------------------------------------------------------------------
//!	Configure this FundamentalEstimator with the specified parameters. 
//!	
//!	\param lowerBoundHz is the minimum instantaneous fundamental 
//! frequency in Hz.
//!	\param upperBoundHz is the maximum instantaneous fundamental 
//! frequency in Hz.
void FundamentalEstimator::configure( double lowerBoundHz, double upperBoundHz )
{
    VERIFY_ARG( FundamentalEstimator::configure, lowerBoundHz != upperBoundHz );
    VERIFY_ARG( FundamentalEstimator::configure, 0 <= lowerBoundHz );
    VERIFY_ARG( FundamentalEstimator::configure, 0 <= upperBoundHz );
    if ( lowerBoundHz > upperBoundHz )
    {
        std::swap( lowerBoundHz, upperBoundHz );
    }

	//	use specified fundamental frequency bounds:
    m_lowerBound = lowerBoundHz;
    m_upperBound = upperBoundHz;
	
    //  precision defaults to 0.1 Hz
    setPrecision( DefaultPrecision );
    
	//	floor defaults to -60 dB:
	setAmpFloor( DefaultAmpFloor );
	
	//	frequency ceiling defaults to 10 kHz
    setFreqCeiling( DefaultFreqCeiling );
		
	//	hop time (in seconds) is the inverse of the
	//	window width....really. Smith and Serra (1990) cite 
	//	Allen (1977) saying: a good choice of hop is the window 
	//	length divided by the main lobe width in frequency samples,
	//	which turns out to be just the inverse of the width.
    //  The window width is twice the lower bound on the fundamental
    //  frequency. 
    //
    //  Smaller hops than this make little sense, but larger ones
    //  are reasonable.
    setHopTime( 0.5 / lowerBoundHz );
	
    //  clear the envelope
    m_f0Env.clear();
    m_energyEnv.clear();
}

// -- analysis --
// ---------------------------------------------------------------------------
//	analyze
// ---------------------------------------------------------------------------
//!	Estimate the fundamental for a vector of (mono) samples at the given 
//!	sample rate (in Hz). The time-varying fundamental frequency estimate
//! is stored in a LinearEnvelope, accessed by fundamentalEnv(). Another 
//! LinearEnvelope, acessed by energyEnv(), stores a time-varying estimate
//! of the spectral energy (root sum of spectral amplitude squares) that
//! constributed to each fundamental frequency estimate.
//!	
//! \param  vec is a vector of floating point samples
//!	\param 	srate is the sample rate of the samples in the buffer
//
void FundamentalEstimator::analyze( const std::vector<double> & vec, double srate )
{ 
    analyze( &(vec[0]),  &(vec[0]) + vec.size(), srate ); 
}

// ---------------------------------------------------------------------------
//	analyze
// ---------------------------------------------------------------------------
//!	The fundamental for a range of (mono) samples at the given 
//!	sample rate (in Hz).	
//!	
//!	\param 	bufBegin is a pointer to a buffer of doubleing point samples
//!	\param 	bufEnd is (one-past) the end of a buffer of floating point 
//!			samples
//!	\param 	srate is the sample rate of the samples in the buffer
void FundamentalEstimator::analyze( const double * bufBegin, 
                                    const double * bufEnd, 
                                    double srate )
{ 
 	//	configure the reassigned spectral analyzer, 
    //	always use odd-length windows:
    const double sidelobeLevel = - ampFloor(); // amp floor is negative
    double winshape = KaiserWindow::computeShape( sidelobeLevel );
    long winlen = KaiserWindow::computeLength( 2.0 * lowerBound() / srate, winshape );    
    if ( 1 != (winlen % 2) ) 
    {
        ++winlen;
    }
    
    std::vector< double > window( winlen );
    KaiserWindow::create( window, winshape );
   
 	ReassignedSpectrum spectrum( window );
	
    //	configure the peak selection and partial formation policies:
    SpectralPeakSelector selector( srate, 0.85 * lowerBound() );
 	
    //  clear the envelopes in preparation for creating a new one
    m_f0Env.clear();
    m_energyEnv.clear();

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
            Peaks & peaks = selector.extractPeaks( spectrum, 0.85 * lowerBound(), 0.5 * hopTime() ); 
            const double floordB = ampFloor();
            Peaks::iterator rejected = selector.thinPeaks( floordB, FADE, currentFrameTime );
			
			//	remove rejected Breakpoints:
			peaks.erase( rejected, peaks.end() );
                        
            //  collect amplitudes and frequencies and try to 
            //  estimate the fundamental
            buildEnvelopes( peaks, currentFrameTime );

			//	slide the analysis window:
            winMiddle += long( m_hopTime * srate ); //  hop in samples, truncated

		}	//	end of loop over short-time frames
    }
	catch ( Exception & ex ) 
	{
		ex.append( "fundamental frequency estimation failed." );
		throw;
	}
}

// ---------------------------------------------------------------------------
//	analyze
// ---------------------------------------------------------------------------
//!	Estimate the fundamental for a (valid) range of Partials, using 
//! the Partial Breakpoints as spectral peaks in the estimation 
//!	algorithm. The time-varying fundamental frequency estimate
//! is stored in a LinearEnvelope, accessed by fundamentalEnv(). Another 
//! LinearEnvelope, acessed by energyEnv(), stores a time-varying estimate
//! of the spectral energy (root sum of spectral amplitude squares) that
//! constributed to each fundamental frequency estimate.
//!         
//! \param  begin_partials is the beginning of a sequence of Partials.
//! \param  end_partials is the end of a sequence of Partials.
#if 0 // !defined(NO_TEMPLATE_MEMBERS)
    template<typename Iter>
    void FundamentalEstimator::analyze( Iter begin_partials, Iter end_partials )
#else 
    typedef PartialList::const_iterator Iter;
    void FundamentalEstimator::analyze( PartialList::const_iterator begin_partials, 
                                        PartialList::const_iterator end_partials )
#endif
{
    //  clear the envelopes in preparation for creating a new one
    m_f0Env.clear();
    m_energyEnv.clear();

    try
    {
        //  iterate over the valid times for estimating the fundamental
        //  frequency using the specified hop time:
        std::pair< double, double > span =
            PartialUtils::timeSpan( begin_partials, end_partials );
        double currentFrameTime = 0;
        //  don't start way before any partials begin:
        while ( currentFrameTime + m_hopTime < span.first )
        {
            currentFrameTime += m_hopTime;
        }
        
        Peaks peaks;
        while ( currentFrameTime - m_hopTime < span.second )
        {
            peaks.clear();
            
            //  collect Breakpoints near to currentFrameTime
            for ( Iter it = begin_partials; it != end_partials; ++it )
            {
                Partial::const_iterator partPos = it->findNearest( currentFrameTime );
                double bpTime = partPos.time();
                if ( currentFrameTime - m_hopTime <= bpTime &&
                     currentFrameTime + m_hopTime >= bpTime )
                {
                    peaks.push_back( Peaks::value_type( bpTime, partPos.breakpoint() ) );
                }
            }
            
            //  collect amplitudes and frequencies and try to 
            //  estimate the fundamental
            buildEnvelopes( peaks, currentFrameTime );
        
            currentFrameTime += m_hopTime;
        }
    }
	catch ( Exception & ex ) 
	{
		ex.append( "fundamental frequency estimation failed." );
		throw;
	}
}

// -- parameter access --

// ---------------------------------------------------------------------------
//	ampFloor
// ---------------------------------------------------------------------------
//!	Return the amplitude floor in dB, 
//! relative to full amplitude sine wave, absolute
//! amplitude threshold (negative) below which spectral
//! peaks will not be considered in the estimation of 
//! the fundamental (default is -60 dB).			
//
double 
FundamentalEstimator::ampFloor( void ) const 
{ 
    //  convert to dB
    return 20.0 * std::log10( m_ampFloor ); 
}

// ---------------------------------------------------------------------------
//	freqCeiling
// ---------------------------------------------------------------------------
//!	Return the frequency ceiling in Hz, the
//! frequency threshold above which spectral
//! peaks will not be considered in the estimation of 
//! the fundamental (default is 10 kHz).			
//
double 
FundamentalEstimator::freqCeiling( void ) const 
{ 
    return m_freqCeiling; 
}
// ---------------------------------------------------------------------------
//	hopTime
// ---------------------------------------------------------------------------
//!	Return the hop time in seconds, time between (attempted) fundamental
//! frequency estimates.
//
double 
FundamentalEstimator::hopTime( void ) const 
{ 
    return m_hopTime; 
}

// ---------------------------------------------------------------------------
//	lowerBound
// ---------------------------------------------------------------------------
//!	Return the minimum instantaneous fundamental frequency, in Hz
//! that can be detected by this FundamentalEstimator.
//
double 
FundamentalEstimator::lowerBound( void ) const 
{ 
    return m_lowerBound; 
}

// ---------------------------------------------------------------------------
//	precision
// ---------------------------------------------------------------------------
//!	Return the precision of the estimate in Hz, the
//! fundamental frequency will be estimated to 
//! within this range (default is 0.1 Hz).
//
double 
FundamentalEstimator::precision( void ) const 
{ 
    return m_precision; 
}

// ---------------------------------------------------------------------------
//	upperBound
// ---------------------------------------------------------------------------
//!	Return the maximum instantaneous fundamental frequency, in Hz
//! that can be detected by this FundamentalEstimator.
//
double 
FundamentalEstimator::upperBound( void ) const 
{ 
    return m_upperBound;
}

// -- parameter mutation --

// ---------------------------------------------------------------------------
//	setAmpFloor
// ---------------------------------------------------------------------------
//!	Set the amplitude floor in dB, 
//! relative to full amplitude sine wave, absolute
//! amplitude threshold (negative) below which spectral
//! peaks will not be considered in the estimation of 
//! the fundamental (default is -60 dB).
//!	
//!	\param x is the new value of this parameter. 				
//
void 
FundamentalEstimator::setAmpFloor( double x ) 
{ 
	VERIFY_ARG( setAmpFloor, x < 0 );
    //  convert to absolute
    m_ampFloor = std::pow( 10., 0.05 * x ); 
}

// ---------------------------------------------------------------------------
//	setFreqCeiling
// ---------------------------------------------------------------------------
//!	Set the frequency ceiling in Hz, the
//! frequency threshold above which spectral
//! peaks will not be considered in the estimation of 
//! the fundamental (default is 10 kHz). Must be
//! greater than the lower bound.
//!	
//!	\param x is the new value of this parameter. 				
//
void 
FundamentalEstimator::setFreqCeiling( double x ) 
{ 
	VERIFY_ARG( setFreqCeiling, x > m_lowerBound );
    m_freqCeiling = x; 
}

// ---------------------------------------------------------------------------
//	setHopTime
// ---------------------------------------------------------------------------
//!	Set the hop time in seconds, time between (attempted) fundamental
//! frequency estimates.
//!	
//!	\param x is the new value of this parameter. 				
//
void 
FundamentalEstimator::setHopTime( double x ) 
{ 
	VERIFY_ARG( setHopTime, x > 0 );
    m_hopTime = x; 
}

// ---------------------------------------------------------------------------
//	setPrecision
// ---------------------------------------------------------------------------
//!	Set the precision of the estimate in Hz, the
//! fundamental frequency will be estimated to 
//! within this range (default is 0.1 Hz).
//!	
//!	\param x is the new value of this parameter. 				
//
void 
FundamentalEstimator::setPrecision( double x ) 
{ 
	VERIFY_ARG( setPrecision, x > 0 );
    m_precision = x; 
}

//  -- envelope access --

// ---------------------------------------------------------------------------
//  envelope
// ---------------------------------------------------------------------------
const LinearEnvelope & 
FundamentalEstimator::fundamentalEnv( void ) const
{   
    return m_f0Env; 
}

// ---------------------------------------------------------------------------
//  envelope
// ---------------------------------------------------------------------------
const LinearEnvelope & 
FundamentalEstimator::energyEnv( void ) const
{   
    return m_energyEnv; 
}

//  -- Envelope interface --

// ---------------------------------------------------------------------------
//  clone
// ---------------------------------------------------------------------------
//! Return an exact copy of this FundamentalEstimator (following the
//! Prototype pattern).
//!
//! \return a new FundamentalEstimator instance
FundamentalEstimator * FundamentalEstimator::clone( void ) const 
{ 
    return new FundamentalEstimator( *this ); 
}
    
// ---------------------------------------------------------------------------
//  valueAt
// ---------------------------------------------------------------------------
//! Return an estimate of the fundamental frequency
//! at the specified time by evaluating the fundamental
//! frequency envelope constructed in the most recent
//! call to analyze().
//!
//! \param  time is the time at which to estimate the fundamental
//! \return the estimate of fundamental frequency in Hz
double FundamentalEstimator::valueAt( double time ) const
{
    return m_f0Env.valueAt( time );
}        

// -- helper --

// ---------------------------------------------------------------------------
//  buildEnvelope - helper
// ---------------------------------------------------------------------------
//
void 
FundamentalEstimator::buildEnvelopes( const Peaks & peaks, double frameTime )
{
    //  workspace
    static std::vector< double > amplitudes, frequencies;

    amplitudes.clear();
    frequencies.clear();
    
    double timeSum = 0;
    double sqrAmpSum = 0;
    unsigned int peakCount = 0;
    for ( Peaks::const_iterator spkpos = peaks.begin(); spkpos != peaks.end(); ++spkpos )
    {
        if ( spkpos->second.amplitude() > m_ampFloor &&
             spkpos->second.frequency() < m_freqCeiling )
        {
            double amp = spkpos->second.amplitude();
            sqrAmpSum += amp * amp;
            
            amplitudes.push_back( amp );
            frequencies.push_back( spkpos->second.frequency() );
            
            timeSum += spkpos->first;
            ++peakCount;
        }
    }
    
    //  compute the time for these estimates:
    double estTime = frameTime;
    if ( 0 < peakCount )
    {
        estTime += ( timeSum / peakCount );
    }
    
    //  estimate F0 if possible
    if ( ! amplitudes.empty() )
    {
        //  estimate f0
        double f0 = iterative_estimate( amplitudes, frequencies, 
                                        m_lowerBound,
                                        m_upperBound,
                                        m_precision );
        
        if ( f0 > m_lowerBound && f0 < m_upperBound )
        {
            double estTime = frameTime + ( timeSum / peakCount );
            m_f0Env.insert( estTime, f0 );
        }
    }
    
    //  always add energy estimate
    m_energyEnv.insert( estTime, std::sqrt( sqrAmpSum ) );    
}

}   //  end of namespace Loris

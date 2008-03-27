#ifndef INCLUDE_FUNDESTIMATOR_H
#define INCLUDE_FUNDESTIMATOR_H
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
 * FundamentalEstimator.h
 *
 * Definition of class FundamentalEstimator for computing an estimate of 
 * time-varying fundamental frequency from a sequence of samples using a
 * frequency domain maximum likelihood algorithm adapted from Quatieri's 
 * speech signal processing textbook. The analysis and spectral peak 
 * selection mechanisms are taken directly from the Loris Analyzer (see
 * Analyzer.C).
 *
 * HEY!!! This is still not integrated into the distribution, probably
 * needs a decent name, and a unit test.
 *
 * Kelly Fitz, 3 July 2006
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */


/*
	HEY HEY HEY 
	The interface for this thing is not really that great. Ideally, 
	I think, one would construct one of these things from either a
	sequence of Partials or a sequence of samples, and then could
	be queried at any time (provking an analysis that would yield
	an estimate, or an exception), or could contruct a LinearEnvelope
	by a sequence of queries. This would require only two different 
	mechanisms for collecting amps and frequencies, the rest is common.
	
	Idea: integrate the functionality provided by this class into 
	the Fundamental class, which _is_ part of the distribution.
*/

#include "Envelope.h"
#include "LinearEnvelope.h"
#include "PartialList.h"
#include "SpectralPeaks.h"

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	class FundamentalEstimator
//
//!	Class FundamentalEstimator represents an algorithm for 
//! time-varying fundamental frequency estimation based on
//! time-frequency reassigned spectral analysis. This class
//! is adapted from the Analyzer class (see Analyzer.h), and 
//! performs the same spectral analysis and peak extraction, 
//! but does not form Partials.
//!	
//!	For more information about Reassigned Bandwidth-Enhanced 
//!	Analysis and the Reassigned Bandwidth-Enhanced Additive Sound 
//!	Model, refer to the Loris website: www.cerlsoundgroup.org/Loris/.
//!
//! FundamentalEstimator implements the Envelope interface (see
//! Envelope.h), and evaluates to an estimate of the fundamental
//! frequency in Hz.
//
class FundamentalEstimator : public Envelope
{
//	-- public interface --
public:

//	-- construction --

	//!	Construct a new FundamentalEstimator configured with the 	
	//!	specified parameters.  lowerBoundHz and upperBoundHz are 
    //! frequency bounds on the search for a likely estimate
    //! of the fundamental (a narrower range will speed 
    //! up the search). 
	//!	
	//!	\param lowerBoundHz is the minimum instantaneous fundamental 
    //! frequency in Hz.
	//!	\param upperBoundHz is the maximum instantaneous fundamental 
    //! frequency in Hz.
    //! \throw InvalidArgument if f1==f2 or
    //! or if either frequency is negative.
	FundamentalEstimator( double lowerBoundHz, double upperBoundHz );

	//!	Construct  a new FundamentalEstimator having identical
	//!	parameter configuration to another FundamentalEstimator. 
	//!	
	//!	\param other is the FundamentalEstimator to copy.	
	FundamentalEstimator( const FundamentalEstimator & other );

	//!	Destroy this FundamentalEstimator.
	~FundamentalEstimator( void );

	//!	Construct  a new FundamentalEstimator having identical
	//!	parameter configuration to another FundamentalEstimator. 
	//!	
	//!	\param rhs is the FundamentalEstimator to copy.	
	FundamentalEstimator & operator=( const FundamentalEstimator & rhs );

//	-- configuration --

	//!	Configure this FundamentalEstimator with the specified parameters. 
	//!	
	//!	\param lowerBoundHz is the minimum instantaneous fundamental 
    //! frequency in Hz.
	//!	\param upperBoundHz is the maximum instantaneous fundamental 
    //! frequency in Hz.
    void configure( double lowerBoundHz, double upperBoundHz );

//	-- analysis --

	//!	Estimate the fundamental for a vector of (mono) samples at the given 
	//!	sample rate (in Hz). The time-varying fundamental frequency estimate
    //! is stored in a LinearEnvelope, accessed by fundamentalEnv(). Another 
    //! LinearEnvelope, acessed by energyEnv(), stores a time-varying estimate
    //! of the spectral energy (root sum of spectral amplitude squares) that
    //! constributed to each fundamental frequency estimate.
	//!	
    //! \param  vec is a vector of floating point samples
	//!	\param 	srate is the sample rate of the samples in the buffer
	void analyze( const std::vector<double> & vec, double srate );

	//!	Estimate the fundamental for a range of (mono) samples at the given 
	//!	sample rate (in Hz). The time-varying fundamental frequency estimate
    //! is stored in a LinearEnvelope, accessed by fundamentalEnv(). Another 
    //! LinearEnvelope, acessed by energyEnv(), stores a time-varying estimate
    //! of the spectral energy (root sum of spectral amplitude squares) that
    //! constributed to each fundamental frequency estimate.
	//!	
	//!	\param 	bufBegin is a pointer to a buffer of doubleing point samples
	//!	\param 	bufEnd is (one-past) the end of a buffer of floating point 
	//!			samples
	//!	\param 	srate is the sample rate of the samples in the buffer
	void analyze( const double * bufBegin, const double * bufEnd, double srate );

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
    void analyze( Iter begin_partials, Iter end_partials ); 
#else 
    void analyze( PartialList::const_iterator begin_partials, 
                  PartialList::const_iterator end_partials );
#endif
	
//	-- parameter access --

	//!	Return the amplitude floor in dB, 
    //! relative to full amplitude sine wave, absolute
    //! amplitude threshold (negative) below which spectral
    //! peaks will not be considered in the estimation of 
    //! the fundamental (default is -60 dB).			
	double ampFloor( void ) const;

	//!	Return the frequency ceiling in Hz, the
    //! frequency threshold above which spectral
    //! peaks will not be considered in the estimation of 
    //! the fundamental (default is 10 kHz).			
  	double freqCeiling( void ) const;

	//!	Return the hop time in seconds, time between (attempted) fundamental
    //! frequency estimates.
	double hopTime( void ) const;

	//!	Return the minimum instantaneous fundamental frequency, in Hz
    //! that can be detected by this FundamentalEstimator.
	double lowerBound( void ) const;

	//!	Return the precision of the estimate in Hz, the
    //! fundamental frequency will be estimated to 
    //! within this range (default is 0.1 Hz).
	double precision( void ) const;

	//!	Return the maximum instantaneous fundamental frequency, in Hz
    //! that can be detected by this FundamentalEstimator.
	double upperBound( void ) const;
	 
//	-- parameter mutation --

	//!	Set the amplitude floor in dB, 
    //! relative to full amplitude sine wave, absolute
    //! amplitude threshold (negative) below which spectral
    //! peaks will not be considered in the estimation of 
    //! the fundamental (default is -60 dB).
    //!	
	//!	\param x is the new value of this parameter.			
  	void setAmpFloor( double x );

	//!	Set the frequency ceiling in Hz, the
    //! frequency threshold above which spectral
    //! peaks will not be considered in the estimation of 
    //! the fundamental (default is 10 kHz). Must be
    //! greater than the lower bound.
	//!	
	//!	\param x is the new value of this parameter.			
	void setFreqCeiling( double x );

	//!	Set the hop time in seconds, time between (attempted) fundamental
    //! frequency estimates.
	//!	
	//!	\param x is the new value of this parameter.			
	void setHopTime( double x );

	//!	Set the precision of the estimate in Hz, the
    //! fundamental frequency will be estimated to 
    //! within this range (default is 0.1 Hz).
    //!	
	//!	\param x is the new value of this parameter.			
	void setPrecision( double x );

//  -- envelope access --

    //! Return the envelope storing the most recent fundamental
    //! frequency estimate.
    const LinearEnvelope & fundamentalEnv( void ) const;

    //! Return the envelope storing the most recent fundamental
    //! energy estimate.
    const LinearEnvelope & energyEnv( void ) const;

//  -- Envelope interface --

    //! Return an exact copy of this FundamentalEstimator (following the
    //! Prototype pattern).
    //!
    //! \return a new FundamentalEstimator instance
    virtual FundamentalEstimator * clone( void ) const;
    
    //! Return an estimate of the fundamental frequency
    //! at the specified time by evaluating the fundamental
    //! frequency envelope constructed in the most recent
    //! call to analyze().
    //!
    //! \param  time is the time at which to estimate the fundamental
    //! \return the estimate of fundamental frequency in Hz
    virtual double valueAt( double time ) const;
        
private:
//  -- private helper --

    void buildEnvelopes( const Peaks & peaks, double frameTime );

//  -- private member variables --

    double m_lowerBound;        //!  in Hz, minimum instantaneous fundamental frequency
    
    double m_upperBound;        //!  in Hz, maximum instantaneous fundamental frequency

    double m_precision;         //!  in Hz, fundamental frequency will be estimated to 
                                //!  within this range (default is 0.1 Hz)
    
    double m_ampFloor;        	//!  absolute amplitude threshold (negative) below which spectral
                                //!  peaks will not be considered in the estimation of 
                                //!  the fundamental (default is equivalent to -60 dB)
        
    double m_freqCeiling;      	//!  in Hz, frequency threshold above which spectral
                                //!  peaks will not be considered in the estimation of 
                                //!  the fundamental (default is 10 kHz)
        
    double m_hopTime;         	//!  in seconds, time between (attempted) fundamental
                                //!  frequency estimates
                    
    LinearEnvelope m_f0Env;     //! the time varying fundamental frequency estimate, 
                                //! computed in the most recent invocation of analyze() 
                
    LinearEnvelope m_energyEnv; //! the time varying signal energy estimate, 
                                //! computed in the most recent invocation of analyze() 
                
//  -- default parameters --

    static const double DefaultAmpFloor;    //! the default amplitude threshold in dB
    
    static const double DefaultPrecision;   //! the default frequency precision in Hz

    static const double DefaultFreqCeiling; //! the default frequency threshold in Hz
    
};	//	end of class FundamentalEstimator

}	//	end of namespace Loris

#endif /* ndef INCLUDE_FUNDESTIMATOR_H */

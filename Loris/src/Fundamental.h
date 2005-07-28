#ifndef INCLUDE_FUNDAMENTAL_H
#define INCLUDE_FUNDAMENTAL_H
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
 * Fundamental.h
 *
 * Definition of class Fundamenal for computing an estimate of 
 * fundamental frequency from a sequence of Partials using a 
 * maximum likelihood algorithm.
 *
 * Kelly Fitz, 10 June 2004
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "Exception.h"
#include "Envelope.h"
#include "BreakpointEnvelope.h"
#include "PartialList.h"

//	begin namespace
namespace Loris {


// ---------------------------------------------------------------------------
//	class Fundamental
//
//!	Fundamental represents a time varying estimate of fundamental
//!	frequency based on maximum likelihood analysis of a sequence
//!	of Partials. The estimate can be queried at a specific time
//!	or a BreakpointEnvelope can be constructed by sampling the
//!	fundamental frequency estiamte at regular intervals.
//!	
//! The reliability of the estimate depends on the quality of the 
//! analysis, so make sure that the partials yield a good 
//! reconstruction before attempting to estimate the fundamental.
//!
//!	Fundamental implements the Envelope interface (see
//!	Envelope.h).
//
class Fundamental : public Envelope
{
//	-- instance variables --
	PartialList partials_;		//	store a collated copy of the 
								//	sequence of Partials
	double freqMin_, freqMax_; 	//	frequency bounds on the search for 
								//	a likely estimate
	double ampThreshold_;		//	minimum Partial amplitude (dB), quieter Partials
								//	are ignored when estimating the fundamental
	double freqResolution_;		//	estimates of fundamental frequency are 
								//	computed iteratively until within this
								//	many Hz of the local most likely value
	
//	-- public interface --
public:
//	-- construction --

	//!	Construct a fundamental estimator for a sequence
	//!	of Partials. [begin_partials, end_partials) must 
	//!	specify a valid range of Partials. f1 and f2 are 
	//!	frequency bounds on the search for a likely estimate
	//!	of the fundamental (a narrower range will speed 
	//!	up the search). Throws InvalidArgument if f1==f2 or
	//!	or if either frequency is negative.
	//!		 	
	//! \param  begin_partials is the beginning of a sequence of Partials.
	//! \param  end_partials is the end of a sequence of Partials.
	//! \param  fmin is the lower bound on the fundamental
	//!         frequency estimate
	//! \param  fmax is the lower bound on the fundamental
	//!         frequency estimate
#if !defined(NO_TEMPLATE_MEMBERS)
	template<typename Iter>
	Fundamental( Iter begin_partials, Iter end_partials,
				 double fmin, double fmax  ); 
#else
	Fundamental( PartialList::const_iterator begin_partials, 
			  	 PartialList::const_iterator end_partials,
				 double fmin, double fmax );
#endif
	
	//	copy, assign, and destroy are free, the 
	//	compiler-generated versions are OK	 
	 
//	-- estimation of fundamental frequency --

	//! Return the estimate of the fundamental frequency
	//! at the specified time. Throws InvalidArgument if
	//! there are no Partials having sufficient energy to
	//! contribute to an estimate of the fundamental frequency
	//! at the specified time. Throws InvalidObject if no likely 
	//! estimate is found in the frequency range (freqMin_, freqMax_).
	//!
	//! \param  time is the time at which to estimate the fundamental
	//! \return the estimate of fundamental frequency in Hz
	double estimateAt( double time ) const;

	 
    //! Function call operator, same as estimateAt, for
    //! using Fundamental as a functor.
    //! 
	//! \param  time is the time at which to estimate the fundamental
	//! \return the estimate of fundamental frequency in Hz
	double operator() ( double time ) const;
	 
    //! Return a BreakpointEnvelope that evaluates to a linear
    //! envelope approximation to the fundamental frequency 
    //! estimate sampled at regular intervals. interval is the
    //! sampling interval in seconds. Throws InvalidArgument
    //! if no Partials have sufficient energy to contribute
    //! to an estimate of the fundamental frequency at any 
    //! time in the range [t1,t2]. Throws InvalidObject if 
    //! no likely estimate is found in the frequency range 
    //! (freqMin_, freqMax_).
    //!
    //! \param  interval is the time between breakpoints in the
    //!         fundamental envelope
    //! \return a new BreakpointEnvelope
	BreakpointEnvelope constructEnvelope( double interval ) const;
     
    //! Return a BreakpointEnvelope that evaluates to a linear
    //! envelope approximation to the fundamental frequency 
    //! estimate sampled at regular intervals. Consider only
    //! the time between t1 and t2. interval is the
    //! sampling interval in seconds. Throws InvalidArgument
    //! if no Partials have sufficient energy to contribute
    //! to an estimate of the fundamental frequency at any 
    //! time in the range [t1,t2]. Throws InvalidObject if 
    //! no likely estimate is found in the frequency range 
    //! (freqMin_, freqMax_).
    //!
    //! \param  t1 is the beginning of the time interval
    //! \param  t2 is the end of the time interval
    //! \param  interval is the time between breakpoints in the
    //!         fundamental envelope
    //! \return a new BreakpointEnvelope
	BreakpointEnvelope 
	constructEnvelope( double t1, double t2, double interval ) const;
 
//	-- parameter access/mutation --

    //! Get the minimum Partial amplitude in dB (relative to a 
    //! full amplitude sine wave), quieter Partials are ignored when 
    //! estimating the fundamental.
    //!
    //! \return the ampitude threshold in dB
	double ampThreshold( void ) const { return ampThreshold_; }
	
    //! Get the minimum Partial amplitude in dB (relative to a 
    //! full amplitude sine wave), quieter Partials are ignored when 
    //! estimating the fundamental.
    //!
    //! \param  x is the new amplitude threshold
	void setAmpThreshold( double x );
	
    //! Get the resolution of the fundamental frequency estimates. 
    //! Estimates of fundamental frequency are computed iteratively 
    //! until within this many Hz of the local most likely value. 
    //!
    //! \return the resolution in Hz
	double freqResolution( void ) const { return freqResolution_; }
	
    //! Set the resolution of the fundamental frequency estimates. 
    //! Estimates of fundamental frequency are computed iteratively 
    //! until within this many Hz of the local most likely value. 
    //!
    //! \param  x is the new resolution in Hz
	void setFreqResolution( double x ); 
    
//	-- Envelope interface --

    //! Return an exact copy of this FrequencyReference (following the
    //! Prototype pattern).
    //!
    //! \return a new Fundamental instance
	virtual Fundamental * clone( void ) const 
		{ return new Fundamental( *this ); }
	
	virtual double valueAt( double time ) const
		{ return estimateAt( time ); }
    //! Same as estimateAt:
    //! Return the estimate of the fundamental frequency
    //! at the specified time. Throws InvalidArgument if
    //! there are no Partials having sufficient energy to
    //! contribute to an estimate of the fundamental frequency
    //! at the specified time.
    //!
	//! \param  time is the time at which to estimate the fundamental
	//! \return the estimate of fundamental frequency in Hz
	 
//	-- default parameters --

	static const double DefaultThreshold;   //! the default amplitude threshold in dB
	static const double DefaultResolution;  //! the default frequency resolution in Hz

private:
//	-- internal implementation members --
	void preparePartials( void );	//	preprocess the Partials for speed
	
};	// end of class FrequencyReference


// ---------------------------------------------------------------------------
//	constructor from Partial range
// ---------------------------------------------------------------------------
//	Construct a fundamental estimator for a sequence
//	of Partials. [begin_partials, end_partials) must 
//	specify a valid range of Partials. f1 and f2 are 
//	frequency bounds on the search for a likely estimate
//	of the fundamental (a narrower range will speed 
//	up the search). Throws InvalidArgument if f1==f2 or
//	if either frequency is negative.
//
#if !defined(NO_TEMPLATE_MEMBERS)
template <typename Iter>
Fundamental::Fundamental( Iter begin_partials, Iter end_partials,
						  double f1, double f2  ) :
#else
inline
Fundamental::Fundamental( PartialList::const_iterator begin_partials, 
						  PartialList::const_iterator end_partials,
						  double f1, double f2 ) :
#endif
	//	initializers
	partials_( begin_partials, end_partials ),
	freqMin_( (f1<f2)?(f1):(f2) ),	//	the lesser of f1 and f2
	freqMax_( (f1<f2)?(f2):(f1) ),	//	the greater of f1 and f2
	ampThreshold_( DefaultThreshold ),
	freqResolution_( DefaultResolution )
{
	//	sanity check:
	if ( f1 == f2 )
	{
		Throw( InvalidArgument, "Cannot estimate the fundamental over "
								"an empty frequency range." );
	}
	if ( f1 < 0 )
	{
		Throw( InvalidArgument, "Cannot estimate the fundamental over "
								"a negative frequency range." );
	}
	
	preparePartials();
}

}	//	end of namespace Loris

#endif	// ndef INCLUDE_FUNDAMENTAL_H

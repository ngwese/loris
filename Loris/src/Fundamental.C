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
 * Fundamental.C
 *
 * Implementation of class Fundamenal for computing an estimate of 
 * fundamental frequency from a sequence of Partials using a 
 * maximum likelihood algorithm.
 *
 * Kelly Fitz, 10 June 2004
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include "config.h"
#endif

#include "Fundamental.h"

#include "Breakpoint.h"
#include "Collator.h"
#include "Exception.h"
#include "LinearEnvelope.h"
#include "Partial.h"
#include "PartialList.h"
#include "PartialUtils.h"
#include "estimateF0.h"

#include <cmath>
#include <utility>

#include <vector>
using std::vector;

#if defined(HAVE_M_PI) && (HAVE_M_PI)
	const double Pi = M_PI;
#else
	const double Pi = 3.14159265358979324;
#endif

//	begin namespace
namespace Loris {

//	forward declarations for helper, defined below:
static void 
collect_ampsNfreqs( PartialList::const_iterator begin, 
					PartialList::const_iterator end,  
					double t,
					vector<double> & amps, 
					vector<double> & freqs,
					double threshold );

//	default parameters:
const double Fundamental::DefaultThreshold = -60;
const double Fundamental::DefaultResolution = .1;

// ---------------------------------------------------------------------------
//	setAmpThreshold
// ---------------------------------------------------------------------------
//	Set the minimum Partial amplitude in dB (relative to a full amplitude
//	sine wave), quieter Partials are ignored when estimating the fundamental.
//
void 
Fundamental::setAmpThreshold( double x )
{
	if ( x > 0 )
	{
		Throw( InvalidArgument, "amplitude threshold must be expressed in (negative) "
							   "dB relative to a full amplitude sine wave" );
	}
	ampThreshold_ = x;
}
	
// ---------------------------------------------------------------------------
//	setFreqResolution
// ---------------------------------------------------------------------------
//	Set the resolution of the fundamental frequency estimates. 
//	Estimates of fundamental frequency are computed iteratively until within 
//	this many Hz of the local most likely value.
//
void 
Fundamental::setFreqResolution( double x )
{
	if ( x <= 0 )
	{
		Throw( InvalidArgument, "frequency resolution (Hz) must be positiive" );
	}
	freqResolution_ = x;
}
	

// ---------------------------------------------------------------------------
//	estimateAt
// ---------------------------------------------------------------------------
//	Return the estimate of the fundamental frequency
//	at the specified time. Throws InvalidArgument if
//	there are no Partials having sufficient energy to
//	contribute to an estimate of the fundamental frequency
//	at the specified time. Throws InvalidObject if no likely 
//	estimate is found in the frequency range (freqMin_, freqMax_). 
//	
//	Smoothing of these estimates is not necessary or productive,
//	because, like the Partial parameters from which they are
//	derived, they are pretty slowly-varying.
//
double 
Fundamental::estimateAt( double time ) const
{
	//	collect the Partial amplitudes and
	//	frequencies at `time':
	vector<double> amps, freqs;
	collect_ampsNfreqs( partials_.begin(), partials_.end(), time, 
					    amps, freqs, ampThreshold_ );

	if ( amps.empty() )
	{
		Throw( InvalidArgument, "No partials have significant energy at the "
								      "specified time." );
	}
   
   double f0 = iterative_estimate( amps, freqs, freqMin_, freqMax_, 
									        freqResolution_ );
   if ( f0 <= freqMin_ || f0 >= freqMax_ )
   {
      Throw( InvalidObject, "Cannot construct a reliable estimate "
                            "on the specified range of frequencies." );
   }	

   return f0;
}
 
// ---------------------------------------------------------------------------
//	constructEnvelope
// ---------------------------------------------------------------------------
//	Return a LinearEnvelope that evaluates to a linear
//	envelope approximation to the fundamental frequency 
//	estimate sampled at regular intervals. interval is the
//	sampling interval in seconds. Throws InvalidArgument
//	if no Partials have sufficient energy to contribute
//	to an estimate of the fundamental frequency at any 
//	time in the range [t1,t2]. Throws InvalidObject if 
//	no likely estimate is found in the frequency range 
//	(freqMin_, freqMax_).
//
LinearEnvelope 
Fundamental::constructEnvelope( double interval ) const
{
	std::pair< double, double > span =
		PartialUtils::timeSpan( partials_.begin(), partials_.end() );
	return constructEnvelope( span.first, span.second, interval );
}

// ---------------------------------------------------------------------------
//	constructEnvelope
// ---------------------------------------------------------------------------
//	Return a LinearEnvelope that evaluates to a linear
//	envelope approximation to the fundamental frequency 
//	estimate sampled at regular intervals. Consider only
//	the time between t1 and t2. interval is the
//	sampling interval in seconds. Throws InvalidArgument
//	if no Partials have sufficient energy to contribute
//	to an estimate of the fundamental frequency at any 
//	time in the range [t1,t2]. Throws InvalidObject if 
//	no likely estimate is found in the frequency range 
//	(freqMin_, freqMax_).
//
LinearEnvelope 
Fundamental::constructEnvelope( double t1, double t2, double interval ) const
{
	//	make t1 the starting time and t2 the ending time:
	if ( t1 > t2 )
	{
		std::swap( t1, t2 );
	}

	LinearEnvelope env;
	vector<double> amps, freqs;
	double t = t1;
	bool found_energy = false;
	
	//	invariant:
	//	t is a time on the range [t1,t2] at which the fundamental
	//	should be estimated, env contains all previous estimates
	//	that were deemed reliable (not boundaries), found_energy
	//	is true if any estimate has been computed at any prioir
	//	time, even if it was deemed unreliable, or false otherwise.
	while ( t <=t2 )
	{
		//	collect the Partial amplitudes and
		//	frequencies at time t:
		collect_ampsNfreqs( partials_.begin(), partials_.end(), t, 
						    amps, freqs, ampThreshold_ );
		
		if ( ! amps.empty() )
		{
			found_energy = true;
			double f0 = iterative_estimate( amps, freqs, freqMin_, freqMax_,
                                            freqResolution_ );
			//	reject boundary frequencies
			if ( f0 > freqMin_ && f0 < freqMax_ )
			{
				env.insertBreakpoint( t, f0 );
			}
		}
		
		t += interval;		
	}
	
	if ( ! found_energy )
	{
		Throw( InvalidObject, "No Partials have sufficient energy to "
							  "estimate the fundamental." );
	}
	else if ( env.size() == 0 )
	{
		Throw( InvalidObject, "Cannot construct a reliable estimate "
                              "on the specified range of frequencies." );
	}
	
	//	apply a smoothing filter to the fundamental estimates:
	//	median_filter( env.begin(), env.end(), SmoothingNPts );
	//
	//	no reason to do this, the estimates, like the Partial
	//	parameters from which they are derived, are slowly-varying.
	
	return env;
}

// ---------------------------------------------------------------------------
//	preparePartials
// ---------------------------------------------------------------------------
//	Private helper to reduce the Partials in order to speed up the 
//	estimation process.
//
void
Fundamental::preparePartials( void )
{
	//	remove all labels first, only want to collate the Partials:
	PartialList::iterator it;
	for ( it = partials_.begin(); it != partials_.end(); ++it )
	{
		it->setLabel( 0 );
	}
	
	//	collate the partials
	Collator coll;
	coll.collate( partials_ );
}				   

// ---------------------------------------------------------------------------
// ---------- helper for new fundamental estimation method ------------------
// ---------------------------------------------------------------------------
//	collect_ampsNfreqs
//
//	Collect the amplitudes and frequnecies of all
//	Partials having sufficient amplitude at time t.
//
static void 
collect_ampsNfreqs( PartialList::const_iterator begin, 
					PartialList::const_iterator end, 
					double t,
					vector<double> & amps, 
					vector<double> & freqs,
					double threshold )
{
	//	only consider Partials having amplitude above 
	//	a certain threshold (-60 dB?):
	const double absThreshold = std::pow( 10.0, threshold*.05 );
	
	amps.clear();
	freqs.clear();
	
	PartialList::const_iterator it;
	for ( it = begin; it != end; ++it )
	{
		double a = it->amplitudeAt(t);
		if ( a > absThreshold )
		{
			double sine_amp = std::sqrt(1 - it->bandwidthAt(t)) * a;
			amps.push_back( sine_amp );
			freqs.push_back( it->frequencyAt(t) );
		}
	}
}

}	//	end of namespace Loris

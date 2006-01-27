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
 * Resampler.C
 *
 * Implementation of class Resampler, for converting reassigned Partial envelopes
 * into more conventional additive synthesis envelopes, having data points
 * at regular time intervals. The benefits of reassigned analysis are NOT
 * lost in this process, since the elimination of unreliable data and the
 * reduction of temporal smearing are reflected in the resampled data.
 *
 * Lippold, 7 Aug 2003
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 *
 * Phase correction added by Kelly 13 Dec 2005.
 */
#if HAVE_CONFIG_H
	#include "config.h"
#endif

#include "Resampler.h"
#include "Breakpoint.h"
#include "Exception.h"
#include "Notifier.h"
#include "Partial.h"

#include "phasefix.h"
#define PHASE_CORRECT

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	constructor
// ---------------------------------------------------------------------------
//! Construct a new Resampler using the specified sampling
//! interval.
//!
//! \param  sampleInterval is the resampling interval in seconds, 
//!         Breakpoint data is computed at integer multiples of
//!         sampleInterval seconds.
//! \throw  InvalidArgument if sampleInterval is not positive.
//
Resampler::Resampler( double sampleInterval ) :
   interval_( sampleInterval )
{
   if ( sampleInterval <= 0. )
   {
      Throw( InvalidArgument, "Resampler sample interval must be positive." );
   }
}

// ---------------------------------------------------------------------------
//	resample
// ---------------------------------------------------------------------------
//! Resample a Partial using this Resampler's stored sampling interval.
//! The Breakpoint times in the resampled Partial will comprise a  
//! contiguous sequence of integer multiples of the sampling interval,
//! beginning with the multiple nearest to the Partial's start time and
//! ending with the multiple nearest to the Partial's end time. Resampling
//! is performed in-place. 
//!
//! \param  p is the Partial to resample
//
void 
Resampler::resample( Partial & p ) const
{
	debugger << "resampling Partial having " << p.numBreakpoints() 
			 << " Breakpoints" << endl;

	//	create the new Partial:
	Partial newp;
	newp.setLabel( p.label() );

	//  find time of first and last breakpoint for the resampled envelope:
	double firstTime = interval_ * int( 0.5 + p.startTime() / interval_ );
	double stopTime  = p.endTime() + ( 0.5 * interval_ );
	
	// To damp or not to damp?
	// When correcting phase, use damping if the resamping
	// interval is less than the length of a period, otherwise
	// don't damp the correction.
	//
	// No, maybe damping is always needed for small intervals?
	// This smooth fade is a kludge that seems to work. 
	double damping = 0.5;
	const double MAGICKLUDGE = 0.012;
	if ( interval_  > MAGICKLUDGE )
	{
		damping += std::min( 0.5, 100 * (interval_ - MAGICKLUDGE) );
	}
	// debugger << "damping is " << damping << endl;
	
	//  resample:
	for (  double tim = firstTime; tim < stopTime; tim += interval_ ) 
	{
		Breakpoint newbp( p.frequencyAt( tim ), p.amplitudeAt( tim ), 
						  p.bandwidthAt( tim ), p.phaseAt( tim ) );
						  
		#if defined(PHASE_CORRECT)	
		if ( newp.numBreakpoints() != 0 )
		{			  
			//	correct frequency to match phase:
			matchPhaseFwd( newp.last(), newbp, interval_, damping );			
		}
		#endif
		
		newp.insert( tim, newbp );
	}

	//	store the new Partial:
	p = newp;
}

}	//	end of namespace Loris


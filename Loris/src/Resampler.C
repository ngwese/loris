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

//  helper declarations:
static void resample_dense( Partial & p, double interval );
static void resample_sparse( Partial & p, double interval );

// ---------------------------------------------------------------------------
//	constructor
// ---------------------------------------------------------------------------
//! Construct a new Resampler using the specified sampling
//! interval and sparse resampling.
//!
//! \param  sampleInterval is the resampling interval in seconds, 
//!         Breakpoint data is computed at integer multiples of
//!         sampleInterval seconds.
//! \throw  InvalidArgument if sampleInterval is not positive.
//
Resampler::Resampler( double sampleInterval ) :
   interval_( sampleInterval ),
   dense_( false )
{
   if ( sampleInterval <= 0. )
   {
      Throw( InvalidArgument, "Resampler sample interval must be positive." );
   }
}

// ---------------------------------------------------------------------------
//	setDenseResampling
// ---------------------------------------------------------------------------
//! Select dense or sparse resampling.
//!
//! \param  useDense is a boolean flag indicating that dense
//!         resamping (Breakpoint at every integer multiple of the 
//!         resampling interval) should be performed. If false (the
//!         default), sparse resampling (Breakpoints only at multiples
//!         of the resampling interval near Breakpoint times in the
//!         original Partial) is performed.
//
void Resampler::setDenseResampling( bool useDense )
{
    dense_ = useDense;
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
    if ( dense_ )
    {
        resample_dense( p, interval_ );
    }
    else
    {
        resample_sparse( p, interval_ );
    }
}

// ---------------------------------------------------------------------------
//	resample_dense
// ---------------------------------------------------------------------------
//! Helper function to perform dense resampling at a specified interval.
//! The Breakpoint times in the resampled Partial will comprise a  
//! contiguous sequence of integer multiples of the sampling interval,
//! beginning with the multiple nearest to the Partial's start time and
//! ending with the multiple nearest to the Partial's end time. Resampling
//! is performed in-place. 
//!
//! \param  p is the Partial to resample
//! \param  interval is the resamping interval in seconds
//
static void resample_dense( Partial & p, double interval ) 
{
	debugger << "resampling Partial labeled " << p.label()
	         << " having " << p.numBreakpoints() 
			 << " Breakpoints" << endl;

	//	create the new Partial:
	Partial newp;
	newp.setLabel( p.label() );

	//  find time of first and last breakpoint for the resampled envelope:
	double firstTime = interval * int( 0.5 + p.startTime() / interval );
	double stopTime  = p.endTime() + ( 0.5 * interval );
	
	#if defined(PHASE_CORRECT)
	// To damp or not to damp?
	// When correcting phase, use damping if the resampling
	// interval is less than the length of a period, otherwise
	// don't damp the correction.
	//
	// No, maybe damping is always needed for small intervals?
	// This smooth fade is a kludge that seems to work. 
	double damping = 0.5;
	const double MAGICKLUDGE = 0.012;
	if ( interval  > MAGICKLUDGE )
	{
		damping += std::min( 0.5, 100 * (interval - MAGICKLUDGE) );
	}
	// debugger << "damping is " << damping << endl;
	#endif
	
	//  resample:
	for (  double tim = firstTime; tim < stopTime; tim += interval ) 
	{
		Breakpoint newbp( p.frequencyAt( tim ), p.amplitudeAt( tim ), 
						  p.bandwidthAt( tim ), p.phaseAt( tim ) );
						  
		#if defined(PHASE_CORRECT)	
		if ( newp.numBreakpoints() != 0 )
		{			  
			matchPhaseFwd( newp.last(), newbp, interval, damping );			
		}
		#endif
		
		newp.insert( tim, newbp );
	}

	debugger << "resamplied Partial has " << newp.numBreakpoints() 
			 << " Breakpoints" << endl;

	//	store the new Partial:
	p = newp;
}

// ---------------------------------------------------------------------------
//	resample_sparse
// ---------------------------------------------------------------------------
//! Helper function to perform sparse resampling at a specified interval.
//! The Breakpoint times in the resampled Partial will comprise a  
//! sparse sequence of integer multiples of the sampling interval,
//! beginning with the multiple nearest to the Partial's start time and
//! ending with the multiple nearest to the Partial's end time, and including
//! only multiples that are near to Breakpoint times in the original Partial.
//! Resampling is performed in-place. 
//!
//! \param  p is the Partial to resample
//! \param  interval is the resamping interval in seconds
//
static void resample_sparse( Partial & p, double interval ) 
{
	debugger << "resampling Partial labeled " << p.label()
	         << " having " << p.numBreakpoints() 
			 << " Breakpoints" << endl;

	//	create the new Partial:
	Partial newp;
	newp.setLabel( p.label() );

	#if defined(PHASE_CORRECT)
	// To damp or not to damp?
	// When correcting phase, use damping if the resampling
	// interval is less than the length of a period, otherwise
	// don't damp the correction.
	//
	// No, maybe damping is always needed for small intervals?
	// This smooth fade is a kludge that seems to work. 
	double damping = 0.5;
	const double MAGICKLUDGE = 0.012;
	if ( interval  > MAGICKLUDGE )
	{
		damping += std::min( 0.5, 100 * (interval - MAGICKLUDGE) );
	}
	// debugger << "damping is " << damping << endl;
	#endif
	
	//  resample:
	double curtime = 0;
	double halfstep = .5 * interval;
	Partial::const_iterator iter = p.begin();
	while( iter != p.end() )
	{
	    double bpt = iter.time();
	    if ( bpt < curtime - halfstep )
	    {
	        //  advance Breakpoint iterator:
	        ++iter;
	    }    
	    else if ( curtime < bpt - halfstep )
	    {
	        //  advance current time:
	        curtime += interval;
	    }
	    else
	    {
	        //  make a resampled Breakpoint:
		    Breakpoint newbp( p.frequencyAt( curtime ), p.amplitudeAt( curtime ), 
						      p.bandwidthAt( curtime ), p.phaseAt( curtime ) );
	        
    		#if defined(PHASE_CORRECT)	
    		if ( newp.numBreakpoints() != 0 )
    		{			  
    		    double dt = curtime - newp.endTime();

            	// To damp or not to damp?
            	// When correcting phase, use damping if the resampling
            	// interval is less than the length of a period, otherwise
            	// don't damp the correction.
            	//
            	// No, maybe damping is always needed for small intervals?
            	// This smooth fade is a kludge that seems to work. 
            	double damping = 0.5;
            	const double MAGICKLUDGE = 0.012;
            	if ( dt  > MAGICKLUDGE )
            	{
            		damping += std::min( 0.5, 100 * (dt - MAGICKLUDGE) );
            	}

    			matchPhaseFwd( newp.last(), newbp, dt, damping );			
    		}
    		#endif
    		
    		newp.insert( curtime, newbp );
    		
    		//  advance the Breakpoint iterator and the current time:
    		++iter;
    		curtime += interval;
        }  	
  	}
	
	debugger << "resampled Partial has " << newp.numBreakpoints() 
			 << " Breakpoints" << endl;

	//	store the new Partial:
	p = newp;
}

}	//	end of namespace Loris


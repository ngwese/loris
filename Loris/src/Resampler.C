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

#include <cmath>

//	begin namespace
namespace Loris {

//  helper declarations:
static void resample_dense( Partial & p, double interval );
static void resample_sparse( Partial & p, double interval );
static void insert_resampled_at( Partial & newp, const Partial & p, 
                                 double curtime, double interval );
static bool check_error_at( Partial & newp, const Partial & p, 
                            double time, double interval );

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
   dense_( false ),
   phaseCorrect_( true )
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
//! Specify phase-corrected resampling, or not. If phase
//! correct, Partial frequencies are altered slightly
//! to match, as nearly as possible, the Breakpoint 
//! phases after resampling. Phases are updated so that
//! the Partial frequencies and phases are consistent after
//! resampling.
//!
//! \param  correctPhase is a boolean flag specifying that 
//!         (if true) frequency/phase correction should be
//!         applied after resampling.
void Resampler::setPhaseCorrect( bool correctPhase )
{
    phaseCorrect_ = correctPhase;
}

// ---------------------------------------------------------------------------
//	resample
// ---------------------------------------------------------------------------
//! Resample a Partial using this Resampler's stored quanitization interval.
//! If sparse resampling (the default) has be selected, Breakpoint times
//! are quantized to integer multiples of the resampling interval.
//! If dense resampling is selected, a Breakpoint will be provided at
//! every integer multiple of the resampling interval in the time span of
//! the Partial, starting and ending with the nearest multiples to the
//! ends of the Partial. Frequencies and phases are corrected to be in 
//! agreement and to match as nearly as possible the resampled phases if
//! phase correct resampling is specified (the default). Resampling
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
    
	if ( phaseCorrect_ )
    {
        // To damp or not to damp?
        // When correcting phase, use damping if the resampling
        // interval is less than the length of a period, otherwise
        // don't damp the correction.
        //
        // No, maybe damping is always needed for small intervals?
        // This smooth fade is a kludge that seems to work. 
        // 
        // Used to do this each time a Breakpoint was inserted, but
        // it amounts to the same thing as just doing it at the end.
        // fixFrequency doesn't currently allow specification of
        // the damping, may need to add that back later.
        /*
        double damping = 0.5;
        const double MAGICKLUDGE = 0.012;
        if ( interval  > MAGICKLUDGE )
        {
            damping += std::min( 0.5, 100 * (interval - MAGICKLUDGE) );
        }
        // debugger << "damping is " << damping << endl;
        */
        fixFrequency( p ); // use default maxFixPct
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
	
	
	//  resample:
	for (  double tim = firstTime; tim < stopTime; tim += interval ) 
	{
        insert_resampled_at( newp, p, tim, interval );
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
	
	//  resample:
	double curtime = 0;
	double halfstep = .5 * interval;
	Partial::const_iterator iter = p.begin();
    
    unsigned int countSkippedSteps = 0;
    double prevInsertTime = 0;
    
	while( iter != p.end() )
	{            
	    double bpt = iter.time();

        debugger << " ----------------------- \n"
                 << "considering bp at time " << bpt
                 << " and amplitude " << iter->amplitude()
                 << "\nquanitzation step time is " << curtime
                 << endl;
            
	    if ( bpt < curtime - halfstep )
	    {
	        //  advance Breakpoint iterator:
	        ++iter;
            
            debugger << "skipping bp" << endl;
	    }    
	    else if (curtime < bpt - halfstep)
        {
            //  advance current time:
            curtime += interval;
            
            ++countSkippedSteps;
            
            debugger << "skipping quantization step" << endl;
        }
        else
        {
            insert_resampled_at( newp, p, curtime, interval );
            
            //  check for errors introduced by skippint steps
            if ( ( 0 < countSkippedSteps ) &&
                 check_error_at( newp, p, curtime-interval, interval ) )
            {
                insert_resampled_at( newp, p, curtime-interval, interval );
            }
            if ( ( 1 < countSkippedSteps ) &&
                 check_error_at( newp, p, prevInsertTime+interval, interval )  )
            {
                insert_resampled_at( newp, p, prevInsertTime+interval, interval );
            }

            prevInsertTime = curtime;
            countSkippedSteps = 0;
            
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

// ---------------------------------------------------------------------------
//	insert_resampled_at (helper)
// ---------------------------------------------------------------------------
//  Sparse resampling helper for inserting a resampled Breakpoint.
//
static void 
insert_resampled_at( Partial & newp, const Partial & p, 
                     double curtime, double interval )
{
    //  make a resampled Breakpoint:
    Breakpoint newbp = p.parametersAt( curtime );
    
    //  handle end points to reduce error at ends
    if ( curtime < p.startTime() )
    {
        newbp.setAmplitude( p.first().amplitude() );
    }
    else if ( curtime > p.endTime() )
    {
        newbp.setAmplitude( p.last().amplitude() );
    }
    
    /*
    double overrideAmp = 
        p.findNearest( curtime ).breakpoint().amplitude();
    newbp.setAmplitude( overrideAmp );
    */
    
    /*
    Used to do this here, now do it all at once in resample.
    
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
    */
    
    newp.insert( curtime, newbp );
    
    debugger << "inserted Breakpoint having amplitude " << newbp.amplitude() 
             << " at time " << curtime << endl;
}

// ---------------------------------------------------------------------------
//	check_error_at (helper)
// ---------------------------------------------------------------------------
//  Sparse resampling helper for correcting errors introduced by skipping
//  quantization steps. Fix by not skipping so many!
//
static bool 
check_error_at( Partial & newp, const Partial & p, 
                double time, double interval )
{
    //  don't insert extra Breakpoints past the ends
    //  of the Partial
    if ( time < p.startTime() || time > p.endTime() )
    {
        return false;
    }

    Breakpoint original = p.parametersAt( time, interval );
    Breakpoint resampled = newp.parametersAt( time );
        
    //  amplitude tolerance is 1% of original
    const double eps = 1E-6;
    double ampErr = std::fabs( original.amplitude() - resampled.amplitude() ) / 
                    ( original.amplitude() + eps );
    if ( ampErr > 0.01 )
    {
        return true;
    }
    
    //  frequency tolerance is 1% of original
    double freqErr = std::fabs( original.frequency() - resampled.frequency() ) / 
                        ( original.frequency() );
    if ( freqErr > 0.01 )
    {
        return true;
    }

    //  bandwidth tolerance is 10% of original
    double bwErr = std::fabs( original.bandwidth() - resampled.bandwidth() ) / 
                        ( original.bandwidth() + eps );
    if ( bwErr > 0.1 )
    {
        return true;
    }
    
    return false;
}

}	//	end of namespace Loris


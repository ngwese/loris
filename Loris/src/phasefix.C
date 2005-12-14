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
 * phasefix.C
 *
 * Implements a phase correction algorithm that perturbs slightly the 
 * frequencies or Breakpoints in a Partial so that the rendered Partial 
 * will achieve (or be closer to) the analyzed Breakpoint phases.
 *
 * Kelly Fitz, 23 Sept 04
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#if HAVE_CONFIG_H
	#include "config.h"
#endif

#include "phasefix.h"

#include "Breakpoint.h"
#include "Exception.h"
#include "Notifier.h"
#include "Partial.h"


#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>

#if defined(HAVE_M_PI) && (HAVE_M_PI)
	const double Pi = M_PI;
#else
	const double Pi = 3.14159265358979324;
#endif

//	begin namespace
namespace Loris {

#pragma mark -- local helpers -- 

// ---------------------------------------------------------------------------
//	wrapPi
//	Wrap an unwrapped phase value to the range (-pi,pi].
//
inline double wrapPi( double x )
{
	x = std::fmod( x, 2*Pi );
	
	if ( x > Pi )
	{
		x = x - ( 2*Pi );
	}
	else if ( x < -Pi )
	{
		x = x + ( 2*Pi );
	}
	return x;
}

// ---------------------------------------------------------------------------
//	isNonNull
//	Return true if a Breakpoint has non-zero amplitude, false otherwise.
//
static bool isNonNull( const Breakpoint & bp )
{
	return bp.amplitude() != 0.;
}

// ---------------------------------------------------------------------------
//	phaseTravel
//
//	Compute the sinusoidal phase travel between two Breakpoints.
//	Return the total unwrapped phase travel.
//
inline double phaseTravel( const Breakpoint & bp0, const Breakpoint & bp1, 
						   double dt )
{
	double f0 = bp0.frequency();
	double f1 = bp1.frequency();
	double favg = .5 * ( f0 + f1 );
	return 2 * Pi * favg * dt;
}

// ---------------------------------------------------------------------------
//	phaseTravel
//
//	Compute the sinusoidal phase travel between two Breakpoints.
//	Return the total unwrapped phase travel.
//
static double phaseTravel( Partial::const_iterator bp0, Partial::const_iterator bp1 )
{
	return phaseTravel( bp0.breakpoint(), bp1.breakpoint(), 
	                    bp1.time() - bp0.time() );
	/*
	double f0 = bp0->frequency();
	double t0 = bp0.time();
	double f1 = bp1->frequency();
	double t1 = bp1.time();
	double favg = .5 * ( f0 + f1 );
	double dt = t1 - t0;
	return 2 * Pi * favg * dt;
	*/
}

#pragma mark -- phase correction -- 

// ---------------------------------------------------------------------------
//	fixPhaseBefore
//
//! Recompute phases of all Breakpoints earlier than the specified time 
//! so that the synthesize phases of those earlier Breakpoints matches 
//! the stored phase, and the synthesized phase at the specified
//! time matches the stored (not recomputed) phase.
//! 
//! Backward phase-fixing stops if a null (zero-amplitude) Breakpoint
//! is encountered, because nulls are interpreted as phase reset points
//! in Loris. If a null is encountered, the remainder of the Partial
//! (the front part) is fixed in the forward direction, beginning at
//! the start of the Partial.
//!
//! \param p    The Partial whose phases should be fixed.
//! \param t    The time before which phases should be adjusted.
//
void fixPhaseBefore( Partial & p, double t )
{
    if ( 1 < p.numBreakpoints() )
    {
        Partial::iterator pos = p.findNearest( t );
        while ( pos != p.begin() && isNonNull( pos.breakpoint() ) )
        {
            Partial::iterator posFwd = pos--;
            double travel = phaseTravel( pos, posFwd );
            pos.breakpoint().setPhase( wrapPi( posFwd.breakpoint().phase() - travel ) );
        }
        
        // if a null was encountered, then stop fixing backwards,
        // and fix the front of the Partial in the forward direction:
        if ( pos != p.begin() )
        {
            fixPhaseForward( p, p.startTime(), (--pos).time() );
        }
    }
}

// ---------------------------------------------------------------------------
//	fixPhaseAfter
//
//! Recompute phases of all Breakpoints later than the specified time 
//! so that the synthesize phases of those later Breakpoints matches 
//! the stored phase, as long as the synthesized phase at the specified
//! time matches the stored (not recomputed) phase.
//! 
//! Phase fixing is only applied to non-null (nonzero-amplitude) Breakpoints,
//! because null Breakpoints are interpreted as phase reset points in 
//! Loris. If a null is encountered, its phase is simply left unmodified,
//! and future phases wil be recomputed from that one.
//!
//! \param p    The Partial whose phases should be fixed.
//! \param t    The time after which phases should be adjusted.
//
void fixPhaseAfter( Partial & p, double t )
{
    fixPhaseForward( p, t, p.endTime() );
/*
    if ( 1 < p.numBreakpoints() )
    {
        Partial::iterator pos = p.findNearest( t );
        Partial::const_iterator stopHere = --( p.end() );
        while ( pos != stopHere )
        {
            Partial::iterator posPrev = pos++;
            double travel = phaseTravel( posPrev, pos );
            pos.breakpoint().setPhase( wrapPi( posPrev.breakpoint().phase() + travel ) );
        }
    }
*/
}

// ---------------------------------------------------------------------------
//	fixPhaseForward
//
//! Recompute phases of all Breakpoints later than the specified time 
//! so that the synthesize phases of those later Breakpoints matches 
//! the stored phase, as long as the synthesized phase at the specified
//! time matches the stored (not recomputed) phase. Breakpoints later than
//! tend are unmodified.
//! 
//! Phase fixing is only applied to non-null (nonzero-amplitude) Breakpoints,
//! because null Breakpoints are interpreted as phase reset points in 
//! Loris. If a null is encountered, its phase is simply left unmodified,
//! and future phases wil be recomputed from that one.
//!
//! \param p    The Partial whose phases should be fixed.
//! \param tbeg The phases and frequencies of Breakpoints later than the 
//!             one nearest this time will be modified.
//! \param tend The phases and frequencies of Breakpoints earlier than the 
//!             one nearest this time will be modified. Should be greater 
//!             than tbeg, or else they will be swapped.
//
void fixPhaseForward( Partial & p, double tbeg, double tend )
{
    if ( tbeg > tend )
    {
        std::swap( tbeg, tend );
    }

    if ( 1 < p.numBreakpoints() )
    {
        Partial::iterator pos = p.findNearest( tbeg );
        Partial::const_iterator stopHere = p.findAfter( tend );
        if ( pos != stopHere )
        {
            --stopHere;
        }
        while ( pos != stopHere )
        {
            Partial::iterator posPrev = pos++;
            if ( isNonNull( pos.breakpoint() ) )
            {
                double travel = phaseTravel( posPrev, pos );
                pos.breakpoint().setPhase( wrapPi( posPrev.breakpoint().phase() + travel ) );
            }
        }
    }
}

// ---------------------------------------------------------------------------
//	fixPhaseAt
//
//! Recompute phases of all Breakpoints in a Partial
//! so that the synthesize phases match the stored phases, 
//! and the synthesized phase at (nearest) the specified
//! time matches the stored (not recomputed) phase.
//! 
//! Backward phase-fixing stops if a null (zero-amplitude) Breakpoint
//! is encountered, because nulls are interpreted as phase reset points
//! in Loris. If a null is encountered, the remainder of the Partial
//! (the front part) is fixed in the forward direction, beginning at
//! the start of the Partial. Forward phase fixing is only applied 
//! to non-null (nonzero-amplitude) Breakpoints. If a null is encountered, 
//! its phase is simply left unmodified, and future phases wil be 
//! recomputed from that one.
//!
//! \param p    The Partial whose phases should be fixed.
//! \param t    The time at which phases should be made correct.
//
void fixPhaseAt( Loris::Partial & p, double t )
{
    if ( 1 < p.numBreakpoints() )
    {
        fixPhaseBefore( p, t );
        fixPhaseAfter( p, t );
    }
}

// ---------------------------------------------------------------------------
//  fixPhaseBetween
//
//!	Fix the phase travel between two times by adjusting the
//!	frequency and phase of Breakpoints between those two times.
//!
//!	This algorithm assumes that there is nothing interesting about the
//!	phases of the intervening Breakpoints, and modifies their frequencies 
//!	as little as possible to achieve the correct amount of phase travel 
//!	such that the frequencies and phases at the specified times
//!	match the stored values. The phases of all the Breakpoints between 
//! the specified times are recomputed.
//!
//! THIS DOES NOT YET TREAT NULL BREAKPOINTS DIFFERENTLY FROM OTHERS.
//!
//! \pre        There must be at least one Breakpoint in the
//!             Partial between the specified times tbeg and tend.
//! \post       The phases and frequencies of the Breakpoints in the 
//!             range have been recomputed such that an oscillator
//!             initialized to the parameters of the first Breakpoint
//!             will arrive at the parameters of the last Breakpoint,
//!             and all the intervening Breakpoints will be matched.
//!	\param p    The partial whose phases and frequencies will be recomputed. 
//!             The Breakpoint at this position is unaltered.
//! \param tbeg The phases and frequencies of Breakpoints later than the 
//!             one nearest this time will be modified.
//! \param tend The phases and frequencies of Breakpoints earlier than the 
//!             one nearest this time will be modified. Should be greater 
//!             than tbeg, or else they will be swapped.
//
void fixPhaseBetween( Partial & p, double tbeg, double tend )
{  
    if ( tbeg > tend )
    {
        std::swap( tbeg, tend );
    }
    
    // for Partials that do not extend over the entire
    // specified time range, just recompute phases from
    // beginning or end of the range:
    if ( p.endTime() < tend )
    {
        // OK if start time is also after tbeg, will
        // just recompute phases from start of p.
        fixPhaseAfter( p, tbeg );
    }
    else if ( p.startTime() > tbeg )
    {
        fixPhaseBefore( p, tend );
    }
    else
    {
        // invariant:
        // p begins before tbeg and ends after tend.
        
        /*
        Partial::iterator b = p.findAfter( tbeg );
        Assert( p.begin() != b );
        --b;
        
        Partial::iterator e = p.findAfter( tend );
        Assert( p.end() != e );
        Assert( p.begin() != e );
        */
        
        Partial::iterator b = p.findNearest( tbeg );
        Partial::iterator e = p.findNearest( tend );
               
    	if ( std::distance( b, e ) < 2 )
    	{
    	    // Preconditions not met, cannot fix the phase travel.
    	    debugger << "cannot fix phase between " << tbeg << " and " << tend
    	             << ", there are no Breakpoints between those times" << endl;
    	    return;
    	}
    	
    	//	Accumulate the actual phase travel over the Breakpoint
    	//	span, and count the envelope segments.
    	double travel = 0;
    	Partial::iterator next = b;
    	do
    	{
    		Partial::iterator prev = next++;
    		travel += phaseTravel( prev, next );
    	} while( next != e );

    	//	Compute the desired amount of phase travel:
    	double deviation = wrapPi( e.breakpoint().phase() - ( b.breakpoint().phase() + travel ) );
    	double desired = travel + deviation;
    	
    	/*
    	debugger << "---------- fixing breakpoint frequencies over time ( " 
    		     << b.time() << " , " << e.time() << " )" << endl;
    		 
    	debugger << "desired travel: " << desired << endl;
    	debugger << "actual travel: " << travel << endl;
    	*/
    	
    	//	Compute the amount by which to perturb the frequencies of
    	//	all the null Breakpoints between b and e.
    	//
    	//	The accumulated phase travel is the sum of the average frequency
    	//	(in radians) of each segment times the duration of each segment
    	//	(the actual phase travel is computed this way). If this sum is
    	//	computed with each null-Breakpoint frequency perturbed (additively) 
    	//	by delta, and set equal to the desired phase travel, then it
    	//	can be simplified to:
    	//		delta = 2 * ( phase error ) / ( tN + tN-1 - t1 - t0 )
    	//	where tN is the time of e, tN-1 is the time of its predecessor,
    	//	t0 is the time of b, and t1 is the time of b's successor.
    	//
    	Partial::iterator iter = b;
    	double t0 = iter.time();
    	++iter;
    	double t1 = iter.time();
    	iter = e;
    	double tN = iter.time();
    	--iter;
    	double tNm1 = iter.time();
    	
    	Assert( t1 < tN );	//	else there were no Breakpoints in between
    	
    	double delta = ( 2 * ( desired - travel ) / ( tN + tNm1 - t1 - t0 ) ) / ( 2 * Pi );
    	
    	//	Perturb the null-Breakpoint frequencies.
    	double DEBUGtravel = 0;
    	next = b;
    	Partial::iterator prev = next++;
    	while ( next != e )
    	{
    		//debugger << "changing frequency from " << next.breakpoint().frequency();
    		next.breakpoint().setFrequency( next.breakpoint().frequency() + delta );
    		//debugger << " to " << next.breakpoint().frequency() << endl;
    		
    		double newtravel = phaseTravel( prev, next );
    		DEBUGtravel += newtravel;
    		
    		//debugger << "changing phase from " << wrapPi( next.breakpoint().phase() );
    		next.breakpoint().setPhase( wrapPi( prev.breakpoint().phase() + newtravel ) );
    		//debugger << " to " << next.breakpoint().phase() << endl;
    		
    		prev = next++;
    	}
    	DEBUGtravel += phaseTravel( prev, next );
        /*
    	debugger << "travel: " << DEBUGtravel << endl;
    	
    	debugger << "desired: " << e.breakpoint().phase() << endl;
    	debugger << "got: " << wrapPi( prev.breakpoint().phase() + phaseTravel( prev, next ) ) << endl;
    	debugger << "---------- done." << endl;
    	*/
    }
}

// ---------------------------------------------------------------------------
//	matchPhaseFwd
//
//!	Compute the target frequency that will affect the
//!	predicted (by the Breakpoint phases) amount of
//!	sinusoidal phase travel between two breakpoints, 
//!	and assign that frequency to the target Breakpoint.
//!	After computing the new frequency, update the phase of
//!	the later Breakpoint.
//!
//! The most common kinds of errors are local (or burst) errors in 
//! frequency and phase. These errors are best corrected by correcting
//! less than half the detected error at any time. Correcting more
//! than that will produce frequency oscillations for the remainder of
//! the Partial, in the case of a single bad frequency (as is common
//! at the onset of a tone). Any damping factor less then one will 
//! converge eventually, .5 or less will converge without oscillating.
//! Use the damping argument to control the damping of the correction.
//!	Specify 1 for no damping.
//!
//! \pre		The two Breakpoints are assumed to be consecutive in
//!				a Partial.
//! \param		bp0	The earlier Breakpoint.
//! \param		bp1	The later Breakpoint.
//! \param		dt The time (in seconds) between bp0 and bp1.
//! \param		damping The fraction of the amount of phase error that will
//!				be corrected (.5 or less will prevent frequency oscilation 
//!				due to burst errors in phase). 
//! \param		maxFixPct The maximum amount of frequency adjustment
//!				that can be made to the frequency of bp1, expressed
//!				as a precentage of the unmodified frequency of bp1.
//!				If the necessary amount of frequency adjustment exceeds
//!				this amount, then the phase will not be matched, 
//!				but will be updated as well to be consistent with
//!				the frequencies. (default is 0.2%)
//
void matchPhaseFwd( const Breakpoint & bp0, Breakpoint & bp1,
				    double dt, double damping, double maxFixPct )
{
	double travel = phaseTravel( bp0, bp1, dt ); 
	double err = wrapPi( bp1.phase() - ( bp0.phase() + travel ) );
	
	//  The most common kinds of errors are local (or burst) errors in 
	//  frequency and phase. These errors are best corrected by correcting
	//  less than half the detected error at any time. Correcting more
	//  than that will produce frequency oscillations for the remainder of
	//  the Partial, in the case of a single bad frequency (as is common
	//  at the onset of a tone). Any damping factor less then one will 
	//  converge eventually, .5 or less will converge without oscillating.
	//  #define DAMPING .5
	travel += damping * err;
	
	double f0 = bp0.frequency();
	double ftgt = ( travel / ( Pi * dt ) ) - f0;
	
	#ifdef Loris_Debug
	debugger << "matchPhaseFwd: correcting " << bp1.frequency() << " to " << ftgt 
			 << " (phase " << wrapPi( bp1.phase() ) << "), ";
	#endif
	
	//	If the target is not a null breakpoint, may need to 
	//	clamp the amount of frequency modification.
	if ( bp1.amplitude() != 0. )
	{	
		if ( ftgt > bp1.frequency() * ( 1 + (maxFixPct*.01) ) )
		{
			ftgt = bp1.frequency() * ( 1 + (maxFixPct*.01) );
		}
		else if ( ftgt < bp1.frequency() * ( 1 - (maxFixPct*.01) ) )
		{
			ftgt = bp1.frequency() * ( 1 - (maxFixPct*.01) );
		}
	}
	bp1.setFrequency( ftgt );
	
	//	Recompute the phase according to the new frequency.
	double phi = wrapPi( bp0.phase() + phaseTravel( bp0, bp1, dt ) );
	bp1.setPhase( phi );

	#ifdef Loris_Debug
	debugger << "achieved " << ftgt << " (phase " << phi << ")" << endl;
	#endif
}

// ---------------------------------------------------------------------------
//	fixFrequency
//
//!	Adjust frequencies of the Breakpoints in the 
//! specified Partial such that the rendered Partial 
//!	achieves (or matches as nearly as possible, within 
//!	the constraint of the maximum allowable frequency
//! alteration) the analyzed phases. 
//!
//! THIS DOES NOT YET TREAT NULL BREAKPOINTS DIFFERENTLY FROM OTHERS.
//!
//!  \param     partial The Partial whose frequencies,
//!             and possibly phases (if the frequencies
//!             cannot be sufficiently altered to match
//!             the phases), will be recomputed.
//!  \param     maxFixPct The maximum allowable frequency 
//!             alteration, default is 0.2%.
//
void fixFrequency( Partial & partial, double maxFixPct )
{	
	if ( partial.numBreakpoints() > 1 )
	{	
		Partial::iterator next = partial.begin();
		Partial::iterator prev = next++;
		while ( next != partial.end() )		
		{
			matchPhaseFwd( prev.breakpoint(), next.breakpoint(), 
						   next.time() - prev.time(), 0.5, maxFixPct );
			prev = next++;
		}
    }
#if 0
            // Not anymore:
            //
            //! Null Breakpoints (zero amplitude) are treated
            //! differently. The phases and frequencies of null
            //! Breakpoints are perturbed to match the frequencies
            //! and phases of the surrounding non-null Breakpoints
            //! (using fixPhaseTravel).

			else
			{
				//	identify a span of null Breakpoints
				next = std::find_if( next, partial.end(), isNonNull );	
				if ( next != partial.end() )
				{	
				    //  next is a non-null Breakpoint later than
				    //  prev, and there is at least one null
				    //  Breakpoint between prev and next.
					fixPhaseTravel( prev, next );
				}
				else
				{
					// 	What if the rest of the Partial is
					//	null Breakpoints (or next was the
					//	last Breakpoint in the Partial)?		
					//
					//	Assume that null Breakpoints are 
					//	uninteresting, just recompute their
					//	phases.
					/*
					next = prev;
					for ( ++next; next != partial.end(); ++next )
					{
						next.breakpoint().setPhase( prev.breakpoint().phase() + phaseTravel( prev, next ) )	;
						prev = next;
					}	
					*/
					computePhaseFwd( prev, next );
					--next;						
				}
			}
			prev = next++;
		}
#endif
}

}	//	end of namespace Loris

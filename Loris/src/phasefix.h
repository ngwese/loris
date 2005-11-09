#ifndef PHASEFIX_H
#define PHASEFIX_H
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
 * phasefix.h
 *
 * Declares a phase correction algorithm that perturbs slightly the 
 * frequencies or Breakpoints in a Partial so that the rendered Partial 
 * will achieve (or be closer to) the analyzed Breakpoint phases.
 *
 * Kelly Fitz, 23 Sept 04
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "Partial.h"

//	begin namespace
namespace Loris {

//  FUNCTION PROTOYPES

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
void fixPhaseBefore( Partial & p, double t );

//	fixPhaseBefore (range)
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
//! \param b    The beginning of a range of Partials whose phases 
//!             should be fixed.
//! \param e    The end of a range of Partials whose phases 
//!             should be fixed.
//! \param t    The time before which phases should be adjusted.
//
template < class Iter >
void fixPhaseBefore( Iter b, Iter e, double t )
{
    while ( b != e )
    {
        fixPhaseBefore( *b, t );
        ++b;
    }
}

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
void fixPhaseAfter( Partial & p, double t );

//	fixPhaseAfter (range)
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
//! \param b    The beginning of a range of Partials whose phases 
//!             should be fixed.
//! \param e    The end of a range of Partials whose phases 
//!             should be fixed.
//! \param t    The time after which phases should be adjusted.
//
template < class Iter >
void fixPhaseAfter( Iter b, Iter e, double t )
{
    while ( b != e )
    {
        fixPhaseAfter( *b, t );
        ++b;
    }
}

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
void fixPhaseForward( Partial & p, double tbeg, double tend );

//	fixPhaseForward (range)
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
//! \param b    The beginning of a range of Partials whose phases 
//!             should be fixed.
//! \param e    The end of a range of Partials whose phases 
//!             should be fixed.
//! \param tbeg The phases and frequencies of Breakpoints later than the 
//!             one nearest this time will be modified.
//! \param tend The phases and frequencies of Breakpoints earlier than the 
//!             one nearest this time will be modified. Should be greater 
//!             than tbeg, or else they will be swapped.
//
template < class Iter >
void fixPhaseForward( Iter b, Iter e, double tbeg, double tend )
{
    while ( b != e )
    {
        fixPhaseAfter( *b, tbeg, tend );
        ++b;
    }
}


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
void fixPhaseAt( Partial & p, double t );

//	fixPhaseAt (range)
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
//! \param b    The beginning of a range of Partials whose phases 
//!             should be fixed.
//! \param e    The end of a range of Partials whose phases 
//!             should be fixed.
//! \param t    The time at which phases should be made correct.
//
template < class Iter >
void fixPhaseAt( Iter b, Iter e, double t )
{
    while ( b != e )
    {
        fixPhaseAt( *b, t );
        ++b;
    }
}

//	fixPhaseBetween
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
//! \pre      Thre must be at least one Breakpoint in the
//!           Partial between the specified times t1 and t2.
//!           If this condition is not met, the Partial is
//!           unmodified.
//! \post     The phases and frequencies of the Breakpoints in the 
//!           range have been recomputed such that an oscillator
//!           initialized to the parameters of the first Breakpoint
//!           will arrive at the parameters of the last Breakpoint,
//!           and all the intervening Breakpoints will be matched.
//!	\param p  The partial whose phases and frequencies will be recomputed. 
//!           The Breakpoint at this position is unaltered.
//! \param t1 The time before which Partial frequencies and phases will 
//!           not be modified.
//! \param t2 The time after which Partial frequencies and phases will 
//!           not be modified. Should be greater than t1, or else they
//!           will be swapped.
//
void fixPhaseBetween( Partial & p, double t1, double t2 );

//	fixPhaseBetween (range)
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
//! \pre        Thre must be at least one Breakpoint in each
//!             Partial between the specified times t1 and t2.
//!             If this condition is not met, the Partial is
//!             unmodified.
//! \post       The phases and frequencies of the Breakpoints in the 
//!             range have been recomputed such that an oscillator
//!             initialized to the parameters of the first Breakpoint
//!             will arrive at the parameters of the last Breakpoint,
//!             and all the intervening Breakpoints will be matched.
//! \param b    The beginning of a range of Partials whose phases 
//!             should be fixed.
//! \param e    The end of a range of Partials whose phases 
//!             should be fixed.
//! \param t1   The time before which Partial frequencies and phases will 
//!             not be modified.
//! \param t2   The time after which Partial frequencies and phases will 
//!             not be modified. Should be greater than t1, or else they
//!             will be swapped.
//
template < class Iter >
void fixPhaseBetween( Iter b, Iter e, double t1, double t2 )
{
    while ( b != e )
    {
        fixPhaseBetween( *b, t1, t2 );
        ++b;
    }
}


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
void fixFrequency( Partial & partial, double maxFixPct = 0.2 );


//	fixFrequency
//
//!	Adjust frequencies of the Breakpoints in the 
//! specified Partials such that the rendered Partial 
//!	achieves (or matches as nearly as possible, within 
//!	the constraint of the maximum allowable frequency
//! alteration) the analyzed phases. 
//!
//! THIS DOES NOT YET TREAT NULL BREAKPOINTS DIFFERENTLY FROM OTHERS.
//!
//! \param b    The beginning of a range of Partials whose 
//!             frequencies should be fixed.
//! \param e    The end of a range of Partials whose frequencies 
//!             should be fixed.
//!  \param     maxFixPct The maximum allowable frequency 
//!             alteration, default is 0.2%.
//
template < class Iter >
void fixFrequency( Iter b, Iter e, double maxFixPct = 0.2 )
{
    while ( b != e )
    {
        fixFrequency( *b, maxFixPct );
        ++b;
    }
}

}	//	end of namespace Loris

#endif // ndef PHASEFIX_H


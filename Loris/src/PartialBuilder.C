/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2007 by Kelly Fitz and Lippold Haken
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
 * PartialBuilder.C
 *
 * Implementation of a class representing a policy for connecting peaks
 * extracted from a reassigned time-frequency spectrum to form ridges
 * and construct Partials.
 *
 * This strategy attemps to follow a reference frequency envelope when 
 * forming Partials, by prewarping all peak frequencies according to the
 * (inverse of) frequency reference envelope. At the end of the analysis, 
 * Partial frequencies need to be un-warped by calling fixPartialFrequencies().
 *
 * The first attempt was the same as the basic partial formation strategy, 
 * but for purposes of matching, peak frequencies are scaled by the ratio
 * of the reference envelope's value at the previous frame to its value
 * at the current frame. This was not adequate, didn't store enough history
 * so it wasn't really following the reference envelope, just using it to
 * make a local decision about how frequency should drift from one frame to
 * the next.
 *
 * Kelly Fitz, 28 May 2003
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include "config.h"
#endif

#include "PartialBuilder.h"

#include "BreakpointEnvelope.h"
#include "Envelope.h"
#include "Notifier.h"
#include "Partial.h"
#include "PartialList.h"
#include "PartialPtrs.h"
#include "SpectralPeaks.h"

#include <algorithm>
#include <cmath>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	construction
// ---------------------------------------------------------------------------
PartialBuilder::PartialBuilder( double drift ) :
	reference( BreakpointEnvelope(1.0) ),
	freqDrift( drift )
{
}

// ---------------------------------------------------------------------------
//	construction
// ---------------------------------------------------------------------------
PartialBuilder::PartialBuilder( double drift, const Envelope & env ) :
	reference( env ),
	freqDrift( drift )
{
}

// ---------------------------------------------------------------------------
//	end_frequency
// ---------------------------------------------------------------------------
//	Return the frequency of the last Breakpoint in a Partial.
//
static inline double end_frequency( const Partial & partial )
{
	return partial.last().frequency();
}

// ---------------------------------------------------------------------------
//	freq_distance
// ---------------------------------------------------------------------------
//	Helper function, used in formPartials().
//	Returns the (positive) frequency distance between a Breakpoint 
//	and the last Breakpoint in a Partial.
//
static inline double 
freq_distance( const Partial & partial, const Breakpoint & bp )
{
	return std::fabs( end_frequency( partial ) - bp.frequency() );
}

// ---------------------------------------------------------------------------
//	better_match
// ---------------------------------------------------------------------------
//	predicate for choosing the better of two proposed
//	Partial-to-Breakpoint matches.
//
//	Return true if the first match is better, otherwise
//	return false.
//
static bool 
better_match( const Partial & part1, const Breakpoint & bp1,
			  const Partial & part2, const Breakpoint & bp2 )
{
	Assert( part1.numBreakpoints() > 0 );
	Assert( part2.numBreakpoints() > 0 );
	
	return freq_distance( part1, bp1 ) < freq_distance( part2, bp2 );
}			 

// ---------------------------------------------------------------------------
//	peak_frequency
// ---------------------------------------------------------------------------
//	Helper for finding the Breakpoint frequency for a spectral peak.
//
static double peak_frequency( const Peaks::value_type & p )
{
	return p.breakpoint.frequency();
}

// ---------------------------------------------------------------------------
//	sort_increasing_freq
// ---------------------------------------------------------------------------
//	Comparitor for sorting spectral peaks in order of 
//	increasing frequency, or finding maximum frequency.
//
static bool sort_increasing_freq( const Peaks::value_type & lhs, 
								  const Peaks::value_type & rhs )
{ 
	return peak_frequency( lhs ) < peak_frequency( rhs ); 
}

// ---------------------------------------------------------------------------
//	formPartials
// ---------------------------------------------------------------------------
//	Append the peaks (Breakpoint) extracted from a reassigned time-frequency
//	spectrum to eligible Partials, where possible. Peaks that cannot
//	be used to extend eliglble Partials spawn new Partials.
//
//	This is the same as the basic partial formation strategy, except that
//	before matching, all frequencies are normalized by the value of the 
//	reference envelope at the time of the current frame. This means that
//	the frequency envelopes of all the Partials are goofey, and need to 
//	be un-normalized by calling fixPartialFrequencies (below) at the end
//	of the analysis.
//
void 
PartialBuilder::formPartials( Peaks & peaks, double frameTime )
{
	const double refValue = reference.valueAt( frameTime );
	Assert( refValue > 0 );
	
	double normalizedDrift = freqDrift / refValue;
	
	newlyEligible.clear();
	
	unsigned int matchCount = 0;	//	for debugging
	
	//	normalize all peak frequencies according to the frequency
	//	reference envelope -- do this before sorting!
	for ( Peaks::iterator bpIter = peaks.begin(); bpIter != peaks.end(); ++bpIter ) 
	{
		Breakpoint & bp = bpIter->breakpoint;
		const double peakTime = frameTime + bpIter->time;
		bp.setFrequency( bp.frequency() / reference.valueAt( peakTime ) );
	}
		
	
	//	frequency-sort the spectral peaks:
	//	(the eligible partials are always sorted by
	//	increasing frequency if we always sort the
	//	peaks this way)
	std::sort( peaks.begin(), peaks.end(), sort_increasing_freq );
	
	PartialPtrs::iterator eligible = eligiblePartials.begin();
	for ( Peaks::iterator bpIter = peaks.begin(); bpIter != peaks.end(); ++bpIter ) 
	{
		const Breakpoint & bp = bpIter->breakpoint;
		const double peakTime = frameTime + bpIter->time;
		
		// 	find the Partial that is nearest in frequency to the Peak:
		PartialPtrs::iterator nextEligible = eligible;
		if ( eligible != eligiblePartials.end() &&
			 end_frequency( **eligible ) < bp.frequency() )
		{
			++nextEligible;
			while ( nextEligible != eligiblePartials.end() &&
					end_frequency( **nextEligible ) < bp.frequency() )
			{
				++nextEligible;
				++eligible;
			}
			
			if ( nextEligible != eligiblePartials.end() &&
				 better_match( **nextEligible, bp, **eligible, bp ) )
			{
				eligible = nextEligible;
			}
		}
		
		// 	INVARIANT:
		//
		//	eligible is the position of the nearest (in frequency)
		//	eligible Partial (pointer) or it is eligiblePartials.end().
		//
		//	nextEligible is the eligible Partial with frequency 
		//	greater than bp, or it is eligiblePartials.end().
			
		/*
		if ( nextEligible != eligiblePartials.end() )
		{
			debugger << matchFrequency << "( " << end_frequency( **eligible )
					 << ", " << end_frequency( **nextEligible ) << ")" << endl;
		}
		*/
								
		//	create a new Partial is there is no eligible Partial,
		//	or the frequency difference to the eligible Partial is 
		//	too great, or the next peak is a better match for the 
		//	eligible Partial, otherwise add this peak to the eligible
		//	Partial:
		Peaks::iterator nextPeak = Peaks::iterator( bpIter ); ++nextPeak;
								  // ++Peaks::iterator( bpIter );
		if ( eligible == eligiblePartials.end() ||
			 freq_distance( **eligible, bp ) > normalizedDrift ||
			 ( nextPeak != peaks.end() &&
			   better_match( **eligible, nextPeak->breakpoint, **eligible, bp ) ) )
		{
			Partial p;
			p.insert( peakTime, bp );
			partials_.push_back( p );
			newlyEligible.push_back( & partials_.back() );
		}		
		else 
		{
			(*eligible)->insert( peakTime, bp );
			newlyEligible.push_back( *eligible );
			
			++matchCount;
		}
		
		//	update eligible, nextEligible is the eligible Partial
		//	with frequency greater than bp, or it is eligiblePartials.end():
		eligible = nextEligible;
	}			 
	 	
	eligiblePartials = newlyEligible;
	
	debugger << "formPartials() matched " << matchCount << endl;
	debugger << newlyEligible.size() << " newly eligible partials" << endl;
}

// ---------------------------------------------------------------------------
//	fixPartialFrequencies
// ---------------------------------------------------------------------------
//	Undo the frequency normalization performed in formPartials, return a
//	reference to the partials.
//
PartialList &
PartialBuilder::fixPartialFrequencies( void )
{
	for ( PartialList::iterator part = partials_.begin(); 
		  part != partials_.end();
		  ++part )
	{
		for ( Partial::iterator bp = part->begin();
			  bp != part->end();
			  ++bp )
		{
			double f = bp.breakpoint().frequency() * reference.valueAt( bp.time() );
			bp.breakpoint().setFrequency( f );
		}
	}
	
	return partials_;
}



}	//	end of namespace Loris

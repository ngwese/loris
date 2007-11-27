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
 * This strategy attemps to follow a mFreqWarping frequency envelope when 
 * forming Partials, by prewarping all peak frequencies according to the
 * (inverse of) frequency mFreqWarping envelope. At the end of the analysis, 
 * Partial frequencies need to be un-warped by calling fixPartialFrequencies().
 *
 * The first attempt was the same as the basic partial formation strategy, 
 * but for purposes of matching, peak frequencies are scaled by the ratio
 * of the mFreqWarping envelope's value at the previous frame to its value
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
//  Construct a new builder that constrains Partial frequnecy
//  drift by the specified drift value in Hz.
//
PartialBuilder::PartialBuilder( double drift ) :
	mFreqWarping( new BreakpointEnvelope(1.0) ),
	mFreqDrift( drift )
{
}

// ---------------------------------------------------------------------------
//	construction
// ---------------------------------------------------------------------------
//  Construct a new builder that constrains Partial frequnecy
//  drift by the specified drift value in Hz. The frequency
//  warping envelope is applied to the spectral peak frequencies
//  and the frequency drift parameter in each frame before peaks
//  are linked to eligible Partials. All the Partial frequencies
//  need to be un-warped at the ned of the building process, by
//  calling finishBuilding().
//
PartialBuilder::PartialBuilder( double drift, const Envelope & env ) :
	mFreqWarping( env.clone() ),
	mFreqDrift( drift )
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
//	buildPartials
// ---------------------------------------------------------------------------
//	Append spectral peaks, extracted from a reassigned time-frequency
//	spectrum, to eligible Partials, where possible. Peaks that cannot
//	be used to extend eliglble Partials spawn new Partials.
//
//	This is similar to the basic MQ partial formation strategy, except that
//	before matching, all frequencies are normalized by the value of the 
//	warping envelope at the time of the current frame. This means that
//	the frequency envelopes of all the Partials are warped, and need to 
//	be un-normalized by calling finishBuilding at the end of the building
//  process.
//
void 
PartialBuilder::buildPartials( Peaks & peaks, double frameTime )
{
	const double refValue = mFreqWarping->valueAt( frameTime );
	Assert( refValue > 0 );
	
	double normalizedDrift = mFreqDrift / refValue;
	
	mNewlyEligible.clear();
	
	unsigned int matchCount = 0;	//	for debugging
	
	//	normalize all peak frequencies according to the frequency
	//	mFreqWarping envelope -- do this before sorting!
	for ( Peaks::iterator bpIter = peaks.begin(); bpIter != peaks.end(); ++bpIter ) 
	{
		Breakpoint & bp = bpIter->breakpoint;
		const double peakTime = frameTime + bpIter->time;
		bp.setFrequency( bp.frequency() / mFreqWarping->valueAt( peakTime ) );
	}
		
	
	//	frequency-sort the spectral peaks:
	//	(the eligible partials are always sorted by
	//	increasing frequency if we always sort the
	//	peaks this way)
	std::sort( peaks.begin(), peaks.end(), sort_increasing_freq );
	
	PartialPtrs::iterator eligible = mEligiblePartials.begin();
	for ( Peaks::iterator bpIter = peaks.begin(); bpIter != peaks.end(); ++bpIter ) 
	{
		const Breakpoint & bp = bpIter->breakpoint;
		const double peakTime = frameTime + bpIter->time;
		
		// 	find the Partial that is nearest in frequency to the Peak:
		PartialPtrs::iterator nextEligible = eligible;
		if ( eligible != mEligiblePartials.end() &&
			 end_frequency( **eligible ) < bp.frequency() )
		{
			++nextEligible;
			while ( nextEligible != mEligiblePartials.end() &&
					end_frequency( **nextEligible ) < bp.frequency() )
			{
				++nextEligible;
				++eligible;
			}
			
			if ( nextEligible != mEligiblePartials.end() &&
				 better_match( **nextEligible, bp, **eligible, bp ) )
			{
				eligible = nextEligible;
			}
		}
		
		// 	INVARIANT:
		//
		//	eligible is the position of the nearest (in frequency)
		//	eligible Partial (pointer) or it is mEligiblePartials.end().
		//
		//	nextEligible is the eligible Partial with frequency 
		//	greater than bp, or it is mEligiblePartials.end().
			
		/*
		if ( nextEligible != mEligiblePartials.end() )
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
		if ( eligible == mEligiblePartials.end() ||
			 freq_distance( **eligible, bp ) > normalizedDrift ||
			 ( nextPeak != peaks.end() &&
			   better_match( **eligible, nextPeak->breakpoint, **eligible, bp ) ) )
		{
			Partial p;
			p.insert( peakTime, bp );
			mCollectedPartials.push_back( p );
			mNewlyEligible.push_back( & mCollectedPartials.back() );
		}		
		else 
		{
			(*eligible)->insert( peakTime, bp );
			mNewlyEligible.push_back( *eligible );
			
			++matchCount;
		}
		
		//	update eligible, nextEligible is the eligible Partial
		//	with frequency greater than bp, or it is mEligiblePartials.end():
		eligible = nextEligible;
	}			 
	 	
	mEligiblePartials = mNewlyEligible;
	
	debugger << "formPartials() matched " << matchCount << endl;
	debugger << mNewlyEligible.size() << " newly eligible partials" << endl;
}

// ---------------------------------------------------------------------------
//	finishBuilding
// ---------------------------------------------------------------------------
//	Un-do the frequency warping performed in buildPartials, and return 
//	the Partials that were built. After calling finishBuilding, the
//  builder is returned to its initial state, and ready to build another
//  set of Partials. Partials are returned by appending them to the 
//  supplied PartialList.
//
void
PartialBuilder::finishBuilding( PartialList & product )
{
    //  unwarp all Breakpoint frequencies:
	for ( PartialList::iterator part = mCollectedPartials.begin(); 
		  part != mCollectedPartials.end();
		  ++part )
	{
		for ( Partial::iterator bp = part->begin();
			  bp != part->end();
			  ++bp )
		{
			double f = bp.breakpoint().frequency() * mFreqWarping->valueAt( bp.time() );
			bp.breakpoint().setFrequency( f );
		}
	}
	
    //  append the collected Partials to the product list:
	product.splice( product.end(), mCollectedPartials );
    
    //  reset the builder state:
    mEligiblePartials.clear();
    mNewlyEligible.clear();
}



}	//	end of namespace Loris

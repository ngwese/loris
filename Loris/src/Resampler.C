/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
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
 */
#if HAVE_CONFIG_H
	#include <config.h>
#endif

#include <Resampler.h>
#include <Breakpoint.h>
#include <Exception.h>
#include <Notifier.h>
#include <Partial.h>
#include <PartialList.h>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	constructor
// ---------------------------------------------------------------------------
//
Resampler::Resampler( double sampleInterval ) 
{
	interval_ = sampleInterval;
}

// ---------------------------------------------------------------------------
//	resample
// ---------------------------------------------------------------------------
//	Resample a Partial using this Resampler's stored sampling interval.
//	The Breakpoint times in the resampled Partial will comprise a  
//	contiguous sequence of integer multiples of the sampling interval,
//	beginning with the multiple nearest to the Partial's start time and
//	ending withthe multiple nearest to the Partial's end time. Resampling
//	is performed in-place. 
//
void 
Resampler::resample( Partial & p )
{
	debugger << "resampling Partial having " << p.numBreakpoints() 
			 << " Breakpoints" << endl;

	//	create the new Partial:
	Partial newp;
	newp.setLabel( p.label() );

	//  find time of first and last breakpoint for the resampled envelope:
	double firstTime = interval_ * int( 0.5 + p.startTime() / interval_ );
	double lastTime  = interval_ * int( 0.5 + p.endTime()   / interval_ );
	
	//  resample:
	for ( double tim = firstTime; tim <= lastTime; tim += interval_ ) 
	{
		Breakpoint newbp( p.frequencyAt( tim ), p.amplitudeAt( tim ), 
						  p.bandwidthAt( tim ), p.phaseAt( tim ) );
		newp.insert( tim, newbp );
	}

	//	store the new Partial:
	p = newp;

}


}	//	end of namespace Loris


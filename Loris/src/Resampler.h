#ifndef INCLUDE_RESAMPLER_H
#define INCLUDE_RESAMPLER_H
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
 * Resampler.h
 *
 * Definition of class Resampler, for converting reassigned Partial envelopes
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

#include <PartialList.h>

//	begin namespace
namespace Loris {

class Partial;

// ---------------------------------------------------------------------------
//	class Resampler
//
//	Class Resampler represents an algorithm for resampling Partial envelopes
//	at regular time intervals. Resampling makes the envelope data more suitable 
//	for exchange (as SDIF data, for example) with other applications that
//	cannot process raw (continuously-distributed) reassigned data. Resampling
//	will often greatly reduce the size of the data (by greatly reducing the 
//	number of Breakpoints in the Partials) without adversely affecting the
//	quality of the reconstruction.
//
class Resampler
{
//	-- instance variables --
	double interval_;	// the resampling interval
	
//	-- public interface --
public:
//	-- construction --
	Resampler( double sampleInterval );
	/*	Construct a new Resampler using the specified sampling
		interval.
	 */
	 
//	-- resampling --
	void resample( Partial & p );
	/*	Resample a Partial using this Resampler's stored sampling interval.
		The Breakpoint times in the resampled Partial will comprise a  
		contiguous sequence of integer multiples of the sampling interval,
		beginning with the multiple nearest to the Partial's start time and
		ending withthe multiple nearest to the Partial's end time. Resampling
		is performed in-place. 
	 */
	 
	void resample( PartialList::iterator begin, PartialList::iterator end  )
	{
		while ( begin != end )
			resample( *begin++ );
	}
	/*
		Resample all Partials in the specified (half-open) range using this
		Resampler's stored sampling interval, so that the Breakpoints in 
		the Partial envelopes will all lie on a common temporal grid.
		The Breakpoint times in the resampled Partial will comprise a  
		contiguous sequence of integer multiples of the sampling interval,
		beginning with the multiple nearest to the Partial's start time and
		ending withthe multiple nearest to the Partial's end time. Resampling
		is performed in-place. 
	 */

};	//	end of class Resampler

}	//	end of namespace Loris

#endif /* ndef INCLUDE_RESAMPLER_H */


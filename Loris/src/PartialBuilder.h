#ifndef INCLUDE_PARTIALBUILDER_H
#define INCLUDE_PARTIALBUILDER_H
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
 * PartialBuilder.h
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
 * Kelly Fitz, 28 May 2003
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 
#include "Partial.h"
#include "PartialList.h"
#include "PartialPtrs.h"
#include "SpectralPeaks.h"

//	begin namespace
namespace Loris {

class Envelope;

// ---------------------------------------------------------------------------
//	class PartialBuilder
//
//	A class representing the process of connecting peaks (ridges) on a 
//	reassigned time-frequency surface to form Partials.
//
class PartialBuilder
{
// --- interface ---
public:
	//	construction:
	explicit PartialBuilder( double drift );
	PartialBuilder( double drift, const Envelope & env );
	
	//	Append the peaks (Breakpoint) extracted from a reassigned time-frequency
	//	spectrum to eligible Partials, where possible. Peaks that cannot
	//	be used to extend eliglble Partials spawn new Partials.
	void formPartials( Peaks & peaks, double frameTime );

	//	Undo the freuqency normalization performed in formPartials, return a
	//	reference to the partials.
	PartialList & fixPartialFrequencies( void );

	//	Partial access:
	PartialList & partials( void ) { return partials_; }
	
// --- implementation ---
private:
	PartialList partials_;	//	collect partials here
		
	PartialPtrs eligiblePartials, newlyEligible;// 	keep track of eligible partials here	
	const Envelope & reference;					//	reference envelope
	double freqDrift;
	
};	//	end of class PartialBuilder

}	//	end of namespace Loris

#endif /* ndef INCLUDE_PARTIALBUILDER_H */

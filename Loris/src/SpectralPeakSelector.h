#ifndef INCLUDE_SPECTRALPEAKSELECTOR_H
#define INCLUDE_SPECTRALPEAKSELECTOR_H
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
 * SpectralPeakSelector.h
 *
 * Definition of a class representing a policy for selecting energy
 * peaks in a reassigned spectrum to be used in Partial formation.
 *
 * Kelly Fitz, 28 May 2003
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 
#include <Partial.h>
#include <SpectralPeaks.h>

//	begin namespace
namespace Loris {

class Envelope;
class ReassignedSpectrum;

// ---------------------------------------------------------------------------
//	class SpectralPeakSelector
//
//	A class representing the process of selecting and thinning
//	peaks (ridges) on a reassigned time-frequency surface.
//
class SpectralPeakSelector
{
// --- interface ---
public:
	//	construction:
	SpectralPeakSelector( double srate, double res );

	//	Collect and return magnitude peaks in the lower half of the spectrum, 
	//	ignoring those having frequencies below the specified minimum, and
	//	those having large time corrections.
	Peaks & extractPeaks( ReassignedSpectrum & spectrum, 
						  double minFrequency, double maxTimeOffset );
						  
	//	Reject peaks that are too close in frequency to a louder peak that is
	//	being retained, and peaks that are too quiet. Peaks that are retained,
	//	but are quiet enough to be in the specified fadeRange should be faded.
	//	
	//	Rejected peaks are placed at the end of the peak collection.
	//	Return the first position in the collection containing a rejected peak,
	//	or the end of the collection if no peaks are rejected.
	Peaks::iterator thinPeaks( double ampFloordB, double fadeRangedB, double frameTime );

	//	peak access:
	Peaks & peaks( void ) { return peaks_; }
	
// --- implementation ---
private:
	Peaks peaks_;	//	collect peaks here
	
	double freqResolution;
	double sampleRate;
	
	
};	//	end of class SpectralPeakSelector

}	//	end of namespace Loris

#endif /* ndef INCLUDE_SPECTRALPEAKSELECTOR_H */

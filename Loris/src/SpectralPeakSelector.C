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
 * SpectralPeakSelector.C
 *
 * Implementation of a class representing a policy for selecting energy
 * peaks in a reassigned spectrum to be used in Partial formation.
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

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

#include "Breakpoint.h"
#include "Envelope.h"
#include "Notifier.h"
#include "Partial.h"	//	yuk, just for label type
#include "ReassignedSpectrum.h"
#include "SpectralPeakSelector.h"


//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	construction
// ---------------------------------------------------------------------------
SpectralPeakSelector::SpectralPeakSelector( double srate, double res ) :
	freqResolution( res ),
	sampleRate( srate )
{
}

// ---------------------------------------------------------------------------
//	extractPeaks
// ---------------------------------------------------------------------------
//	Collect and return magnitude peaks in the lower half of the spectrum, 
//	ignoring those having frequencies below the specified minimum, and
//	those having large time corrections.
//
Peaks & 
SpectralPeakSelector::extractPeaks( ReassignedSpectrum & spectrum, 
									double minFrequency,
									double maxTimeOffset )
{
	using namespace std; // for abs and fabs

	const double sampsToHz = sampleRate / spectrum.size();
	const double oneOverSR = 1. / sampleRate;
	const double minFreqSample = minFrequency / sampsToHz;
	const double maxCorrectionSamples = maxTimeOffset * sampleRate;
	
	peaks_.clear();
	
	for ( int j = 1; j < (spectrum.size() / 2) - 1; ++j ) 
	{
		if ( abs(spectrum[j]) > abs(spectrum[j-1]) && 
			 abs(spectrum[j]) > abs(spectrum[j+1])) 
		{				
			//	skip low-frequency peaks:
			double fsample = spectrum.reassignedFrequency( j );
			if ( fsample < minFreqSample )
				continue;

			//	skip peaks with large time corrections:
			double timeCorrectionSamps = spectrum.reassignedTime( j );
			if ( fabs(timeCorrectionSamps) > maxCorrectionSamples )
				continue;
				
			double mag = spectrum.reassignedMagnitude( fsample, j );

			//	(reassignedPhase() must be called with time correction 
			//	in samples!)
			double phase = spectrum.reassignedPhase( j, fsample, timeCorrectionSamps );
			
			//	also store the corrected peak time in seconds, won't
			//	be able to compute it later:
			double time = timeCorrectionSamps * oneOverSR;
			Breakpoint bp ( fsample * sampsToHz, mag, 0., phase );
			peaks_.push_back( std::make_pair( time, bp ) );
						
		}	//	end if itsa peak
	}
	
	debugger << "extractPeaks found " << peaks_.size() << endl;
		
	return peaks_;
}

// ---------------------------------------------------------------------------
//	sort_peaks_greater_amplitude
// ---------------------------------------------------------------------------
//	predicate used for sorting peaks in order of decreasing amplitude:
static bool sort_peaks_greater_amplitude( const Peaks::value_type & lhs, 
										  const Peaks::value_type & rhs )
{ 
	return lhs.second.amplitude() > rhs.second.amplitude(); 
}

// ---------------------------------------------------------------------------
//	can_mask
// ---------------------------------------------------------------------------
//	functor used for identying peaks that are too close
//	in frequency to another louder peak:
struct can_mask
{
	//	masking occurs if any (louder) peak falls
	//	in the frequency range delimited by fmin and fmax:
	bool operator()( const Peaks::value_type & v )  const
	{ 
		return	( v.second.frequency() > _fmin ) && 
				( v.second.frequency() < _fmax ); 
	}
		
	//	constructor:
	can_mask( double x, double y ) : 
		_fmin( x ), _fmax( y ) 
		{ if (x>y) std::swap(x,y); }
		
	//	bounds:
private:
	double _fmin, _fmax;
};

// ---------------------------------------------------------------------------
//	thinPeaks
// ---------------------------------------------------------------------------
//	Reject peaks that are too quiet (low amplitude). Peaks that are retained,
//	but are quiet enough to be in the specified fadeRange should be faded.
//
//	This is exactly the same as the basic peak selection strategy, there
//	is no tracking here.
//	
//	Rejected peaks are placed at the end of the peak collection.
//	Return the first position in the collection containing a rejected peak,
//	or the end of the collection if no peaks are rejected.
//
Peaks::iterator 
SpectralPeakSelector::thinPeaks( double ampFloordB, double fadeRangedB, 
								 double /* frameTime not used */  )
{
	//	compute absolute magnitude thresholds:
	const double threshold = std::pow( 10., 0.05 * ampFloordB );
	const double beginFade = std::pow( 10., 0.05 * (ampFloordB+fadeRangedB) );

	//	louder peaks are preferred, so consider them 
	//	in order of louder magnitude:
	std::sort( peaks_.begin(), peaks_.end(), sort_peaks_greater_amplitude );
	
	Peaks::iterator it = peaks_.begin();
	Peaks::iterator beginRejected = it;
	while ( it != peaks_.end() ) 
	{
		Breakpoint & bp = it->second;
		
		//	keep this peak if it is loud enough and not
		//	 too near in frequency to a louder one:
		double lower = bp.frequency() - freqResolution;
		double upper = bp.frequency() + freqResolution;
		if ( bp.amplitude() > threshold &&
			 beginRejected == std::find_if( peaks_.begin(), beginRejected, can_mask(lower, upper) ) )
		{
			//	this peak is a keeper, fade its
			//	amplitude if it is too quiet:
			if ( bp.amplitude() < beginFade )
			{
				double alpha = (beginFade - bp.amplitude())/(beginFade - threshold);
				bp.setAmplitude( bp.amplitude() * (1. - alpha) );
			}
			
			//	keep retained peaks at the front of the collection:
			if ( it != beginRejected )
			{
				std::swap( *it, *beginRejected );
			}
			++beginRejected;
		}
		++it;
	}
	
	debugger << "thinPeaks retained " << std::distance( peaks_.begin(), beginRejected ) << endl;
	/*
	it = peaks_.begin();
	while ( it != beginRejected )
	{
		debugger << it->second.frequency() << endl;
		++it;
	}
	debugger << "thinPeaks rejected " << std::distance( beginRejected, peaks_.end() ) << endl;
	while ( it != peaks_.end() )
	{
		debugger << it->second.frequency() << endl;
		++it;
	}
	*/
	return beginRejected;
}

}	//	end of namespace Loris

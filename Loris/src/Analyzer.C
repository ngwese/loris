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
 * Analyzer.C
 *
 * Implementation of class Loris::Analyzer.
 *
 * Kelly Fitz, 5 Dec 99
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include <config.h>
#endif

#include <Analyzer.h>
#include <AssociateBandwidth.h>
#include <Breakpoint.h>
#include <Exception.h>
#include <KaiserWindow.h>
#include <Notifier.h>
#include <Partial.h>
#include <PartialPtrs.h>
#include <ReassignedSpectrum.h>

#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>
#include <vector>

using namespace std;

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	internal analysis helper declarations
// ---------------------------------------------------------------------------

typedef std::vector< std::pair< double, Breakpoint > > FRAME;

static void extractPeaks( FRAME & frame, double frameTime, 
						  Analyzer & analyzer, ReassignedSpectrum & spectrum,
						  AssociateBandwidth & bwAssociation, 
						  double sampleRate );
static void thinPeaks( FRAME & frame, double freqResolution, AssociateBandwidth & bwAssociation );
static void formPartials( FRAME & frame, PartialPtrs & eligiblePartials,
						  double freqDrift, PartialList & partials );

// ---------------------------------------------------------------------------
//	Analyzer constructor - frequency resolution only
// ---------------------------------------------------------------------------
//	The core analysis parameter is the frequency resolution, the minimum
//	instantaneous frequency spacing between partials. Configure all other
//	parameters according to this parameter, subsequent parameter mutations
//	will be independent.
//
Analyzer::Analyzer( double resolutionHz )
{
	configure( resolutionHz, resolutionHz );
}

// ---------------------------------------------------------------------------
//	Analyzer constructor
// ---------------------------------------------------------------------------
//	The core analysis parameter is the frequency resolution, the minimum
//	instantaneous frequency spacing between partials, but most of the time
//	we configure the short-time analysis using a window that is approximately
//	twice as wide as the frequency resolution. Configure the analyzer 
//	parameters according to independently specified frequency resolution
//	and analysis window width (both in Hz), subsequent parameter mutations
//	will be independent.
//
Analyzer::Analyzer( double resolutionHz, double windowWidthHz )
{
	configure( resolutionHz, windowWidthHz );
}

// ---------------------------------------------------------------------------
//	Analyzer copy constructor
// ---------------------------------------------------------------------------
//	Copy all parameter configuration from the other Analyzer, but not 
//	the collected Partials.
//
Analyzer::Analyzer( const Analyzer & other ) :
	_resolution( other._resolution ),
	_floor( other._floor ),
	_windowWidth( other._windowWidth ),
	_minFrequency( other._minFrequency ),
	_drift( other._drift ),
	_hop( other._hop ),
	_cropTime( other._cropTime ),
	_bwRegionWidth( other._bwRegionWidth )
{
}

// ---------------------------------------------------------------------------
//	Analyzer assignment
// ---------------------------------------------------------------------------
//	Copy all parameter configuration from the other Analyzer, but not 
//	the collected Partials. Do not modify the list of previously collected
//	Partials.
//
Analyzer & 
Analyzer::operator =( const Analyzer & rhs )
{
	if ( this != & rhs ) 
	{
		_resolution = rhs._resolution;
		_floor = rhs._floor;
		_windowWidth = rhs._windowWidth;
		_minFrequency = rhs._minFrequency;
		_drift = rhs._drift;
		_hop = rhs._hop;
		_cropTime = rhs._cropTime;
		_bwRegionWidth = rhs._bwRegionWidth;
	}
	return *this;
}

// ---------------------------------------------------------------------------
//	configure
// ---------------------------------------------------------------------------
//	Compute default values for analysis parameters from the two core
//	parameters, the frequency resolution and the analysis window width.
//
//	There are basically three classes of analysis parameters:
//	- the resolution, and params that are usually related to (or
//	identical to) the resolution (minimum frequency and drift)
//	- the window width and params that are usually related to (or
//	identical to) the window width (hop and crop times)
//	- indepenendent parameters (bw region width and amp floor)
//
void
Analyzer::configure( double resolutionHz, double windowWidthHz )
{
	_resolution = resolutionHz;
	
	//	floor defaults to -90 dB:
	_floor = -90.;
	
	//	window width should generally be approximately 
	//	equal to, and never more than twice the 
	//	frequency resolution:
	_windowWidth = windowWidthHz;
	
	//	for the minimum frequency, below which no data is kept,
	//	use the frequency resolution by default (this makes 
	//	Lip happy, and is always safe?) and allow the client 
	//	to change it to anything at all.
	_minFrequency = _resolution;
	
	//	frequency drift in Hz is the maximum difference
	//	in frequency between consecutive Breakpoints in
	//	a Partial, by default, make it equal to one half
	//	the frequency resolution:
	_drift = .5 * _resolution;
	
	//	hop time (in seconds) is the inverse of the
	//	window width....really. Smith and Serra (1990) cite 
	//	Allen (1977) saying: a good choice of hop is the window 
	//	length divided by the main lobe width in frequency samples,
	//	which turns out to be just the inverse of the width.
	_hop = 1. / _windowWidth;
	
	//	crop time (in seconds) is the maximum allowable time
	//	correction, beyond which a reassigned spectral component
	//	is considered unreliable, and not considered eligible for
	//	Breakpoint formation in extractPeaks(). By default, use
	//	the hop time (should it be half that?):
	_cropTime = _hop;
	
	//	bandwidth association region width 
	//	defaults to 2 kHz, corresponding to 
	//	1 kHz region center spacing:
	_bwRegionWidth = 2000.;
}

// ---------------------------------------------------------------------------
//	Analyzer destructor
// ---------------------------------------------------------------------------
//
Analyzer::~Analyzer( void )
{
}

// ---------------------------------------------------------------------------
//	analyze
// ---------------------------------------------------------------------------
//
void 
Analyzer::analyze( const double * bufBegin, const double * bufEnd, double srate )
{
	//	construct state objects for this analysis:	
	
	//	analysis window parameters:
	//	use the amplitude floor as the window attenuation 
	double winshape = KaiserWindow::computeShape( - ampFloor() );
	long winlen = KaiserWindow::computeLength( windowWidth() / srate, winshape );
	
	//	always use odd-length windows:
	if (! (winlen % 2)) {
		++winlen;
	}
	debugger << "Using Kaiser window of length " << winlen << endl;
	
	//	configure window:
	std::vector< double > window( winlen );
	KaiserWindow::create( window, winshape );
	
	//	configure spectrum:
	ReassignedSpectrum spectrum( window );
		
	//	configure bw association strategy, which 
	//	needs to know about the window:
	AssociateBandwidth bwAssociation( bwRegionWidth(), srate );
	
	//	collection of ptrs to Partials eligible for matching:
	PartialPtrs eligiblePartials;
	
//
//	need to check for bogus parameters somewhere!!!!
//

	//	compute hop time in samples:
	const long hop = long( hopTime() * srate );	//	truncate
	
	//	window length is first half length + second
	//	half length + the "center" sample (this works
	//	for even and odd length windows):
	const long firstHalfWinLength = winlen / 2;
	const long secondHalfWinLength = (winlen - 1) / 2;
		
	//	loop over short-time analysis frames:
	try 
	{ 
		for ( const double * winMiddle = bufBegin; 
			  winMiddle < bufEnd;
			  winMiddle += hop ) 
		{
			//	compute the time of this analysis frame:
			const double frameTime = long(winMiddle - bufBegin) / srate;
			 
			//	compute reassigned spectrum:
			//  sampsBegin is the position of the first sample to be transformed,
			//	sampsEnd is the position after the last sample to be transformed.
			const double * sampsBegin = std::max( winMiddle - firstHalfWinLength, bufBegin );
			const double * sampsEnd = std::min( winMiddle + secondHalfWinLength + 1, bufEnd );
			spectrum.transform( sampsBegin, winMiddle, sampsEnd );
			
			//	extract peaks from the spectrum:
			FRAME frame;
			extractPeaks( frame, frameTime, *this, spectrum, bwAssociation, srate );	
			thinPeaks( frame, freqResolution(), bwAssociation );

#if !defined(No_BW_Association)
			//	perform bandwidth association:
			//
			//	accumulate retained Breakpoints as sinusoids, 
			//	thinned breakpoints are accumulated as noise:
			//	(see also thinPeaks() and extractPeaks())
			FRAME::iterator it;
			for ( it = frame.begin(); it != frame.end(); ++it )
			{
				bwAssociation.accumulateSinusoid( it->second.frequency(), it->second.amplitude() );
			}
			
			//	associate bandwidth with each Breakpoint here:
			for ( it = frame.begin(); it != frame.end(); ++it )
			{
				bwAssociation.associate( it->second );
			}
			
			//	reset after association, yuk:
			bwAssociation.reset();
#endif	//	 !defined(No_BW_Association)

			//	form Partials from the extracted Breakpoints:
			formPartials( frame, eligiblePartials, freqDrift(), partials() );

		}	//	end of loop over short-time frames
	}
	catch ( Exception & ex ) 
	{
		ex.append( "analysis failed." );
		throw;
	}
}

#pragma mark -- internal analysis helpers --

// ---------------------------------------------------------------------------
//	extractPeaks
// ---------------------------------------------------------------------------
//	The reassigned spectrum has been computed, and short-time peaks
//	identified. From those peaks, construct Breakpoints, subject to 
//	some selection criteria. 
//
//	The peaks from the reassigned spectrum are frequency-sorted (implicitly)
//	so the frame generated here is automatically frequency-sorted.
//
//	This argument list is grotesque, really ought to be able to make this cleaner.
//
static void extractPeaks( FRAME & frame, double frameTime, 
						  Analyzer & analyzer, ReassignedSpectrum & spectrum,
						  AssociateBandwidth & bwAssociation, 
						  double sampleRate )
{
	const double ampFloor = analyzer.ampFloor();
	const double freqFloor = analyzer.freqFloor();
	const double cropTime = analyzer.cropTime();
	
	const double threshold = pow( 10., 0.05 * ampFloor );	//	absolute magnitude threshold
	const double sampsToHz = sampleRate / spectrum.size();
	
	//	look for magnitude peaks in the spectrum:
	for ( int j = 1; j < (spectrum.size() / 2) - 1; ++j ) 
	{
		if ( abs(spectrum[j]) > abs(spectrum[j-1]) && 
			 abs(spectrum[j]) > abs(spectrum[j+1])) 
		{				
			//	compute the fractional frequency sample
			//	and the frequency:
			double fsample = spectrum.reassignedFrequency( j );	//	fractional sample
			double fHz = fsample * sampsToHz;
			
			//	ignore peaks below our frequency and amplitude floors:
			if ( fHz < freqFloor )
				continue;
				
			//	find the time correction (in samples!) for this peak:
			double timeCorrectionSamps = spectrum.reassignedTime( j );
			
			//	ignore peaks with large time corrections:
			//	if I do this, then off-center peaks are not part of
			//	association, not part of thinning, and the cropping/breaking
			//	check in formPartials() is unnecessary (large time corrections
			//	will already have been removed).
			if ( fabs(timeCorrectionSamps) > cropTime * sampleRate )
				continue;
				
			//	breakpoints are extracted and thinned and the survivors are
			//	accumulated as sinusoids in the association process.
			double mag = spectrum.reassignedMagnitude( fsample, j );
			if ( mag < threshold )
			{
				bwAssociation.accumulateNoise( fHz, mag );
				continue;
			}
											
			//	retain a spectral peak corresponding to this sample:
			//	(reassignedPhase() must be called with time correction 
			//	in samples!)
			double phase = spectrum.reassignedPhase( j, fsample, timeCorrectionSamps );
			
			//	also store the corrected peak time in seconds, won't
			//	be able to compute it later:
			double time = frameTime + (timeCorrectionSamps / sampleRate);
			frame.push_back( std::make_pair( time, Breakpoint( fHz, mag, 0., phase ) ) );
						
		}	//	end if itsa peak
	}
}

// ---------------------------------------------------------------------------
//	thinPeaks
// ---------------------------------------------------------------------------
//	After all of the peaks have been collected, 
//	sort the by magnitude and thin them according
//	to the specified partial density.
//

static bool sort_frame_greater_amplitude( const FRAME::value_type & lhs, 
										  const FRAME::value_type & rhs )
{ 
	return lhs.second.amplitude() > rhs.second.amplitude(); 
}

static bool has_zero_amp( const FRAME::value_type & v ) 
{ 
	return v.second.amplitude() == 0.; 
} 

/*	a Breakpoint can mask if it is within a specified frequency
	range and has non-zero amplitude
 */
struct can_mask
{
	bool operator()( const FRAME::value_type & v )  const
	{ 
		return	(v.second.amplitude() > 0.) &&
				(v.second.frequency() > _fmin) && 
				(v.second.frequency() < _fmax); 
	}
		
	//	constructor:
	can_mask( double x, double y ) : 
		_fmin( x ), _fmax( y ) 
		{ if (x>y) std::swap(x,y); }
		
	//	bounds:
	private:
		double _fmin, _fmax;
};

static void thinPeaks( FRAME & frame, double freqResolution, AssociateBandwidth & bwAssociation )
{
	//	can't do anything if there's fewer than two Breakpoints:
	if ( frame.size() < 2 )
	{
		return;
	}
	
	std::sort( frame.begin(), frame.end(), sort_frame_greater_amplitude );
	
	//	nothing can mask the loudest peak, so I can start with the
	//	second one, _and_ I can safely decrement the iterator when 
	//	I need to remove the element at its postion:
	FRAME::iterator it = frame.begin();
	for ( ++it; it != frame.end(); ++it ) 
	{
		Breakpoint & bp = it->second;
		
		//	search all louder peaks for one that is too near
		//	in frequency:
		double lower = bp.frequency() - freqResolution;
		double upper = bp.frequency() + freqResolution;
		if ( it != find_if( frame.begin(), it, can_mask(lower, upper) ) )
		{
			//	find_if returns the end of the range (it) if it finds nothing; 
			//	if it found something else, accumulate *it as noise, and
			//	remove *it from the frame:
			bwAssociation.accumulateNoise( bp.frequency(), bp.amplitude() );
			bp.setAmplitude(0.0);
		}
	}
	
	//	remove all Breakpoints whose amplitudes were set to zero:
	frame.erase( std::remove_if( frame.begin(), frame.end(), has_zero_amp ), frame.end() );
}

// ---------------------------------------------------------------------------
//	distance
// ---------------------------------------------------------------------------
//	Helper function, used lots in formPartials().
//	Returns the (positive) frequency distance between a Breakpoint 
//	and the last Breakpoint in a Partial.
//
static inline double distance( const Partial & partial, 
							   const Breakpoint & bp )
{
	return fabs( (--partial.end()).breakpoint().frequency() - bp.frequency() );
}

struct less_freq_difference
{
	double freq;
	
	less_freq_difference( double f ) : freq( f )  {}
	
	bool operator() (const Partial * lhs, const Partial * rhs) const
	{
		return 	fabs( (--(lhs->end())).breakpoint().frequency() - freq ) < 
		 		fabs( (--(rhs->end())).breakpoint().frequency() - freq );
				
		//	in the (extremely) unlikely event of a Partial with no 
		//	Breakpoints, decrementing the iterator would yield undefined 
		//	behavior. This version, at least, will throw an exception, but
		//	it is _much_ slower, and since we know that there are no empty
		//	Partials around, be don't have to be so careful:
		//
		//	return	fabs( lhs->frequencyAt( lhs->endTime() ) - freq ) < 
		//			fabs( rhs->frequencyAt( rhs->endTime() ) - freq );
	}
};

// ---------------------------------------------------------------------------
//	formPartials
// ---------------------------------------------------------------------------
//	Append the Breakpoints to existing Partials, if appropriate, or else 
//	give birth to new Partials.
//

static bool sort_frame_lesser_freq( const FRAME::value_type & lhs, 
									const FRAME::value_type & rhs )
{ 
	return lhs.second.frequency() < rhs.second.frequency(); 
}

static void formPartials( FRAME & frame, PartialPtrs & eligiblePartials,
						  double freqDrift, PartialList & partials )
{
	PartialPtrs newlyEligible;
	
	//	frequency-sort the frame:
	std::sort( frame.begin(), frame.end(), sort_frame_lesser_freq );
	
	//	loop over short-time spectral peaks:
	FRAME::iterator bpIter;
	for( bpIter = frame.begin(); bpIter != frame.end(); ++bpIter ) 
	{
		const Breakpoint & bp = bpIter->second;
		const double peakTime = bpIter->first;
		
		// 	find the Partial that is nearest in frequency to the Peak:
		Partial * nearest = NULL;
		if ( ! eligiblePartials.empty() )
		{
			nearest = * min_element( eligiblePartials.begin(), eligiblePartials.end(),
									 less_freq_difference( bp.frequency() ) );
		}
		
		//	(now have nearest Partial)
		//	Create a new Partial with this Breakpoint if:
		//	- no candidate (nearest) Partial was found (no eligible Partials) (1)
		//	- the nearest Partial is still too far away (2)
		//	- the next Breakpoint in the Frame exists and is
		//		closer to the nearest Partial (3)
		//	- the previous Breakpoint in the Frame exists and is
		//		closer to the nearest Partial (4)
		//
		//	Otherwise, add this Breakpoint to the nearest Partial.
		double thisdist = (nearest != NULL) ? (distance(*nearest, bp)) : (0.);
		FRAME::iterator next = bpIter+1;
		// FRAME::iterator prev = bpIter-1;
		// --prev;	//	hey, how do we know we can do _this_? What if bpIter is the first one?
					//	This might give undefined results, even though we make sure not to use it.
		if ( nearest == NULL /* (1) */ || 
			 thisdist > freqDrift /* (2) */ ||
			 ( next != frame.end() && thisdist > distance( *nearest, next->second ) ) /* (3) */ ||
			 ( bpIter != frame.begin() && thisdist > distance( *nearest, (bpIter-1)->second ) ) /* (4) */ ) 
		{
			//	create a new Partial, beginning with this Breakpoint at
			//	the specified time, and add it to the collection:
			Partial p;
			p.insert( peakTime, bp );
			partials.push_back( p );
			newlyEligible.push_back( & partials.back() );
		}
		else 
		{
			nearest->insert( peakTime, bp );
			newlyEligible.push_back( &(*nearest) );
		}
	}			 
	 	
	eligiblePartials = newlyEligible;
}

}	//	end of namespace Loris

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

#define No_BW_Association
#define Noise_Partials

using namespace std;

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	internal analysis helper declarations
// ---------------------------------------------------------------------------

struct AnalyzerState;
static void extractPeaks( AnalyzerState & state );
static void thinPeaks( AnalyzerState & state );
static void formPartials( AnalyzerState & state );
#if defined(Noise_Partials)
static void formNoisePartials( AnalyzerState & state );
#endif

// ---------------------------------------------------------------------------
//	AnalyzerState definition
// ---------------------------------------------------------------------------
//	Definition of a structure representing the state of a single analysis.
//	Encapsulates all the stuff that has to be passed around to the 
//	various helper functions, declared above.
//
typedef std::vector< std::pair< double, Breakpoint > > FRAME;

struct AnalyzerState
{
//	-- analysis state variables --
	//	reassigned short-time spectrum:
	std::auto_ptr< ReassignedSpectrum > spectrum;
    
    //	bandwidth association strategy:
#if !defined(No_BW_Association)
	std::auto_ptr< AssociateBandwidth > bwAssociation;
#endif
		
	//	collection of ptrs to Partials eligible for matching:
	PartialPtrs eligiblePartials;
    
    //	the current frame (collection of 
    //	reassigned spectral peaks):
    FRAME currentFrame;
    //	reference to the Analyzer instance, for parameter
    //	access:
    Analyzer & analyzer;

    //	the sample rate of the samples being analyzed:
    double sampleRate;
    
    //	the time corresponding to the currenf frame:
    double currentFrameTime;

#if defined(Noise_Partials)
	FRAME currentNoiseFrame;
	PartialPtrs eligibleNoisePartials;
#endif
   
	
//	-- construction --
//	(use compiler-constructed destructor)
	AnalyzerState( Analyzer & anal, double srate );
	
};	//	end of struct AnalyzerState


// ---------------------------------------------------------------------------
//	AnalyzerState constructor
// ---------------------------------------------------------------------------
//
AnalyzerState::AnalyzerState( Analyzer & anal, double srate ) :
    analyzer( anal ),
	sampleRate( srate ),
    currentFrameTime( 0. )
{	

	try 
    {
        //	analysis window parameters:
        //	use the amplitude floor as the window attenuation 
        double winshape = KaiserWindow::computeShape( - analyzer.ampFloor() );
        long winlen = KaiserWindow::computeLength( analyzer.windowWidth() / sampleRate, winshape );
        
        //	always use odd-length windows:
        if (! (winlen % 2)) 
            ++winlen;
        debugger << "Using Kaiser window of length " << winlen << endl;
        
        //	configure window:
        std::vector< double > window( winlen );
        KaiserWindow::create( window, winshape );
		
		//	configure spectrum:
		spectrum.reset( new ReassignedSpectrum( window ) );
		
		//	configure bw association strategy, which 
		//	needs to know about the window:
#if !defined(No_BW_Association)
		bwAssociation.reset( new AssociateBandwidth( analyzer.bwRegionWidth(), sampleRate ) );
#endif
	}
	catch ( Exception & ex ) 
    {
		ex.append( "couldn't initialize the Analyzer state." );
		throw;
	}
}

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
//	Analyzer destructor
// ---------------------------------------------------------------------------
//
Analyzer::~Analyzer( void )
{
}

#pragma mark -- configuration --

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
    
    //	peform bandwidth association bu default:
    _assocBW = true;
}

#pragma mark -- analysis --

// ---------------------------------------------------------------------------
//	analyze
// ---------------------------------------------------------------------------
//
void 
Analyzer::analyze( const double * bufBegin, const double * bufEnd, double srate )
{
	//	construct state object for this analysis:	
    AnalyzerState state( *this, srate );

//
//	need to check for bogus parameters somewhere!!!!
//

	//	compute hop time in samples:
	const long hop = long( hopTime() * srate );	//	truncate
	
	//	window length is first half length + second
	//	half length + the "center" sample (this works
	//	for even and odd length windows):
    const long winlen = state.spectrum->window().size();
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
			state.currentFrameTime = long(winMiddle - bufBegin) / srate;
			 
			//	compute reassigned spectrum:
			//  sampsBegin is the position of the first sample to be transformed,
			//	sampsEnd is the position after the last sample to be transformed.
			const double * sampsBegin = std::max( winMiddle - firstHalfWinLength, bufBegin );
			const double * sampsEnd = std::min( winMiddle + secondHalfWinLength + 1, bufEnd );
			state.spectrum->transform( sampsBegin, winMiddle, sampsEnd );
			
			//	extract peaks from the spectrum:
			state.currentFrame.clear();
#if defined(Noise_Partials)
			state.currentNoiseFrame.clear();
#endif
			extractPeaks( state );	
			thinPeaks( state );

#if !defined(No_BW_Association)
			//	perform bandwidth association:
			//
			//	accumulate retained Breakpoints as sinusoids, 
			//	thinned breakpoints are accumulated as noise:
			//	(see also thinPeaks() and extractPeaks())
			FRAME & frame = state.currentFrame;
			for ( FRAME::iterator it = frame.begin(); it != frame.end(); ++it )
			{
				state.bwAssociation->accumulateSinusoid( it->second.frequency(), it->second.amplitude() );
			}
			
			//	associate bandwidth with each Breakpoint here:
			for ( FRAME::iterator it = frame.begin(); it != frame.end(); ++it )
			{
				state.bwAssociation->associate( it->second );
			}
			
			//	reset after association, yuk:
			state.bwAssociation->reset();
#endif	//	 !defined(No_BW_Association)

			//	form Partials from the extracted Breakpoints:
			formPartials( state );
#if defined(Noise_Partials)
            formNoisePartials( state );
#endif

		}	//	end of loop over short-time frames
	}
	catch ( Exception & ex ) 
	{
		ex.append( "analysis failed." );
		throw;
	}
}

#pragma mark -- parameter access --

// ---------------------------------------------------------------------------
//	ampFloor
// ---------------------------------------------------------------------------
//	Return the amplitude floor (lowest detected spectral amplitude),  			
//	in (negative) dB, for this Analyzer. 				
//
double 
Analyzer::ampFloor( void ) const 
{ 
    return _floor; 
}

// ---------------------------------------------------------------------------
//	associateBandwith
// ---------------------------------------------------------------------------
//	Return true if this Analyzer is configured to peform bandwidth
//	association to distribute noise energy among extracted Partials, 
//	and false if noise energy will be collected in noise Partials,
//	labeled -1 in this Analyzer's PartialList.
//
bool 
Analyzer::associateBandwith( void ) const 
{ 
    return _assocBW; 
}

// ---------------------------------------------------------------------------
//	bwRegionWidth
// ---------------------------------------------------------------------------
//	Return the width (in Hz) of the Bandwidth Association regions
//	used by this Analyzer.
//
//		This parameter is deprecated and not generally useful. It will be
//		removed in a future release.
//
double 
Analyzer::bwRegionWidth( void ) const
{ 
    debugger << "Analyzer::bwRegionWidth() is a deprecated member, and will be removed in a future Loris release." << endl;
    return _bwRegionWidth; 
}

// ---------------------------------------------------------------------------
//	cropTime
// ---------------------------------------------------------------------------
//	Return the crop time (maximum temporal displacement of a time-
//	frequency data point from the time-domain center of the analysis
//	window, beyond which data points are considered "unreliable")
//	for this Analyzer.
//		
//	This parameter is deprecated and not generally useful. It will be
//	removed in a future release.
//
double 
Analyzer::cropTime( void ) const 
{ 
    debugger << "Analyzer::cropTime() is a deprecated member, and will be removed in a future Loris release." << endl;
    return _cropTime; 
}

// ---------------------------------------------------------------------------
//	freqDrift
// ---------------------------------------------------------------------------
//	Return the maximum allowable frequency difference 
//	consecutive Breakpoints in a Partial envelope for this Analyzer. 				
//
double 
Analyzer::freqDrift( void ) const { return _drift;}

// ---------------------------------------------------------------------------
//	freqFloor
// ---------------------------------------------------------------------------
//	Return the frequency floor (minimum instantaneous Partial  				
//	frequency), in Hz, for this Analyzer. 				
//
double 
Analyzer::freqFloor( void ) const { return _minFrequency; }

// ---------------------------------------------------------------------------
//	freqResolution
// ---------------------------------------------------------------------------
//	Return the frequency resolution (minimum instantaneous frequency  		
//	difference between Partials) for this Analyzer.
//
double 
Analyzer::freqResolution( void ) const { return _resolution; }

// ---------------------------------------------------------------------------
//	hopTime
// ---------------------------------------------------------------------------
//	Return the hop time (which corresponds approximately to the 
//	average density of Partial envelope Breakpoint data) for this 
//	Analyzer.
//
double 
Analyzer::hopTime( void ) const { return _hop; }

// ---------------------------------------------------------------------------
//	windowWidth
// ---------------------------------------------------------------------------
//	Return the frequency-domain main lobe width (measured between 
//	zero-crossings) of the analysis window used by this Analyzer. 				
//
double 
Analyzer::windowWidth( void ) const { return _windowWidth; }

#pragma mark -- parameter mutation --

// ---------------------------------------------------------------------------
//	setAmpFloor
// ---------------------------------------------------------------------------
//	Set the amplitude floor (lowest detected spectral amplitude), in  			
//	(negative) dB, for this Analyzer. 				
//
void 
Analyzer::setAmpFloor( double x ) { _floor = x; }

// ---------------------------------------------------------------------------
//	setAssociateBandwidth
// ---------------------------------------------------------------------------
//	If true, configure this Analyzer to peform bandwidth
//	association to distribute noise energy among extracted Partials.
//	If false, collect noise energy in noise Partials, assign them
//	the label -1, and retain them in this Analyzer's PartialList.
//
void 
Analyzer::setAssociateBandwidth( bool b )
{
    _assocBW = b;
}

// ---------------------------------------------------------------------------
//	setBwRegionWidth
// ---------------------------------------------------------------------------
//	Set the width (in Hz) of the Bandwidth Association regions
//	used by this Analyzer.
//		
//	This parameter is deprecated and not generally useful. It will be
//	removed in a future release.
//
void 
Analyzer::setBwRegionWidth( double x ) 
{ 
    debugger << "Analyzer::setBwRegionWidth() is a deprecated member, and will be removed in a future Loris release." << endl;
    _bwRegionWidth = x; 
}	

// ---------------------------------------------------------------------------
//	setCropTime
// ---------------------------------------------------------------------------
//	Set the crop time (maximum temporal displacement of a time-
//	frequency data point from the time-domain center of the analysis
//	window, beyond which data points are considered "unreliable")
//	for this Analyzer.
//		
//	This parameter is deprecated and not generally useful. It will be
//	removed in a future release.
//
void 
Analyzer::setCropTime( double x ) 
{ 
    debugger << "Analyzer::setCropTime() is a deprecated member, and will be removed in a future Loris release." << endl;
    _cropTime = x; 
}

// ---------------------------------------------------------------------------
//	setFreqDrift
// ---------------------------------------------------------------------------
//	Set the maximum allowable frequency difference between 					
//	consecutive Breakpoints in a Partial envelope for this Analyzer. 				
//
void 
Analyzer::setFreqDrift( double x ) { _drift = x; }

// ---------------------------------------------------------------------------
//	setFreqFloor
// ---------------------------------------------------------------------------
//	Set the amplitude floor (minimum instantaneous Partial  				
//	frequency), in Hz, for this Analyzer. 				
//
void 
Analyzer::setFreqFloor( double x ) { _minFrequency = x; }

// ---------------------------------------------------------------------------
//	setFreqResolution
// ---------------------------------------------------------------------------
//	Set the frequency resolution (minimum instantaneous frequency  		
//	difference between Partials) for this Analyzer. (Does not cause 	
//	other parameters to be recomputed.) 									
//
void 
Analyzer::setFreqResolution( double x ) { _resolution = x; }

// ---------------------------------------------------------------------------
//	setHopTime
// ---------------------------------------------------------------------------
//	Set the hop time (which corresponds approximately to the average
//	density of Partial envelope Breakpoint data) for this Analyzer.
//
void 
Analyzer::setHopTime( double x ) { _hop = x; }

// ---------------------------------------------------------------------------
//	setWindowWidth
// ---------------------------------------------------------------------------
//	Set the frequency-domain main lobe width (measured between 
//	zero-crossings) of the analysis window used by this Analyzer.
//
void 
Analyzer::setWindowWidth( double x ) { _windowWidth = x; }

#pragma mark -- PartialList access --

// ---------------------------------------------------------------------------
//	partials
// ---------------------------------------------------------------------------
//	Return a mutable reference to this Analyzer's list of 
//	analyzed Partials. 
//
PartialList & 
Analyzer::partials( void ) { return _partials; }

// ---------------------------------------------------------------------------
//	partials
// ---------------------------------------------------------------------------
//	Return an immutable (const) reference to this Analyzer's 
//	list of analyzed Partials. 
//
const PartialList & 
Analyzer::partials( void ) const { return _partials; }

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
static void extractPeaks( AnalyzerState & state )
{
    //	get state objects:
    FRAME & frame = state.currentFrame;
    ReassignedSpectrum & spectrum = *state.spectrum;
#if !defined(No_BW_Association)
    AssociateBandwidth & bwAssociation = *state.bwAssociation;
#endif

    //	get analysis parameters:
	const double ampFloor = state.analyzer.ampFloor();
	const double freqFloor = state.analyzer.freqFloor();
	const double cropTime = state.analyzer.cropTime();
    
    const double sampleRate = state.sampleRate;
    const double frameTime = state.currentFrameTime;
	
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
#if !defined(No_BW_Association)
				bwAssociation.accumulateNoise( fHz, mag );
#endif
#if defined(Noise_Partials)
                double phase = spectrum.reassignedPhase( j, fsample, timeCorrectionSamps );
                double time = frameTime + (timeCorrectionSamps / sampleRate);
                state.currentNoiseFrame.push_back( std::make_pair( time, Breakpoint( fHz, mag, 1., phase ) ) );
#endif
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

static void thinPeaks( AnalyzerState & state )
{
    //	get state objects:
    FRAME & frame = state.currentFrame;
#if !defined(No_BW_Association)
    AssociateBandwidth & bwAssociation = *state.bwAssociation;
#endif

    //	get analysis parameters:
	const double freqResolution = state.analyzer.freqResolution();

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
#if !defined(No_BW_Association)
			bwAssociation.accumulateNoise( bp.frequency(), bp.amplitude() );
#endif
#if defined(Noise_Partials)
            bp.setBandwidth(1.);
            state.currentNoiseFrame.push_back( std::make_pair( it->first, bp ) );
#endif
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

static void formPartials( AnalyzerState & state )
{
    //	get state objects:
    FRAME & frame = state.currentFrame;
    PartialPtrs & eligiblePartials = state.eligiblePartials;
    PartialList & partials = state.analyzer.partials();

    //	get analysis parameters:
	const double freqDrift = state.analyzer.freqDrift();

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

#if defined(Noise_Partials)
static void formNoisePartials( AnalyzerState & state )
{
    std::cout << "forming noise Partials" << endl;
    
    //	get state objects:
    FRAME & frame = state.currentNoiseFrame;
    PartialPtrs & eligiblePartials = state.eligibleNoisePartials;
    PartialList & partials = state.analyzer.partials();

    //	get analysis parameters:
	const double freqDrift = state.analyzer.freqDrift();

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
            p.setLabel(-1);
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
    
    std::cout << "there are now " << eligiblePartials.size() << " eligible noise Partials" << endl;
}

#endif

}	//	end of namespace Loris

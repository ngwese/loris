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
#include <Loris_prefix.h>
#include "Analyzer.h"
#include "AssociateBandwidth.h"
#include "DistributeEnergy.h"
#include "Exception.h"
#include "KaiserWindow.h"
#include "ReassignedSpectrum.h"
#include "BreakpointUtils.h"
#include "notifier.h"
#include <list>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <algorithm>

#if defined(Debug_Loris) 
	#include <cstdio>
#endif

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	AnalyzerState definition
// ---------------------------------------------------------------------------
//	Definition of a class representing the state of a single analysis.
//	Encapsulates the spectrum analyzer, the bandwidth association strategy, 
//	the energy distribution strategy, and the kludgey peak time cache.
//
class AnalyzerState
{
	//	state variables:
	std::auto_ptr< ReassignedSpectrum > _spectrum;
	std::auto_ptr< AssociateBandwidth > _bw;
	std::auto_ptr< DistributeEnergy > _energy;
	
	std::map< double, double > _peakTimeCache;	//	yuck

	double _sampleRate;
	
	std::vector< Partial * > _eligiblePartials;		//	yuck?
	
public:
//	construction:
//	(use compiler-constructed destructor)
	AnalyzerState( const Analyzer & anal, double srate );
	
//	accessors:
	ReassignedSpectrum & spectrum(void) { return *_spectrum; }
	AssociateBandwidth & bwAssociation(void) { return *_bw; }
	DistributeEnergy & eDistribution(void) { return *_energy; }
	
	std::map< double, double > & peakTimeCache(void) { return _peakTimeCache; }
	
	double sampleRate(void) { return _sampleRate; }

	std::vector< Partial * > & eligiblePartials(void) { return _eligiblePartials; }
	
};	//	end of class AnalyzerState


// ---------------------------------------------------------------------------
//	AnalyzerState constructor
// ---------------------------------------------------------------------------
//
AnalyzerState::AnalyzerState( const Analyzer & anal, double srate ) :
	_sampleRate( srate )
{	

	try {
		//	window parameters:
		const double Window_Attenuation = 95.;	//	always
		double winshape = KaiserWindow::computeShape( Window_Attenuation );
		long winlen = 
			KaiserWindow::computeLength( anal.windowWidth() / srate, Window_Attenuation );
		
		//	always use odd-length windows:
		if (! (winlen % 2)) {
			++winlen;
		}
		debugger << "Using Kaiser window of length " << winlen << endl;
		
		//	configure window:
		std::vector< double > v( winlen );
		KaiserWindow::create( v, winshape );
		
		//	configure spectrum:
		_spectrum.reset( new ReassignedSpectrum( v ) );
		
		//	configure bw association strategy, which 
		//	needs to know about the window:
		_bw.reset( new AssociateBandwidth( anal.bwRegionWidth(), srate ) );
		
		//	configure the energy distribution strategy:
		_energy.reset( new DistributeEnergy( 0.5 * anal.bwRegionWidth() ) );
	}
	catch ( Exception & ex ) {
		ex.append( "couldn't create a ReassignedSpectrum." );
		throw;
	}
}

// ---------------------------------------------------------------------------
//	Analyzer constructor
// ---------------------------------------------------------------------------
//	The core analysis parameter is the frequency resolution, the minimum
//	instantaneous frequency spacing between partials. Configure all other
//	parameters according to this parameter, subsequent parameter mutations
//	will be independent.
//
Analyzer::Analyzer( double resolutionHz )
{
	configure( resolutionHz );
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
//	Compute default values for analysis parameters from the single core
//	parameter, the frequency resolution.
//
//	There are basically three classes of analysis parameters:
//	- the resolution, and params that are usually related to (or
//	identical to) the resolution (minimum frequency and drift)
//	- the window width and params that are usually related to (or
//	identical to) the window width (hop and crop times)
//	- indepenendent parameters (bw region width and amp floor)
//
void
Analyzer::configure( double resolutionHz )
{
	_resolution = resolutionHz;
	
	//	floor defaults to -90 dB:
	_floor = -90.;
	
	//	window width should generally be approximately 
	//	equal to, and never more than twice the 
	//	frequency resolution:
	_windowWidth = _resolution;
	
	//	OLD COMMENT:
	//	the bare minimum component frequency that should be
	//	considered corresponds to two periods of a sine wave
	//	in the analysis window (this is pretty minimal) and
	//	this minimum is enforced in extractPeaks(). 
	//	The _minFrequency allows a higher frequency threshold
	//	to be set, for harmonic analyses, for example:
	//
	//	NEW COMMENT:
	//	I think that's ugly, the two periods thing can't
	//	be determined until the analysis is being run (because
	//	it is sample rate dependent) so the client can't tell
	//	whether its frequencyFloor setting is relevant or
	//	is being overriden by the two periods thing. Instead,
	//	use the frequency resolution by default (this makes 
	//	Lip happy, and is always safe?) and allow the client 
	//	to change it to anything at all.
	_minFrequency = _resolution;
	
	//	frequency drift in Hz is the maximum difference
	//	in frequency between consecutive Breakpoints in
	//	a Partial, by default, make it equal the frequency 
	//	resolution:
	_drift = _resolution;
	
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
//	construct a state object for this analysis:	
	AnalyzerState state( *this, srate );
	
//	loop over short-time analysis frames:
//
//	need to check for bogus parameters somewhere.
//
	const long hop = long( hopTime() * srate );	//	truncate
		
	try { 
		for ( const double * winMiddle = bufBegin; 
			  winMiddle < bufEnd;
			  winMiddle += hop ) 
		{
			//	compute the time of this analysis frame:
			const double frameTime = long(winMiddle - bufBegin) / srate;
			 
			//	compute reassigned spectrum:
			//	make this better!
			state.spectrum().transform( bufBegin, winMiddle, bufEnd );
			
			//	extract peaks from the spectrum:
			std::list< Breakpoint > f;
			extractPeaks( f, frameTime, state );	
			thinPeaks( f, state );

#if !defined(No_BW_Association)
			//	perform bandwidth association:
			//
			//	accumulate retained Breakpoints as sinusoids, 
			//	thinned breakpoints are accumulated as noise:
			//	(see also thinPeaks() and extractPeaks())
			std::list< Breakpoint >::iterator it;
			for ( it = f.begin(); it != f.end(); ++it )
			{
				state.bwAssociation().accumulateSinusoid( it->frequency(), it->amplitude() );
			}
			
			//	associate bandwidth with 
			//	each Breakpoint here:
			for ( it = f.begin(); it != f.end(); ++it )
			{
				state.bwAssociation().associate( *it );
			}
			
			//	reset after association, yuk:
			state.bwAssociation().reset();
#endif	//	 !defined(No_BW_Association)

			//	form Partials from the extracted Breakpoints:
			formPartials( f, frameTime, state );

		}	//	end of loop over short-time frames
		
		pruneBogusPartials( state );
	}
	catch ( Exception & ex ) 
	{
		ex.append( "analysis failed." );
		throw;
	}
}

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
void 
Analyzer::extractPeaks( std::list< Breakpoint > & frame, double frameTime, 
						AnalyzerState & state )
{
	const double threshold = std::pow( 10., 0.05 * ampFloor() );	//	absolute magnitude threshold
	const double sampsToHz = state.sampleRate() / state.spectrum().size();
	
	//	cache corrected times for the extracted breakpoints, so 
	//	that they don't hafta be computed over and over again:
	state.peakTimeCache().clear();
		
#if defined(Debug_Loris)	
	bool spit = false;
	std::FILE * spitfile;
	if ( spit )
	{
		spitfile = std::fopen( "peaks", "w" );
	}
#endif

	//	look for magnitude peaks in the spectrum:
	for ( int j = 1; j < (state.spectrum().size() / 2) - 1; ++j ) 
	{
		if ( abs(state.spectrum()[j]) > abs(state.spectrum()[j-1]) && 
			 abs(state.spectrum()[j]) > abs(state.spectrum()[j+1])) 
		{
#if defined(Debug_Loris)	
			if (spit)
				fprintf( spitfile, "found peak at sample %ld\n", j );
#endif
				
			//	compute the fractional frequency sample
			//	and the frequency:
			double fsample = state.spectrum().reassignedFrequency( j );	//	fractional sample
			double fHz = fsample * sampsToHz;
			
			//	ignore peaks below our frequency and amplitude floors:
			if ( fHz < freqFloor() )
				continue;
				
			//	find the time correction (in samples!) for this peak:
			double timeCorrectionSamps = state.spectrum().reassignedTime( j );
			
			//	ignore peaks with large time corrections:
			//	if I do this, then off-center peaks are not part of
			//	association, not part of thinning, and the cropping/breaking
			//	check in formPartials() is unnecessary (large time corrections
			//	will already have been removed).
			if ( std::abs(timeCorrectionSamps) > cropTime() * state.sampleRate() )
				continue;
				
			//	breakpoints are extracted and thinned and the survivors are
			//	accumulated as sinusoids in the association process.
			double mag = state.spectrum().reassignedMagnitude( fsample, j );
			
			//	the second part is a sinusoidality measure (?)
			if ( mag < threshold ) // || std::abs( fsample - j ) > 1. ) 
			{
				state.bwAssociation().accumulateNoise( fHz, mag );
				continue;
			}
			else
			{
				//state.bwAssociation().accumulateSinusoid( fHz, mag );			
			}

#if 0	//	this seems wrong defined(Like_Lemur)
			double correctionAbove = state.spectrum().reassignedTime( std::ceil( fsample ) );
			double correctionBelow = state.spectrum().reassignedTime( std::floor( fsample ) );

			//	compute weighted average time correction, assign:
			double alpha =  fsample - std::floor( fsample );
			timeCorrectionSamps = ( alpha * correctionAbove ) + 
								(( 1. - alpha ) * correctionBelow );
#endif
											
			//	retain a spectral peak corresponding to this sample:
			//	(reassignedPhase() must be called with time correction 
			//	in samples!)
			double phase = state.spectrum().reassignedPhase( j, fsample, timeCorrectionSamps );
			frame.push_back( Breakpoint( fHz, mag, 0., phase ) );
			
			//	cache the peak time, won't have j available when
			//	ready to insert it into a Partial (convert time 
			//	correction to seconds):
			double time = frameTime + (timeCorrectionSamps / state.sampleRate());
			state.peakTimeCache()[ fHz ] = time;
			
#if defined(Debug_Loris)	
			if (spit)
				fprintf( spitfile, "kept breakpoint with freq %lf, amp %lf, time %lf\n", 
							fHz, mag, time );
#endif
			
		}	//	end if itsa peak
	}
	
#if defined(Debug_Loris)	
	if ( spit )
		fclose( spitfile );
#endif
}

// ---------------------------------------------------------------------------
//	thinPeaks
// ---------------------------------------------------------------------------
//	After all of the peaks have been collected, 
//	sort the by magnitude and thin them according
//	to the specified partial density.
//
void 
Analyzer::thinPeaks( std::list< Breakpoint > & frame, AnalyzerState & state )
{
	frame.sort( BreakpointUtils::greater_amplitude() );
	
	//	nothing can mask the loudest peak, so I can start with the
	//	second one, _and_ I can safely decrement the iterator when 
	//	I need to remove the element at its postion:
	std::list< Breakpoint >::iterator it = frame.begin();
	for ( ++it; it != frame.end(); ++it ) 
	{
		//	search all louder peaks for one that is too near
		//	in frequency:
		double lower = it->frequency() - freqResolution();
		double upper = it->frequency() + freqResolution();
		if ( it != find_if( frame.begin(), it, 
							BreakpointUtils::frequency_between(lower, upper) ) ) 
		{
			//	find_if returns the end of the range (it) if it finds nothing; 
			//	if it found something else, accumulate *it as noise, and
			//	remove *it from the frame:
			state.bwAssociation().accumulateNoise( it->frequency(), it->amplitude() );
			frame.erase( it-- );
		}
	}
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
	//	need a more efficient way to check for invalid Partials!
	Assert(partial.begin() != partial.end());
	return std::abs( (--partial.end()).breakpoint().frequency() - bp.frequency() );
}

// ---------------------------------------------------------------------------
//	formPartials
// ---------------------------------------------------------------------------
//	Append the Breakpoints to existing Partials, if appropriate, or else 
//	give birth to new Partials.
//
// #define sucks
#ifdef sucks
void 
Analyzer::formPartials( std::list< Breakpoint > & frame, double /* frameTime */ , 
						AnalyzerState & state )
{
	std::vector< Partial * > newlyEligible;
	
	//	frequency-sort the frame:
	frame.sort( BreakpointUtils::less_frequency() );
	
	//	loop over short-time peaks:
	std::list< Breakpoint >::iterator bpIter;
	for( bpIter = frame.begin(); bpIter != frame.end(); ++bpIter ) 
	{
		const Breakpoint & peak = *bpIter;
		const double peakTime = state.peakTimeCache()[ peak.frequency() ];

		//	Loop over all eligible Partials 
		//	find the candidate Partials  nearest in frequency 
		//	to peak.
		//
		//	Since we build the eligiblePartials collection in the
		//	order that we process Breakpoints in the frame, 
		//	increasing-frequency order, the eligiblePartials will also
		//	be sorted in order of increasing frequency of the
		//	last Breakpoint.)
		//
		//	Find the lowest end-frequency Partial that is
		//	higher in frequency than peak, if such a Partial
		//	exists in eligiblePartials:
		//
		//	(this initializer could probably be outside this loop)
		std::vector< Partial * >::iterator candidate = state.eligiblePartials().begin();
		while ( candidate != state.eligiblePartials().end() &&
				(*candidate)->frequencyAt(peakTime) < peak.frequency() )
		{
			++candidate;			
		}
		
		//	now candidate points to the end or to 
		//	the lowest-frequency Partial above this
		//	peak's frequency, remember this Partial,
		//	and its predecessor in eligiblePartials, the highest
		//	end-frequency Partial lower in frequency 
		//	than peak, if such a Partial exists in 
		//	eligiblePartials:
		Partial * firstChoice = NULL, * secondChoice = NULL;
		if ( candidate != state.eligiblePartials().end() )
		{
			secondChoice = *candidate;
		}
		
		if ( candidate != state.eligiblePartials().begin() )
		{
			--candidate;
			firstChoice = *candidate;
		}
		
		//	it may be that the candidate below ("firstChoice")
		//	has already been matched to a peak in this frame,
		//	in which case firstChoice is the last element of
		//	newlyEligible and we should set firstChoice to secondChoice
		//	and secondChoice to NULL:
		if ( newlyEligible.size() > 0 && newlyEligible.back() == firstChoice )
		{
			firstChoice = secondChoice;
			secondChoice = NULL;
		}
		
		//	if secondChoice is nearer than firstChoice, and
		//	both exist, swap:
		if ( firstChoice != NULL && secondChoice != NULL )
		{
			if ( distance( *secondChoice, peak ) < distance( *firstChoice, peak ) )
			{
				std::swap( firstChoice, secondChoice );
			}
		}
		
		//	Now have first and seconc choice candidate Partials.
		//	We will create a new Partial with this Breakpoint if:
		//	- no candidate (firstChoice) Partial was found (1)
		//	- the firstChoice Partial is still too far away (2)
		//	- the next Breakpoint in the Frame exists and is
		//		closer to the firstChoice Partial (3)
		//	- the previous Breakpoint in the Frame exists and is
		//		closer to the firstChoice Partial (4)
		//
		//	if there is no firstChoice Partial, or the firstChoice is
		//	too distant, spawn a new Partial:
		if ( firstChoice == NULL /* (1) */ ||
			 distance(*firstChoice, peak) > freqDrift() )
		{
			spawnPartial( peakTime, peak );
			newlyEligible.push_back( & partials().back() );
			continue;
		}
		
		//	otherwise, try to match with firstChoice:
		Breakpoint * peakAbove = NULL;
		std::list< Breakpoint >::iterator next = bpIter;
		++next;
		if ( next != frame.end() )
		{
			peakAbove = &(*next);
		}
		
		Breakpoint * peakBelow = NULL;
		if ( bpIter != frame.begin() )
		{
			std::list< Breakpoint >::iterator prev = bpIter;
			--prev;
			peakBelow = &(*prev);
		}
		
		double thisdist = distance(*firstChoice, peak);
		
		if ( thisdist < distance( *firstChoice, *peakAbove ) /* (3) */ &&
			 thisdist <= distance( *firstChoice, *peakBelow ) /* (4) */ ) 
		{
			firstChoice->insert( peakTime, peak );
			newlyEligible.push_back( &(*firstChoice) );
			continue;
		}
	
		//	if firstChoice match fails because an adjacent Breakpoint
		//	in the frame is closer, then try to match with the
		//	secondChoice:
		//
		//	if there is no secondChoice Partial, or the secondChoice is
		//	too distant, spawn a new Partial:
		if ( secondChoice == NULL /* (1) */ ||
			 distance(*secondChoice, peak) > freqDrift() )
		{
			spawnPartial( peakTime, peak );
			newlyEligible.push_back( & partials().back() );
			continue;
		}
		
		//	determine which other Breakpoint might be a better
		//	match for secondChoice, if such a Breakpoint exists
		//	in this frame:
		double endFreq = secondChoice->frequencyAt( peakTime );
		Breakpoint * other;
		if ( endFreq > peak.frequency() )
		{
			other = peakAbove;
		}
		else
		{
			other = peakBelow;
		}
		
		//	if there is no other potential match, or this peak is 
		//	nearer than the other potential match, then match:
		if ( other == NULL || 
			 distance( *secondChoice, peak ) < distance( *secondChoice, *other ) )
		{
			secondChoice->insert( peakTime, peak );
			newlyEligible.push_back( &(*secondChoice) );
		}
		else //	oh well, spawn another
		{
			spawnPartial( peakTime, peak );
			newlyEligible.push_back( & partials().back() );
		}
		
		//	done.		
	}			 
	 	
	 state.eligiblePartials() = newlyEligible;
}

#else //	not def sucks
void 
Analyzer::formPartials( std::list< Breakpoint > & frame, double /* frameTime */, 
						AnalyzerState & state )
{
	std::vector< Partial * > newlyEligible;
	
	//	frequency-sort the frame:
	frame.sort( BreakpointUtils::less_frequency() );
	
	//	loop over short-time peaks:
	std::list< Breakpoint >::iterator bpIter;
	for( bpIter = frame.begin(); bpIter != frame.end(); ++bpIter ) 
	{
		const Breakpoint & peak = *bpIter;
		const double peakTime = state.peakTimeCache()[ peak.frequency() ];
		
		//	loop over all eligible Partials, find the Partial
		//	that is nearest in frequency to the Peak:
		Partial * nearest = NULL;
		std::vector< Partial * >::iterator candidate;
		for ( candidate = state.eligiblePartials().begin();
			  candidate != state.eligiblePartials().end();
			  ++candidate )
		{
			//	remember this Partial if it is nearer in frequency 
			//	to the Breakpoint than every other Partial:
			if ( ! nearest || distance( **candidate, peak ) < distance( *nearest, peak ) ) 
			{
				nearest = *candidate;
			}
		}			
		
		//	(now have nearest Partial)
		//	Create a new Partial with this Breakpoint if:
		//	- no candidate (nearest) Partial was found (1)
		//	- the nearest Partial is still too far away (2)
		//	- the next Breakpoint in the Frame exists and is
		//		closer to the nearest Partial (3)
		//	- the previous Breakpoint in the Frame exists and is
		//		closer to the nearest Partial (4)
		//
		//	Otherwise, add this Breakpoint to the nearest Partial.
		double thisdist = (nearest != NULL) ? (distance(*nearest, peak)) : (0.);
		std::list< Breakpoint >::iterator next = bpIter;
		++next;
		std::list< Breakpoint >::iterator prev = bpIter;
		--prev;
		if ( nearest == NULL /* (1) */ || 
			 thisdist > freqDrift() /* (2) */ ||
			 ( next != frame.end() && thisdist > distance( *nearest, *next ) ) /* (3) */ ||
			 ( bpIter != frame.begin() && thisdist > distance( *nearest, *prev ) ) /* (4) */ ) 
		{
			spawnPartial( peakTime, peak );
			newlyEligible.push_back( & partials().back() );
		}
		else 
		{
				nearest->insert( peakTime, peak );
				newlyEligible.push_back( &(*nearest) );
		}
	}			 
	 	
	 state.eligiblePartials() = newlyEligible;
}

#endif	//	def sucks

// ---------------------------------------------------------------------------
//	spawnPartial
// ---------------------------------------------------------------------------
//	Create a new Partial, beginning with the specified Breakpoint at
//	the specified time, and add it to the collection.
//
void 
Analyzer::spawnPartial( double time, const Breakpoint & bp )
{
	Partial p;
	p.insert( time, bp );
	partials().push_back( p );
}

// ---------------------------------------------------------------------------
//	pruneBogusPartials
// ---------------------------------------------------------------------------
//	Analysis may yield many Partials of zero duration, no sense
//	in retaining those.
//
//	Maybe there should be some other notion of a too-short Partial.
//	Should it be a parameter?
//
//	Actually, maybe the energy in these Partials is important for very 
//	impulsive sounds?
//
void
Analyzer::pruneBogusPartials( AnalyzerState & state )
{
#define DONT_PRUNE
#if defined(DONT_PRUNE)
	debugger << "Analyzer not pruning zero-duration Partials." << endl;
	return;
#endif

	//	collect the very short Partials:
	std::list<Partial> veryshortones;
	for ( std::list< Partial >::iterator it = partials().begin(); 
		  it != partials().end(); 
		  /* ++it */ ) 
	{
		//	need to be careful with the iterator update, 
		//	because erasure or splice will invalidate it:
		std::list< Partial >::iterator next = it;
		++next;
		if ( it->duration() == 0. ) {
			veryshortones.splice( veryshortones.end(), partials(), it );
		}
		it = next;
	}
	
	//	distribute their energy:
	for ( std::list<Partial>::iterator it = veryshortones.begin();
		  it != veryshortones.end();
		  ++it ) 
	{
#if !defined(No_BW_Association)
		state.eDistribution().distribute( *it, partials().begin(), partials().end() );
#endif
	}

	debugger << "Analyzer pruned " << veryshortones.size() << " zero-duration Partials." << endl;
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

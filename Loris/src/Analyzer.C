// ===========================================================================
//	Analyzer.C
//	
//	Implementation of class Loris::Analyzer.
//
//	-kel 5 Dec 99
//
// ===========================================================================
#include "Analyzer.h"
#include "AssociateBandwidth.h"
#include "DistributeEnergy.h"
#include "Exception.h"
#include "KaiserWindow.h"
#include "ReassignedSpectrum.h"
#include "notifier.h"
#include <list>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>

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
	
public:
//	construction:
	AnalyzerState( const Analyzer & anal, double srate );
	//~AnalyzerState();		use compiler-constructed destructor
	
//	accessors:
	ReassignedSpectrum & spectrum(void) { return *_spectrum; }
	AssociateBandwidth & bwAssociation(void) { return *_bw; }
	DistributeEnergy & eDistribution(void) { return *_energy; }
	
	std::map< double, double > & peakTimeCache(void) { return _peakTimeCache; }
	
	double sampleRate(void) { return _sampleRate; }

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
		_bw.reset( new AssociateBandwidth( *_spectrum, srate, anal.bwRegionWidth(), anal.cropTime() ) );
		
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
//	configure
// ---------------------------------------------------------------------------
//	Compute default values for analysis parameters from the single core
//	parameter, the frequency resolution.
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
	//	the hop time:
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
Analyzer::analyze( const std::vector< double > & buf, double srate, double offset /* = 0. */ )
{
//	construct a state object for this analysis:	
	AnalyzerState state( *this, srate );
	
//	Somewhere up here, we should partition the partials
//	collection, make sure we don't try to match with partials
//	from an earlier analysis. Then, we should sort the whole 
//	collection at the end. This is easy, just make the current
//	end of the partials collection part of the Analyzer state, 
//	and use it in formPartials().

//	loop over short-time analysis frames:
//	Arbitrarily, the first window is centered hop
//	samples before the beginning of the buffer (though 
//	there's no way we would ever keep something from that 
//	frame, it would have too large a time correction), and 
//	the last window is the first one centered more than
//	hop samples past the end of the buffer.
//	
//	UPDATE THIS COMMENT
//
//	Also, need to check for bogus parameters somewhere.
//
	const long hop = long( hopTime() * srate );	//	truncate
	try { 
		for ( long winMiddleIdx = 0; 
			  winMiddleIdx < buf.size();
			  winMiddleIdx += hop ) {
			//	compute the time of this analysis frame:
			const double frameTime = ( winMiddleIdx / srate ) + offset;
			 
			//	compute reassigned spectrum:
			//	make this better!
			state.spectrum().transform( buf, winMiddleIdx );
			
			//	extract peaks from the spectrum:
			std::list< Breakpoint > f;
			extractPeaks( f, frameTime, state );	
			thinPeaks( f );

			//	perform bandwidth association:
			state.bwAssociation().associate( f.begin(), f.end() );
			
			//	form Partials from the extracted Breakpoints:
			formPartials( f, frameTime, state );

		}	//	end of loop over short-time frames
		
		pruneBogusPartials( state );
	}
	catch ( Exception & ex ) {
		ex.append( "analysis failed." );
		throw;
	}
}

// ---------------------------------------------------------------------------
//	Breakpoint comparitors
// ---------------------------------------------------------------------------
//	For using STL algorithms on collections of Breakpoints.
//	No real reason for these to be templates, actually.
//
template<class T>
struct less_frequency
{
	bool operator()( const T & lhs, const T & rhs ) const
		{ return lhs.frequency() < rhs.frequency(); }
};

template<class T>
struct greater_amplitude
{
	bool operator()( const T & lhs, const T & rhs ) const
		{ return lhs.amplitude() > rhs.amplitude(); }
};	

template<class T>
struct frequency_between
{
	frequency_between( double x, double y ) : 
		_fmin( x ), _fmax( y ) 
		{ if (x>y) std::swap(x,y); }
	bool operator()( const T & t )  const
		{ 
			return (t.frequency() > _fmin) && 
				   (t.frequency() < _fmax); 
		}
	private:
		double _fmin, _fmax;
};


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
	const long maxCorrection = long( cropTime() * state.sampleRate() );
	
	//	FORMERLY:
	//	the bare minimum component frequency that should be
	//	considered corresponds to two periods of a sine wave
	//	in the analysis window (this is pretty minimal):
	//
	//	no longer, just use freqFloor.
	const double fmin = freqFloor();
		//std::max( freqFloor(), 2. / (state.spectrum().window().size() / state.sampleRate()) );
	
	//	cache corrected times for the extracted breakpoints, so 
	//	that they don't hafta be computed over and over again:
	state.peakTimeCache().clear();	
	
	//	look for magnitude peaks in the spectrum:
	for ( int j = 1; j < (state.spectrum().size() / 2) - 1; ++j ) {
		if ( abs(state.spectrum()[j]) > abs(state.spectrum()[j-1]) && 
			 abs(state.spectrum()[j]) > abs(state.spectrum()[j+1])) {
			 //	compute the fractional frequency sample
			//	and the frequency:
			double fsample = state.spectrum().reassignedFrequency( j );	//	fractional sample
			double fHz = fsample * sampsToHz;
			
			//	if the frequency is too low (not enough periods
			//	in the analysis window), reject it:
			if ( fHz < fmin ) 
				continue;
				
			//	itsa magnitude peak, does it clear the amplitude floor?
			double mag = state.spectrum().reassignedMagnitude( fsample, j );
			if ( mag < threshold )
				continue;
			
			//	if the time correction for this peak is large,
			//	reject it:
			double timeCorrection = state.spectrum().reassignedTime( fsample );
			if ( std::abs(timeCorrection) > maxCorrection )
				continue;
				
			//	retain a spectral peak corresponding to this sample:
			double phase = state.spectrum().reassignedPhase( fsample, timeCorrection );
			frame.push_back( Breakpoint( fHz, mag, 0., phase ) );
			
			//	cache the peak time, rather than recomputing it when
			//	ready to insert it into a Partial:
			double time = frameTime + ( timeCorrection / state.sampleRate() );
			state.peakTimeCache()[ fHz ] = time;
			
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
void 
Analyzer::thinPeaks( std::list< Breakpoint > & frame )
{
	frame.sort( greater_amplitude<Breakpoint>() );
	
	//	nothing can mask the loudest peak, so I can start with the
	//	second one, _and_ I can safely decrement the iterator when 
	//	I need to remove the element at its postion:
	std::list< Breakpoint >::iterator it = frame.begin();
	for ( ++it; it != frame.end(); ++it ) {
		//	search all louder peaks for one that is too near
		//	in frequency:
		double lower = it->frequency() - freqResolution();
		double upper = it->frequency() + freqResolution();
		if ( it != find_if( frame.begin(), it, frequency_between< Breakpoint >( lower, upper ) ) ) {
			//	find_if returns the end of the range (it) if it finds nothing; 
			//	remove *it from the frame
			frame.erase( it-- );
		}
	}
}

// ---------------------------------------------------------------------------
//	distance
// ---------------------------------------------------------------------------
//	Helper function, used lots in formPartials().
//	Returns the (positive) frequency distance between a Breakpoint 
//	at a particular time and a Partial.
//
static inline double distance( const Partial & partial, 
							   const Breakpoint & bp, 
							   double time )
{
	return std::abs( partial.frequencyAt( time ) - bp.frequency() );
}

// ---------------------------------------------------------------------------
//	formPartials
// ---------------------------------------------------------------------------
//	Append the Breakpoints to existing Partials, if appropriate, or else 
//	give birth to new Partials.
//
void 
Analyzer::formPartials( std::list< Breakpoint > & frame, double frameTime, AnalyzerState & state )
{
	//	frequency-sort the frame:
	frame.sort( less_frequency<Breakpoint>() );
	
	//	loop over short-time peaks:
	std::list< Breakpoint >::iterator bpIter;
	for( bpIter = frame.begin(); bpIter != frame.end(); ++bpIter ) {
		const Breakpoint & peak = *bpIter;
		const double peakTime = state.peakTimeCache()[ peak.frequency() ];
		
		//	compute the time after which a Partial
		//	must have Breakpoints in order to be 
		//	eligible to receive this Breakpoint:
		//	The earliest Breakpoint we could have kept 
		//	from the previous frame:
		double tooEarly = frameTime - (2. * hopTime());
		
		//	compute the time before which a Partial
		//	must end in order to be eligible to receive
		//	this Breakpoint, in case the Analyzer is used
		//	for more than a single input buffer.
		double tooLate = std::min( frameTime, peakTime );
		
		//	loop over all Partials, find the eligible Partial
		//	that is nearest in frequency to the Peak:
		PartialList::iterator nearest = partials().end();
		for ( PartialList::iterator pIter = partials().begin(); pIter != partials().end(); ++pIter ) {
			//	check end time for eligibility:
			if ( pIter->endTime() < tooEarly || pIter->endTime() >= tooLate ) {
				continue;	//	loop over all Partials
			}
			
			//	remember this Partial if it is nearer in frequency 
			//	to the Breakpoint than every other Partial:
			if ( nearest == partials().end() || 
				 distance( *pIter, peak, peakTime ) < distance( *nearest, peak, peakTime ) ) {
				//	this Partial is nearest (so far):
				nearest = pIter;
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
		double thisdist = (nearest != partials().end()) ? 
							(distance(*nearest, peak, peakTime)) : 
							(0.);
		std::list< Breakpoint >::iterator next = bpIter;
		++next;
		std::list< Breakpoint >::iterator prev = bpIter;
		--prev;
		if ( nearest == partials().end() /* (1) */ || 
			 thisdist > 0.5 * freqResolution() /* (2) */ ||
			 ( next != frame.end() && 
			 	thisdist > distance( *nearest, *(next), state.peakTimeCache()[ next->frequency() ] ) ) /* (3) */ ||
			 ( bpIter != frame.begin() && 
			 	thisdist > distance( *nearest, *(prev), state.peakTimeCache()[ prev->frequency() ] ) ) /* (4) */ ) {
			 	
			 /*debugger << "spawning a partial at frequency " << peak.frequency() <<
			 			 " amplitude " << peak.amplitude() <<
			 			 " and time " << peakTime << endl;
			 */
			 spawnPartial( peakTime, peak );
		}
		else {
			/*debugger << "matching a partial at frequency " << peak.frequency() <<
			 			" amplitude " << peak.amplitude() <<
			 			" and time " << peakTime << endl;
			*/
			nearest->insert( peakTime, peak );
		}
	}			 
}

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
void
Analyzer::pruneBogusPartials( AnalyzerState & state )
{
	//	collect the very short Partials:
	std::list<Partial> veryshortones;
	for ( PartialList::iterator it = partials().begin(); 
		  it != partials().end(); 
		  /* ++it */ ) {
		//	need to be careful with the iterator update, 
		//	because erasure or splice will invalidate it:
		PartialList::iterator next = it;
		++next;
		if ( it->duration() == 0. ) {
			veryshortones.splice( veryshortones.end(), partials(), it );
		}
		it = next;
	}
	
	//	distribute their energy:
	for ( std::list<Partial>::iterator it = veryshortones.begin();
		  it != veryshortones.end();
		  ++it ) {
		state.eDistribution().distribute( *it, partials().begin(), partials().end() );
	}

	debugger << "Analyzer pruned " << veryshortones.size() << " zero-duration Partials." << endl;
}



#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

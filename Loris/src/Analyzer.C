// ===========================================================================
//	Analyzer.C
//	
//	Implementation of class Loris::Analyzer.
//
//	-kel 5 Dec 99
//
// ===========================================================================
#include "Analyzer.h"
#include "Exception.h"
#include "KaiserWindow.h"
#include "Notifier.h"
#include "AssociateBandwidth.h"
#include "DistributeEnergy.h"

using namespace std;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	Analyzer constructor
// ---------------------------------------------------------------------------
//	Choose reasonable default values. Duh.
//
Analyzer::Analyzer( void ) :
	_freqResolution( 100 ),
	_noiseFloor( -90. ),
	_windowWidth( 200 ),
	_windowAtten( 90 ),
	_srate( 1. ),
	_minfreq( 0. ),
	_hop( 1 )
{
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
Analyzer::analyze( const vector< double > & buf, double srate )
{
//	construct _spectrum if necessary:
//	(changes in the analysis parameters may
//	require reconstruction of the spectrogram, 
//	so construct it at the last minute)
	if (! _spectrum.get() ) {
		createSpectrum( srate );
	}
	
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
	const long latestIdx = buf.size() + 2L * hopSize();
	try { 
		for ( _winMiddleIdx = - hopSize(); 
			  _winMiddleIdx < latestIdx; 
			  _winMiddleIdx += hopSize() ) {
			 
			//debugger << "analyzing frame centered at " << _winMiddleIdx << endl; 
			 
			//	compute reassigned spectrum:
			_spectrum->transform( buf, _winMiddleIdx );
			
			//	extract peaks from the spectrum:
			Frame f;
			extractPeaks( f );	
			thinPeaks( f );

			//	perform bandwidth association:
			_bw->associate( f.begin(), f.end() );

			/*	I wonder if I can possibly get away with
				requiring the transform window center to 
				be a valid iterator on buf...
				
			*/
			if ( _winMiddleIdx < 0 || _winMiddleIdx >= buf.size() ) {
				debugger << "kept " << f.size() << " peaks at index " << _winMiddleIdx << endl;
			}
			
			//	form Partials from the extracted Breakpoints:
			formPartials( f );

		}	//	end of loop over short-time frames
		
		pruneBogusPartials();
	}
	catch ( Exception & ex ) {
		ex.append( "analysis failed." );
		throw;
	}
	
	//debugger << "analysis complete" << endl;
}



// ---------------------------------------------------------------------------
//	createSpectrum
// ---------------------------------------------------------------------------
//	Compute the analysis window, and then create a reassigned spectrum 
//	analyzer.
//
void
Analyzer::createSpectrum( double srate )
{	
	_srate = srate;
	
	//	window parameters:
	long winlen = KaiserWindow::computeLength( _windowWidth / srate, _windowAtten );
	if (! (winlen % 2)) {
		++winlen;
	}
	double winshape = KaiserWindow::computeShape( _windowAtten );
	
	//	compute the hop size:
	//	Smith and Serra (1990) cite Allen (1977) saying: a good choice of hop 
	//	is the window length divided by the main lobe width in frequency samples.
	//	Don't include zero padding in the computation of width (i.e. use window
	//	length instead of transform length).
	const double mlw_samp = _windowWidth * (winlen / srate);
	_hop = round( winlen / mlw_samp );
	
	//	lower frequency bound for Breakpoint extraction,
	//	require at least two (no, three!) periods in the window:
	//_minfreq = max( 2. / ( winlen / srate ), _freqResolution );
	_minfreq = 2. / ( winlen / srate );
	
	try {
		//	configure window:
		vector< double > v( winlen );
		KaiserWindow::create( v, winshape );
		
		//	configure spectrum:
		_spectrum.reset( new ReassignedSpectrum( v ) );
		
		//	configure bw association strategy, which 
		//	needs to know about the window:
		_bw.reset( new AssociateBandwidth( *_spectrum, srate ) );
	}
	catch ( Exception & ex ) {
		ex.append( "couldn't create a ReassignedSpectrum." );
		throw;
	}
	
	debugger << "created reassigned spectrum analyzer: window length " <<
			winlen << ", hop size " << _hop << ", minimum frequency " <<
			_minfreq << endl;
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
template<class T>
struct less_frequency
{
	boolean operator()( const T & lhs, const T & rhs ) const
		{ return lhs.frequency() < rhs.frequency(); }
};

template<class T>
struct greater_amplitude
{
	boolean operator()( const T & lhs, const T & rhs ) const
		{ return lhs.amplitude() > rhs.amplitude(); }
};	

template<class T>
struct frequency_between
{
	frequency_between( double x, double y ) : 
		_fmin( x ), _fmax( y ) 
		{ if (x>y) swap(x,y); }
	boolean operator()( const T & t )  const
		{ 
			return (t.frequency() > _fmin) && 
				   (t.frequency() < _fmax); 
		}
	private:
		double _fmin, _fmax;
};

	
void 
Analyzer::extractPeaks( Frame & frame )
{
	const double threshold = pow( 10., 0.05 * noiseFloor() );	//	absolute magnitude threshold
	const double sampsToHz = sampleRate() / _spectrum->size();
	
	#ifdef __THUMP__
	//	HEY!
	double magSum = _spectrum->magnitude( 0 );
	double eSum = _spectrum->magnitude( 0 ) * _spectrum->magnitude( 0 );
	
	vector< double > hey, ho, letsogo;
	hey.push_back( _spectrum->magnitude( 0 ) );
	ho.push_back( _spectrum->reassignedFrequency( 0 ) );
	letsogo.push_back( _spectrum->reassignedTime( 0 ) );
	
	for ( int k = 1; _spectrum->magnitude( k ) < _spectrum->magnitude( k-1 ); ++k ) {
		magSum += 2. * _spectrum->magnitude( k );
		eSum += 2.* (_spectrum->magnitude( k ) * _spectrum->magnitude( k ));
		hey.push_back( _spectrum->magnitude( k ) );
		ho.push_back( _spectrum->reassignedFrequency( k ) );
		letsogo.push_back( _spectrum->reassignedTime( k ) );
	}
	
	//	keep a DC peak:
	double DCmag = _spectrum->magnitude( 0 );
	if ( DCmag > _spectrum->magnitude( 1 ) &&
		 DCmag > threshold ) {
		double timeCorrection = _spectrum->reassignedTime( 0 );
		if ( abs(timeCorrection) <= hopSize() ) {
			double time = frameTime()/* + ( timeCorrection / sampleRate() ) */;
			frame.push_back( Peak( 0, DCmag, 0., 0., time ) );
		}
	}
	
	if ( frame.size() > 0 ) {
		Peak & peak = frame.front();
		debugger << "frame has peak at frequency " << peak.frequency() <<
		 			 " amplitude " << peak.amplitude() <<
		 			 " and time " << peak.time() << endl;
	}
	#endif
	
	//	look for magnitude peaks in the spectrum:
	for ( int j = 1; j < (_spectrum->size() / 2) - 1; ++j ) {
		if ( abs((*_spectrum)[j]) > abs((*_spectrum)[j-1]) && 
			 abs((*_spectrum)[j]) > abs((*_spectrum)[j+1])) {
			//	itsa magnitude peak, does it clear the noise floor?
			double mag = _spectrum->magnitude( j );
			if ( mag < threshold )
				continue;
			
			//	compute the fractional frequency sample
			//	and the frequency:
			double fsample = _spectrum->reassignedFrequency( j );	//	fractional sample
			double fHz = fsample * sampsToHz;
			
			//	if the frequency is too low (not enough periods
			//	in the analysis window), reject it:
			if ( fHz < _minfreq ) 
				continue;
				
			//	if the time correction for this peak is large,
			//	reject it:
			double timeCorrection = _spectrum->reassignedTime( fsample );
			if ( abs(timeCorrection) > hopSize() )
				continue;
				
			//	retain a spectral peak corresponding to
			//	this sample:
			double phase = _spectrum->reassignedPhase( fsample, timeCorrection );
			double time = frameTime() + ( timeCorrection / sampleRate() );
			frame.push_back( Peak( fHz, mag, 0., phase, time ) );
		
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
Analyzer::thinPeaks( Frame & frame )
{
	frame.sort( greater_amplitude<Peak>() );
	
	//debugger << "had " << frame.size() << " peaks in this frame" << endl;

	//	nothing can mask the loudest peak, so I can start with the
	//	second one, _and_ I can safely decrement the iterator when 
	//	I need to remove the element at its postion:
	Frame::iterator it = frame.begin();
	for ( ++it; it != frame.end(); ++it ) {
		//	search all louder peaks for one that is too near
		//	in frequency:
		double lower = it->frequency() - partialSeparation();
		double upper = it->frequency() + partialSeparation();
		if ( it != find_if( frame.begin(), it, frequency_between< Peak >( lower, upper ) ) ) {
			//	find_if returns the end of the range (it) if it finds nothing; 
			//	remove *it from the frame
			frame.erase( it-- );
		}
	}
	
	//debugger << "kept " << frame.size() << " peaks in this frame" << endl;
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
	return abs( partial.frequencyAt( time ) - bp.frequency() );
}

// ---------------------------------------------------------------------------
//	formPartials
// ---------------------------------------------------------------------------
//	Append the Breakpoints to existing Partials, if appropriate, or else 
//	give birth to new Partials.
//
void 
Analyzer::formPartials( Frame & frame )
{
	//	frequency-sort the frame:
	frame.sort( less_frequency<Peak>() );
	
	//	loop over short-time peaks:
	for( Frame::iterator bpIter = frame.begin(); bpIter != frame.end(); ++bpIter ) {
		const Peak & peak = *bpIter;
		
		//	compute the time after which a Partial
		//	must have Breakpoints in order to be 
		//	eligible to receive this Breakpoint:
		//	The earliest Breakpoint we could have kept 
		//	from the previous frame:
		double tooEarly = frameTime() - (2. * frameLength());
		
		//	loop over all Partials, find the eligible Partial
		//	that is nearest in frequency to the Peak:
		partial_iterator nearest = partials().end();
		for ( partial_iterator pIter = partials().begin(); pIter != partials().end(); ++pIter ) {
			//	candidate Partials must have 
			//	recent envelope tails:
			if ( pIter->endTime() < tooEarly ) {
				continue;	//	loop over all Partials
			}
			
			//	remember this Partial if it is nearer in frequency 
			//	to the Breakpoint than every other Partial:
			if ( nearest == partials().end() || 
				 distance( *pIter, peak, peak.time() ) < distance( *nearest, peak, peak.time() ) ) {
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
							(distance(*nearest, peak, peak.time())) : 
							(0.);
		Frame::iterator next = bpIter;
		++next;
		Frame::iterator prev = bpIter;
		--prev;
		if ( nearest == partials().end() /* (1) */ || 
			 thisdist > captureRange() /* (2) */ ||
			 ( next != frame.end() && 
			 	thisdist > distance( *nearest, *(next), next->time() ) ) /* (3) */ ||
			 ( bpIter != frame.begin() && 
			 	thisdist > distance( *nearest, *(prev), prev->time() ) ) /* (4) */ ) {
			 	
			 /*debugger << "spawning a partial at frequency " << peak.frequency() <<
			 			 " amplitude " << peak.amplitude() <<
			 			 " and time " << peak.time() << endl;
			 */
			 spawnPartial( peak.time(), peak );
		}
		else {
			/*debugger << "matching a partial at frequency " << peak.frequency() <<
			 			" amplitude " << peak.amplitude() <<
			 			" and time " << peak.time() << endl;
			*/
			nearest->insert( peak.time(), peak );
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
//	Discovered that we have many Partials of zero duration, no sense
//	in retaining those.
//
void
Analyzer::pruneBogusPartials( void )
{
	long countem = 0;
	for ( partial_iterator it = partials().begin(); 
		  it != partials().end(); 
		  /* ++it */ ) {
		//	need to be careful with the iterator update, 
		//	because erasure will invalidate it:
		partial_iterator next = it;
		++next;
		if ( it->duration() == 0. ) {
			distributeEnergy( *it, partials().begin(), partials().end() );
			partials().erase( it );
			++countem;
		}
		it = next;
	}
	debugger << "Analyzer removed " << countem << " zero-duration Partials." << endl;
}


End_Namespace( Loris )

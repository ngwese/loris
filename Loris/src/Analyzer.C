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
//	Arbitrarily, the fiirst window is centered hop
//	samples before the beginning of the buffer, and 
//	the last window is the first one centered more than
//	hop samples past the end of the buffer.
	const long latestIdx = buf.size() + 2L * hopSize();
	try { 
		for ( _winMiddleIdx = - hopSize(); 
			  _winMiddleIdx < latestIdx; 
			  _winMiddleIdx += hopSize() ) {
			 
			//debugger << "analyzing frame centered at " << _winMiddleIdx << endl; 
			 
			//	compute reassigned spectrum:
			//	actually, need to call this with a 
			//	sub range of the sample buffer.
			_spectrum->transform( buf, _winMiddleIdx );
			
			//	extract Breakpoints from the spectrum:
			Frame breakpoints = extractBreakpoints();	

			//	perform bandwidth association:
			
			//	form Partials from the extracted Breakpoints:
			formPartials( breakpoints );

		}	//	end of loop over short-time frames
	}
	catch ( Exception & ex ) {
		ex.append( "analysis failed." );
		throw;
	}
	
	debugger << "analysis complete" << endl;
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
	if (winlen % 2) {
		++winlen;
	}
	double winshape = KaiserWindow::computeShape( _windowAtten );
	
	//	compute the hop size:
	//	Smith and Serra (1990) cite (Allen 1977) saying: a good choice of hop 
	//	is the window length divided by the main lobe width in frequency samples.
	//	Don't include zero padding in the computation of width (i.e. use window
	//	length instead of transform length).
	const double mlw_samp = _windowWidth * (winlen / srate);
	_hop = round( winlen / mlw_samp );
	
	//	lower frequency bound for Breakpoint extraction,
	//	require at least two periods in the window:
	double _minfreq = 2. / ( winlen / srate );
	
	try {
		//	configure window:
		vector< double > v( winlen );
		KaiserWindow::create( v, winshape );
		
		//	configure spectrum:
		_spectrum.reset( new ReassignedSpectrum( v ) );
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
//	extractBreakpoints
// ---------------------------------------------------------------------------
//	The reassigned spectrum has been computed, and short-time peaks
//	identified. From those peaks, construct Breakpoints, subject to 
//	some selection criteria. 
//
//	The peaks from the reassigned spectrum are frequency-sorted (implicitly)
//	so the frame generated here is automatically frequency-sorted.
//
template<class T>
struct less_first_frequency
{
	boolean operator()( const T & lhs, const T & rhs ) const
		{ return lhs.first.frequency() < rhs.first.frequency(); }
};

template<class T>
struct greater_magnitude
{
	boolean operator()( const T & lhs, const T & rhs ) const
		{ return lhs.magnitude() > rhs.magnitude(); }
};	

template<class T>
struct frequency_first_near
{
	frequency_first_near( double f, double howNear ) : 
		_fmin( f - howNear ), _fmax( f + howNear ) {}
	boolean operator()( const T & t )  const
		{ 
			return (t.first.frequency() > _fmin) && 
				   (t.first.frequency() < _fmax); 
		}
	private:
		double _fmin, _fmax;
};

	
Analyzer::Frame 
Analyzer::extractBreakpoints( void )
{
	Frame frame;	//	empty frame
	
	//	collect short-time peaks, and sort them by 
	//	decreasing magnitude:
	ReassignedSpectrum::Peaks tmp = _spectrum->findPeaks( noiseFloor() );			
	vector< ReassignedSpectrum::Peak > peaks( tmp.begin(), tmp.end() );
	sort( peaks.begin(), peaks.end(), greater_magnitude<ReassignedSpectrum::Peak>() );
	
	//	loop over short-time peaks:
	vector< ReassignedSpectrum::Peak >::iterator it;
	for ( it = peaks.begin(); it != peaks.end(); ++it ) {
		//	check against lower frequency bound:
		double peakfreq = _spectrum->reassignedFrequency( it->frequency() ) * sampleRate();
		if ( peakfreq < _minfreq ) {
			continue;	//	loop over short-time peaks
		} 

		//	if the time correction for this peak is large,
		//	forget it, go on to the next one:
		double peakSampleOffset = _spectrum->reassignedTime( it->frequency() );
		if ( abs(peakSampleOffset) > hopSize() ) {
			continue;	//	loop over short-time peaks
		} 
		
		//	make sure its not too close to any louder 
		//	Breakpoints, already retained:
		Frame::const_iterator masker = 
		find_if( frame.begin(), frame.end(), 
				 frequency_first_near< Frame::value_type >( peakfreq, captureRange() ) );
		if ( masker	!= frame.end() ) {
		/*
			debugger << "rejecting Breakpoint at " << peakfreq << " magnitude " <<
					 it->magnitude() << " masked by one at " << 
					 masker->first.frequency() << " magnitude " << 
					 masker->first.amplitude() << endl;
		*/
			continue;
		}	
		
		//	create a Breakpoint corresponding to the
		//	short-time reassigned spectral peak:
		Breakpoint bp( peakfreq, it->magnitude(), 0. /* bandwidth */, 
					   _spectrum->reassignedPhase( it->frequency(), peakSampleOffset ) );
	
		//	add it to the frame:
		frame.push_back( make_pair( bp, frameTime() + (peakSampleOffset / sampleRate()) ) );
	}
	/*
	if ( peaks.size() != frame.size() ) {
		debugger << "had " << peaks.size() << " peaks, kept " << frame.size() << " breakpoints." << endl;
	}
	*/
	sort( frame.begin(), frame.end(), 
		  less_first_frequency<Frame::value_type>() );
	return frame;
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
//	frame is a frequency-sorted collection of Breakpoint-time pairs.
//	Append the Breakpoints to existing Partials, if appropriate, or else 
//	give birth to new Partials.
//
void 
Analyzer::formPartials( const Frame & frame )
{
	//	loop over short-time peaks:
	for( Frame::const_iterator bpIter = frame.begin(); bpIter != frame.end(); ++bpIter ) {
		const Breakpoint & bp = bpIter->first;
		double peakTime = bpIter->second;
		
		//	compute the time after which a Partial
		//	must have Breakpoints in order to be 
		//	eligible to receive this Breakpoint:
		double tooEarly = frameTime() - (1.5 * frameLength());	//	???
		
		//	loop over all Partials:
		partial_iterator pIter, nearest = partials().end();
		for ( pIter = partials().begin(); pIter != partials().end(); ++pIter ) {
			//	candidate Partials must have 
			//	recent envelope tails:
			if ( pIter->endTime() < tooEarly ) {
				continue;	//	loop over all Partials
			}
			
			//	remember this Partial if it is nearer in frequency 
			//	to the Breakpoint than every other Partial:
			if ( nearest == partials().end() || 
				 distance( *pIter, bp, peakTime ) < distance( *nearest, bp, peakTime ) ) {
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
		double thisdist = (nearest != partials().end()) ? (distance(*nearest, bp, peakTime)) : (0.);
		if ( nearest == partials().end() /* (1) */ || 
			 thisdist > captureRange() /* (2) */ ||
			 ( bpIter+1 != frame.end() && 
			 	thisdist > distance( *nearest, (bpIter+1)->first, (bpIter+1)->second ) ) /* (3) */ ||
			 ( bpIter != frame.begin() && 
			 	thisdist > distance( *nearest, (bpIter-1)->first, (bpIter-1)->second ) ) /* (4) */ ) {
			 	
			 //debugger << "spawning a partial at frequency " << bpIter->first.frequency() <<
			 //			" and time " << bpIter->second << endl;
			 spawnPartial( peakTime, bp );
		}
		else {
			//debugger << "matching a partial at frequency " << bpIter->first.frequency() <<
			// 			" and time " << bpIter->second << endl;
			nearest->insert( peakTime, bp );
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


End_Namespace( Loris )

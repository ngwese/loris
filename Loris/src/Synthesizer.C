// ===========================================================================
//	Synthesizer.C
//	
//	Implementation of Loris::Synthesizer.
//	
//	Loris synthesis generates a buffer of samples from a 
//	collection of Partials. 
//
//	-kel 16 Aug 99
//
// ===========================================================================
#include "LorisLib.h"
#include "Synthesizer.h"
#include "Exception.h"
#include "Oscillator.h"
#include "Partial.h"
#include "PartialIterator.h"
#include "notifier.h"

#include <algorithm>
#include <vector>

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
#else
	#include <math.h>
#endif

using namespace std;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	Synthesizer constructor
// ---------------------------------------------------------------------------
//	Default offset and fade are 1ms, so that Partials beginning at time 
//	zero (like all Lemur 5 analyses) have time to ramp in.
//
//	Default osc is an auto_ptr with no reference, indicating that a default 
//	Oscillator should be created and used.
//
//	auto_ptr is used to submit the Oscillator argument to make explicit the
//	source/sink relationship between the caller and the Synthesizer. After
//	the call, the Synthesizer will own the Oscillator, and the client's auto_ptr
//	will have no reference (or ownership).
//
Synthesizer::Synthesizer( vector< double > & buf, double srate ) :
	_sampleRate( srate ),
	_samples( buf ),
	_iter( new BasicPartialIterator() )
{
	//	check to make sure that the sample rate is valid:
	if ( _sampleRate <= 0. ) {
		Throw( InvalidObject, "Synthesizer sample rate must be positive." );
	}
}

// ---------------------------------------------------------------------------
//	Synthesizer copy constructor
// ---------------------------------------------------------------------------
//	This will need to be changed is Oscillator ever becomes a base class.
//
//	This creates a Synthesizer that shares a sample buffer with
//	other. Is this desired?
//
Synthesizer::Synthesizer( const Synthesizer & other ) :
	_sampleRate( other._sampleRate ),
	_samples( other._samples ),
	_iter( other._iter->clone() )
{
}

// ---------------------------------------------------------------------------
//	synthesize
// ---------------------------------------------------------------------------
//	Try to prevent Partial turnon/turnoff artifacts (clicks) by ramping 
//	Partials up from and down to zero amplitude. If possible, the ramp
//	on or off is pasted on the beginning and end of the Partial, but if 
//	the Partial is too close to the buffer boundary (e.g. it starts at time
//	0.), then the ramp at the head or tail of the Partial gets compressed. 
//	Phase is always corrected, and is never altered by the ramping. Samples
//	are never written outside the buffer boundaries.
//
//	A long Partial may generate samples at non-zero amplitude all the way 
//	to the end of the buffer. There's no guarantee that there won't be
//	a click at the end. Yet.
//	
void
Synthesizer::synthesize( const Partial & p, double timeShift /* = 0.*/ )
{
/*
	debugger << "synthesizing Partial from " << p.startTime() <<
			" to " << p.endTime() << " starting phase " <<
			p.initialPhase() << " starting frequency " << 
			p.begin()->second.frequency() << endl;
*/
	iterator()->reset( p );
	
//	don't bother to synthesize Partials having zero duration:
	if ( iterator()->duration() == 0. || 
		 iterator()->endTime() + timeShift < 0. ||
		 (iterator()->startTime() + timeShift) * sampleRate() > _samples.size() )
	{
		return;
	}
	
//	compute initial values assuming fade-in:
	const double FADE_TIME = 0.001; 	//	1 ms
	double itime = iterator()->time() + timeShift - FADE_TIME;
	double ifreq = iterator()->frequency();
	double iamp = 0.;
	double ibw = iterator()->bandwidth();
	
	if ( itime < 0. )
	{
		//	advance the iterator until it refers
		//	to a breakpoint past zero:
		while (iterator()->time() + timeShift < 0.)
		{
			itime = iterator()->time() + timeShift;
			ifreq = iterator()->frequency();
			iamp = iterator()->amplitude();
			ibw = iterator()->bandwidth();
			iterator()->advance();
		}
		
		//	compute interpolated initial values:
		double alpha = - itime / (iterator()->time() + timeShift - itime);
		ifreq = (alpha * iterator()->frequency()) + ((1.-alpha) * ifreq);
		iamp = (alpha * iterator()->amplitude()) + ((1.-alpha) * iamp);
		ibw = (alpha * iterator()->bandwidth()) + ((1.-alpha) * ibw);
		itime = 0.;
	}
	
//	compute the starting phase:
	double dsamps = (iterator()->time() + timeShift - itime) * sampleRate();
	Assert( dsamps >= 0. );
	double avgfreq = 0.5 * (ifreq + iterator()->frequency());
	double iphase = iterator()->phase() - (radianFreq(avgfreq) * dsamps);
		
//	setup the oscillator:
//	Remember that the oscillator only knows about radian frequency! Convert!
	Oscillator osc( radianFreq( ifreq ), iamp, ibw, iphase );
	
//	initialize sample timeShift:
	long curSampleIdx = itime * sampleRate();
	
//	synthesize linear-frequency segments until there aren't any more:
	for ( ; ! iterator()->atEnd(); iterator()->advance() ) 
	{
		//	compute target sample index:
		long tgtsamp = (iterator()->time() + timeShift) * sampleRate();
		if ( tgtsamp >= _samples.size() )
			break;
			
		//	generate samples:
		osc.generateSamples( _samples, 
							 tgtsamp - curSampleIdx, 
							 curSampleIdx,
							 radianFreq( iterator()->frequency() ), 
							 iterator()->amplitude(), 
							 iterator()->bandwidth() );
									  
		//	update the current sample index:
		curSampleIdx = tgtsamp;
	}

//	either ran out of buffer ( tgtsamp >= _sample.size() )
//	or ran out of Breakpoints ( iterator()->atEnd() )
//	synthesize Partial turn-off:
	double tgtradfreq, tgtamp, tgtbw;
	if ( iterator()->atEnd() ) 
	{
		tgtradfreq = osc.radianFreq();
		tgtamp = 0.;
		tgtbw = osc.bandwidth();
	}
	else
	{
		tgtradfreq = radianFreq( iterator()->frequency() );
		tgtamp = iterator()->amplitude();
		tgtbw = iterator()->bandwidth();
	}
	
//	interpolate final values:
	long finalsamp = 
		std::min( curSampleIdx + long(FADE_TIME * sampleRate()), long(_samples.size()) );
	double alpha = (finalsamp - curSampleIdx) / (FADE_TIME * sampleRate());
	tgtradfreq = (alpha * tgtradfreq) + ((1. - alpha) * osc.radianFreq());
	tgtamp = (alpha * tgtamp) + ((1. - alpha) * osc.amplitude());
	tgtbw = (alpha * tgtbw) + ((1. - alpha) * osc.bandwidth());
	
//	generate samples:
	osc.generateSamples( _samples, 
						 finalsamp - curSampleIdx, 
						 curSampleIdx,
						 tgtradfreq, 
						 tgtamp, 
						 tgtbw );	
}

// ---------------------------------------------------------------------------
//	setIterator
// ---------------------------------------------------------------------------
//
PartialIteratorPtr 
Synthesizer::setIterator( PartialIteratorPtr inIter  ) 
{
	PartialIteratorPtr ret( _iter );
	_iter = inIter;
	return ret;
}
	
// ---------------------------------------------------------------------------
//	radianFreq
// ---------------------------------------------------------------------------
//	
inline double 
Synthesizer::radianFreq( double hz ) const
{
	return hz * TwoPi / sampleRate();
}

// ---------------------------------------------------------------------------
//	SynthesisIterator amplitude
// ---------------------------------------------------------------------------
//	Synthesize at zero amplitude above the Nyquist
//	frequency, and without bandwidth enhancement below
//	the specified cutoff frequency.
//
double 
SynthesisIterator::amplitude( void ) const
{
	if ( iterator()->frequency() > _nyquistfreq )
		return 0.;
	else if ( iterator()->frequency() < _bwecutoff )
		return iterator()->amplitude() * sqrt(1. - bwclamp( iterator()->bandwidth() ) );
	else
		return iterator()->amplitude();
}

// ---------------------------------------------------------------------------
//	SynthesisIterator amplitude
// ---------------------------------------------------------------------------
//	Synthesize without bandwidth enhancement below
//	the specified cutoff frequency, also clamp
//	partial bandwidths to reasonable values.
//
double 
SynthesisIterator::bandwidth( void ) const
{
	if ( iterator()->frequency() > _bwecutoff )
		return bwclamp( iterator()->bandwidth() );
	else
		return 0.;
}
	


End_Namespace( Loris )

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
 * Synthesizer.C
 *
 * Implementation of class Loris::SynthesizerSynthesizer, a synthesizer of 
 * bandwidth-enhanced Partials.
 *
 * Kelly Fitz, 16 Aug 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include <config.h>
#endif

#include<Synthesizer.h>
#include<Exception.h>
#include<Oscillator.h>
#include<Partial.h>
#include<Notifier.h>
#include <algorithm>
#include <cmath>

#if defined(HAVE_M_PI) && (HAVE_M_PI)
	const double Pi = M_PI;
#else
	const double Pi = 3.14159265358979324;
#endif

//	begin namespace
namespace Loris {

static long countem = 0;

// ---------------------------------------------------------------------------
//	Synthesizer constructor
// ---------------------------------------------------------------------------
//
Synthesizer::Synthesizer( double srate, double * bufStart, double * bufEnd  ) :
	_sampleRate( srate ),
	_sampleBuffer( bufStart ),
	_sampleBufferSize( bufEnd - bufStart ),
	_osc( new Oscillator() )
{
	//	check to make sure that the sample rate is valid:
	if ( _sampleRate <= 0. ) {
		Throw( InvalidObject, "Synthesizer sample rate must be positive." );
	}

	//	check to make sure that the buffer bounds are valid:
	if ( _sampleBufferSize <= 0 ) {
		Throw( InvalidObject, "Synthesizer buffer length must be positive." );
	}

	//countem = 0;
}

// ---------------------------------------------------------------------------
//	Synthesizer copy constructor
// ---------------------------------------------------------------------------
//	Synthesizer copies share a sample buffer.
//
Synthesizer::Synthesizer( const Synthesizer & other ) :
	_sampleRate( other._sampleRate ),
	_sampleBuffer( other._sampleBuffer ),
	_sampleBufferSize( other._sampleBufferSize ),
	_osc( new Oscillator( *other._osc ) )
{
	//countem = 0;
}


// ---------------------------------------------------------------------------
//  destructor
// ---------------------------------------------------------------------------
//
Synthesizer::~Synthesizer(void)
{
	//debugger << "synthesized " << countem << " partials, bye." << endl;
	//countem = 0;
}

// ---------------------------------------------------------------------------
//	Synthesizer assignment
// ---------------------------------------------------------------------------
//	Synthesizer copies share a sample buffer.
//
Synthesizer & 
Synthesizer::operator= ( const Synthesizer & other )
{
	if ( this != &other )
	{
		_sampleRate = other._sampleRate;
		_sampleBuffer = other._sampleBuffer;
		_sampleBufferSize = other._sampleBufferSize;
	}

	return *this;
}

// ---------------------------------------------------------------------------
//	synthesize
// ---------------------------------------------------------------------------
//	Synthesize a bandwidth-enhanced sinusoidal Partial with the specified 
//	timeShift (in seconds). The Partial parameter data is filtered by the 
//	Synthesizer's PartialView. Zero-amplitude Breakpoints are inserted
//	1 millisecond (Partial::FadeTime()) from either end of the Partial to reduce 
//	turn-on and turn-off artifacts. The client is responsible or insuring
//	that the buffer is long enough to hold all samples from the time-shifted
//	and padded Partials. Synthesizer will not generate samples outside the
//	buffer, but neither will any attempt be made to eliminate clicks at the
//	buffer boundaries.  
//	
void
Synthesizer::synthesize( const Partial & p, double timeShift /* = 0.*/ ) const
{
//
	debugger << "synthesizing Partial from " << p.startTime()*sampleRate() <<
			" to " << p.endTime()*sampleRate() << " starting phase " <<
			p.initialPhase() << " starting frequency " << 
			p.begin()->frequency() << endl;
//
	
//	don't bother to synthesize Partials that will generate no samples in
//	the samples buffer; but note that Partials having a single Breakpoint,
//	while officially of duration "0.", will generate samples due to ramping
//	in and out:
	if ( p.endTime() + timeShift < 0. ||
		(p.startTime() + timeShift) * sampleRate() > numSamples() )
	{
		debugger << "ignoring a partial that would generate no samples" << endl;
		debugger << "start time is " << p.startTime() << " end time is " << p.endTime() << endl;
		return;
	}
	
//	create an iterator on the PartialView:
	PartialConstIterator iterator = p.begin();
	
//	compute the initial oscillator state, assuming
//	a prepended Breakpoint of zero amplitude:
	double itime = iterator.time() + timeShift - Partial::FadeTime();
	double ifreq = iterator.breakpoint().frequency();
	double iamp = 0.;
	double ibw = iterator.breakpoint().bandwidth();

//	interpolate the initial oscillator state if
//	the onset time is before the start of the 
//	buffer:	
	if ( itime < 0. )
	{
		//	advance the iterator until it refers
		//	to a breakpoint past zero:
		while (iterator.time() + timeShift < 0.)
		{
			itime = iterator.time() + timeShift;
			ifreq = iterator.breakpoint().frequency();
			iamp = iterator.breakpoint().amplitude();
			ibw = iterator.breakpoint().bandwidth();
			++iterator;
		}
		
		//	compute interpolated initial values:
		double alpha = - itime / (iterator.time() + timeShift - itime);
		ifreq = (alpha * iterator.breakpoint().frequency()) + ((1.-alpha) * ifreq);
		iamp = (alpha * iterator.breakpoint().amplitude()) + ((1.-alpha) * iamp);
		ibw = (alpha * iterator.breakpoint().bandwidth()) + ((1.-alpha) * ibw);
		itime = 0.;
	}
	
//	compute the starting phase:
	double dsamps = (iterator.time() + timeShift - itime) * sampleRate();
	Assert( dsamps >= 0. );
	double avgfreq = 0.5 * (ifreq + iterator.breakpoint().frequency());
	double iphase = iterator.breakpoint().phase() - (radianFreq(avgfreq) * dsamps);

//	don't alias:
	if ( radianFreq( ifreq ) > Pi )	
	{
		iamp = 0.;
	}

//	clamp bandwidth:
	if ( ibw > 1. )
	{
		debugger << "clamping bandwidth at 1." << endl;
		ibw = 1.;
	}
	else if ( ibw < 0. )
	{ 
		debugger << "clamping bandwidth at 0." << endl;
		ibw = 0.;
	}
		
//	setup the oscillator:
//	Remember that the oscillator only knows about radian frequency! Convert!
	_osc->setRadianFreq( radianFreq( ifreq ) );
	_osc->setAmplitude( iamp );
	_osc->setBandwidth( ibw );
	_osc->setPhase( iphase );
	
//	initialize sample buffer index:
	long curSampleIdx = long(itime * sampleRate());
	
//	synthesize linear-frequency segments until there aren't any more
//	segments or the segments threaten to run off the end of the buffer:
	//const PartialConstIterator End = p.end();
	//for ( ; iterator != End; ++iterator ) 
	while ( iterator != p.end() )
	{
		//	compute target sample index:
		long tgtsamp = long( (iterator.time() + timeShift) * sampleRate() );
		if ( tgtsamp >= numSamples() )
			break;
			
		//	generate samples:
		//	(buffer, beginIdx, endIdx, freq, amp, bw)
		//	(Could check first whether any non-zero samples
		//	will be generated, if not, can just set target
		//	values.)
		_osc->generateSamples( _sampleBuffer + curSampleIdx, _sampleBuffer + tgtsamp,
								radianFreq( iterator.breakpoint().frequency() ), 
								iterator.breakpoint().amplitude(), 
								iterator.breakpoint().bandwidth() );
									  
		//	if the current oscillator amplitude is
		//	zero, reset the phase (note: the iterator
		//	values are the target values, so the phase
		//	should be set _after_ generating samples,
		//	when the oscillator and iterator are in-sync):
		if ( _osc->amplitude() == 0. )
			_osc->setPhase( iterator.breakpoint().phase() );
			
		//	update the current sample index:
		curSampleIdx = tgtsamp;

		//	advance iterator:
		++iterator;
	}

	// debugger << "out of loop at target samp " << curSampleIdx << endl;

//	either ran out of buffer ( tgtsamp >= _sample.size() )
//	or ran out of Breakpoints ( iterator.atEnd() ), 
//	compute the final target oscillator state assuming 
//	an appended Breakpoint of zero amplitude:
	double tgtradfreq, tgtamp, tgtbw;
	if ( iterator == p.end() ) 
	{
		tgtradfreq = _osc->radianFreq();
		tgtamp = 0.;
		tgtbw = _osc->bandwidth();
	}
	else
	{
		tgtradfreq = radianFreq( iterator.breakpoint().frequency() );
		tgtamp = iterator.breakpoint().amplitude();
		tgtbw = iterator.breakpoint().bandwidth();
	}
	
//	interpolate final oscillator state if the target 
//	final sample is past the end of the buffer:
	long finalsamp = 
		std::min( curSampleIdx + long(Partial::FadeTime() * sampleRate()), numSamples() );
	double alpha = 
		(finalsamp - curSampleIdx) / (Partial::FadeTime() * sampleRate());
	tgtradfreq = (alpha * tgtradfreq) + ((1. - alpha) * _osc->radianFreq());
	tgtamp = (alpha * tgtamp) + ((1. - alpha) * _osc->amplitude());
	tgtbw = (alpha * tgtbw) + ((1. - alpha) * _osc->bandwidth());
	
//	generate samples:
//	(buffer, beginIdx, endIdx, freq, amp, bw)
	_osc->generateSamples( _sampleBuffer + curSampleIdx, _sampleBuffer + finalsamp,
					   tgtradfreq, tgtamp, tgtbw );	

	// ++countem;
}
	
// ---------------------------------------------------------------------------
//	radianFreq
// ---------------------------------------------------------------------------
//	Compute radian frequency (used by Loris::Oscillator) from frequency in Hz.
//	
inline double 
Synthesizer::radianFreq( double hz ) const
{
	return hz * 2. * Pi / sampleRate();
}

}	//	end of namespace Loris

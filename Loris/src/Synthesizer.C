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
#include "Synthesizer.h"
#include "Exception.h"
#include "Oscillator.h"
#include "Partial.h"
#include "PartialView.h"
#include "notifier.h"
#include "pi.h"
#include <algorithm>
#include <vector>
#include <cmath>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

static long countem = 0;

// ---------------------------------------------------------------------------
//	Synthesizer constructor
// ---------------------------------------------------------------------------
//
Synthesizer::Synthesizer( std::vector< double > & buf, double srate ) :
	_sampleRate( srate ),
	_samples( buf ),
	_view( new BasicPartialView() )
{
	//	check to make sure that the sample rate is valid:
	if ( _sampleRate <= 0. ) {
		Throw( InvalidObject, "Synthesizer sample rate must be positive." );
	}

	countem = 0;
}

// ---------------------------------------------------------------------------
//	Synthesizer copy constructor
// ---------------------------------------------------------------------------
//	Create a copy of other by cloning its PartialView and sharing its
//	sample buffer.
//
Synthesizer::Synthesizer( const Synthesizer & other ) :
	_sampleRate( other._sampleRate ),
	_samples( other._samples ),
	_view( other._view->clone() )
{
}


// ---------------------------------------------------------------------------
//  destructor
// ---------------------------------------------------------------------------
//
Synthesizer::~Synthesizer(void)
{
	debugger << "synthesized " << countem << " partials, bye." << endl;
	countem = 0;
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
Synthesizer::synthesize( const Partial & p, double timeShift /* = 0.*/ )
{
/*
	debugger << "synthesizing Partial from " << p.startTime()*sampleRate() <<
			" to " << p.endTime()*sampleRate() << " starting phase " <<
			p.initialPhase() << " starting frequency " << 
			p.begin()->frequency() << endl;
*/
	view().view( p );
	
//	don't bother to synthesize Partials having zero duration:
	if ( view().duration() == 0. || 
		 view().endTime() + timeShift < 0. ||
		 (view().startTime() + timeShift) * sampleRate() > _samples.size() )
	{
		debugger << "ignoring a partial that would generate no samples" << endl;
		debugger << "start time is " << view().startTime() << " end time is " << view().endTime() << endl;
		return;
	}
	
//	create an iterator on the PartialView:
	PartialViewIterator iterator = view().begin();
	
//	compute the initial oscillator state, assuming
//	a prepended Breakpoint of zero amplitude:
	double itime = iterator.time() + timeShift - Partial::FadeTime();
	double ifreq = iterator.frequency();
	double iamp = 0.;
	double ibw = iterator.bandwidth();

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
			ifreq = iterator.frequency();
			iamp = iterator.amplitude();
			ibw = iterator.bandwidth();
			iterator.advance();
		}
		
		//	compute interpolated initial values:
		double alpha = - itime / (iterator.time() + timeShift - itime);
		ifreq = (alpha * iterator.frequency()) + ((1.-alpha) * ifreq);
		iamp = (alpha * iterator.amplitude()) + ((1.-alpha) * iamp);
		ibw = (alpha * iterator.bandwidth()) + ((1.-alpha) * ibw);
		itime = 0.;
	}
	
//	compute the starting phase:
	double dsamps = (iterator.time() + timeShift - itime) * sampleRate();
	Assert( dsamps >= 0. );
	double avgfreq = 0.5 * (ifreq + iterator.frequency());
	double iphase = iterator.phase() - (radianFreq(avgfreq) * dsamps);
		
//	setup the oscillator:
//	Remember that the oscillator only knows about radian frequency! Convert!
	Oscillator osc( radianFreq( ifreq ), iamp, ibw, iphase );
	
//	initialize sample buffer index:
	long curSampleIdx = itime * sampleRate();
	
//	synthesize linear-frequency segments until there aren't any more
//	segments or the segments threaten to run off the end of the buffer:
	for ( ; ! iterator.atEnd(); iterator.advance() ) 
	{
		//	compute target sample index:
		long tgtsamp = (iterator.time() + timeShift) * sampleRate();
		if ( tgtsamp >= _samples.size() )
			break;
			
		//	generate samples:
		//	(buffer, beginIdx, endIdx, freq, amp, bw)
		osc.generateSamples( _samples, curSampleIdx, tgtsamp, 
							 radianFreq( iterator.frequency() ), 
							 iterator.amplitude(), 
							 iterator.bandwidth() );
									  
		//	if the current oscillator amplitude is
		//	zero, reset the phase (note: the iterator
		//	values are the target values, so the phase
		//	should be set _after_ generating samples,
		//	when the oscillator and iterator are in-sync):
		if ( osc.amplitude() == 0. )
			osc.setPhase( iterator.phase() );
			
		//	update the current sample index:
		curSampleIdx = tgtsamp;
	}

	// debugger << "out of loop at target samp " << curSampleIdx << endl;

//	either ran out of buffer ( tgtsamp >= _sample.size() )
//	or ran out of Breakpoints ( iterator.atEnd() ), 
//	compute the final target oscillator state assuming 
//	an appended Breakpoint of zero amplitude:
	double tgtradfreq, tgtamp, tgtbw;
	if ( iterator.atEnd() ) 
	{
		tgtradfreq = osc.radianFreq();
		tgtamp = 0.;
		tgtbw = osc.bandwidth();
	}
	else
	{
		tgtradfreq = radianFreq( iterator.frequency() );
		tgtamp = iterator.amplitude();
		tgtbw = iterator.bandwidth();
	}
	
//	interpolate final oscillator state if the target 
//	final sample is past the end of the buffer:
	long finalsamp = 
		std::min( curSampleIdx + long(Partial::FadeTime() * sampleRate()), 
				  long(_samples.size()) );
	double alpha = 
		(finalsamp - curSampleIdx) / (Partial::FadeTime() * sampleRate());
	tgtradfreq = (alpha * tgtradfreq) + ((1. - alpha) * osc.radianFreq());
	tgtamp = (alpha * tgtamp) + ((1. - alpha) * osc.amplitude());
	tgtbw = (alpha * tgtbw) + ((1. - alpha) * osc.bandwidth());
	
//	generate samples:
//	(buffer, beginIdx, endIdx, freq, amp, bw)
	osc.generateSamples( _samples, curSampleIdx, finalsamp, 
						 tgtradfreq, tgtamp, tgtbw );	

	++countem;
}

// ---------------------------------------------------------------------------
//	setView
// ---------------------------------------------------------------------------
//
void 
Synthesizer::setView( const PartialView & v  ) 
{
	_view.reset( v.clone() );
}
	
// ---------------------------------------------------------------------------
//	radianFreq
// ---------------------------------------------------------------------------
//	Compute radian frequency (used by Loris::Oscillator) from frequency in Hz.
//	
inline double 
Synthesizer::radianFreq( double hz ) const
{
	return hz * TwoPi / sampleRate();
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

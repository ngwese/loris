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
 * Synthesizer.c++
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

#include <JitterySynthesizer.h>
#include <ODonnellOscil.h>

#include <Breakpoint.h>
#include <BreakpointUtils.h>
#include <Envelope.h>
#include <Exception.h>
#include <Notifier.h>
#include <Partial.h>

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
//	The default fadeTime is 1 ms. (.001)
//
Synthesizer::Synthesizer( double samplerate, std::vector<double> & buffer, double fade  ) :
	sampleBuffer( buffer ),
	tfade( fade ),
	srate( samplerate )
{
	//	check to make sure that the sample rate is valid:
	if ( srate <= 0. ) 
	{
		Throw( InvalidObject, "Synthesizer sample rate must be positive." );
	}

	//	check to make sure that the buffer bounds are valid:
	if ( buffer.size() == 0 ) 
	{
		Throw( InvalidObject, "Synthesizer buffer length must be positive." );
	}

	//	check to make sure that the specified fade time
	//	is valid:
	if ( tfade < 0. )
	{
		Throw( InvalidObject, "Synthesizer Partial fade time must be non-negative." );
	}

	//countem = 0;
}

// ---------------------------------------------------------------------------
//	Synthesizer copy constructor
// ---------------------------------------------------------------------------
//	Synthesizer copies share a sample buffer.
//
Synthesizer::Synthesizer( const Synthesizer & rhs ) :
	osc( rhs.osc ),
	sampleBuffer( rhs.sampleBuffer ),
	tfade( rhs.tfade ),
	srate( rhs.srate ),
	jitterGain( rhs.jitterGain->clone() ),
	jitterCoherence( rhs.jitterCoherence->clone() ),
	jitterCutoff( rhs.jitterCutoff )
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
Synthesizer::operator=( const Synthesizer & rhs )
{
	if ( this != &rhs )
	{
		osc = rhs.osc;
		sampleBuffer = rhs.sampleBuffer;
		tfade = rhs.tfade;
		srate = rhs.srate;
		jitterGain.reset( rhs.jitterGain->clone() );
		jitterCoherence.reset( rhs.jitterCoherence->clone() );
		jitterCutoff = rhs.jitterCutoff;
	}

	return *this;
}

// ---------------------------------------------------------------------------
//	synthesize
// ---------------------------------------------------------------------------
//	Synthesize a bandwidth-enhanced sinusoidal Partial. Zero-amplitude
//	Breakpoints are inserted at either end of the Partial to reduce
//	turn-on and turn-off artifacts, as described above. The client is
//	responsible or insuring that this Synthesizer's buffer is long enough
//	to hold all samples from the time-shifted and padded Partials.
//	Synthesizer will not generate samples outside the buffer, but neither
//	will any attempt be made to eliminate clicks at the buffer boundaries.
//	
void
Synthesizer::synthesize( const Partial & p )
{
	if ( p.numBreakpoints() == 0 )
	{
		debugger << "Synthesizer ignoring a partial that contains no Breakpoints" << endl;
		return;
	}
	
	if ( p.startTime() < 0 )
	{
		Throw( InvalidPartial, "Tried to synthesize a Partial having start time less than 0." );
	}

	debugger << "synthesizing Partial from " << p.startTime() * srate <<
			" to " << p.endTime() * srate << " starting phase " <<
			p.initialPhase() << " starting frequency " << 
			p.first().frequency() << endl;

	//	resize the sample buffer if necessary:
	typedef unsigned long index_type;
	index_type endSamp = index_type( (p.endTime() + tfade) * srate );
	if ( endSamp+1 > sampleBuffer.size() )
	{
		//	pad by one sample:
		sampleBuffer.resize( endSamp+1 );
	}
	
	//	compute the starting time for synthesis of this Partial,
	//	tfade before the Partial's startTime, but not before 0:
	double itime = (tfade < p.startTime() ) ? (p.startTime() - tfade) : 0.;
	index_type currentSamp = index_type(itime * srate);
	
	//	reset the oscillator:
	osc.resetEnvelopes( BreakpointUtils::makeNullBefore( p.first(), p.startTime() - itime ), srate );
	osc.resetJitter( jitterAt( itime ), 
					 coherenceAt( itime, p.label() ),
					 currentSamp );

	//	synthesize linear-frequency segments until there aren't any more
	//	segments or the segments threaten to run off the end of the buffer:
	const Partial::label_type pnum = p.label();
	for ( Partial::const_iterator it = p.begin(); it != p.end(); ++it )
	{
		index_type tgtSamp = index_type( it.time() * srate );
		Assert( tgtSamp >= currentSamp );
		
		osc.oscillate( &(sampleBuffer[currentSamp]), &(sampleBuffer[tgtSamp]),
					   it.breakpoint(), srate,
					   jitterAt( it.time() ), coherenceAt( it.time(), pnum ), pnum );
									  
		//	if the current oscillator amplitude is
		//	zero, reset the phase (note: the iterator
		//	values are the target values, so the phase
		//	should be set _after_ generating samples,
		//	when the oscillator and iterator are in-sync):
		if ( it.breakpoint().amplitude() == 0. )
			osc.resetPhase( it.breakpoint().phase() );
			
		currentSamp = tgtSamp;
	}

	// debugger << "out of loop at target samp " << currentsamp << endl;
	
	double endTime = endSamp / srate;
	osc.oscillate( &(sampleBuffer[currentSamp]), &(sampleBuffer[endSamp]),
				   BreakpointUtils::makeNullAfter( p.last(), tfade ), srate,
				   jitterAt( endTime ), coherenceAt( endTime, pnum ), pnum );
	
	// ++countem;
}
	
#pragma mark -- access --
// ---------------------------------------------------------------------------
//	fadeTime
// ---------------------------------------------------------------------------
double 
Synthesizer::fadeTime( void ) const 
{
	return tfade;
}

// ---------------------------------------------------------------------------
//	numSamples 
// ---------------------------------------------------------------------------
std::vector<double>::size_type
Synthesizer::numSamples( void ) const 
{
	return sampleBuffer.size();
}


// ---------------------------------------------------------------------------
//	sampleRate
// ---------------------------------------------------------------------------
double 
Synthesizer::sampleRate( void ) const 
{
	return srate;
}

// ---------------------------------------------------------------------------
//	samples (const)
// ---------------------------------------------------------------------------
const std::vector<double>
Synthesizer::samples( void ) const 
{
	return sampleBuffer;
}

// ---------------------------------------------------------------------------
//	samples (non-const)
// ---------------------------------------------------------------------------
std::vector<double>
Synthesizer::samples( void )  
{
	return sampleBuffer;
}

#pragma mark -- mutation --

// ---------------------------------------------------------------------------
//	setFadeTime
// ---------------------------------------------------------------------------
void 
Synthesizer::setFadeTime( double partialFadeTime )  
{
	//	check to make sure that the specified fade time
	//	is valid:
	if ( partialFadeTime < 0. )
	{
		Throw( InvalidObject, "Synthesizer Partial fade time must be non-negative." );
	}

	tfade = partialFadeTime;
}

// ---------------------------------------------------------------------------
//	configureJitter
// ---------------------------------------------------------------------------
//	Configure this jittery Synthesizer to use the specified envelops
//	controlling jitter gain and coherence. Partials labeled greater
//	than the coherence cutoff are completely incoherent.
//
void 
Synthesizer::configureJitter( const Envelope & gain, const Envelope & coherence,
							  unsigned int coherenceCutoff )
{	
	jitterGain.reset( gain.clone() );
	jitterCoherence.reset( coherence.clone() );
	jitterCutoff = coherenceCutoff;
}

#pragma mark -- helpers --

// ---------------------------------------------------------------------------
//	coherenceAt
// ---------------------------------------------------------------------------
//	The jitter coherence is zero if no coherence envelope has been
//	specified, and if the Partial number is not below the coherence
//	cutoff.
//
double 
Synthesizer::coherenceAt( double t, int pnum ) const 
{ 
	return ( useJitter() && (pnum > 0) && (pnum <= jitterCutoff) ) ? 
			jitterCoherence->valueAt(t) : 0.; 
}

// ---------------------------------------------------------------------------
//	jitterAt
// ---------------------------------------------------------------------------
//	The jitter gain is zero if no jitter envelope has been specified.
//
double 
Synthesizer::jitterAt( double t ) const 
{ 
	return useJitter() ? jitterGain->valueAt(t) : 0.; 
}

// ---------------------------------------------------------------------------
//	useJitter
// ---------------------------------------------------------------------------
bool 
Synthesizer::useJitter( void ) const
{
	return jitterGain.get() != 0;
}

}	//	end of namespace Loris

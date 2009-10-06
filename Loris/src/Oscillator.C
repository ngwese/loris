/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2009 by Kelly Fitz and Lippold Haken
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
 * Oscillator.C
 *
 * Implementation of class Loris::Oscillator, a Bandwidth-Enhanced Oscillator.
 *
 * Kelly Fitz, 31 Aug 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 
#if HAVE_CONFIG_H
	#include "config.h"
#endif

#include "Oscillator.h"

#include "Filter.h"
#include "Partial.h"
#include "Notifier.h"

#include <cmath>
#include <vector>

#if defined(HAVE_M_PI) && (HAVE_M_PI)
	const double Pi = M_PI;
#else
	const double Pi = 3.14159265358979324;
#endif
const double TwoPi = 2*Pi;

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	protoype filter
// ---------------------------------------------------------------------------
//	Static local function for obtaining a prototype Filter
//	to use in Oscillator construction. Eventually, allow
//	external (client) specification of the Filter prototype.
//
static const Filter & prototype_filter( void )
{
	//	Chebychev order 3, cutoff 500, ripple -1.
	//
	//	Coefficients obtained from http://www.cs.york.ac.uk/~fisher/mkfilter/
	//	Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
	//
	static const double Gain = 4.663939184e+04;
	static const double ExtraScaling = 6.;
	static const double MaCoefs[] = { 1., 3., 3., 1. }; 
	static const double ArCoefs[] = { 1., 2.9258684252, -2.8580608586, 0.9320209046 };

	static const Filter proto( MaCoefs, MaCoefs + 4, ArCoefs, ArCoefs + 4, ExtraScaling/Gain );
	return proto;
}

// ---------------------------------------------------------------------------
//	one_pole_filter
// ---------------------------------------------------------------------------
//	Construct a single pole filter with the specified feedback coefficient	
//	and automatically-computed gain .
//
static Filter one_pole_filter( double feedback )
{
	double b[] = {1};
	double a[] = {1, feedback };
	return Filter( b, b+1, a, a+2, 1 - feedback );
}

// ---------------------------------------------------------------------------
//	Oscillator construction
// ---------------------------------------------------------------------------
//	Initialize stochastic modulators and state variables.
//
Oscillator::Oscillator( void ) :
	m_modulator( 1.0 /* seed */ ),
	m_filter( prototype_filter() ),
	m_instfrequency( 0 ),
	m_instamplitude( 0 ),
	m_instbandwidth( 0 ),
	m_determphase( 0 )
{
}

// ---------------------------------------------------------------------------
//	resetEnvelopes
// ---------------------------------------------------------------------------
//	Reset the instantaneous envelope parameters 
// 	(frequency, amplitude, bandwidth, and phase).
// 	The sample rate is needed to convert the 
// 	Breakpoint frequency (Hz) to radians per sample.
//
void 
Oscillator::resetEnvelopes( const Breakpoint & bp, double srate )
{
	//	Remember that the oscillator only knows about 
	//	radian frequency! Convert!
	m_instfrequency = bp.frequency() * TwoPi / srate;
	m_instamplitude = bp.amplitude();
	m_instbandwidth = bp.bandwidth();
	m_determphase = bp.phase();
	
 	//	clamp bandwidth:
	if ( m_instbandwidth > 1. )
	{
		debugger << "clamping bandwidth at 1." << endl;
		m_instbandwidth = 1.;
	}
	else if ( m_instbandwidth < 0. )
	{ 
		debugger << "clamping bandwidth at 0." << endl;
		m_instbandwidth = 0.;
	}

	//	don't alias:
	if ( m_instfrequency > Pi )
	{ 
		debugger << "fading out aliasing Partial" << endl;
		m_instamplitude = 0.;
	}
	
	//	Reset the fitler state too.
	m_filter.clear();
	
}

// ---------------------------------------------------------------------------
//	resetPhase
// ---------------------------------------------------------------------------
//	Reset the phase of the Oscillator to the specified
//	value, and clear the accumulated phase modulation. (?)
//	Or not.
//	This is done when the amplitude of a Partial goes to 
//	zero, so that onsets are preserved in distilled
//	and collated Partials.
//
void 
Oscillator::resetPhase( double ph )
{
	m_determphase = ph;
}

// ---------------------------------------------------------------------------
//	m2pi
// ---------------------------------------------------------------------------
//	O'Donnell's phase wrapping function.
//
static inline double m2pi( double x )
{
	using namespace std; //	floor should be in std
	#define ROUND(x) (floor(.5 + (x)))
	return x + ( TwoPi * ROUND(-x/TwoPi) );
}

// ---------------------------------------------------------------------------
//	generateSamples
// ---------------------------------------------------------------------------
//	Accumulate bandwidth-enhanced sinusoidal samples modulating the 
//	oscillator state from its current values of radian frequency,
//	amplitude, and bandwidth to the specified target values, into
//	the specified half-open range of doubles.
//
//	The caller must ensure that the range is valid. Target parameters
//	are bounds-checked. 
//
void
Oscillator::oscillate( double * begin, double * end,
					   const Breakpoint & bp, double srate )
{
	double targetFreq = bp.frequency() * TwoPi / srate, 
		   targetAmp = bp.amplitude(), 
		   targetBw = bp.bandwidth();
	
	//	clamp bandwidth:
	if ( targetBw > 1. )
	{
		debugger << "clamping bandwidth at 1." << endl;
		targetBw = 1.;
	}
	else if ( targetBw < 0. )
	{ 
		debugger << "clamping bandwidth at 0." << endl;
		targetBw = 0.;
	}
		
	//	don't alias:
	if ( targetFreq > Pi )	//	radian Nyquist rate
	{
		debugger << "fading out Partial above Nyquist rate" << endl;
		targetAmp = 0.;
	}

	//	compute trajectories:
	const double dTime = 1. / (end - begin);
	const double dFreq = (targetFreq - m_instfrequency) * dTime;
	const double dAmp = (targetAmp - m_instamplitude)  * dTime;
	const double dBw = (targetBw - m_instbandwidth)  * dTime;

	//	could use temporary local variables for speed... nah!
	//	Cannot possibly be worth it when I am computing square roots 
	//	and cosines!
	double am, nz;
	for ( double * putItHere = begin; putItHere != end; ++putItHere )
	{
		//	use math functions in namespace std:
		using namespace std;

		//	compute amplitude modulation due to bandwidth:
		//
		//	This will give the right amplitude modulation when scaled
		//	by the Partial amplitude:
		//
		//	carrier amp: sqrt( 1. - bandwidth ) * amp
		//	modulation index: sqrt( 2. * bandwidth ) * amp
		//
		nz = m_filter.sample( m_modulator.nextSample() );
		am = sqrt( 1. - m_instbandwidth ) + ( nz * sqrt( 2. * m_instbandwidth ) );	
				
		//	compute a sample and add it into the buffer:
		*putItHere += am * m_instamplitude * cos( m_determphase );
			
		//	update the instantaneous oscillator state:
		m_determphase += m_instfrequency;	//	frequency is radians per sample
		m_instfrequency += dFreq;
		m_instamplitude += dAmp;
		m_instbandwidth += dBw;
		if (m_instbandwidth < 0.)
		{
			m_instbandwidth = 0.;
		}
			
	}	// end of sample computation loop
	
	//	wrap phase to prevent eventual loss of precision at
	//	high oscillation frequencies:
	//	(Doesn't really matter much exactly how we wrap it, 
	//	as long as it brings the phase nearer to zero.)
	m_determphase = m2pi( m_determphase );
	
	//	set the state variables to their target values,
	//	just in case they didn't arrive exactly (overshooting
	//	amplitude or, especially, bandwidth, could be bad, and
	//	it does happen):
	m_instfrequency = targetFreq;
	m_instamplitude = targetAmp;
	m_instbandwidth = targetBw;
}

}	//	end of namespace Loris

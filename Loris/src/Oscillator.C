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
 * Oscillator.c++
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
	#include <config.h>
#endif

#include <Oscillator.h>

#include <Filter.h>
#include <Partial.h>
#include <Notifier.h>

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
	bwModulator( prototype_filter() ),
	i_frequency( 0 ),
	i_amplitude( 0 ),
	i_bandwidth( 0 ),
	determ_phase( 0 )
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
	i_frequency = bp.frequency() * TwoPi / srate;
	i_amplitude = bp.amplitude();
	i_bandwidth = bp.bandwidth();
	determ_phase = bp.phase();
	
 	//	clamp bandwidth:
	if ( i_bandwidth > 1. )
	{
		debugger << "clamping bandwidth at 1." << endl;
		i_bandwidth = 1.;
	}
	else if ( i_bandwidth < 0. )
	{ 
		debugger << "clamping bandwidth at 0." << endl;
		i_bandwidth = 0.;
	}

	//	don't alias:
	if ( i_frequency > Pi )
	{ 
		debugger << "fading out aliasing Partial" << endl;
		i_amplitude = 0.;
	}
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
	determ_phase = ph;
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
	const double dFreq = (targetFreq - i_frequency) * dTime;
	const double dAmp = (targetAmp - i_amplitude)  * dTime;
	const double dBw = (targetBw - i_bandwidth)  * dTime;

	//	could use temporary local variables for speed... nah!
	//	Cannot possibly be worth it when I am computing square roots 
	//	and cosines!
	double am;
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
		am = sqrt( 1. - i_bandwidth ) + ( bwModulator() * sqrt( 2. * i_bandwidth ) );	
				
		//	compute a sample and add it into the buffer:
		*putItHere += am * i_amplitude * cos( determ_phase );
			
		//	update the instantaneous oscillator state:
		determ_phase += i_frequency;	//	frequency is radians per sample
		i_frequency += dFreq;
		i_amplitude += dAmp;
		i_bandwidth += dBw;
		if (i_bandwidth < 0.)
			i_bandwidth = 0.;
			
	}	// end of sample computation loop
	
	//	wrap phase to prevent eventual loss of precision at
	//	high oscillation frequencies:
	//	(Doesn't really matter much exactly how we wrap it, 
	//	as long as it brings the phase nearer to zero.)
	determ_phase = m2pi( determ_phase );
	
	//	set the state variables to their target values,
	//	just in case they didn't arrive exactly (overshooting
	//	amplitude or, especially, bandwidth, could be bad, and
	//	it does happen):
	i_frequency = targetFreq;
	i_amplitude = targetAmp;
	i_bandwidth = targetBw;
}

}	//	end of namespace Loris

/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2004 by Kelly Fitz and Lippold Haken
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
 * Breakpoint.C
 *
 * Implementation of class Loris::Breakpoint.
 *
 * Kelly Fitz, 16 Aug 99
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include "config.h"
#endif

#include "Breakpoint.h"
#include "Exception.h"
#include <cmath>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	class Breakpoint
//
//!	@class Breakpoint Breakpoint.h loris/Breakpoint.h
//!	
//!	Class Breakpoint represents a single breakpoint in the
//!	Partial parameter (frequency, amplitude, bandwidth) envelope.
//!	Instantaneous phase is also stored, but is only used at the onset of 
//!	a partial, or when it makes a transition from zero to nonzero amplitude.
//!	
//!	Loris Partials represent reassigned bandwidth-enhanced model components.
//!	A Partial consists of a chain of Breakpoints describing the time-varying
//!	frequency, amplitude, and bandwidth (noisiness) of the component.
//!	For more information about Reassigned Bandwidth-Enhanced 
//!	Analysis and the Reassigned Bandwidth-Enhanced Additive Sound 
//!	Model, refer to the Loris website: 
//!	www.cerlsoundgroup.org/Loris/.
//!	
//!	Breakpoint is a leaf class, do not subclass.
//


#pragma mark -
#pragma mark construction

// ---------------------------------------------------------------------------
//	Breakpoint default constructor
// ---------------------------------------------------------------------------
//!	Construct a new Breakpoint with all parameters initialized to 0.
Breakpoint::Breakpoint( void ) :
	_frequency( 0. ),
	_amplitude( 0. ),
	_bandwidth( 0. ),
	_phase( 0. )
{
}

// ---------------------------------------------------------------------------
//	Breakpoint constructor
// ---------------------------------------------------------------------------
//!	Construct a new Breakpoint with the specified parameters.
//!	
//!	@param f is the intial frequency.
//!	@param a is the initial amplitude.
//!	@param b is the initial bandwidth.
//!	@param p is the initial phase, if specified (if unspecified, 0 
//!	is assumed).
//
Breakpoint::Breakpoint( double f, double a, double b, double p ) :
	_frequency( f ),
	_amplitude( a ),
	_bandwidth( b ),
	_phase( p )
{
}

// ---------------------------------------------------------------------------
//	operator==
// ---------------------------------------------------------------------------
//!	Comparison operator: return true if the frequency, amplitude, 
//!	bandwidth, and phase of this Breakpoint are equal to those of
//!	the Breakpoint rhs. Otherwise, return false.
//!	
//!	@param rhs is the Breakpoint to comparebool 
//
bool
Breakpoint::operator==( const Breakpoint & rhs ) const
{
	return _frequency == rhs._frequency &&
			_amplitude == rhs._amplitude &&
			_bandwidth == rhs._bandwidth &&
			_phase == rhs._phase;
}

// ---------------------------------------------------------------------------
//	addNoiseEnergy
// ---------------------------------------------------------------------------
//!	Add noise (bandwidth) energy to this Breakpoint by computing new 
//!	amplitude and bandwidth values. enoise may be negative, but 
//!	noise energy cannot be removed (negative energy added) in excess 
//!	of the current noise energy.
//!	
//!	@param enoise is the amount of noise energy to add to
//!	this Breakpoint.
//
void 
Breakpoint::addNoiseEnergy( double enoise )
{
	//	compute current energies:
	double e = amplitude() * amplitude();	//	current total energy
	double n = e * bandwidth();				//	current noise energy
	
	//	Assert( e >= n );
	//	could complain, but its recoverable, just fix it:
	if ( e < n )
		e = n;
	
	//	guard against divide-by-zero, and don't allow
	//	the sinusoidal energy to decrease:
	if ( n + enoise > 0. ) 
	{
		//	if new noise energy is positive, total
		//	energy must also be positive:
		//	Assert( e + enoise > 0 );
		setBandwidth( (n + enoise) / (e + enoise) );
		setAmplitude( std::sqrt(e + enoise) );
	}
	else 
	{
		//	if new noise energy is negative, leave 
		//	all sinusoidal energy:
		setBandwidth( 0. );
		setAmplitude( std::sqrt( e - n ) );
	}
}

}	//	end of namespace Loris





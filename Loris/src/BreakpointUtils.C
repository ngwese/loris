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
 * BreakpointUtils.C
 *
 * Breakpoint utility functions collected in namespace BreakpointUtils.
 *
 * Kelly Fitz, 23 May 2002
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#if HAVE_CONFIG_H
	#include <config.h>
#endif

#include <BreakpointUtils.h>
#include <cmath>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	addNoise
// ---------------------------------------------------------------------------
//	Add noise (bandwidth) energy to a Breakpoint by computing new 
//	amplitude and bandwidth values. enoise may be negative, but 
//	noise energy cannot be removed (negative energy added) in excess 
//	of the current noise energy.
//
void 
BreakpointUtils::addNoiseEnergy( Breakpoint & bp, double enoise )
{
	//	compute current energies:
	double e = bp.amplitude() * bp.amplitude();	//	current total energy
	double n = e * bp.bandwidth();				//	current noise energy
	
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
		bp.setBandwidth( (n + enoise) / (e + enoise) );
		bp.setAmplitude( std::sqrt(e + enoise) );
	}
	else 
	{
		//	if new noise energy is negative, leave 
		//	all sinusoidal energy:
		bp.setBandwidth( 0. );
		bp.setAmplitude( std::sqrt( e - n ) );
	}
}

}	//	end of namespace Loris





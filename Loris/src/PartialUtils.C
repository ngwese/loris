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
 * PartialUtils.C
 *
 * Partial utility functions collected in namespace PartialUtils.
 *
 * Kelly Fitz, 17 June 2003
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include <config.h>
#endif

#include <PartialUtils.h>

#include <Envelope.h>
#include <Partial.h>

#include <cmath>
#include <functional>
#include <utility>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	scale_amp::operator()
// ---------------------------------------------------------------------------
//	Scale the amplitude of the specified Partial according to
//	an envelope representing a time-varying amplitude scale value.
//
void 
PartialUtils::scale_amp::operator()( Partial & p ) const
{
	for ( Partial::iterator pos = p.begin(); pos != p.end(); ++pos ) 
	{		
		pos.breakpoint().setAmplitude( pos.breakpoint().amplitude() * 
									   env.valueAt(pos.time()) );
	}	
}

// ---------------------------------------------------------------------------
//	scale_noise_ratio::operator()
// ---------------------------------------------------------------------------
//	Scale the relative noise content of the specified Partial according 
//	to an envelope representing a (time-varying) noise energy 
//	scale value.
//
void 
PartialUtils::scale_noise_ratio::operator()( Partial & p ) const
{
	for ( Partial::iterator pos = p.begin(); pos != p.end(); ++pos ) 
	{		
		//	compute new bandwidth value:
		double bw = pos.breakpoint().bandwidth();
		if ( bw < 1. ) 
		{
			double ratio = bw  / (1. - bw);
			ratio *= env.valueAt(pos.time());
			bw = ratio / (1. + ratio);
		}
		else 
		{
			bw = 1.;
		}
		
		pos.breakpoint().setBandwidth( bw );
	}	
}

// ---------------------------------------------------------------------------
//	shift_pitch::operator()
// ---------------------------------------------------------------------------
//	Shift the pitch of the specified Partial according to 
//	the given pitch envelope. The pitch envelope is assumed to have 
//	units of cents (1/100 of a halfstep).
//
void 
PartialUtils::shift_pitch::operator()( Partial & p ) const
{
	for ( Partial::iterator pos = p.begin(); pos != p.end(); ++pos ) 
	{		
		//	compute frequency scale:
		double scale = std::pow(2., (0.01 * env.valueAt(pos.time())) /12.);				
		pos.breakpoint().setFrequency( pos.breakpoint().frequency() * scale );
	}	
}

}	//	end of namespace Loris


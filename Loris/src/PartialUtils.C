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

#include <Breakpoint.h>
#include <Envelope.h>
#include <Partial.h>

#include <cmath>
#include <functional>
#include <utility>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	crop::operator()
// ---------------------------------------------------------------------------
//	Trim a Partial by removing Breakpoints outside a specified time span.
//	Insert a Breakpoint at the boundary when cropping occurs.
//
void
PartialUtils::crop::operator()( Partial & p ) const
{
	//	crop beginning of Partial
	Partial::iterator it = p.findAfter( minTime );
	if ( it != p.begin() )
	{
		Breakpoint bp = p.parametersAt( minTime );
		it = p.insert( minTime, bp );
		it = p.erase( p.begin(), it );
	}
	
	//	crop end of Partial
	it = p.findAfter( maxTime );
	if ( it != p.end() )
	{
		Breakpoint bp = p.parametersAt( maxTime );
		it = p.insert( maxTime, bp );
		it = p.erase( ++it, p.end() );
	}
}

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
//	scale_bandwidth::operator()
// ---------------------------------------------------------------------------
//	Scale the bandwidth of the specified Partial according to
//	an envelope representing a time-varying bandwidth scale value.
//
void 
PartialUtils::scale_bandwidth::operator()( Partial & p ) const
{
	for ( Partial::iterator pos = p.begin(); pos != p.end(); ++pos ) 
	{		
		pos.breakpoint().setBandwidth( pos.breakpoint().bandwidth() * 
									   env.valueAt(pos.time()) );
	}	
}

// ---------------------------------------------------------------------------
//	scale_frequency::operator()
// ---------------------------------------------------------------------------
//	Scale the frequency of the specified Partial according to
//	an envelope representing a time-varying frequency scale value.
//
void 
PartialUtils::scale_frequency::operator()( Partial & p ) const
{
	for ( Partial::iterator pos = p.begin(); pos != p.end(); ++pos ) 
	{		
		pos.breakpoint().setFrequency( pos.breakpoint().frequency() * 
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

// ---------------------------------------------------------------------------
//	shift_time::operator()
// ---------------------------------------------------------------------------
//	Shift the time of all the Breakpoints in a Partial by a constant amount.
//
void 
PartialUtils::shift_time::operator()( Partial & p ) const
{
	Partial result;
	result.setLabel( p.label() );
	
	for ( Partial::iterator pos = p.begin(); pos != p.end(); ++pos ) 
	{		
		result.insert( pos.time() + offset, pos.breakpoint() );
	}	
	p = result;
}

}	//	end of namespace Loris


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
 * PartialUtils.C
 *
 *	A group of Partial utility function objects for use with STL 
 *	searching and sorting algorithms. PartialUtils is a namespace
 *	within the Loris namespace.
 *
 * Kelly Fitz, 17 June 2003
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include "config.h"
#endif

#include "PartialUtils.h"

#include "Breakpoint.h"
#include "BreakpointEnvelope.h"
#include "Envelope.h"
#include "Partial.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <utility>

//	begin namespace
namespace Loris {

namespace PartialUtils {


#pragma mark -- base class --

// ---------------------------------------------------------------------------
//	PartialMutator constructor from double
// ---------------------------------------------------------------------------
PartialMutator::PartialMutator( double x ) : 
	env( new BreakpointEnvelope( x ) ) 
{
}
	
// ---------------------------------------------------------------------------
//	PartialMutator constructor from envelope
// ---------------------------------------------------------------------------
PartialMutator::PartialMutator( const Envelope & e ) : 
	env( e.clone() ) 
{
}

// ---------------------------------------------------------------------------
//	PartialMutator copy constructor
// ---------------------------------------------------------------------------
PartialMutator::PartialMutator( const PartialMutator & rhs ) : 
	env( rhs.env->clone() ) 
{
}

// ---------------------------------------------------------------------------
//	PartialMutator destructor
// ---------------------------------------------------------------------------
PartialMutator::~PartialMutator( void )
{
	delete env;
}

// ---------------------------------------------------------------------------
//	PartialMutator assignment operator
// ---------------------------------------------------------------------------
PartialMutator &
PartialMutator::operator=( const PartialMutator & rhs )
{
	if ( this != &rhs )
	{	
		delete env;
		env = rhs.env->clone();
	}
	return *this;
}
	
#pragma mark -- amplitude scaling --
	
// ---------------------------------------------------------------------------
//	AmplitudeScaler function call operator
// ---------------------------------------------------------------------------
//	Scale the amplitude of the specified Partial according to
//	an envelope representing a time-varying amplitude scale value.
//
void 
AmplitudeScaler::operator()( Partial & p ) const
{
	for ( Partial::iterator pos = p.begin(); pos != p.end(); ++pos ) 
	{		
		pos.breakpoint().setAmplitude( pos.breakpoint().amplitude() * 
									          env->valueAt( pos.time() ) );
	}	
}

// ---------------------------------------------------------------------------
//	BandwidthScaler function call operator
// ---------------------------------------------------------------------------
//	Scale the bandwidth of the specified Partial according to
//	an envelope representing a time-varying bandwidth scale value.
//
void 
BandwidthScaler::operator()( Partial & p ) const
{
	for ( Partial::iterator pos = p.begin(); pos != p.end(); ++pos ) 
	{		
		pos.breakpoint().setBandwidth( pos.breakpoint().bandwidth() * 
									   env->valueAt( pos.time() ) );
	}	
}

// ---------------------------------------------------------------------------
//	FrequencyScaler function call operator
// ---------------------------------------------------------------------------
//	Scale the frequency of the specified Partial according to
//	an envelope representing a time-varying frequency scale value.
//
void 
FrequencyScaler::operator()( Partial & p ) const
{
	for ( Partial::iterator pos = p.begin(); pos != p.end(); ++pos ) 
	{		
		pos.breakpoint().setFrequency( pos.breakpoint().frequency() * 
									   env->valueAt( pos.time() ) );
	}	
}

// ---------------------------------------------------------------------------
//	NoiseRatioScaler function call operator
// ---------------------------------------------------------------------------
//	Scale the relative noise content of the specified Partial according 
//	to an envelope representing a (time-varying) noise energy 
//	scale value.
//
void 
NoiseRatioScaler::operator()( Partial & p ) const
{
	for ( Partial::iterator pos = p.begin(); pos != p.end(); ++pos ) 
	{		
		//	compute new bandwidth value:
		double bw = pos.breakpoint().bandwidth();
		if ( bw < 1. ) 
		{
			double ratio = bw  / (1. - bw);
			ratio *= env->valueAt( pos.time() );
			bw = ratio / ( 1. + ratio );
		}
		else 
		{
			bw = 1.;
		}		
		pos.breakpoint().setBandwidth( bw );
	}	
}

// ---------------------------------------------------------------------------
//	PitchShifter function call operator
// ---------------------------------------------------------------------------
//	Shift the pitch of the specified Partial according to 
//	the given pitch envelope. The pitch envelope is assumed to have 
//	units of cents (1/100 of a halfstep).
//
void 
PitchShifter::operator()( Partial & p ) const
{
	for ( Partial::iterator pos = p.begin(); pos != p.end(); ++pos ) 
	{		
		//	compute frequency scale:
		double scale = 
			std::pow( 2., ( 0.01 * env->valueAt( pos.time() ) ) / 12. );				
		pos.breakpoint().setFrequency( pos.breakpoint().frequency() * scale );
	}	
}

// ---------------------------------------------------------------------------
//	Cropper function call operator
// ---------------------------------------------------------------------------
//	Trim a Partial by removing Breakpoints outside a specified time span.
//	Insert a Breakpoint at the boundary when cropping occurs.
//
void 
Cropper::operator()( Partial & p ) const
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
//	TimeShifter function call operator
// ---------------------------------------------------------------------------
//	Shift the time of all the Breakpoints in a Partial by a constant amount.
//
void 
TimeShifter::operator()( Partial & p ) const
{
	//	Since the Breakpoint times are immutable, the only way to 
	//	shift the Partial in time is to construct a new Partial and
	//	assign it to the argument p.
	Partial result;
	result.setLabel( p.label() );
	
	for ( Partial::iterator pos = p.begin(); pos != p.end(); ++pos ) 
	{		
		result.insert( pos.time() + offset, pos.breakpoint() );
	}	
	p = result;
}

}	//	end of namespace PartialUtils

}	//	end of namespace Loris


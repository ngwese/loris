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
 * BreakpointEnvelope.C
 *
 * Implementation of class BreakpointEnvelope.
 *
 * Kelly Fitz, 8 Aug 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include "config.h"
#endif

#include "BreakpointEnvelope.h"
#include "Notifier.h"

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	constructor
// ---------------------------------------------------------------------------
//
BreakpointEnvelope::BreakpointEnvelope(void)
{
}

// ---------------------------------------------------------------------------
//	constructor with initial (or constant) value
// ---------------------------------------------------------------------------
//
BreakpointEnvelope::BreakpointEnvelope( double initialValue )
{
	insertBreakpoint( 0., initialValue );
}

// ---------------------------------------------------------------------------
//	clone
// ---------------------------------------------------------------------------
//
BreakpointEnvelope * 
BreakpointEnvelope::clone( void ) const
{
	return new BreakpointEnvelope( *this );
}

// ---------------------------------------------------------------------------
//	insertBreakpoint
// ---------------------------------------------------------------------------
//	Insert or replace a breakpoint at x.
//	
void
BreakpointEnvelope::insertBreakpoint( double x, double y )
{
	(*this)[x] = y;
}

// ---------------------------------------------------------------------------
//	valueAt
// ---------------------------------------------------------------------------
//	BreakpointEnvelope isa map<double, double>, so iterators are references to
//	key/value pairs.
//
double
BreakpointEnvelope::valueAt( double x ) const
{
	//	return zero if no breakpoints have been specified:
	if ( size() == 0 ) 
	{
		return 0.;
	}

	const_iterator it = lower_bound( x );

	if ( it == begin() ) 
	{
		//	x is less than the first breakpoint, extend:
		return it->second;
	}
	else if ( it == end() ) 
	{
		//	x is greater than the last breakpoint, extend:
		// 	(no direct way to access the last element of a map)
		return (--it)->second;
	}
	else 
	{
		//	linear interpolation between consecutive breakpoints:
		double xgreater = it->first;
		double ygreater = it->second;
		--it;
		double xless = it->first;
		double yless = it->second;
		
		double alpha = (x -  xless) / (xgreater - xless);
		return ( alpha * ygreater ) + ( (1. - alpha) * yless );
	}
}

}	//	end of namespace Loris

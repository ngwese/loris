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
//	class BreakpointEnvelope
//
//!	@class BreakpointEnvelope BreakpointEnvelope.h loris/BreakpointEnvelope.h
//!	
//!	A BreakpointEnvelope represents a linear segment breakpoint 
//!	function with infinite extension at each end (that is, evalutaing 
//!	the envelope past either end of the breakpoint function yields the 
//!	value at the nearest end point).
//!
//!	BreakpointEnvelope implements the Envelope interface, described
//!	by the abstract class Envelope. 
//!
//!	BreakpointEnvelope inherits the types
//!		@li @c size_type
//!		@li @c value_type
//!		@li @c iterator
//!		@li @c const_iterator
//!
//!	and the member functions
//!		@li <tt>size_type size( void ) const</tt>
//!		@li <tt>bool empty( void ) const</tt>
//!		@li <tt>iterator begin( void )</tt>
//!		@li <tt>const_iterator begin( void ) const</tt>
//!		@li <tt>iterator end( void )</tt>
//!		@li <tt>const_iterator end( void ) const</tt>
//!
//!	from <tt>std::map< double, double ></tt>.
//

// ---------------------------------------------------------------------------
//	constructor
// ---------------------------------------------------------------------------
//!	Construct a new BreakpointEnvelope having no 
//!	breakpoints (and an implicit value of 0 everywhere).		
//
BreakpointEnvelope::BreakpointEnvelope(void)
{
}

// ---------------------------------------------------------------------------
//	constructor with initial (or constant) value
// ---------------------------------------------------------------------------
//!	Construct and return a new BreakpointEnvelope having a 
//!	single breakpoint at 0 (and an implicit value everywhere)
//!	of initialValue.		
//!	
//!	@param initialValue is the value of this BreakpointEnvelope
//!	at time 0.	
//
BreakpointEnvelope::BreakpointEnvelope( double initialValue )
{
	insertBreakpoint( 0., initialValue );
}

// ---------------------------------------------------------------------------
//	clone
// ---------------------------------------------------------------------------
//!	Return an exact copy of this BreakpointEnvelope
//!	(polymorphic copy, following the Prototype pattern).
//
BreakpointEnvelope * 
BreakpointEnvelope::clone( void ) const
{
	return new BreakpointEnvelope( *this );
}

// ---------------------------------------------------------------------------
//	insert
// ---------------------------------------------------------------------------
//!	Insert a breakpoint representing the specified (time, value) 
//!	pair into this BreakpointEnvelope. If there is already a 
//!	breakpoint at the specified time, it will be replaced with 
//!	the new breakpoint.
//!	
//!	@param time is the time at which to insert a new breakpoint
//!	@param value is the value of the new breakpoint
//	
void
BreakpointEnvelope::insert( double time, double value )
{
	(*this)[time] = value;
}

// ---------------------------------------------------------------------------
//	valueAt
// ---------------------------------------------------------------------------
//!	Return the linearly-interpolated value of this BreakpointEnvelope at 
//!	the specified time.
//!	
//!	@param t is the time at which to evaluate this 
//!	BreakpointEnvelope.
//
double
BreakpointEnvelope::valueAt( double t ) const
{
	//	return zero if no breakpoints have been specified:
	if ( size() == 0 ) 
	{
		return 0.;
	}

	const_iterator it = lower_bound( t );

	if ( it == begin() ) 
	{
		//	t is less than the first breakpoint, extend:
		return it->second;
	}
	else if ( it == end() ) 
	{
		//	t is greater than the last breakpoint, extend:
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
		
		double alpha = (t -  xless) / (xgreater - xless);
		return ( alpha * ygreater ) + ( (1. - alpha) * yless );
	}
}

}	//	end of namespace Loris

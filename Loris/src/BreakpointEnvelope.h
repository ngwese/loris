#ifndef INCLUDE_BREAKPOINTENVELOPE_H
#define INCLUDE_BREAKPOINTENVELOPE_H
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
 * BreakpointEnvelope.h
 *
 * Definition of class BreakpointEnvelope.
 *
 * Kelly Fitz, 21 July 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include<Envelope.h>
#include <map>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	class BreakpointEnvelope
//
//	A BreakpointEnvelope represents a linear segment breakpoint 
//	function with infinite extension at each end (that is, evalutaing 
//	the envelope past either end of the breakpoint function yields the 
//	value at the nearest end point).
//
//	BreakpointEnvelope implements the Envelope interface, described
//	by the abstract class Envelope. 
//
class BreakpointEnvelope : public Envelope, private std::map< double, double >
{
//	-- public interface --
public:
//	-- construction --
	BreakpointEnvelope( void );
	/*	Construct and return a new BreakpointEnvelope having no 
		breakpoints (and an implicit value of 0. everywhere).			
	 */
	explicit BreakpointEnvelope( double initialValue );
	/*	Construct and return a new BreakpointEnvelope having a 
		single breakpoint at 0. (and an implicit value everywhere)
		of initialValue.			
	 */

	//	compiler-generated copy, assignment, and destruction are OK.
	
//	-- Envelope interface --
	virtual BreakpointEnvelope * clone( void ) const;
	/*	Return an exact copy of this BreakpointEnvelope
		(following the Prototype pattern).
	 */

	virtual double valueAt( double x ) const;	
	/*	Return the linearly-interpolated value of this BreakpointEnvelope at 
		the specified time.							
	 */
	
//	-- envelope composition --
	void insertBreakpoint( double time, double value );
	/*	Insert a breakpoint representing the specified (time, value) 
		pair into this BreakpointEnvelope. If there is already a 
		breakpoint at the specified time, it will be replaced with 
		the new breakpoint.
	 */
	 
	//	import some interface from std::map:
	using std::map< double, double >::size;
	using std::map< double, double >::empty;
	using std::map< double, double >::begin;
	using std::map< double, double >::end;
	using std::map< double, double >::size_type;
	using std::map< double, double >::value_type;
	using std::map< double, double >::iterator;
	using std::map< double, double >::const_iterator;

};	//	end of class BreakpointEnvelope

}	//	end of namespace Loris

#endif /* ndef INCLUDE_BREAKPOINTENVELOPE_H */

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
 *	lorisBpEnvelope_pi.i
 *
 *	SWIG interface file supporting the Loris procedural inteface
 *	declared in loris.h. Specifically, this interface file describes
 *	the procedural interface for the BreakpointEnvelope class, implemented
 *	in lorisBpEnvelope_pi.C. Include this file in loris.i to include
 *	the BreakpointEnvelope procedural interface in the scripting module.
 *
 *	The SWIG interface (loris.i) uses the class interface in 
 *	lorisBpEnvelope.i rather than this more cumbersome procedural 
 *	interface, so this file is no longer used.
 *
 *
 * Kelly Fitz, 13 Nov 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

/* ---------------------------------------------------------------- */
/*		BreakpointEnvelope object interface								
/*
 */
%section "PartialList object interface"
%text %{
	A BreakpointEnvelope represents a linear segment breakpoint 
	function with infinite extension at each end (that is, the 
	values past either end of the breakpoint function have the 
	values at the nearest end).
%}
 
BreakpointEnvelope * createBreakpointEnvelope( void );
/*	Construct and return a new BreakpointEnvelope having no 
	breakpoints (and an implicit value of 0. everywhere).			
 */

void destroyBreakpointEnvelope( BreakpointEnvelope * ptr_this );
/*	Destroy this BreakpointEnvelope. 								
 */
 
void breakpointEnvelope_insertBreakpoint( BreakpointEnvelope * ptr_this,
					double time, double val );
/*	Insert a breakpoint representing the specified (time, value) 
	pair into this BreakpointEnvelope. If there is already a 
	breakpoint at the specified time, it will be replaced with 
	the new breakpoint.
 */

double breakpointEnvelope_valueAt( BreakpointEnvelope * ptr_this, 
								   double time );
/*	Return the interpolated value of this BreakpointEnvelope at the 
	specified time.							
 */


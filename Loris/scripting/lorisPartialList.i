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
 *	lorisPartialList.i
 *
 *	SWIG interface file describing the PartialList class.
 *	A PartialList is a std::list< Loris::Partial >.
 *	Include this file in loris.i to include the PartialList class
 *	interface in the scripting module. (Can be used with the 
 *	-shadow option to SWIG to build an Analyzer class in the 
 *	Python interface.) This file does not support exactly the 
 *	public interface of the C++ std::list class, but has been 
 *	modified to better support SWIG and scripting languages.
 *
 *
 * Kelly Fitz, 17 Nov 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

%{
#include "Partial.h"
#include <list>
typedef std::list< Loris::Partial > PartialList;
%}

// ---------------------------------------------------------------------------
//	class PartialList
//	
class PartialList
/*	A PartialList represents a collection of Bandwidth-Enhanced 
	Partials, each having a trio of synchronous, non-uniformly-
	sampled breakpoint envelopes representing the time-varying 
	frequency, amplitude, and noisiness of a single bandwidth-
	enhanced sinusoid.

	For more information about Bandwidth-Enhanced Partials and the  
	Reassigned Bandwidth-Enhanced Additive Sound Model, refer to
	the Loris website: www.cerlsoundgroup.org/Loris/
*/
{
public:
	PartialList( void );
	/*	Return a new empty PartialList.
	 */

	~PartialList( void );
	/*	Destroy this PartialList.
	 */

	void clear( void );
	/*	Remove (and destroy) all the Partials from this PartialList,
		leaving it empty.
	 */
	
	unsigned long size( void );
	/*	Return the number of Partials in this PartialList.
	 */
	
%addmethods
{
	%new PartialList * copy( void )
	{
		return new PartialList( *self );
	}
	/*	Return a new PartialList that is a copy of this 
		PartialList (i.e. has identical Partials).
	 */
	 
	void splice( PartialList & other )
	{
		if ( self == &other )
		{
			std::string s( "Cannot splice a PartialList onto itself!" );
			throw s;
		}
		self->splice( self->end(), other );
	}
	/*	Splice all the Partials in the other PartialList onto the end of
		this PartialList, leaving the other empty.
	 */
}

};	//	end of (SWIG) class PartialList

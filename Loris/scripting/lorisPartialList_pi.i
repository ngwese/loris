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
 *	lorisPartialList_pi.i
 *
 *	SWIG interface file supporting the Loris procedural inteface
 *	declared in loris.h. Specifically, this interface file describes
 *	the procedural interface for the PartialList class, implemented
 *	in lorisPartialList_pi.C. Include this file in loris.i to include
 *	the PartialList procedural interface in the scripting module.
 *
 *	The SWIG interface (loris.i) uses the class interface in 
 *	lorisPartialList.i rather than this more cumbersome procedural 
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
/*		PartialList object interface
/*
 */
%section "PartialList object interface"
%text %{
	A PartialList represents a collection of Bandwidth-Enhanced 
	Partials, each having a trio of synchronous, non-uniformly-
	sampled breakpoint envelopes representing the time-varying 
	frequency, amplitude, and noisiness of a single bandwidth-
	enhanced sinusoid.

	For more information about Bandwidth-Enhanced Partials and the  
	Reassigned Bandwidth-Enhanced Additive Sound Model, refer to
	the Loris website: www.cerlsoundgroup.org/Loris/
%}

PartialList * createPartialList( void );
/*	Return a new empty PartialList.
 */
void destroyPartialList( PartialList * ptr_this );
/*	Destroy this PartialList.
 */
void partialList_clear( PartialList * ptr_this );
/*	Remove (and destroy) all the Partials from this PartialList,
	leaving it empty.
 */
void partialList_copy( PartialList * ptr_this, 
					   const PartialList * src );
/*	Make this PartialList a copy of the source PartialList by making
	copies of all of the Partials in the source and adding them to 
	this PartialList.
 */
unsigned long partialList_size( const PartialList * ptr_this );
/*	Return the number of Partials in this PartialList.
 */
void partialList_splice( PartialList * ptr_this, 
						 PartialList * src );
/*	Splice all the Partials in the source PartialList onto the end of
	this PartialList, leaving the source empty.
 */


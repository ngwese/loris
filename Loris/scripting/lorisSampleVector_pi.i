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
 *	lorisSampleVector_pi.i
 *
 *	SWIG interface file supporting the Loris procedural inteface
 *	declared in loris.h. Specifically, this interface file describes
 *	the procedural interface for the SampleVector class, implemented
 *	in lorisSampleVector_pi.C. Include this file in loris.i to include
 *	the SampleVector procedural interface in the scripting module.
 *
 *	The SWIG interface (loris.i) uses the class interface in 
 *	lorisSampleVector.i rather than this more cumbersome procedural 
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
/*		SampleVector object interface								
/*
 */
%section "SampleVector object interface"
%text %{
	A SampleVector represents a vector of floating point samples of
	an audio waveform. In Loris, the samples are assumed to have 
	values on the range (-1., 1.) (though this is not enforced or 
	checked). 
%}
 
SampleVector * createSampleVector( unsigned long size = 0 );
/*	Return a new SampleVector having the specified number of samples,
	all of whose value is 0. 
	
	If no size is specified, 0 is assumed.
 */
void destroySampleVector( SampleVector * ptr_this );
/*	Destroy this SampleVector.
 */
void sampleVector_copy( SampleVector * ptr_this, 
						const SampleVector * src );
/*	Make this SampleVector a copy of the source SampleVector, having 
	the same number of samples, and samples at every position in this
	SampleVector having the same value as the corresponding sample in
	the source.
 */
double sampleVector_getAt( const SampleVector * ptr_this, unsigned long idx );
/*	Return the value of the sample at the given position (index) in
	this SampleVector.
 */
void sampleVector_setAt( SampleVector * ptr_this, unsigned long idx, double x );
/*	Set the value of the sample at the given position (index) in
	this SampleVector.
 */
unsigned long sampleVector_getLength( const SampleVector * ptr_this );
/*	Return the number of samples represented by this SampleVector.
 */
void sampleVector_setLength( SampleVector * ptr_this, unsigned long size );
/*	Change the number of samples represented by this SampleVector. If
	the given size is greater than the current size, all new samples 
	will have value 0. If the given size is less than the current 
	size, then samples in excess of the given size are removed.
 */


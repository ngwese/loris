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
 *	lorisSampleVector.i
 *
 *	SWIG interface file describing the SampleVector class.
 *	A SampleVector is a std::vector< double >.
 *	Include this file in loris.i to include the SampleVector class
 *	interface in the scripting module. (Can be used with the 
 *	-shadow option to SWIG to build a SampleVector class in the 
 *	Python interface.) This file does not support exactly the 
 *	public interface of the C++ std::vector class, but has been 
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
#include <vector>
typedef std::vector< double > SampleVector;
%}

// ---------------------------------------------------------------------------
//	class SampleVector
//	
class SampleVector
/*	A SampleVector represents a vector of floating point samples of
	an audio waveform. In Loris, the samples are assumed to have 
	values on the range (-1., 1.) (though this is not enforced or 
	checked). 
*/
{
public:
	SampleVector( unsigned long size = 0 );
	/*	Return a new SampleVector having the specified number of samples,
		all of whose value is 0. 
		
		If no size is specified, 0 is assumed.
	 */
	 
	 ~SampleVector( void );
	 /*	Destroy this SampleVector.
	  */

	unsigned long size( void );
	/*	Return the number of samples represented by this SampleVector.
	 */

	void resize( unsigned long size );
	/*	Change the number of samples represented by this SampleVector. If
		the given size is greater than the current size, all new samples 
		will have value 0. If the given size is less than the current 
		size, then samples in excess of the given size are removed.
	 */
	 
	 void clear( void );
	 /*	Remove all samples from this SampleVector, and set its
	 	size to zero.
	   */

%addmethods
{
	double getAt( unsigned long idx )
	{
		// return self->at(idx);	//	g++ doesn't implement at()?
		if ( idx >= self->size() )
			throw std::out_of_range("vector::at index out of range");
		return (*self)[idx];
	}
	/*	Return the value of the sample at the given position (index) in
		this SampleVector.
	 */
	void setAt( unsigned long idx, double x )
	{
		// self->at(idx) = x;	//	g++ doesn't implement at()?
		if ( idx >= self->size() )
			throw std::out_of_range("vector::at index out of range");
		(*self)[idx] = x;
	}
	/*	Set the value of the sample at the given position (index) in
		this SampleVector.
	 */
}

};	//	end of (SWIG) class SampleVector

//	define a copy constructor:
//	(this should give the right documentation, the 
//	right ownership, the right function name in the
//	module, etc.)
%{
SampleVector * SampleVectorCopy_( const SampleVector * other )
{
	return new SampleVector( *other );
}
%}

%name( SampleVectorCopy )  
%new SampleVector * SampleVectorCopy_( const SampleVector * other );
/*	Return a new SampleVector that is a copy of this 
	SampleVector, having the same number of samples, 
	and samples at every position in the copy having 
	the same value as the corresponding sample in
	this SampleVector.
 */

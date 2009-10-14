/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2009 by Kelly Fitz and Lippold Haken
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
 *  lorisSynthesizer.i
 *
 *  SWIG interface file describing classes associated with Loris bandwidth-
 *  enhanced sinusoidal synthesis, including the Synthesizer, Oscillator,
 *  and Filter classes. Include this file in loris.i to include thse classes 
 *  in the scripting module. 
 *
 * Kelly Fitz, 13 Oct 2009
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 
/* ***************** inserted C++ code ***************** */
%{

#include <Filter.h>
#include <Oscillator.h>
#include <Synthesizer.h>

using Loris::Filter;
using Loris::Oscillator;

%}
/* ***************** end of inserted C++ code ***************** */

// ---------------------------------------------------------------------------
//  class Filter

%feature("docstring",
"Filter is an Direct Form II realization of a filter specified
by its difference equation coefficients and (optionally) gain,  
applied to the filter output (defaults to 1.). Coefficients are
specified and stored in order of increasing delay.

    Implements the rational transfer function

                              -1               -nb
                  b[0] + b[1]z  + ... + b[nb] z
        Y(z) = G ---------------------------------- X(z)
                              -1               -na
                  a[0] + a[1]z  + ... + a[na] z

    where b[k] are the feed forward coefficients, and a[k] are the feedback 
coefficients. If a[0] is not 1, then both a and b are normalized by a[0].
G is the additional filter gain, and is unity if unspecified.


Filter is implemented using a std::deque to store the filter state, 
and relies on the efficiency of that class. If deque is not implemented
using some sort of circular buffer (as it should be -- deque is guaranteed
to be efficient for repeated insertion and removal at both ends), then
this filter class will be slow.") Filter;

class Filter
{
public:

//  --- lifecycle ---


%extend
{

%feature("docstring",
"Initialize a Filter having the specified coefficients, and
order equal to the larger of the two coefficient ranges.
Coefficients in the sequences are stored in increasing order
(lowest order coefficient first).

If template members are allowed, then the coefficients
can be stored in any kind of iterator range, otherwise,
they must be in an array of doubles.

    ffwdcoefs is a sequence of feed-forward coefficients
    fbackcoefs is a sequence of feedback coefficients
    gain is an optional gain scale applied to the filtered signal
") Filter;

    Filter( const std::vector< double > & ffwdcoefs,
            const std::vector< double > & fbackcoefs,
            double gain = 1 )
    {
        return new Filter( &ffwdcoefs[0], &ffwdcoefs[0] + ffwdcoefs.size(),
                           &fbackcoefs[0], &fbackcoefs[0] + fbackcoefs.size(),
                           gain );
    }
    
}

%feature("docstring",
"Make a copy of another digital filter. 
Do not copy the filter state (delay line).") Filter;

    Filter( const Filter & other );
    

%feature("docstring",
"Destroy this Filter.") ~Filter;

    ~Filter( void );

//  --- filtering ---

%feature("docstring",
"Compute a filtered sample from the next input sample.") apply;

    double apply( double input );


//  --- access/mutation ---


%feature("docstring",
"Provide access to the numerator (feed-forward) coefficients
of this filter. The coefficients are stored in order of increasing
delay (lowest order coefficient first).") numerator;
	
	std::vector< double > numerator( void );

    
%feature("docstring",
"Provide access to the denominator (feedback) coefficients
of this filter. The coefficients are stored in order of increasing
delay (lowest order coefficient first).") denominator;
	
	std::vector< double > denominator( void );

	
%feature("docstring",
"Clear the filter state.") clear;

    void clear( void );


};




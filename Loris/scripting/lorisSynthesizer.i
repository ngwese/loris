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
 
/*
	TODO:
	
	No need to provide access to all these classes, just define a 
	parameters structure that is used by all invocations of synthesize().
	
	Possibly allow a new instance of the struct to be created and configured
	and passed to synthesize. 
	
	Params should include fade time, sample rate, filter coefficients, 
	and a flag for whether or not to render with BW enhancement. 
*/


 
/* ***************** inserted C++ code ***************** */
%{

#include <LorisExceptions.h>
#include <Filter.h>
#include <Synthesizer.h>

using Loris::Filter;
using Loris::Oscillator;
using Loris::Synthesizer;

typedef Loris::Synthesizer::Parameters SynthesisParameters;

/*
enum SynthesisEnhancementFlag
{ 
    SinusoidalSynthesis = (int)Loris::Synthesizer::Sinusoidal,  
    BwEnhancedSynthesis = (int)Loris::Synthesizer::BwEnhanced 
};
*/

%}
/* ***************** end of inserted C++ code ***************** */

/* *************** synthesis parameters struct **************** */
/*
//  Both of these methods are supposed to create read-only variables
//  in the scripting interface, but as of SWIG 1.3.39, neither one
//  prevents assigning new values to these 'constants', rendering
//  them useless.
enum 
{ 
    SinusoidalSynthesis,  
    BwEnhancedSynthesis
};

// %constant SinusoidalSynthesis = (int)Loris::Synthesizer::Sinusoidal;  
// %constant BwEnhancedSynthesis = (int)Loris::Synthesizer::BwEnhanced;
*/


class SynthesisParameters
{
public:

    double fadeTime;
    double sampleRate;
    
    %extend
    {
        //  -- filter access and mutation --
        
        std::vector< double > filterCoefsNumerator( void ) 
        {
            return self->filter.numerator();
        }
        
        std::vector< double > filterCoefsDenominator( void )
        {
            return self->filter.denominator();
        }                
        
        void setFilterCoefs( std::vector< double > b, std::vector< double > a )
        {
            if ( 0. == a[0] )
            {
                Throw( InvalidArgument, 
                       "Zeroeth feedback coefficient must be non-zero." );
            }
        
            self->filter.numerator() = b;
            self->filter.denominator() = a;
        }
        
        /*
        //  -- bw enhancement policy access and mutation --
        
        int enhancement( void )
        {
            return self->enhancement;
        }
        
        void setEnhancement( int flag )
        {
            switch (flag)
            {
                case SinusoidalSynthesis:
                    self->enhancement = Loris::Synthesizer::Sinusoidal;
                    break;
                case BwEnhancedSynthesis:
                    self->enhancement = Loris::Synthesizer::BwEnhanced;
                    break;
                default:
                    Throw( Loris::InvalidArgument, 
                        "argument to setEnhancement must be one of SinusoidalSynthesis or BwEnhancedSynthesis" );
            }
        }
        */
    }    
};


%inline
%{
    SynthesisParameters DefaultSynthesisParameters( void )
    {
        return Synthesizer::DefaultParameters();
    }
    
    void SetDefaultSynthesisParameters( const SynthesisParameters & params )
    {
        Synthesizer::SetDefaultParameters( params );
    }
%}



%feature("docstring",
"Synthesize Partials in a PartialList at the given sample rate, and
return the (floating point) samples in a vector. The vector is
sized to hold as many samples as are needed for the complete
synthesis of all the Partials in the PartialList. 

If the samplerate is unspecified, the sample rate in the default 
SynthesisParameters is used.") synthesize;

%newobject synthesize;
%inline 
%{
	std::vector<double> synthesize( const PartialList * partials, double srate )
	{
		std::vector<double> dst;
		try
		{
		    Synthesizer::Parameters params = Synthesizer::DefaultParameters();
		    params.sampleRate = srate;		    
			Synthesizer synth( params, dst );
			synth.synthesize( partials->begin(), partials->end() );
		}
		catch ( std::exception & ex )
		{
			throw_exception( ex.what() );
		}
		return dst;
	}
	
	std::vector<double> synthesize( const PartialList * partials )
	{
		std::vector<double> dst;
		try
		{
			Synthesizer synth( dst );
			synth.synthesize( partials->begin(), partials->end() );
		}
		catch ( std::exception & ex )
		{
			throw_exception( ex.what() );
		}
		return dst;
	}
%}


#if 0

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



// ---------------------------------------------------------------------------
//	class Synthesizer
//

%feature("docstring",
"A Synthesizer renders bandwidth-enhanced Partials into a buffer
of samples. 

Class Synthesizer represents an algorithm for rendering
bandwidth-enhanced Partials as floating point (double) samples at a
specified sampling rate, and accumulating them into a buffer. 

The Synthesizer does not own the sample buffer, the client is responsible
for its construction and destruction, and many Synthesizers may share
a buffer.") Synthesizer;

class Synthesizer
{
public:

%feature("docstring",
"Construct a Synthesizer using the specified sampling rate, sample
buffer (a standard library vector), and Partial
fade time (in seconds). Since Partials generated by the Loris Analyzer
generally begin and end at non-zero amplitude, zero-amplitude
Breakpoints are inserted at either end of the Partial, at a temporal
distance equal to the fade time, to reduce turn-on and turn-off
artifacts. If the fade time is unspecified, the default value of one
millisecond (0.001 seconds) is used.

	srate is the rate (Hz) at which to synthesize samples
		  (must be positive).
	buffer is the buffer into which rendered samples
		   should be accumulated.
	fadeTime is the Partial fade time in seconds (must be non-negative, 
			 default is 0.001).
") Synthesizer;

	Synthesizer( double srate, std::vector<double> & buffer, double fadeTime = .001 );


%feature("docstring",
"Synthesize a bandwidth-enhanced sinusoidal Partial. Zero-amplitude
Breakpoints are inserted at either end of the Partial to reduce
turn-on and turn-off artifacts, as described above. The synthesizer
will resize the buffer as necessary to accommodate all the samples,
including the fade out. Previous contents of the buffer are not
overwritten. Partials with start times earlier than the Partial fade
time will have shorter onset fades. Partials are not rendered at
frequencies above the half-sample rate.") synthesize;

	void synthesize( const Partial & p );
	
%extend
{

%feature("docstring",
"Synthesize all Partials on the specified half-open (STL-style) range.
Null Breakpoints are inserted at either end of the Partial to reduce
turn-on and turn-off artifacts, as described above. The synthesizer
will resize the buffer as necessary to accommodate all the samples,
including the fade outs. Previous contents of the buffer are not
overwritten. Partials with start times earlier than the Partial fade
time will have shorter onset fades.  Partials are not rendered at
frequencies above the half-sample rate. ") synthesize;

	void synthesize( const PartialList & plist )
	{
		self->synthesize( plist.begin(), plist.end() );
	}
	
}

%feature("docstring",
"Return this Synthesizer's Partial fade time, in seconds.") fadeTime;

	double fadeTime( void ) const;

%feature("docstring",
"Return the sampling rate (in Hz) for this Synthesizer.") sampleRate;

	double sampleRate( void ) const;


%feature("docstring",
"Return a reference to the sample buffer used (not
owned) by this Synthesizer.") samples;
	
	std::vector<double> & samples( void );

%feature("docstring",
"Return access to the Filter used by this Synthesizer's 
Oscillator to implement bandwidth-enhanced sinusoidal 
synthesis. (Can use this access to make changes to the
filter coefficients.)") bwEnhancementFilter;

	Filter & bwEnhancementFilter( void );

%feature("docstring",
"Set this Synthesizer's fade time to the specified value 
(in seconds, must be non-negative).

	t is the new Partial fade time in seconds.
") setFadeTime;

	void setFadeTime( double t );

};

#endif
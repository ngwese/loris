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
 * FrequencyReference.C
 *
 * Implementation of class FrequencyReference.
 *
 * Kelly Fitz, 3 Dec 2001
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include "config.h"
#endif

#include "FrequencyReference.h"
#include "Breakpoint.h"
#include "BreakpointEnvelope.h"
#include "Notifier.h"
#include "Partial.h"
#include "PartialList.h"

#include <algorithm>
#include <cmath>

//	begin namespace
namespace Loris {


//	forward declarations for helpers, defined below:
static PartialList::const_iterator
findLongestPartialInFreqRange( PartialList::const_iterator begin, 
							   PartialList::const_iterator end, 
							   double minFreq, double maxFreq );
static void
buildEnvelopeFromPartial( BreakpointEnvelope & env, const Partial & p, long numsamps );							   

// ---------------------------------------------------------------------------
//	construction
// ---------------------------------------------------------------------------
//	Create a reference frequency envelope from the longest Partial found in 
//	a given iterator range and in a specified frequency range. The envelope
//	will have the specified number of samples.
//
FrequencyReference::FrequencyReference( PartialList::const_iterator begin, 
										PartialList::const_iterator end, 
										double minFreq, double maxFreq,
										long numSamps ) :
	_env( new BreakpointEnvelope() )
{
	if ( numSamps < 1 )
		Throw( InvalidArgument, "A frequency reference envelope must have a positive number of samples." );

	//	sanity:
	if ( maxFreq < minFreq )
		std::swap( minFreq, maxFreq );

#ifdef Loris_Debug
	debugger << "Finding frequency reference envelope in range " <<
	debugger << minFreq << " to " << maxFreq << " Hz, from " <<
	debugger << std::distance(begin,end) << " Partials" << std::endl;
#endif

	//	find the longest Partial in the specified frequency range:
	PartialList::const_iterator longest  = 
		findLongestPartialInFreqRange( begin, end, minFreq, maxFreq );
		
	if ( longest == end )
		Throw( InvalidArgument, "No Partials attain their maximum sinusoidal energy within the specified frequency range." );
	
	//	build the Envelope by sampling the longest Partial's frequency
	//	envelope numSamps times:
	double dt = longest->duration() / ( numSamps + 1 );
	for ( long i = 0; i < numSamps; ++i ) 
	{
		double t = longest->startTime() + ((i+1) * dt);
		double f = longest->frequencyAt(t);
		_env->insertBreakpoint( t, f );
	}
}


// ---------------------------------------------------------------------------
//	construction
// ---------------------------------------------------------------------------
//	Create a reference frequency envelope from the longest Partial found in 
//	a given iterator range and in a specified frequency range.
//
//	When the number of envelope samples is not specified, sample the longest
//	Partial's frequency envelope at every Breakpoint.
//
FrequencyReference::FrequencyReference( PartialList::const_iterator begin, 
										PartialList::const_iterator end, 
										double minFreq, double maxFreq ) :
	_env( new BreakpointEnvelope() )
{
	//	sanity:
	if ( maxFreq < minFreq )
		std::swap( minFreq, maxFreq );

#ifdef Loris_Debug
	debugger << "Finding frequency reference envelope in range " <<
	debugger << minFreq << " to " << maxFreq << " Hz, from " <<
	debugger << std::distance(begin,end) << " Partials" << std::endl;
#endif

	//	find the longest Partial in the specified frequency range:
	PartialList::const_iterator longest  = 
		findLongestPartialInFreqRange( begin, end, minFreq, maxFreq );
		
	if ( longest == end )
		Throw( InvalidArgument, "No Partials attain their maximum sinusoidal energy within the specified frequency range." );
	
	//	build the Envelope by sampling the longest Partial's frequency
	//	envelope at each Breakpoint:
	for ( Partial::const_iterator it = longest->begin(); it != longest->end(); ++it )
	{
		_env->insertBreakpoint( it.time(), it.breakpoint().frequency() );
	}
}

// ---------------------------------------------------------------------------
//	copy construction
// ---------------------------------------------------------------------------
//
FrequencyReference::FrequencyReference( const FrequencyReference & other ) :
	_env( other._env->clone() )
{
}

// ---------------------------------------------------------------------------
//	destruction
// ---------------------------------------------------------------------------
//
FrequencyReference::~FrequencyReference()
{
}

// ---------------------------------------------------------------------------
//	assignment
// ---------------------------------------------------------------------------
//
FrequencyReference &
FrequencyReference::operator = ( const FrequencyReference & rhs )
{
	if ( &rhs != this )
	{
		_env.reset( rhs._env->clone() );
	}
	return *this;
}

// ---------------------------------------------------------------------------
//	clone
// ---------------------------------------------------------------------------
//
FrequencyReference * 
FrequencyReference::clone( void ) const
{
	return new FrequencyReference( *this );
}

// ---------------------------------------------------------------------------
//	valueAt
// ---------------------------------------------------------------------------
//
double
FrequencyReference::valueAt( double x ) const
{
	return _env->valueAt(x);
}

// ---------------------------------------------------------------------------
//	envelope
// ---------------------------------------------------------------------------
//	Conversion to BreakpointEnvelope return a BreakpointEnvelope that 
//	evaluates indentically to this FrequencyReference at all time.
//
BreakpointEnvelope 
FrequencyReference::envelope( void ) const 
{ 
    return *_env; 
}

// ---------------------------------------------------------------------------
//	timeOfPeakEnergy (static helper function)
// ---------------------------------------------------------------------------
//	Return the time at which the given Partial attains its
//	maximum sinusoidal energy.
//
static double timeOfPeakEnergy( const Partial & p )
{
	Partial::const_iterator partialIter = p.begin();
	double maxAmp = 
		partialIter->amplitude() * std::sqrt( 1. - partialIter->bandwidth() );
	double time = partialIter.time();
	
	for ( ++partialIter; partialIter != p.end(); ++partialIter ) 
	{
		double a = partialIter->amplitude() * 
					std::sqrt( 1. - partialIter->bandwidth() );
		if ( a > maxAmp ) 
		{
			maxAmp = a;
			time = partialIter.time();
		}
	}			
	
	return time;
}
// ---------------------------------------------------------------------------
//	IsInFrequencyRange
// ---------------------------------------------------------------------------
//	Function object for finding Partials that attain their maximum
//	sinusoidal energy at a frequency within a specified range.
//
struct IsInFrequencyRange
{
	double minFreq, maxFreq;
	IsInFrequencyRange( double min, double max ) :
		minFreq( min ),
		maxFreq( max )
	{
		//	sanity:
		if ( maxFreq < minFreq )
			std::swap( minFreq, maxFreq );
	}
	
	bool operator() ( const Partial & p )
	{
		double compareFreq = p.frequencyAt( timeOfPeakEnergy( p ) );
		return compareFreq >= minFreq  && compareFreq <= maxFreq;
	}
};

// ---------------------------------------------------------------------------
//	findLongestPartialInFreqRange (static helper function)
// ---------------------------------------------------------------------------
//	Return the longest Partial in the half open range [begin, end)
//	that attains its maximum sinusoidal energy at a frequency within 
//	a specified range.
//
static PartialList::const_iterator
findLongestPartialInFreqRange( PartialList::const_iterator begin, 
							   PartialList::const_iterator end, 
							   double minFreq, double maxFreq )
{
	PartialList::const_iterator it = 
		std::find_if( begin, end, IsInFrequencyRange(minFreq, maxFreq) );
	
	//	there may be no Partials in the specified frequency range:
	if ( it == end )
		return it;
		
	PartialList::const_iterator longest = it;
	for ( it = std::find_if( ++it, end, IsInFrequencyRange(minFreq, maxFreq) );
		  it != end;
		  it = std::find_if( ++it, end, IsInFrequencyRange(minFreq, maxFreq) ) )
	{
		if ( it->duration() > longest->duration() )
			longest = it;
	}
		
	return longest;
}

// ---------------------------------------------------------------------------
//	buildEnvelopeFromPartial (static helper function)
// ---------------------------------------------------------------------------
//
static void
buildEnvelopeFromPartial( BreakpointEnvelope & env, const Partial & p, long numsamps )
{
	double dt = p.duration() / ( numsamps + 1 );
	for ( long i = 0; i < numsamps; ++i ) 
	{
		double t = p.startTime() + ((i+1) * dt);
		double f = p.frequencyAt(t);
		env.insertBreakpoint( t, f );
	}
}					   

}	//	end of namespace Loris

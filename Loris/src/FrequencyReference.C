/* 
	FrequencyReference.C

	Author:			Kelly Fitz
	Description:	<describe the FrequencyReference class here>
*/

#include "FrequencyReference.h"
#include <Breakpoint.h>
#include <BreakpointEnvelope.h>
#include <Partial.h>

using namespace Loris; 

//	forward declarations for helpers, defined below:
static std::list< Partial >::const_iterator
findLongestPartialInFreqRange( std::list<Partial>::const_iterator begin, 
							   std::list<Partial>::const_iterator end, 
							   double minFreq, double maxFreq );


FrequencyReference::FrequencyReference( std::list<Partial>::const_iterator begin, 
										std::list<Partial>::const_iterator end, 
										double minFreq, double maxFreq ) :
	_env( new BreakpointEnvelope() )
{
	//	sanity:
	if ( maxFreq < minFreq )
		std::swap( minFreq, maxFreq );

	//	find the longest Partial in the specified frequency range:
	std::list< Partial >::const_iterator longest  = 
		findLongestPartialInFreqRange( begin, end, minFreq, maxFreq );
		
	if ( longest == end )
		Throw( InvalidArgument, "No Partials attain their maximum sinusoidal energy within the specified frequency range." );
	
	//	construct a BreakpointEnvelope by sampling the longest
	//	Partial at regular (small) interval, using at least 10 
	//	points:
	//	(omits the very first and last Breakpoints)
	//
	const double APPROX_DT = .030;	//	30 milliseconds
	long numSamps = std::max( long( longest->duration() / APPROX_DT ), 10L );
	double dt = longest->duration() / ( numSamps + 1 );
	for ( long i = 0; i < numSamps; ++i ) 
	{
		double t = longest->startTime() + ((i+1) * dt);
		double f = longest->frequencyAt(t);
		_env->insertBreakpoint( t, f );
	}
}


FrequencyReference::~FrequencyReference()
{
}


//	helper functions:

//	timeOfPeakEnergy
//
//	Return the time at which the given Partial attains its
//	maximum sinusoidal energy.
//
static double timeOfPeakEnergy( const Partial & p )
{
	PartialConstIterator partialIter = p.begin();
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

//	IsInFrequencyRange
//
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

//	findLongestPartialInFreqRange
//
//	Return the longest Partial in the half open range [begin, end)
//	that attains its maximum sinusoidal energy at a frequency within 
//	a specified range.
static std::list< Partial >::const_iterator
findLongestPartialInFreqRange( std::list<Partial>::const_iterator begin, 
							   std::list<Partial>::const_iterator end, 
							   double minFreq, double maxFreq )
{
	std::list<Partial>::const_iterator it = 
		std::find_if( begin, end, IsInFrequencyRange(minFreq, maxFreq) );
	
	//	there may be no Partials in the specified frequency range:
	if ( it == end )
		return it;
		
	std::list<Partial>::const_iterator longest = it;
	for ( it = std::find_if( ++it, end, IsInFrequencyRange(minFreq, maxFreq) );
		  it != end;
		  it = std::find_if( ++it, end, IsInFrequencyRange(minFreq, maxFreq) ) )
	{
		if ( it->duration() > longest->duration() )
			longest = it;
	}
		
	return longest;
}


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
 * Partial.C
 *
 * Implementation of class Loris::Partial.
 *
 * Kelly Fitz, 16 Aug 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "Partial.h"
#include "Breakpoint.h"
#include "Exception.h"
#include "notifier.h"

#include <cmath>

//	Pi:
static const double Pi = M_PI;
static const double TwoPi = 2. * M_PI;

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

#ifdef Debug_Loris
long Partial::DebugCounter = 0L;
#endif

// ---------------------------------------------------------------------------
//	Partial constructor
// ---------------------------------------------------------------------------
//
Partial::Partial( void ) :
	_label( 0L )
{
#ifdef Debug_Loris
	++DebugCounter;
#endif
}	

// ---------------------------------------------------------------------------
//	Partial copy constructor
// ---------------------------------------------------------------------------
//
Partial::Partial( const Partial & other ) :
	_bpmap( other._bpmap ),
	_label( other._label )
{
#ifdef Debug_Loris
	++DebugCounter;
#endif
}

// ---------------------------------------------------------------------------
//	Partial destructor
// ---------------------------------------------------------------------------
//
Partial::~Partial( void )
{
#ifdef Debug_Loris
	--DebugCounter;
#endif
}	

// ---------------------------------------------------------------------------
//	initialPhase
// ---------------------------------------------------------------------------
//	Return starting phase in radians, except (InvalidPartial) if there
//	are no Breakpoints.
//
double
Partial::initialPhase( void ) const
{
	if ( _bpmap.size() == 0 )
		Throw( InvalidPartial, "Tried find intial phase of a Partial with no Breakpoints." );

	return begin()->phase();
}

// ---------------------------------------------------------------------------
//	startTime
// ---------------------------------------------------------------------------
//	Return start time in seconds, except (InvalidPartial) if there
//	are no Breakpoints.
//
double
Partial::startTime( void ) const
{
	if ( _bpmap.size() == 0 )
		Throw( InvalidPartial, "Tried find start time of a Partial with no Breakpoints." );

	return begin().time();
}

// ---------------------------------------------------------------------------
//	endTime
// ---------------------------------------------------------------------------
//	Return end time in seconds, except (InvalidPartial) if there
//	are no Breakpoints.
//
double
Partial::endTime( void ) const
{
	if ( _bpmap.size() == 0 )
		Throw( InvalidPartial, "Tried find end time of a Partial with no Breakpoints." );

	return (--end()).time();
}

// ---------------------------------------------------------------------------
//	duration
// ---------------------------------------------------------------------------
//	Return time, in seconds, spanned by this Partial, or 0. if there
//	are no Breakpoints.
//
double
Partial::duration( void ) const
{
	if ( _bpmap.size() == 0 )
		return 0.;

	return endTime() - startTime();
}

// ---------------------------------------------------------------------------
//	begin (non-const)
// ---------------------------------------------------------------------------
//	Iterator generation.
//
PartialIterator 
Partial::begin( void )
{ 
	return PartialIterator( _bpmap.begin() ); 
}

// ---------------------------------------------------------------------------
//	end (non-const)
// ---------------------------------------------------------------------------
//	Iterator generation.
//
PartialIterator 
Partial::end( void )
{ 
	return PartialIterator( _bpmap.end() ); 
}

// ---------------------------------------------------------------------------
//	begin (const)
// ---------------------------------------------------------------------------
//	Const iterator generation.
//
PartialConstIterator 
Partial::begin( void ) const
{ 
	return PartialConstIterator( _bpmap.begin() ); 
}

// ---------------------------------------------------------------------------
//	end (const)
// ---------------------------------------------------------------------------
//	Iterator generation.
//
PartialConstIterator 
Partial::end( void ) const
{ 
	return PartialConstIterator( _bpmap.end() ); 
}

// ---------------------------------------------------------------------------
//	insert
// ---------------------------------------------------------------------------
//	Insert a copy of the specified Breakpoint at time (seconds),
//	return position (iterator) of the inserted Breakpoint.
//	If there is already a Breakpoint at time, assign
//	bp to it (copying parameters).
//
//	Could except:
//	allocation of a new Breakpoint could fail, throwing a std::bad__alloc.
//
PartialIterator
Partial::insert( double time, const Breakpoint & bp )
{
	_bpmap[ time ] = bp;
	return PartialIterator( _bpmap.find(time) );
}

// ---------------------------------------------------------------------------
//	findPos (const version)
// ---------------------------------------------------------------------------
//	Return the insertion position for a Breakpoint at
//	the specified time (that is, the position of the first
//	Breakpoint at a time later than or equal to the specified 
//	time).
//
PartialConstIterator
Partial::findPos( double time ) const
{
	return PartialConstIterator( _bpmap.lower_bound( time ) );
}

// ---------------------------------------------------------------------------
//	findPos (non-const version)
// ---------------------------------------------------------------------------
//	Return the insertion position for a Breakpoint at
//	the specified time (that is, the position of the first
//	Breakpoint at a time later than or equal to the specified 
//	time).
//
PartialIterator
Partial::findPos( double time )
{
	return PartialIterator( _bpmap.lower_bound( time ) );
} 

// ---------------------------------------------------------------------------
//	frequencyAt
// ---------------------------------------------------------------------------
//	
double
Partial::frequencyAt( double time ) const
{
	if ( _bpmap.size() == 0 )
		Throw( InvalidPartial, "Tried to interpolate a Partial with no Breakpoints." );

	//	lower_bound returns a reference to the lowest
	//	position that would be higher than an element
	//	having key equal to time:
	PartialConstIterator it = findPos( time );
		
	if ( it == _bpmap.begin() ) {
	//	time is before the onset of the Partial:
		return it->frequency();
	}
	else if ( it == _bpmap.end() ) {
	//	time is past the end of the Partial:
		return (--it)->frequency();
	}
	else {
	//	interpolate between it and its predeccessor
	//	(we checked already that it is not begin):
		const Breakpoint & hi = *it;
		double hitime = it.time();
		const Breakpoint & lo = *(--it);
		double lotime = it.time();
		double alpha = (time - lotime) / (hitime - lotime);
		return (alpha * hi.frequency()) + ((1. - alpha) * lo.frequency());
	}
}

// ---------------------------------------------------------------------------
//	amplitudeAt
// ---------------------------------------------------------------------------
//	
double
Partial::amplitudeAt( double time ) const
{
	if ( _bpmap.size() == 0 )
		Throw( InvalidPartial, "Tried to interpolate a Partial with no Breakpoints." );

	//	lower_bound returns a reference to the lowest
	//	position that would be higher than an element
	//	having key equal to time:
	PartialConstIterator it = findPos( time );
		
	if ( it == _bpmap.begin() ) {
	//	time is before the onset of the Partial:
		return 0.;
	}
	else if (it == _bpmap.end() ) {
	//	time is past the end of the Partial:
		return 0.;
	}
	else {
	//	interpolate between it and its predeccessor
	//	(we checked already that it is not begin):
		const Breakpoint & hi = *it;
		double hitime = it.time();
		const Breakpoint & lo = *(--it);
		double lotime = it.time();
		double alpha = (time - lotime) / (hitime - lotime);
		return (alpha * hi.amplitude()) + ((1. - alpha) * lo.amplitude());
	}
}
// ---------------------------------------------------------------------------
//	phaseAt
// ---------------------------------------------------------------------------
//	
double
Partial::phaseAt( double time ) const
{
	if ( _bpmap.size() == 0 )
		Throw( InvalidPartial, "Tried to interpolate a Partial with no Breakpoints." );
	
	//	lower_bound returns a reference to the lowest
	//	position that would be higher than an element
	//	having key equal to time:
	PartialConstIterator it = findPos( time );
		
	//	compute phase:
	//	map iterator is a pair: first is time, 
	//	second is Breakpoint.
	if ( it == _bpmap.begin() ) {
	//	time is before the onset of the Partial:
		double dp = TwoPi * (it.time() - time) * it->frequency();
		return std::fmod( it->phase() - dp, TwoPi);

	}
	else if (it == _bpmap.end() ) {
	//	time is past the end of the Partial:
	//	( first decrement iterator to get the tail Breakpoint)
		--it;
		
		double dp = TwoPi * (time - it.time()) * it->frequency();
		return std::fmod( it->phase() + dp, TwoPi );
	}
	else {
	//	interpolate between it and its predeccessor
	//	(we checked already that it is not begin):
		const Breakpoint & hi = *it;
		double hitime = it.time();
		const Breakpoint & lo = *(--it);
		double lotime = it.time();
		double alpha = (time - lotime) / (hitime - lotime);
		double favg = (0.5 * alpha * hi.frequency()) + 
						((1. - (0.5 * alpha)) * lo.frequency());
		double dp = TwoPi * (time - lotime) * favg;
		return std::fmod( lo.phase() + dp, TwoPi );

	}
}

// ---------------------------------------------------------------------------
//	bandwidthAt
// ---------------------------------------------------------------------------
//	
double
Partial::bandwidthAt( double time ) const
{
	if ( _bpmap.size() == 0 )
		Throw( InvalidPartial, "Tried to interpolate a Partial with no Breakpoints." );
	
	//	lower_bound returns a reference to the lowest
	//	position that would be higher than an element
	//	having key equal to time:
	PartialConstIterator it = findPos( time );
		
	if ( it == _bpmap.begin() ) {
	//	time is before the onset of the Partial:
		return it-> bandwidth();
	}
	else if (it == _bpmap.end() ) {
	//	time is past the end of the Partial:
		return (--it)->bandwidth();
	}
	else {
	//	interpolate between it and its predeccessor
	//	(we checked already that it is not begin):
		const Breakpoint & hi = *it;
		double hitime = it.time();
		const Breakpoint & lo = *(--it);
		double lotime = it.time();
		double alpha = (time - lotime) / (hitime - lotime);
		return (alpha * hi.bandwidth()) + ((1. - alpha) * lo.bandwidth());
	}
}

// ---------------------------------------------------------------------------
//	FadeTime
// ---------------------------------------------------------------------------
//	Static member for making sure that all algorithms
//	that fade Partials in and out use the same fade time.
//	Returns 1 ms.
//	
double
Partial::FadeTime( void )
{
	static const double FADE_TIME = 0.001;	//	1 ms
	return FADE_TIME;
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

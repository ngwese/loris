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

#if HAVE_CONFIG_H
	#include <config.h>
#endif

#include<Partial.h>
#include<Breakpoint.h>
#include<Exception.h>
#include<Notifier.h>

#include <cmath>

#if defined(HAVE_M_PI) && (HAVE_M_PI)
	const double Pi = M_PI;
#else
	const double Pi = 3.14159265358979324;
#endif

//	begin namespace
namespace Loris {

//long Partial::DebugCounter = 0L;

// ---------------------------------------------------------------------------
//	FadeTime (STATIC)
// ---------------------------------------------------------------------------
//	Static member for making sure that all algorithms
//	that fade Partials in and outuse the same fade time.
//	Returns 1 ms.
//
//	Removed this from the Partial interface, but found that
//	it is also used internally. It makes sense as a parameter
//	to the other classes that used it, but not as an argument 
//	to the parameterAt() members. Need a better solution 
//	probably.
//	
static double FadeTime( void )
{
	static const double FADE_TIME = 0.001;	//	1 ms
	return FADE_TIME;
}

// ---------------------------------------------------------------------------
//	Partial constructor
// ---------------------------------------------------------------------------
//
Partial::Partial( void ) :
	_label( 0L )
{
//	++DebugCounter;
}	

// ---------------------------------------------------------------------------
//	Partial copy constructor
// ---------------------------------------------------------------------------
//
Partial::Partial( const Partial & other ) :
	_bpmap( other._bpmap ),
	_label( other._label )
{
//	++DebugCounter;
}

// ---------------------------------------------------------------------------
//	Partial destructor
// ---------------------------------------------------------------------------
//
Partial::~Partial( void )
{
//	--DebugCounter;
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

	return begin().breakpoint().phase();
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
//	absorb
// ---------------------------------------------------------------------------
//	Absorb another Partial's energy as noise (bandwidth), 
//	by accumulating the other's energy as noise energy
//	in the portion of this Partial's envelope that overlaps
//	(in time) with the other Partial's envelope.
//
void 
Partial::absorb( const Partial & other )
{
	Partial::iterator it = findNearest( other.startTime() );
	while ( it != end() && !(it.time() > other.endTime()) )
	{
		// absorb energy from other at the time
		// of this Breakpoint:
		double a = other.amplitudeAt( it.time() );
		it->addNoiseEnergy( a * a );
		
		++it;
	}
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
//	operator==
// ---------------------------------------------------------------------------
//	Return true if this Partial has the same label and Breakpoint map 
//	as rhs, and false otherwise.
//
bool
Partial::operator==( const Partial & rhs ) const
{
	return (label() == rhs.label()) && (_bpmap == rhs._bpmap);
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
//	erase
// ---------------------------------------------------------------------------
//	Erase the Breakpoint at the position of the 
//	given iterator (invalidating the iterator), and
//	return an iterator referring to the next position,
//	or end if pos is the last Breakpoint in the Partial.
PartialIterator 
Partial::erase( PartialIterator pos )
{
	PartialIterator ret = pos;
	if ( pos._iter != _bpmap.end() )
	{
		++ret;
		_bpmap.erase( pos._iter );
	}
	return ret;
}

// ---------------------------------------------------------------------------
//	split
// ---------------------------------------------------------------------------
//	Break this Partial at the specified position (iterator).
//	The Breakpoint at the specified position becomes the first
//	Breakpoint in a new Partial. Breakpoints at the specified
//	position and subsequent positions are removed from this
//	Partial and added to the new Partial, which is returned.
//
Partial 
Partial::split( iterator pos )
{
	Partial res;
	res._bpmap.insert( pos._iter, _bpmap.end() );
	_bpmap.erase( pos._iter, _bpmap.end() );
	return res;
}

// ---------------------------------------------------------------------------
//	findAfter (const version)
// ---------------------------------------------------------------------------
//	Return the insertion position for a Breakpoint at
//	the specified time (that is, the position of the first
//	Breakpoint at a time later than or equal to the specified 
//	time).
//
PartialConstIterator
Partial::findAfter( double time ) const
{
	return PartialConstIterator( _bpmap.lower_bound( time ) );
}

// ---------------------------------------------------------------------------
//	findAfter (non-const version)
// ---------------------------------------------------------------------------
//	Return the insertion position for a Breakpoint at
//	the specified time (that is, the position of the first
//	Breakpoint at a time later than or equal to the specified 
//	time).
//
PartialIterator
Partial::findAfter( double time )
{
	return PartialIterator( _bpmap.lower_bound( time ) );
} 

// ---------------------------------------------------------------------------
//	findNearest (const version)
// ---------------------------------------------------------------------------
//	Return the insertion position for the Breakpoint nearest
//	the specified time. Always returns a valid iterator (the
//	position of the nearest-in-time Breakpoint) unless there
//	are no Breakpoints.
//
PartialConstIterator
Partial::findNearest( double time ) const
{
	//	if there are no Breakpoints, return end:
	if ( numBreakpoints() == 0 )
		return end();
			
	//	get the position of the first Breakpoint after time:
	PartialConstIterator pos( _bpmap.lower_bound( time ) );
	
	//	if there is an earlier Breakpoint that is closer in
	//	time, prefer that one:
	if ( pos != begin() )
	{
		PartialConstIterator prev = pos;
		--prev;
		if ( pos == end() || pos.time() - time > time - prev.time() )
			return prev;
	}

	//	failing all else:	
	return pos;
} 

// ---------------------------------------------------------------------------
//	findNearest (non-const version)
// ---------------------------------------------------------------------------
//	Return the insertion position for the Breakpoint nearest
//	the specified time. Always returns a valid iterator (the
//	position of the nearest-in-time Breakpoint) unless there
//	are no Breakpoints.
//
PartialIterator
Partial::findNearest( double time )
{
	//	if there are no Breakpoints, return end:
	if ( numBreakpoints() == 0 )
		return end();
			
	//	get the position of the first Breakpoint after time:
	PartialIterator pos( _bpmap.lower_bound( time ) );
	
	//	if there is an earlier Breakpoint that is closer in
	//	time, prefer that one:
	if ( pos != begin() )
	{
		PartialIterator prev = pos;
		--prev;
		if ( pos == end() || pos.time() - time > time - prev.time() )
			return prev;
	}

	//	failing all else:	
	return pos;
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
	PartialConstIterator it = findAfter( time );
		
	if ( it == _bpmap.begin() ) 
	{
	//	time is before the onset of the Partial:
		return it.breakpoint().frequency();
	}
	else if ( it == _bpmap.end() ) 
	{
	//	time is past the end of the Partial:
		return (--it).breakpoint().frequency();
	}
	else 
	{
	//	interpolate between it and its predeccessor
	//	(we checked already that it is not begin):
		const Breakpoint & hi = it.breakpoint();
		double hitime = it.time();
		const Breakpoint & lo = (--it).breakpoint();
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
	PartialConstIterator it = findAfter( time );
		
	if ( it == _bpmap.begin() ) 
	{
	//	time is before the onset of the Partial:
	//	fade in ampltude
		if ( time < it.time() - FadeTime() )
		{
			return 0.;
		}
		else
		{
			double alpha = (it.time() - time) / FadeTime();
			return (1. - alpha) * it.breakpoint().amplitude();
		}
	}
	else if (it == _bpmap.end() ) 
	{
	//	time is past the end of the Partial:
	//	fade out ampltude
	//	( first decrement iterator to get the tail Breakpoint)
		--it;
		
		if ( time > it.time() + FadeTime() )
		{
			return 0.;
		}
		else
		{
			double alpha = (time - it.time()) / FadeTime();
			return (1. - alpha) * it.breakpoint().amplitude();
		}
	}
	else 
	{
	//	interpolate between it and its predeccessor
	//	(we checked already that it is not begin):
		const Breakpoint & hi = it.breakpoint();
		double hitime = it.time();
		const Breakpoint & lo = (--it).breakpoint();
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
	PartialConstIterator it = findAfter( time );
		
	//	compute phase:
	//	map iterator is a pair: first is time, 
	//	second is Breakpoint.
	if ( it == _bpmap.begin() ) 
	{
	//	time is before the onset of the Partial:
		double dp = 2. * Pi * (it.time() - time) * it.breakpoint().frequency();
		return std::fmod( it.breakpoint().phase() - dp, 2. * Pi);
	}
	else if (it == _bpmap.end() ) 
	{
	//	time is past the end of the Partial:
	//	( first decrement iterator to get the tail Breakpoint)
		--it;
		
		double dp = 2. * Pi * (time - it.time()) * it.breakpoint().frequency();
		return std::fmod( it.breakpoint().phase() + dp, 2. * Pi );
	}
	else 
	{
	//	interpolate between it and its predeccessor
	//	(we checked already that it is not begin):
		const Breakpoint & hi = it.breakpoint();
		double hitime = it.time();
		const Breakpoint & lo = (--it).breakpoint();
		double lotime = it.time();
		double alpha = (time - lotime) / (hitime - lotime);
		double favg = (0.5 * alpha * hi.frequency()) + 
						((1. - (0.5 * alpha)) * lo.frequency());

		//	need to keep fmod in here because other stuff 
		//	(Spc export and sdif export, for example) rely 
		//	on it:
		if ( alpha < 0.5 )
		{
			double dp = 2. * Pi * (time - lotime) * favg;
			return std::fmod( lo.phase() + dp, 2. * Pi );
		}
		else
		{
			double dp = 2. * Pi * (hitime - time) * favg;
			return std::fmod( hi.phase() - dp, 2. * Pi );
		}

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
	PartialConstIterator it = findAfter( time );
		
	if ( it == _bpmap.begin() ) 
	{
	//	time is before the onset of the Partial:
		return it.breakpoint(). bandwidth();
	}
	else if (it == _bpmap.end() ) 
	{
	//	time is past the end of the Partial:
		return (--it).breakpoint().bandwidth();
	}
	else 
	{
	//	interpolate between it and its predeccessor
	//	(we checked already that it is not begin):
		const Breakpoint & hi = it.breakpoint();
		double hitime = it.time();
		const Breakpoint & lo = (--it).breakpoint();
		double lotime = it.time();
		double alpha = (time - lotime) / (hitime - lotime);
		return (alpha * hi.bandwidth()) + ((1. - alpha) * lo.bandwidth());
	}
}

}	//	end of namespace Loris

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

#include <Partial.h>
#include <Breakpoint.h>
#include <Exception.h>
#include <Notifier.h>

#include <cmath>

#if defined(HAVE_M_PI) && (HAVE_M_PI)
	const double Pi = M_PI;
#else
	const double Pi = 3.14159265358979324;
#endif

//	begin namespace
namespace Loris {

//long Partial::DebugCounter = 0L;

#pragma mark -- construction --

// ---------------------------------------------------------------------------
//	Partial constructor
// ---------------------------------------------------------------------------
//
Partial::Partial( void ) :
	_label( 0 )
{
//	++DebugCounter;
}	

// ---------------------------------------------------------------------------
//	Partial initialized constructor
// ---------------------------------------------------------------------------
//
Partial::Partial( const_iterator beg, const_iterator end ) :
	_bpmap( beg._iter, end._iter ),
	_label( 0 )
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
//	operator=
// ---------------------------------------------------------------------------
//	Make this Partial an exact copy (has an identical set of 
//	Breakpoints, at identical times, and the same label) of another 
//	Partial.
//
Partial & 
Partial::operator=( const Partial & rhs )
{
	if ( this != &rhs )
	{
		_bpmap = rhs._bpmap;
		_label = rhs._label;
	}
	return *this;
}

#pragma mark -- container-dependent implementation --

// ---------------------------------------------------------------------------
//	operator==
// ---------------------------------------------------------------------------
//	Return true if this Partial has the same label and Breakpoint map 
//	as rhs, and false otherwise.
//
bool
Partial::operator==( const Partial & rhs ) const
{
	return (_label == rhs._label) && (_bpmap == rhs._bpmap);
}

// ---------------------------------------------------------------------------
//	begin
// ---------------------------------------------------------------------------
//	Return an iterator refering to the position of the first
//	Breakpoint in this Partial's envelope.
//		
//	For const Partials, returns a const_iterator.
//
Partial::const_iterator Partial::begin( void ) const 
{ 
	return _bpmap.begin(); 
}

Partial::iterator Partial::begin( void ) 
{ 
	return _bpmap.begin(); 
}

// ---------------------------------------------------------------------------
//	end
// ---------------------------------------------------------------------------
//	Return an iterator refering to the position past the last
//	Breakpoint in this Partial's envelope. The iterator returned by
//	end() (like the iterator returned by the end() member of any STL
//	container) does not refer to a valid Breakpoint. 	
//
//	For const Partials, returns a const_iterator.
//
Partial::const_iterator 
Partial::end( void ) const 
{ 
	return _bpmap.end(); 
}

Partial::iterator 
Partial::end( void ) 
{ 
	return _bpmap.end(); 
}

// ---------------------------------------------------------------------------
//	erase
// ---------------------------------------------------------------------------
//	Breakpoint removal: erase the Breakpoints in the specified range,
//	and return an iterator referring to the position after the,
//	erased range.
//
Partial::iterator 
Partial::erase( Partial::iterator beg, Partial::iterator end )
{
	_bpmap.erase( beg._iter, end._iter );
	return end;
}

// ---------------------------------------------------------------------------
//	findAfter
// ---------------------------------------------------------------------------
//	Return an iterator refering to the insertion position for a
//	Breakpoint at the specified time (that is, the position of the first
//	Breakpoint at a time later than the specified time).
//	
//	For const Partials, returns a const_iterator.
//
Partial::const_iterator 
Partial::findAfter( double time ) const
{
	return _bpmap.lower_bound( time );
}

Partial::iterator 
Partial::findAfter( double time ) 
{
	return _bpmap.lower_bound( time );
}

// ---------------------------------------------------------------------------
//	insert
// ---------------------------------------------------------------------------
//	Breakpoint insertion: insert a copy of the specified Breakpoint in the
//	parameter envelope at time (seconds), and return an iterator
//	refering to the position of the inserted Breakpoint.
//
Partial::iterator 
Partial::insert( double time, const Breakpoint & bp )
{
	std::pair< container_type::iterator, bool > result = 
		_bpmap.insert( container_type::value_type(time, bp) );
	if ( ! result.second )
		result.first->second = bp;
	return result.first;
}

// ---------------------------------------------------------------------------
//	size
// ---------------------------------------------------------------------------
//	Return the number of Breakpoints in this Partial.
//
Partial::size_type 
Partial::size( void ) const 
{ 	
	return _bpmap.size(); 
}

#pragma mark -- container-independent implementation --

// ---------------------------------------------------------------------------
//	first
// ---------------------------------------------------------------------------
//	Return a reference to the first Breakpoint in the Partial's
//	envelope. Raises InvalidPartial exception if there are no 
//	Breakpoints.
//
Breakpoint & 
Partial::first( void )
{
	if ( size() == 0 )
		Throw( InvalidPartial, "Tried find first Breakpoint in a Partial with no Breakpoints." );
	return begin().breakpoint();
}

// ---------------------------------------------------------------------------
//	first
// ---------------------------------------------------------------------------
//	Return a reference to the first Breakpoint in the Partial's
//	envelope. Raises InvalidPartial exception if there are no 
//	Breakpoints.
//
const Breakpoint & 
Partial::first( void ) const
{
	if ( size() == 0 )
		Throw( InvalidPartial, "Tried find first Breakpoint in a Partial with no Breakpoints." );
	return begin().breakpoint();
}

// ---------------------------------------------------------------------------
//	last
// ---------------------------------------------------------------------------
//	Return a reference to the last Breakpoint in the Partial's
//	envelope. Raises InvalidPartial exception if there are no 
//	Breakpoints.
//
Breakpoint & 
Partial::last( void )
{
	if ( size() == 0 )
		Throw( InvalidPartial, "Tried find last Breakpoint in a Partial with no Breakpoints." );
	return (--end()).breakpoint();
}

// ---------------------------------------------------------------------------
//	last
// ---------------------------------------------------------------------------
//	Return a reference to the last Breakpoint in the Partial's
//	envelope. Raises InvalidPartial exception if there are no 
//	Breakpoints.
//
const Breakpoint & 
Partial::last( void ) const
{
	if ( size() == 0 )
		Throw( InvalidPartial, "Tried find last Breakpoint in a Partial with no Breakpoints." );
	return (--end()).breakpoint();
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
	if ( numBreakpoints() == 0 )
		Throw( InvalidPartial, "Tried find intial phase of a Partial with no Breakpoints." );

	return first().phase();
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
	if ( numBreakpoints() == 0 )
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
	if ( numBreakpoints() == 0 )
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
	Partial::iterator it = findAfter( other.startTime() );
	while ( it != end() && !(it.time() > other.endTime()) )
	{
		//	only non-null (non-zero-amplitude) Breakpoints
		//	abosrb noise energym because null Breakpoints
		//	are used especially to reset the Partial phase,
		//	and are not part of the normal analyasis data:
		if ( it->amplitude() > 0 )
		{
			// absorb energy from other at the time
			// of this Breakpoint:
			double a = other.amplitudeAt( it.time() );
			it->addNoiseEnergy( a * a );
		}	
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
	if ( numBreakpoints() == 0 )
		return 0.;

	return endTime() - startTime();
}

// ---------------------------------------------------------------------------
//	erase
// ---------------------------------------------------------------------------
//	Erase the Breakpoint at the position of the 
//	given iterator (invalidating the iterator), and
//	return an iterator referring to the next position,
//	or end if pos is the last Breakpoint in the Partial.
Partial::iterator 
Partial::erase( Partial::iterator pos )
{
	if ( pos != end() )
	{
		pos = erase( pos, ++pos );
	}
	return pos;
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
	Partial res( pos, end() );
	erase( pos, end() );
	return res;
}

// ---------------------------------------------------------------------------
//	findNearest (const version)
// ---------------------------------------------------------------------------
//	Return the insertion position for the Breakpoint nearest
//	the specified time. Always returns a valid iterator (the
//	position of the nearest-in-time Breakpoint) unless there
//	are no Breakpoints.
//
Partial::const_iterator
Partial::findNearest( double time ) const
{
	//	if there are no Breakpoints, return end:
	if ( numBreakpoints() == 0 )
		return end();
			
	//	get the position of the first Breakpoint after time:
	Partial::const_iterator pos = findAfter( time );
	
	//	if there is an earlier Breakpoint that is closer in
	//	time, prefer that one:
	if ( pos != begin() )
	{
		Partial::const_iterator prev = pos;
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
Partial::iterator
Partial::findNearest( double time )
{
	//	if there are no Breakpoints, return end:
	if ( numBreakpoints() == 0 )
		return end();
			
	//	get the position of the first Breakpoint after time:
	Partial::iterator pos = findAfter( time );
	
	//	if there is an earlier Breakpoint that is closer in
	//	time, prefer that one:
	if ( pos != begin() )
	{
		Partial::iterator prev = pos;
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
	if ( numBreakpoints() == 0 )
		Throw( InvalidPartial, "Tried to interpolate a Partial with no Breakpoints." );

	//	lower_bound returns a reference to the lowest
	//	position that would be higher than an element
	//	having key equal to time:
	Partial::const_iterator it = findAfter( time );
		
	if ( it == begin() ) 
	{
	//	time is before the onset of the Partial:
		return it.breakpoint().frequency();
	}
	else if ( it == end() ) 
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
//	ShortestSafeFadeTime
// ---------------------------------------------------------------------------
//	Define the default fade time for computing amplitude at the ends
//	of a Partial. Floating point round-off errors make fadeTime == 0.0
//	dangerous and unpredictable. 1 ns is short enough to prevent rounding
//	errors in the least significant bit of a 48-bit mantissa for times
//	up to ten hours.
//
const double Partial::ShortestSafeFadeTime = 1.0E-9;

// ---------------------------------------------------------------------------
//	amplitudeAt
// ---------------------------------------------------------------------------
//	The default fadeTime is ShortestSafeFadeTime.
//
//	
double
Partial::amplitudeAt( double time, double fadeTime ) const
{
	if ( numBreakpoints() == 0 )
		Throw( InvalidPartial, "Tried to interpolate a Partial with no Breakpoints." );

	//	lower_bound returns a reference to the lowest
	//	position that would be higher than an element
	//	having key equal to time:
	Partial::const_iterator it = findAfter( time );
		
	if ( it == begin() ) 
	{
		double alpha =  (time < it.time()) ? 0. : 1.;
		if ( fadeTime > 0 )
		{
			//	fade in ampltude if time is before the onset of the Partial:
			alpha = std::max(0., 1. - ((it.time() - time) / fadeTime) );
		}
		return alpha * it.breakpoint().amplitude();
	}
	else if ( it == end() ) 
	{
		//	( first decrement iterator to get the tail Breakpoint)
		--it;
		
		double alpha =  (time > it.time()) ? 0. : 1.;
		if ( fadeTime > 0 )
		{
			//	fade out ampltude if time is past the end of the Partial:
			alpha = std::max(0., 1. - ((time - it.time()) / fadeTime) );
		}
		return alpha * it.breakpoint().amplitude();
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
	if ( numBreakpoints() == 0 )
		Throw( InvalidPartial, "Tried to interpolate a Partial with no Breakpoints." );
	
	//	lower_bound returns a reference to the lowest
	//	position that would be higher than an element
	//	having key equal to time:
	Partial::const_iterator it = findAfter( time );
		
	//	compute phase:
	if ( it == begin() ) 
	{
	//	time is before the onset of the Partial:
		double dp = 2. * Pi * (it.time() - time) * it.breakpoint().frequency();
		return std::fmod( it.breakpoint().phase() - dp, 2. * Pi);
	}
	else if (it == end() ) 
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
		double favg = (alpha * hi.frequency()) + 
						((1. - alpha) * lo.frequency());

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
	if ( numBreakpoints() == 0 )
		Throw( InvalidPartial, "Tried to interpolate a Partial with no Breakpoints." );
	
	//	lower_bound returns a reference to the lowest
	//	position that would be higher than an element
	//	having key equal to time:
	Partial::const_iterator it = findAfter( time );
		
	if ( it == begin() ) 
	{
	//	time is before the onset of the Partial:
		return it.breakpoint().bandwidth();
	}
	else if (it == end() ) 
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

// ---------------------------------------------------------------------------
//	parametersAt
// ---------------------------------------------------------------------------
//	Return a Breakpoint representing the state of all four envelopes
//	at the specified time. This saves having to search four times when 
//	all four parameters are needed.
//
//	The default fadeTime is ShortestSafeFadeTime.
//
Breakpoint
Partial::parametersAt( double time, double fadeTime ) const 
{
	if ( numBreakpoints() == 0 )
		Throw( InvalidPartial, "Tried to interpolate a Partial with no Breakpoints." );
	
	//	lower_bound returns a reference to the lowest
	//	position that would be higher than an element
	//	having key equal to time:
	Partial::const_iterator it = findAfter( time );
		
	if ( it == begin() ) 
	{
		//	time is before the onset of the Partial:
		//	frequency is starting frequency, 
		//	amplitude is 0 (or fading), bandwidth is starting 
		//	bandwidth, and phase is rolled back.
		double alpha =  (time < it.time()) ? 0. : 1.;
		if ( fadeTime > 0 )
		{
			//	fade in ampltude if time is before the onset of the Partial:
			alpha = std::max(0., 1. - ((it.time() - time) / fadeTime) );
		}
		double amp = alpha * it.breakpoint().amplitude();

		double dp = 2. * Pi * (it.time() - time) * it.breakpoint().frequency();
		double ph = std::fmod( it.breakpoint().phase() - dp, 2. * Pi);
		
		return Breakpoint( it.breakpoint().frequency(), amp, 
						   it.breakpoint().bandwidth(), ph );
	}
	else if (it == end() ) 
	{
		//	time is past the end of the Partial:
		//	frequency is ending frequency, 
		//	amplitude is 0 (or fading), bandwidth is ending 
		//	bandwidth, and phase is rolled forward.
		--it; 
		
		double alpha =  (time > it.time()) ? 0. : 1.;
		if ( fadeTime > 0 )
		{
			//	fade out ampltude if time is past the end of the Partial:
			alpha = std::max(0., 1. - ((time - it.time()) / fadeTime) );
		}
		double amp = alpha * it.breakpoint().amplitude();

		double dp = 2. * Pi * (time - it.time()) * it.breakpoint().frequency();
		double ph = std::fmod( it.breakpoint().phase() + dp, 2. * Pi );

		return Breakpoint( it.breakpoint().frequency(), amp, 
						   it.breakpoint().bandwidth(), ph );
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

		double favg = (alpha * hi.frequency()) + 
						((1. - alpha) * lo.frequency());

		//	need to keep fmod in here because other stuff 
		//	(Spc export and sdif export, for example) rely 
		//	on it:
		double ph = 0;
		if ( alpha < 0.5 )
		{
			double dp = 2. * Pi * (time - lotime) * favg;
			ph = std::fmod( lo.phase() + dp, 2. * Pi );
		}
		else
		{
			double dp = 2. * Pi * (hitime - time) * favg;
			ph = std::fmod( hi.phase() - dp, 2. * Pi );
		}

		return Breakpoint( (alpha * hi.frequency()) + ((1. - alpha) * lo.frequency()),
						   (alpha * hi.amplitude()) + ((1. - alpha) * lo.amplitude()),
						   (alpha * hi.bandwidth()) + ((1. - alpha) * lo.bandwidth()),
						   ph );
				
	}
}

}	//	end of namespace Loris

// ===========================================================================
//	Partial.C
//
//	Implementation of Loris::Partial.
//	
//	Loris Partials represent reassigned bandwidth-enhanced model components.
//	A Partial consists of a chain of Breakpoints describing the time-varying
//	frequency, amplitude, and bandwidth of the component.
//
//	-kel 16 Aug 99
//
// ===========================================================================
#include "Partial.h"
#include "Breakpoint.h"
#include "Exception.h"
#include "pi.h"

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
#else
	#include <math.h>
#endif

//using namespace std;

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	Partial constructor
// ---------------------------------------------------------------------------
//
Partial::Partial( void ) :
	_label( 0L )
{
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
//	begin (non-const)
// ---------------------------------------------------------------------------
//	Iterator generation.
//
Jackson 
Partial::begin( void )
{ 
	return Jackson( _bpmap.begin() ); 
}

// ---------------------------------------------------------------------------
//	end (non-const)
// ---------------------------------------------------------------------------
//	Iterator generation.
//
Jackson 
Partial::end( void )
{ 
	return Jackson( _bpmap.end() ); 
}

// ---------------------------------------------------------------------------
//	begin (const)
// ---------------------------------------------------------------------------
//	Const iterator generation.
//
JacksonConst 
Partial::begin( void ) const
{ 
	return JacksonConst( _bpmap.begin() ); 
}

// ---------------------------------------------------------------------------
//	end (const)
// ---------------------------------------------------------------------------
//	Iterator generation.
//
JacksonConst 
Partial::end( void ) const
{ 
	return JacksonConst( _bpmap.end() ); 
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
Jackson
Partial::insert( double time, const Breakpoint & bp )
{
	_bpmap[ time ] = bp;
	return Jackson( _bpmap.find(time) );
}

// ---------------------------------------------------------------------------
//	findPos (const version)
// ---------------------------------------------------------------------------
//	Return the insertion position for a Breakpoint at
//	the specified time (that is, the position of the first
//	Breakpoint at a time later than or equal to the specified 
//	time).
//
JacksonConst
Partial::findPos( double time ) const
{
	return JacksonConst( _bpmap.lower_bound( time ) );
}

// ---------------------------------------------------------------------------
//	findPos (non-const version)
// ---------------------------------------------------------------------------
//	Return the insertion position for a Breakpoint at
//	the specified time (that is, the position of the first
//	Breakpoint at a time later than or equal to the specified 
//	time).
//
Jackson
Partial::findPos( double time )
{
	return Jackson( _bpmap.lower_bound( time ) );
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
	JacksonConst it = findPos( time );
		
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
	JacksonConst it = findPos( time );
		
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
	JacksonConst it = findPos( time );
		
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
	JacksonConst it = findPos( time );
		
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

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif
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
#include "LorisLib.h"
#include "Partial.h"
#include "Breakpoint.h"
#include "Exception.h"
#include "notifier.h"
#include <algorithm>

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
#else
	#include <math.h>
#endif

using namespace std;

Begin_Namespace( Loris )

#pragma mark -
#pragma mark construction

// ---------------------------------------------------------------------------
//	Partial constructor
// ---------------------------------------------------------------------------
//
Partial::Partial( void ) :
	_label( 0L )
{
}	

#pragma mark -
#pragma mark envelope parameter shortcuts
// ---------------------------------------------------------------------------
//	initialPhase
// ---------------------------------------------------------------------------
//	in radians
//
double
Partial::initialPhase( void ) const
{
	if ( _bpmap.size() > 0 )
		//	map iterator is a pair: first is time, 
		//	second is Breakpoint.
		return _bpmap.begin()->second.phase();
	else
		return 0.;
}

// ---------------------------------------------------------------------------
//	startTime
// ---------------------------------------------------------------------------
//	in seconds
//
double
Partial::startTime( void ) const
{
	if ( _bpmap.size() > 0 )
		//	map iterator is a pair: first is time, 
		//	second is Breakpoint.
		return  _bpmap.begin()->first;
	else
		return 0.;
}

// ---------------------------------------------------------------------------
//	endTime
// ---------------------------------------------------------------------------
//	in seconds
//
double
Partial::endTime( void ) const
{
	if ( _bpmap.size() > 0 )
		//	map iterator is a pair: first is time, 
		//	second is Breakpoint.
		return (--_bpmap.end())->first;
	else
		return 0.;
}

#pragma mark -
#pragma mark envelope access/mutation
// ---------------------------------------------------------------------------
//	insert
// ---------------------------------------------------------------------------
//	Make a copy of bp and insert it at time (seconds),
//	return a pointer to the inserted Breakpoint.
//	If there is already a Breakpoint at time, assign
//	bp to it (copying parameters).
//
//	Could except:
//	allocation of a new Breakpoint could fail, throwing a std::bad__alloc.
//
Breakpoint *
Partial::insert( double time, const Breakpoint & bp )
{
	Breakpoint & pos = _bpmap[ time ];
	pos = bp;
	return & pos;
}

// ---------------------------------------------------------------------------
//	remove
// ---------------------------------------------------------------------------
//	Remove and delete all Breakpoints between start and end (seconds, inclusive),
//	shortening the envelope by (end-start) seconds.
//	Return a pointer to the Breakpoint immediately preceding the 
//	removed time (will be Null if beginning of Partial is removed).
//
//	This could remove all Breakpoints in the Partial without warning!
//	Caller should check for non-zero duration after time removal.
//
//	What kind of return value should this have? I don't want the caller
//	deleting the pointer I return.
//
//
Breakpoint * 
Partial::remove( double tstart, double tend )
{
//	get the order right:
	if ( tstart > tend )
		std::swap( tstart, tend );
		
//	get iterator bounds:
//	lower_bound returns a reference to the lowest
//	position that would be higher than an element
//	having key equal to time:
	std::map< double, Breakpoint >::iterator begin = 
		_bpmap.lower_bound( tstart );
	std::map< double, Breakpoint >::iterator end = 
		_bpmap.lower_bound( tend );
				
//	remember the Breakpoint before the gap, or Null:
	Breakpoint * ret = Null;
	if ( begin != _bpmap.begin() ) {
		std::map< double, Breakpoint >::iterator it = begin;
		--it;
		ret = & it->second;
	}
	
//	remove Breakpoints on the range [begin, end):
	_bpmap.erase( begin, end );
	
//	all Breakpoints later than tend, that is, at positions
//	starting at end, have to be shifted in time to close 
//	the gap:
	double gap = tend - tstart;
	while ( end != _bpmap.end() ) {
		//	insert a copy of this Breakpoint at the new time:
		_bpmap[ end->first - gap ] = end->second;
		
		//	remove this Breakpoint, and incr. iterator:
		_bpmap.erase( end++ );
	}
	
	return ret;
}

// ---------------------------------------------------------------------------
//	findPos (const version)
// ---------------------------------------------------------------------------
//	Return the insertion position for a Breakpoint at
//	the specified time (that is, the position of the first
//	Breakpoint at a time later than the specified time).
//
Partial::const_iterator
Partial::findPos( double time ) const
{
	return _bpmap.lower_bound( time );
}

// ---------------------------------------------------------------------------
//	findPos (non-const version)
// ---------------------------------------------------------------------------
//	Return the insertion position for a Breakpoint at
//	the specified time (that is, the position of the first
//	Breakpoint at a time later than the specified time).
//
Partial::iterator
Partial::findPos( double time )
{
	return _bpmap.lower_bound( time );
}

#pragma mark -
#pragma mark envelope interpolation/extrapolation
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
	std::map< double, Breakpoint >::const_iterator it = findPos( time );
		
	if ( it == _bpmap.begin() ) {
	//	time is before the onset of the Partial:
		return it->second.frequency();
	}
	else if ( it == _bpmap.end() ) {
	//	time is past the end of the Partial:
		return (--it)->second.frequency();
	}
	else {
	//	interpolate between it and its predeccessor
	//	(we checked already that it is not begin):
		const Breakpoint & hi = it->second;
		double hitime = it->first;
		const Breakpoint & lo = (--it)->second;
		double lotime = it->first;
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
	std::map< double, Breakpoint >::const_iterator it = findPos( time );
		
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
		const Breakpoint & hi = it->second;
		double hitime = it->first;
		const Breakpoint & lo = (--it)->second;
		double lotime = it->first;
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
	std::map< double, Breakpoint >::const_iterator it = findPos( time );
		
	//	compute phase:
	//	map iterator is a pair: first is time, 
	//	second is Breakpoint.
	if ( it == _bpmap.begin() ) {
	//	time is before the onset of the Partial:
		double dp = TwoPi * (it->first - time) * it->second.frequency();
		return fmod( it->second.phase() - dp, TwoPi);

	}
	else if (it == _bpmap.end() ) {
	//	time is past the end of the Partial:
	//	( first decrement iterator to get the tail Breakpoint)
		--it;
		
		double dp = TwoPi * (time - it->first) * it->second.frequency();
		return fmod( it->second.phase() + dp, TwoPi );
	}
	else {
	//	interpolate between it and its predeccessor
	//	(we checked already that it is not begin):
		const Breakpoint & hi = it->second;
		double hitime = it->first;
		const Breakpoint & lo = (--it)->second;
		double lotime = it->first;
		double alpha = (time - lotime) / (hitime - lotime);
		double favg = (0.5 * alpha * hi.frequency()) + 
						((1. - (0.5 * alpha)) * lo.frequency());
		double dp = TwoPi * (time - lotime) * favg;
		return fmod( lo.phase() + dp, TwoPi );

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
	std::map< double, Breakpoint >::const_iterator it = findPos( time );
		
	if ( it == _bpmap.begin() ) {
	//	time is before the onset of the Partial:
		return it->second.bandwidth();
	}
	else if (it == _bpmap.end() ) {
	//	time is past the end of the Partial:
		return (--it)->second.bandwidth();
	}
	else {
	//	interpolate between it and its predeccessor
	//	(we checked already that it is not begin):
		const Breakpoint & hi = it->second;
		double hitime = it->first;
		const Breakpoint & lo = (--it)->second;
		double lotime = it->first;
		double alpha = (time - lotime) / (hitime - lotime);
		return (alpha * hi.bandwidth()) + ((1. - alpha) * lo.bandwidth());
	}
}


End_Namespace( Loris )

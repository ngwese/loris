// ===========================================================================
//	PartialIterator.C
//
//	Implementation of Loris::PartialIterator, a const iterator over Partials.
//	PartialIterator and derived classes cannot be used to modify partials,
//	since the subject is a const reference, but they can be used to compute 
//	transformed Partial data on the fly, without creating a new Partial.
//
//	-kel 7 Oct 99
//
// ===========================================================================
#include "LorisLib.h"

#include "Partial.h"
#include "PartialIterator.h"
#include "Breakpoint.h"

Begin_Namespace( Loris )

#pragma mark -
#pragma mark public iterator interface:
// ---------------------------------------------------------------------------
//	construction
// ---------------------------------------------------------------------------
//
PartialIterator::PartialIterator( const Partial & pin ) : 
	_p( & pin ), 
	_cur( pin.head() ) 
{
}

// ---------------------------------------------------------------------------
//	default constructor
// ---------------------------------------------------------------------------
//
PartialIterator::PartialIterator( void ) : 
	_p( Null ), 
	_cur( Null ) 
{
}

// ---------------------------------------------------------------------------
//	advance
// ---------------------------------------------------------------------------
//
void
PartialIterator::advance( void )
{
	if ( _cur != Null ) 
		_cur = _cur->next();
}

// ---------------------------------------------------------------------------
//	reset
// ---------------------------------------------------------------------------
//
void
PartialIterator::reset( const Partial & p )
{
	_p = & p;
	_cur = _p->head();
}

// ---------------------------------------------------------------------------
//	rewind
// ---------------------------------------------------------------------------
//
void
PartialIterator::rewind( void )
{
	_cur = subject().head();
}

// ---------------------------------------------------------------------------
//	atEnd
// ---------------------------------------------------------------------------
//
boolean
PartialIterator::atEnd( void ) const
{
	return _cur == Null;
}

// ---------------------------------------------------------------------------
//	current
// ---------------------------------------------------------------------------
//
const Breakpoint &
PartialIterator::current( void ) const
{
	if ( _cur == Null )
		Throw( InvalidIterator, "Tried to dereference an invalid PartialIterator (current)." );

	return *_cur;
}

// ---------------------------------------------------------------------------
//	subject
// ---------------------------------------------------------------------------
//
const Partial &
PartialIterator::subject( void ) const
{
	if ( _p == Null )
		Throw( InvalidIterator, "Tried to dereference an invalid PartialIterator (subject)." );

	return *_p;
}

#pragma mark -
#pragma mark Partial access:
// ---------------------------------------------------------------------------
//	duration
// ---------------------------------------------------------------------------
//
double
PartialIterator::duration( void ) const
{
	return subject().duration();
}

// ---------------------------------------------------------------------------
//	startTime
// ---------------------------------------------------------------------------
//
double
PartialIterator::startTime( void ) const
{
	return subject().startTime();
}

// ---------------------------------------------------------------------------
//	endTime
// ---------------------------------------------------------------------------
//
double
PartialIterator::endTime( void ) const
{
	return subject().endTime();
}

// ---------------------------------------------------------------------------
//	initialPhase
// ---------------------------------------------------------------------------
//
double
PartialIterator::initialPhase( void ) const
{
	return subject().initialPhase();
}

// ---------------------------------------------------------------------------
//	label
// ---------------------------------------------------------------------------
//
int
PartialIterator::label( void ) const
{
	return subject().label();
}

#pragma mark -
#pragma mark Breakpoint access:
// ---------------------------------------------------------------------------
//	frequency
// ---------------------------------------------------------------------------
//
double
PartialIterator::frequency( void ) const
{
	return current().frequency();
}

// ---------------------------------------------------------------------------
//	amplitude
// ---------------------------------------------------------------------------
//
double
PartialIterator::amplitude( void ) const
{
	return current().amplitude();
}

// ---------------------------------------------------------------------------
//	bandwidth
// ---------------------------------------------------------------------------
//
double
PartialIterator::bandwidth( void ) const
{
	return current().bandwidth();
}

// ---------------------------------------------------------------------------
//	phase
// ---------------------------------------------------------------------------
//
double
PartialIterator::phase( void ) const
{
	return current().phase();
}

// ---------------------------------------------------------------------------
//	time
// ---------------------------------------------------------------------------
//
double
PartialIterator::time( void ) const
{
	return current().time();
}

End_Namespace( Loris )

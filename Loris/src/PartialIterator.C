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
#include "PartialIterator.h"
#include "Partial.h"

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif


#pragma mark -
#pragma mark public iterator interface:
// ---------------------------------------------------------------------------
//	construction
// ---------------------------------------------------------------------------
//
BasicPartialIterator::BasicPartialIterator( const Partial & pin ) : 
	_p( & pin ), 
	_cur( pin.begin() ) 
{
}

// ---------------------------------------------------------------------------
//	default constructor
// ---------------------------------------------------------------------------
//
BasicPartialIterator::BasicPartialIterator( void ) : 
	_p( NULL )
{
}

// ---------------------------------------------------------------------------
//	copy constructor
// ---------------------------------------------------------------------------
//
BasicPartialIterator::BasicPartialIterator( const BasicPartialIterator & other ) : 
	_p( other._p ),
	PartialIterator( other )
{
}

// ---------------------------------------------------------------------------
//	advance
// ---------------------------------------------------------------------------
//
void
BasicPartialIterator::advance( void )
{
	if ( ! atEnd() ) 
		++_cur;
}

// ---------------------------------------------------------------------------
//	reset
// ---------------------------------------------------------------------------
//
void
BasicPartialIterator::reset( const Partial & p )
{
	_p = & p;
	_cur = _p->begin();
}

// ---------------------------------------------------------------------------
//	atEnd
// ---------------------------------------------------------------------------
//
bool
BasicPartialIterator::atEnd( void ) const
{
	return _cur == _p->end();
}

// ---------------------------------------------------------------------------
//	current
// ---------------------------------------------------------------------------
//
const Breakpoint &
BasicPartialIterator::current( void ) const
{
	if ( atEnd() )
		Throw( InvalidIterator, "Tried to dereference an invalid BasicPartialIterator (current)." );

	return *_cur;
}

// ---------------------------------------------------------------------------
//	subject
// ---------------------------------------------------------------------------
//
const Partial &
BasicPartialIterator::subject( void ) const
{
	if ( _p == NULL )
		Throw( InvalidIterator, "Tried to dereference an invalid BasicPartialIterator (subject)." );

	return *_p;
}

#pragma mark -
#pragma mark Partial access:
// ---------------------------------------------------------------------------
//	duration
// ---------------------------------------------------------------------------
//
double
BasicPartialIterator::duration( void ) const
{
	return subject().duration();
}

// ---------------------------------------------------------------------------
//	startTime
// ---------------------------------------------------------------------------
//
double
BasicPartialIterator::startTime( void ) const
{
	return subject().startTime();
}

// ---------------------------------------------------------------------------
//	endTime
// ---------------------------------------------------------------------------
//
double
BasicPartialIterator::endTime( void ) const
{
	return subject().endTime();
}

// ---------------------------------------------------------------------------
//	initialPhase
// ---------------------------------------------------------------------------
//
double
BasicPartialIterator::initialPhase( void ) const
{
	return subject().initialPhase();
}

// ---------------------------------------------------------------------------
//	label
// ---------------------------------------------------------------------------
//
int
BasicPartialIterator::label( void ) const
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
BasicPartialIterator::frequency( void ) const
{
	return current().frequency();
}

// ---------------------------------------------------------------------------
//	amplitude
// ---------------------------------------------------------------------------
//
double
BasicPartialIterator::amplitude( void ) const
{
	return current().amplitude();
}

// ---------------------------------------------------------------------------
//	bandwidth
// ---------------------------------------------------------------------------
//
double
BasicPartialIterator::bandwidth( void ) const
{
	return current().bandwidth();
}

// ---------------------------------------------------------------------------
//	phase
// ---------------------------------------------------------------------------
//
double
BasicPartialIterator::phase( void ) const
{
	return current().phase();
}

// ---------------------------------------------------------------------------
//	time
// ---------------------------------------------------------------------------
//
double
BasicPartialIterator::time( void ) const
{
	return _cur.time();
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

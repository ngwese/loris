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

#include <algorithm>

Begin_Namespace( Loris )

#pragma mark -
#pragma mark construction

// ---------------------------------------------------------------------------
//	Partial constructor
// ---------------------------------------------------------------------------
//
Partial::Partial( void ) :
	mHead( Null ),
	mTail( Null ),
	mLabel( 0L )
{
}

// ---------------------------------------------------------------------------
//	Partial destructor
// ---------------------------------------------------------------------------
//	Partial is responsible for its Breakpoint envelope, and deletes all of
//	its breakpoints when destroyed. 
//
Partial::~Partial( void )
{
	deleteEnvelope();
}

// ---------------------------------------------------------------------------
//	Partial copy constructor
// ---------------------------------------------------------------------------
//	Make a deep copy (i.e. copy the envelope). 
//
//	The envelope copy could except, copyEnvelope() will 
//	delete the envelope if it catches an exception, so 
//	exceptions can just propogate up.
//
Partial::Partial( const Partial & other ) :
	mHead( Null ),
	mTail( Null ),
	mLabel( other.mLabel )
{
	copyEnvelope( other.mHead );
}

// ---------------------------------------------------------------------------
//	operator = (assignment)
// ---------------------------------------------------------------------------
//	Make a deep copy (i.e. copy the envelope). 
//
//	The envelope copy could except, copyEnvelope() will 
//	delete the envelope if it catches an exception, so 
//	exceptions can just propogate up.
//
Partial & 
Partial::operator=( const Partial & other )
{
	//	don't assign self to self
	if ( &other != this ) {	
		deleteEnvelope();
		copyEnvelope( other.head() );	//	cleans up after itself on exception
		setLabel( other.label() );
	}
		
	return *this;
}
	

#pragma mark -
#pragma mark envelope parameter shortcuts
// ---------------------------------------------------------------------------
//	initialPhase
// ---------------------------------------------------------------------------
//	in radians
//
Double
Partial::initialPhase( void ) const
{
	if ( head() != Null )
		return head()->phase();
	else
		return 0.;
}

// ---------------------------------------------------------------------------
//	startTime
// ---------------------------------------------------------------------------
//	in seconds
//
Double
Partial::startTime( void ) const
{
	if ( head() != Null )
		return head()->time();
	else
		return 0.;
}

// ---------------------------------------------------------------------------
//	endTime
// ---------------------------------------------------------------------------
//	in seconds
//
Double
Partial::endTime( void ) const
{
	if ( tail() != Null )
		return tail()->time();
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
//	allocation of a new Breakpoint could fail, throwing a LowMemException.
//
Breakpoint *
Partial::insert( Double time, const Breakpoint & bp )
{
	Breakpoint * pos = find( time );
	
	if ( pos != Null && pos->time() == time ) {
		//	replace:
		*pos = bp;
		return pos;
	}
	else {
		//	insert copy:
		Breakpoint * newp = Breakpoint::Clone( bp );
		if ( pos == Null ) {
			//	insert at head:
			newp->linkTo( head() );
			mHead = newp;
			newp->setTime( time );	
			
			//	if this is the only Breakpoint, 
			//	fix the tail pointer too:
			if ( tail() == Null )
				mTail = newp;
		}
		else {
			//	insert after pos:
			newp->linkTo( pos->next() );
			pos->linkTo( newp );
			newp->setTime( time );	
			
			//	check to see if insert was after tail:
			if ( pos == tail() )	//	pointer comparison
				mTail = newp;
		}
		
		checkEnvelope();
		
		return newp;
	}
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
//	This could except if things get screwed up, and the scooting 
//	generates a RuntimeException. In this case, the Partial is probably
//	garbage, so the exception should be passed up to the caller. 
//
Breakpoint * 
Partial::remove( Double start, Double end )
{
//	get the order right:
	if ( start > end )
		std::swap( start, end );
		
//	maybe we can do no work:
	if ( start > endTime() )
		return tail();

//	find the beginning and end of the removal:
	Breakpoint * beforeStart = find( start );	//	could be Null, but not tail
	Assert( beforeStart != tail() );
	
	Breakpoint * afterStart = 
		( beforeStart == Null ) ? ( head() ) : ( beforeStart->next() );//	cannot be Null
	Assert( afterStart != Null );
	
	Breakpoint * beforeEnd = find( end );		//	cannot be Null
	Assert( beforeEnd != Null );
	
	Breakpoint * afterEnd = beforeEnd->next(); 	//	could be Null
	
	Double timeRemoved = end - start;
	
//	remove and delete Breakpoints:
	while( afterStart != beforeEnd ) {
		afterStart = afterStart->next();
		Breakpoint::Destroy( afterStart->prev() );
	}
	Breakpoint::Destroy( afterStart );

//	link across the gap, checking ends:
	if ( beforeStart != Null ) 
		beforeStart->linkTo( afterEnd );
	else {
		mHead = afterEnd;
		if ( mHead != Null )
			mHead->setPrev( Null );
	}

//	scoot Breakpoints to fill gap, if necessary:	
	if ( afterEnd != Null ) {
		try {
			scoot( afterEnd, Null, - timeRemoved );
		}
		catch ( RuntimeException & ex ) {
			ex.append( "Partial envelope is probably bogus." );
			throw;
		}
	
	}
	else {
		mTail = beforeStart;
		if ( mTail != Null )
			mTail->setNext( Null );
	}
	
	checkEnvelope();
	
	return beforeStart;
	
}

// ---------------------------------------------------------------------------
//	find (const version)
// ---------------------------------------------------------------------------
//	Return a pointer to the Breakpoint immediately preceding
//	the specified time (will be Null if time < startTime).
//
const Breakpoint * 
Partial::find( Double time ) const
{
//	check the easy case first:
	if ( time >= endTime() )
		return tail();
		
	const Breakpoint * p = head();
	while ( p->time() <= time ) {
		p = p->next();
		Assert( p != Null );	//	checked for this case already
	}
	return p->prev(); 	//	may be Null
}

// ---------------------------------------------------------------------------
//	find (non-const version)
// ---------------------------------------------------------------------------
//	Return a pointer to the Breakpoint immediately preceding
//	the specified time (will be Null if time < startTime).
//
Breakpoint * 
Partial::find( Double time )
{
//	check the easy case first:
	if ( time >= endTime() )
		return tail();
		
	Breakpoint * p = head();
	while ( p->time() <= time ) {
		p = p->next();
		Assert( p != Null );	//	checked for this case already
	}
	return p->prev(); 	//	may be Null
}

#pragma mark -
#pragma mark envelope manipulation helpers
// ---------------------------------------------------------------------------
//	copyEnvelope
// ---------------------------------------------------------------------------
//	This could except, callers should be prepared to handle exceptions
//	like memory allocation failures. If an exception is caught, the 
//	bogus envelope will be deleted.
//
void
Partial::copyEnvelope( const Breakpoint * h ) 
{
	deleteEnvelope();	//	just in case 
	
	try {
		//insertAtHead( h->time(), Breakpoint::Clone( * h ) );
		//insertAtTail( h->time(), head() );
		
		for ( ; h != Null; h = h->next() )
			insert( h->time(), *h );
		
	}
	catch( LowMemException & ) {	//	catch a memory error exception here
		//	the envelope is bogus, delete it and pass the
		//	exception up to the caller
		deleteEnvelope();
		throw;
	}
}

// ---------------------------------------------------------------------------
//	deleteEnvelope
// ---------------------------------------------------------------------------
//	Delete all Breakpoints in the envelope and set the head to NULL.
//
void
Partial::deleteEnvelope( void ) 
{
	Breakpoint * bp = head();
	if ( bp == Null )
		return;
		
	while( bp->next() != Null ) {
		bp = bp->next();
		Breakpoint::Destroy( bp->prev() );
	}
	Breakpoint::Destroy( bp );

	mHead = mTail = Null;
}

// ---------------------------------------------------------------------------
//	insertAtHead
// ---------------------------------------------------------------------------
//
void
Partial::insertAtHead( Double time, Breakpoint * bp )
{
	//	sanity check:
	Assert( head() == Null || time < head()->time() );
	
	bp->linkTo( head() );
	mHead = bp;
	bp->setTime( time );	
	
	//	if this is the only Breakpoint, 
	//	fix the tail pointer too:
	if ( tail() == Null )
		mTail = bp;
}

// ---------------------------------------------------------------------------
//	insertAtTail
// ---------------------------------------------------------------------------
//
void
Partial::insertAtTail( Double time, Breakpoint * bp )
{
	//	sanity check:
	Assert( mTail == Null || time > mTail->time() );
	
	if ( mTail != Null )
		mTail->linkTo( bp );
	mTail = bp;	
	bp->setTime( time );	
}

// ---------------------------------------------------------------------------
//	insertBefore
// ---------------------------------------------------------------------------
//
void
Partial::insertBefore( Breakpoint * beforeMe, Double time, Breakpoint * bp )
{
	//	sanity check:
	Assert( time < beforeMe->time() );	//	else not before me
	Assert( beforeMe->prev() != Null );	//	else should insertAtHead()
	
	beforeMe->prev()->linkTo( bp );
	bp->linkTo( beforeMe );
	bp->setTime( time );	
}

// ---------------------------------------------------------------------------
//	scoot
// ---------------------------------------------------------------------------
//	Scoot all Breakpoints in [start, end) by scootBy seconds.
//
//	Excepts if the scooting violates the time order of the Breakpoints.
//	Does _not_ mess up the envelope in this case, so this is a 
//	recoverable condition, unless the envelope is goofed up already.
//
void 
Partial::scoot( Breakpoint * start, Breakpoint * end, Double scootBy )
{
	while( start != end ) {
		Assert( start != Null );
		
		//	check for Breakpoint order violation before changing anything:
		if ( start->prev() != Null && start->time() + scootBy < start->prev()->time() )
			Throw( RuntimeException, "Breakpoint time order violation in Partial::scoot()." );
	
		//	okay to scoot:
		start->setTime( start->time() + scootBy );
		start = start->next();
	}
}

#pragma mark -
#pragma mark debugging
// ---------------------------------------------------------------------------
//	checkEnvelope
// ---------------------------------------------------------------------------
//
void 
Partial::checkEnvelope( void  ) const
{
	for ( const Breakpoint * p  = head(); p != tail(); p = p->next() ) {
		Assert( p->next() != Null );
		Assert( p->time() < p->next()->time() );
		Assert( p->next()->prev() == p );
	}
	Assert( head()->prev() == Null );
	Assert( tail()->next() == Null );
}

End_Namespace( Loris )

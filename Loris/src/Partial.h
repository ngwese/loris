#ifndef __Loris_partial__
#define __Loris_partial__

// ===========================================================================
//	Partial.h
//
//	Class definintion for Loris::Partial.
//	
//	Loris Partials represent reassigned bandwidth-enhanced model components.
//	A Partial consists of a chain of Breakpoints describing the time-varying
//	frequency, amplitude, and bandwidth (noisiness) of the component.
//
//	-kel 16 Aug 99
//
// ===========================================================================

#include "LorisLib.h"

Begin_Namespace( Loris )

class Breakpoint;

// ---------------------------------------------------------------------------
//	class Partial
//
//	Definition of class of objects representing reassigned bandwidth-enhanced 
//	model components in Loris. Partials are described by the Partial parameter 
//	(frequency, amplitude, bandwidth) envelope and a 4-byte label. 
//
//	Leaf class, do not subclass.
//
class Partial
{
//	-- public interface --
public:
//	construction:
	Partial( void );
	~Partial( void );

//	copy and assignment perform deep copy:
	Partial( const Partial & other );
	Partial & operator =( const Partial & other );
	
//	Breakpoint envelope access (const and non-const):
	const Breakpoint * head( void ) const { return mHead; }
	const Breakpoint * tail( void ) const { return mTail; }
	
	Breakpoint * head( void ) { return mHead; }
	Breakpoint * tail( void ) { return mTail; }
	
//	label access/mutation:
	Int label( void ) const { return mLabel; }
	void setLabel( Int l ) { mLabel = l; }
	
//	shortcuts to envelope parameters:
	Double initialPhase( void ) const;
	Double startTime( void ) const;
	Double endTime( void ) const;
	Double duration( void ) const { return endTime() - startTime(); }
	
//	Breakpoint insertion:
//	Make a copy of bp and insert it at time (seconds),
//	return a pointer to the inserted Breakpoint.
	Breakpoint * insert( Double time, const Breakpoint & bp );

//	Breakpoint removal:
//	Remove and delete all Breakpoints between start and end (seconds, inclusive),
//	shortening the envelope by (end-start) seconds.
//	Return a pointer to the Breakpoint immediately preceding the 
//	removed time (will be Null if beginning of Partial is removed).
	Breakpoint * remove( Double start, Double end );
	
//	Breakpoint find:
//	Return a pointer to the Breakpoint immediately preceding
//	the specified time (will be Null if time < startTime).
//	(const and non-const versions)
	const Breakpoint * find( Double time ) const;
	Breakpoint * find( Double time );
	
	
//	debugging:
	void checkEnvelope( void ) const;
	
//	-- private implementation --
private:
//	envelope manipulation helpers:
	void copyEnvelope( const Breakpoint * h );
	void deleteEnvelope( void );
	void insertAtHead( Double time, Breakpoint * bp );
	void insertAtTail( Double time, Breakpoint * bp );
	void insertBefore( Breakpoint * beforeMe, Double time, Breakpoint * bp );
	void scoot( Breakpoint * start, Breakpoint * end, Double scootBy );
	
//	-- instance variables --
//	envelope:
	Breakpoint * mHead;
	Breakpoint * mTail;
	
//	label:
	Int mLabel;

};	//	end of class Partial



End_Namespace( Loris )

#endif	// ndef __Loris_partial__
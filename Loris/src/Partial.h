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
#include "Exception.h"

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
	const Breakpoint * head( void ) const { return _head; }
	const Breakpoint * tail( void ) const { return _tail; }
	
	Breakpoint * head( void ) { return _head; }
	Breakpoint * tail( void ) { return _tail; }
	
//	label access/mutation:
	int label( void ) const { return _label; }
	void setLabel( int l ) { _label = l; }
	
//	shortcuts to envelope parameters:
	double initialPhase( void ) const;
	double startTime( void ) const;
	double endTime( void ) const;
	double duration( void ) const { return endTime() - startTime(); }
	
//	Breakpoint insertion:
//	Make a copy of bp and insert it at time (seconds),
//	return a pointer to the inserted Breakpoint.
	Breakpoint * insert( double time, const Breakpoint & bp );

//	Breakpoint removal:
//	Remove and delete all Breakpoints between start and end (seconds, inclusive),
//	shortening the envelope by (end-start) seconds.
//	Return a pointer to the Breakpoint immediately preceding the 
//	removed time (will be Null if beginning of Partial is removed).
	Breakpoint * remove( double start, double end );
	
//	Breakpoint find:
//	Return a pointer to the Breakpoint immediately preceding
//	the specified time (will be Null if time < startTime).
//	(const and non-const versions)
	const Breakpoint * find( double time ) const;
	Breakpoint * find( double time );
	
	
//	debugging:
	void checkEnvelope( void ) const;
	
//	-- private implementation --
private:
//	envelope manipulation helpers:
	void copyEnvelope( const Breakpoint * h );
	void deleteEnvelope( void );
	void insertAtHead( double time, Breakpoint * bp );
	void insertAtTail( double time, Breakpoint * bp );
	void insertBefore( Breakpoint * beforeMe, double time, Breakpoint * bp );
	void scoot( Breakpoint * start, Breakpoint * end, double scootBy );
	
//	-- instance variables --
//	envelope:
	Breakpoint * _head;
	Breakpoint * _tail;
	
//	label:
	int _label;

};	//	end of class Partial

// ---------------------------------------------------------------------------
//	class InvalidPartial
//
//	Class of exceptions thrown when a Partial is found to be badly configured
//	or otherwise invalid.
//
class InvalidPartial : public InvalidObject
{
public: 
	InvalidPartial( const string & str, const string & where = "" ) : 
		InvalidObject( string("Invalid Partial -- ").append( str ), where ) {}
		
};	//	end of class ImportException


End_Namespace( Loris )

#endif	// ndef __Loris_partial__
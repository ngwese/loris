#ifndef __Loris_notifier__
#define __Loris_notifier__

// ===========================================================================
//	Notifier.h
//	
//	Class definition for Loris::Notifier, a base class for notification
//	objects with a default implementation that uses the console for 
//	reporting.
//
//	Still have to figure out a way to allow this to be subclassed.
//
//	-kel 9 Sept 99
//
// ===========================================================================


#include "LorisLib.h"

#if !defined(USE_DEPRECATED_HEADERS)
	#include <sstream>
	using std::stringstream;
#else
	#include <strstream.h>
	typedef strstream stringstream;
#endif

#include <string>
using std::string;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class Notifier
//
//
class Notifier
{
//	-- public interface --
public:
//	construction:
	Notifier( string s = "" );
	
//	virtual destructor so Notifier can be subclassed:
	virtual ~Notifier( void );	
	
//	reporting:
//	Derived classes can override the reporting behavior to put the 
//	notification somewhere other than standard-out.
	virtual void report( void );

//	implemented with a strstream, which has everything
//	we want except stability:
	stringstream ss;
	
};	//	end of class Notifier

//	prototype for a one-shot notifier:
void notify( string s );

//	streaming operator:
template< class T >
Notifier & 
operator << ( Notifier & note, const T & thing )
{
	note.ss << thing;
	return note;
}

//	lousy debugging macro:
#if defined(Debug_Loris)
	inline void Debug( string s ) { notify( s ); }
#else
	inline void Debug( string ) {}
#endif

End_Namespace( Loris )

#endif	// ndef __Loris_notifier__
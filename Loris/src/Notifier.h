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
	Notifier( const std::string s = "" );
	
//	virtual destructor so Notifier can be subclassed:
	virtual ~Notifier( void );	
	
//	reporting:
//	Derived classes can override the reporting behavior to put the 
//	notification somewhere other than standard-out.
	virtual void report( void );

//	streaming (onto) operator:
	template< class T >
	Notifier & 
	operator << ( const T & thing )
	{
		ss << thing;
		return *this;
	}
	
//	ostream implementation:
	Notifier & put( char c );
	Notifier & write( const char * cstr, long count );
	Notifier & flush( void );
	
private:
//	implemented with a stringstream, which has everything
//	we want except stability:
	stringstream ss;
	
};	//	end of class Notifier

// ---------------------------------------------------------------------------
//	class Debugger
//
//
class Debugger : public Notifier
{
//	-- public interface --
public:
//	construction:
	Debugger( const std::string s = "" ) : Notifier( s ) {}
	
	

};	//	end of class Debugger

// ---------------------------------------------------------------------------
//	prototype for a one-shot notifier:
//
void notify( const std::string s );

// ---------------------------------------------------------------------------
//	prototype for a one-shot error notifier:
//	This one displays its message and aborts.
//
void fatalError( const std::string s );

// ---------------------------------------------------------------------------
//	lousy debugging macro, do better!
#if defined(Debug_Loris)
	inline void Debug( const std::string s ) { notify( s ); }
#else
	inline void Debug( const std::string ) {}
#endif

End_Namespace( Loris )

#endif	// ndef __Loris_notifier__
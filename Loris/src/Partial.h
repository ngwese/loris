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
#include "Exception.h"
#include "Breakpoint.h"
#include <map>
#include <list>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

class PartialIterator;
class PartialConstIterator;

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
//	-- instance variables --
	std::map< double, Breakpoint > _bpmap;	//	Breakpoint envelope:
	int _label;		//	label:

//	-- public interface --
public:
//	construction:
	Partial( void );
	Partial( const Partial & other );
	~Partial( void );
	
//	access:
	int label( void ) const { return _label; }
	double initialPhase( void ) const;
	double startTime( void ) const;
	double endTime( void ) const;
	double duration( void ) const;
	
//	mutation:
	void setLabel( int l ) { _label = l; }
	
//	iterator generation:
	PartialIterator begin( void );
	PartialIterator end( void );
	PartialConstIterator begin( void ) const;
	PartialConstIterator end( void ) const;
	
//	Breakpoint insertion:
//	Make a copy of bp and insert it at time (seconds),
//	return a pointer to the inserted Breakpoint.
	PartialIterator insert( double time, const Breakpoint & bp );

//	Return the insertion position for a Breakpoint at
//	the specified time (that is, the position of the first
//	Breakpoint at a time later than the specified time).
	PartialIterator findPos( double time );
	PartialConstIterator findPos( double time ) const;
	
//	Its nice to be able to find out how many Breakpoints
//	there are:
	long countBreakpoints( void ) const { return _bpmap.size(); }
	
//	partial envelope interpolation/extrapolation:
//	Return the interpolated value of a partial parameter at
//	the specified time. At times beyond the ends of the
//	Partial, frequency and bandwidth hold their boundary values,
//	amplitude is zero, and phase is computed from frequency.
//	There is of sensible definition for any of these for Partials
//	having no Breakpoints, so they except (InvalidPartial) under 
//	that condition.
	double frequencyAt( double time ) const;
	double amplitudeAt( double time ) const;
	double bandwidthAt( double time ) const;
	double phaseAt( double time ) const;
	
//	static member for making sure that all algorithms
//	that fade Partials in and out use the same fade time:
	static double FadeTime( void );
	
#if Debug_Loris
	static long DebugCounter;
#endif
};	//	end of class Partial

// ---------------------------------------------------------------------------
//	class PartialIterator
//
//	This will be called PartialIterator when the other thing called 
//	PartialIterator is renamed and reimplemented as PartialView or
//	EnvelopeView.
//	
//	This is pretty much just a wrapper for the nasty-looking map<>
//	iterators.
//
//	Non-const iterator for the Loris::Partial Breakpoint map. Wraps
//	the non-const iterator for std::map< double, Breakpoint >.
//
class PartialIterator
{
//	-- instance variables --
	std::map< double, Breakpoint >::iterator _iter;
	
//	-- public interface --
public:
//	construction:
//	(allow compiler to generate copy, assignment, and destruction):
	PartialIterator( void ) {}
	
//	iteration:
	PartialIterator& operator ++ () { ++_iter; return *this; }
	PartialIterator operator ++ ( int ) { PartialIterator temp(*this); ++(*this); return temp; }
	PartialIterator& operator -- () { --_iter; return *this; }
	PartialIterator operator -- ( int ) { PartialIterator temp(*this); --(*this); return temp; }
	
//	derference:
	const Breakpoint & operator * ( void ) const { return _iter->second; }
	const Breakpoint * operator -> ( void ) const { return &(_iter->second); }
	Breakpoint & operator * ( void ) { return _iter->second; }
	Breakpoint * operator -> ( void ) { return &(_iter->second); }
	
//	time access (not available through Breakpoint):
	double time( void ) const { return _iter->first; }	

//	comparison:
	friend bool operator == ( const PartialIterator & lhs, const PartialIterator & rhs )
		{ return lhs._iter == rhs._iter; }
	friend bool operator != ( const PartialIterator & lhs, const PartialIterator & rhs )
		{ return lhs._iter != rhs._iter; }
	
//	-- private implementation --
private:
	//	construction by Partial from a map<> iterator:
	PartialIterator( const std::map< double, Breakpoint >::iterator & it ) :
		_iter(it) {}

	friend class Partial;
	friend class PartialConstIterator;	//	for const construction from non-const
	
};	//	end of class PartialIterator

// ---------------------------------------------------------------------------
//	class PartialConstIterator
//
//	Const iterator for the Loris::Partial Breakpoint map. Wraps
//	the const iterator for std::map< double, Breakpoint >.
//
//	Get rid of advance()?
//	can't get rid of derference, but could implement Breakpoint interface
//	so that parameter access would be uniform.
//
class PartialConstIterator
{
//	-- instance variables --
	std::map< double, Breakpoint >::const_iterator _iter;
	
//	-- public interface --
public:
//	construction:
//	(allow compiler to generate copy, assignment, and destruction):
	PartialConstIterator( void ) {}
	PartialConstIterator( const PartialIterator & other ) : _iter( other._iter ) {}
	
//	iteration:
	PartialConstIterator& operator ++ () { ++_iter; return *this; }
	PartialConstIterator operator ++ ( int ) { PartialConstIterator temp(*this); ++(*this); return temp; }
	PartialConstIterator& operator -- () { --_iter; return *this; }
	PartialConstIterator operator -- ( int ) { PartialConstIterator temp(*this); --(*this); return temp; }
	void advance( void ) { ++(*this); }
	
//	derference:
	const Breakpoint & operator * ( void ) const { return _iter->second; }
	const Breakpoint * operator -> ( void ) const { return &(_iter->second); }

//	time access (not available through Breakpoint):
	double time( void ) const { return _iter->first; }	

//	comparison:
	friend bool operator == ( const PartialConstIterator & lhs, const PartialConstIterator & rhs )
		{ return lhs._iter == rhs._iter; }
	friend bool operator != ( const PartialConstIterator & lhs, const PartialConstIterator & rhs )
		{ return lhs._iter != rhs._iter; }
	
//	-- private implementation --
private:
	//	construction by Partial from a map<> iterator:
	PartialConstIterator( const std::map< double, Breakpoint >::const_iterator & it ) :
		_iter(it) {}

	friend class Partial;
	
};	//	end of class PartialConstIterator

// ---------------------------------------------------------------------------
//	class InvalidPartial
//
//	Class of exceptions thrown when a Partial is found to be badly configured
//	or otherwise invalid.
//
class InvalidPartial : public InvalidObject
{
public: 
	InvalidPartial( const std::string & str, const std::string & where = "" ) : 
	InvalidObject( std::string("Invalid Partial -- ").append( str ), where ) {}
		
};	//	end of class InvalidPartial

// ---------------------------------------------------------------------------
//	PartialList
//
//	Standard container for Partials in Loris.
//
typedef std::list< Partial > PartialList;

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef __Loris_partial__

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

class Jackson;
class JacksonConst;

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
private:
//	Breakpoint envelope:
	std::map< double, Breakpoint > _bpmap;
	
//	label:
	int _label;

//	-- public interface --
public:
//	construction:
//	(allow compiler to generate copy, assignment, and destruction):
	Partial( void );
	
//	access:
	int label( void ) const { return _label; }
	double initialPhase( void ) const;
	double startTime( void ) const;
	double endTime( void ) const;
	double duration( void ) const { return endTime() - startTime(); }
	
//	mutation:
	void setLabel( int l ) { _label = l; }
	
//	iterator generation:
	Jackson begin( void );
	Jackson end( void );
	JacksonConst begin( void ) const;
	JacksonConst end( void ) const;
	
//	Breakpoint insertion:
//	Make a copy of bp and insert it at time (seconds),
//	return a pointer to the inserted Breakpoint.
	Jackson insert( double time, const Breakpoint & bp );

//	Return the insertion position for a Breakpoint at
//	the specified time (that is, the position of the first
//	Breakpoint at a time later than the specified time).
	Jackson findPos( double time );
	JacksonConst findPos( double time ) const;
	
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
	
};	//	end of class Partial

// ---------------------------------------------------------------------------
//	class Jackson
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
class Jackson
{
//	-- instance variables --
	std::map< double, Breakpoint >::iterator _iter;
	
//	-- public interface --
public:
//	construction:
//	(allow compiler to generate copy, assignment, and destruction):
	Jackson( void ) {}
	
//	iteration:
	Jackson& operator ++ () { ++_iter; return *this; }
	Jackson operator ++ ( int ) { Jackson temp(*this); ++(*this); return temp; }
	Jackson& operator -- () { --_iter; return *this; }
	Jackson operator -- ( int ) { Jackson temp(*this); --(*this); return temp; }
	
//	derference:
	const Breakpoint & operator * ( void ) const { return _iter->second; }
	const Breakpoint * operator -> ( void ) const { return &(_iter->second); }
	Breakpoint & operator * ( void ) { return _iter->second; }
	Breakpoint * operator -> ( void ) { return &(_iter->second); }
	
//	time access (not available through Breakpoint):
	double time( void ) const { return _iter->first; }	

//	comparison:
	friend bool operator == ( const Jackson & lhs, const Jackson & rhs )
		{ return lhs._iter == rhs._iter; }
	friend bool operator != ( const Jackson & lhs, const Jackson & rhs )
		{ return lhs._iter != rhs._iter; }
	
//	-- private implementation --
private:
	//	construction by Partial from a map<> iterator:
	Jackson( const std::map< double, Breakpoint >::iterator & it ) :
		_iter(it) {}

	friend class Partial;
	friend class JacksonConst;	//	for const construction from non-const
	
};	//	end of class Jackson

// ---------------------------------------------------------------------------
//	class JacksonConst
//
//	Const iterator for the Loris::Partial Breakpoint map. Wraps
//	the const iterator for std::map< double, Breakpoint >.
//
class JacksonConst
{
//	-- instance variables --
	std::map< double, Breakpoint >::const_iterator _iter;
	
//	-- public interface --
public:
//	construction:
//	(allow compiler to generate copy, assignment, and destruction):
	JacksonConst( void ) {}
	JacksonConst( const Jackson & other ) : _iter( other._iter ) {}
	
//	iteration:
	JacksonConst& operator ++ () { ++_iter; return *this; }
	JacksonConst operator ++ ( int ) { JacksonConst temp(*this); ++(*this); return temp; }
	JacksonConst& operator -- () { --_iter; return *this; }
	JacksonConst operator -- ( int ) { JacksonConst temp(*this); --(*this); return temp; }
	void advance( void ) { ++(*this); }
	
//	derference:
	const Breakpoint & operator * ( void ) const { return _iter->second; }
	const Breakpoint * operator -> ( void ) const { return &(_iter->second); }

//	time access (not available through Breakpoint):
	double time( void ) const { return _iter->first; }	

//	comparison:
	friend bool operator == ( const JacksonConst & lhs, const JacksonConst & rhs )
		{ return lhs._iter == rhs._iter; }
	friend bool operator != ( const JacksonConst & lhs, const JacksonConst & rhs )
		{ return lhs._iter != rhs._iter; }
	
//	-- private implementation --
private:
	//	construction by Partial from a map<> iterator:
	JacksonConst( const std::map< double, Breakpoint >::const_iterator & it ) :
		_iter(it) {}

	friend class Partial;
	
};	//	end of class JacksonConst

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
		
#if defined(__sgi) && ! defined(__GNUC__)
//	copying:
//	(exception objects are copied once when caught by reference,
//	or twice when caught by value)
//	This should be generated automatically by the compiler.
	InvalidPartial( const InvalidPartial & other ) : InvalidObject( other ) {}
#endif // lame compiler
		
};	//	end of class InvalidPartial

// ---------------------------------------------------------------------------
//	PartialList
//
//	Standard container for Partials in Loris.
//
typedef std::list< Partial > PartialList;

// ---------------------------------------------------------------------------
//	class PartialCollector
//
//	Many function objects in Loris generate Partials.
//	The Partials are stored in a (STL) list that is accessible to
//	clients. Any Partials remaining in the list are destroyed
//	with the object. Partials can be transfered from one list to 
//	another _without_ copying using list::splice().
//
//	Mixin class.
//	Should this be somewhere else?
//	Should this even exist?
//
class PartialCollector
{
//	-- instance variables --
	PartialList _partials;
			
//	protected construction 
//	(base class only, cannot instantiate)
protected:
	PartialCollector( void ) {}
		
	//	compiler-generated copy constructor and assignment
	//	are adequate, but need to be protected:
	PartialCollector( const PartialCollector & other ) :
		_partials( other._partials ) {}

public:
	//	virtual constructor for subclassing:
	virtual ~PartialCollector( void ) {}

	//	access:
	PartialList & partials( void ) { return _partials; }
	const PartialList & partials( void ) const { return _partials; }

//	-- unimplemented --
	PartialCollector & operator= ( const PartialCollector & other );

};	//	end of mixin class PartialCollector

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef __Loris_partial__
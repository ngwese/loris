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
#include "Breakpoint.h"
#include <map>
#include <list>

Begin_Namespace( Loris )

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
	
//	destructor is not virtual, this is not a base 
//	class, let the compiler generate it:
	//~Partial( void );

//	copy and assignment can be compiler-generated:
	//Partial( const Partial & other );
	//Partial & operator =( const Partial & other );
		
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
	Breakpoint * remove( double tstart, double tend );
	
//	partial envelope interpolation/extrapolation:
//	Return the interpolated value of a partial parameter at
//	the specified time. At times beyond the ends of the
//	Partial, frequency and bandwidth hold their boundary values,
//	amplitude is zero, and phase is computed from frequency.
//	There is of sensible definition for any of these for Partials
//	having no Breakpoints, so they except (InvalidObject) under 
//	that condition.
	double frequencyAt( double time ) const;
	double amplitudeAt( double time ) const;
	double bandwidthAt( double time ) const;
	double phaseAt( double time ) const;
	
//	iterator access:
//	Breakpoints are stored in a map, keyed by the time (in seconds).
//	The only safe way to access the collection diirectly is through
//	iterators on that map: pointers to members of the collection are
//	too dangerous, and references don't allow the possibility of
//	indicating and out-of-bounds value. Moreover, although the 
//	iterators are somewhat inconvenient, one cannot step through the
//	envelope using raw Breakpoints, as they store neither time nor
//	linkage to neighboring Breakpoints. For most applications, the 
//	proxy-iterator class PartialIterator should be used instead of
//	using the Partial::iterators directly.
//
//	Is it possible to deny this access to everyone except the
//	PartialIterator (EnvelopeView) class?
//
	typedef std::map< double, Breakpoint >::iterator iterator;
	typedef std::map< double, Breakpoint >::const_iterator const_iterator;
	
	iterator begin( void ) { return _bpmap.begin(); }
	iterator end( void ) { return _bpmap.end(); }
	const_iterator begin( void ) const { return _bpmap.begin(); }
	const_iterator end( void ) const { return _bpmap.end(); }
	
//	Return the insertion position for a Breakpoint at
//	the specified time (that is, the position of the first
//	Breakpoint at a time later than the specified time).
	iterator findPos( double time );
	const_iterator findPos( double time ) const;
		
//	-- instance variables --
private:
//	envelope:
	std::map< double, Breakpoint > _bpmap;
	
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
//
class PartialCollector
{
//	protected construction 
//	(base class only, cannot instantiate)
protected:
	PartialCollector( void ) {}
		
	//	compiler-generated copy constructor and assignment
	//	are adequate, but need to be protected:
	PartialCollector( const PartialCollector & other ) :
		_partials( other._partials ) {}

private:
	//	not defined:			
	PartialCollector & operator= ( const PartialCollector & );
	
public:
	//	virtual constructor for subclassing:
	virtual ~PartialCollector( void ) {}

	//	access:
	std::list< Partial > & partials( void ) { return _partials; }
	const std::list< Partial > & partials( void ) const { return _partials; }
	
	//	iterator types:
	typedef std::list< Partial >::iterator partial_iterator;
	typedef std::list< Partial >::const_iterator const_partial_iterator;

//	-- instance variables --
//	(should be private)
protected:
	std::list< Partial > _partials;
			
};	//	end of mixin class PartialCollector

End_Namespace( Loris )

#endif	// ndef __Loris_partial__
#ifndef INCLUDE_PARTIAL_H
#define INCLUDE_PARTIAL_H
/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2004 by Kelly Fitz and Lippold Haken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * Partial.h
 *
 * Definition of class Loris::Partial, and definitions and implementations of
 * classes of const and non-const iterators over Partials, and the exception
 * class InvalidPartial, thrown by some Partial members when invoked on a 
 * degenerate Partial having no Breakpoints.
 *
 * Kelly Fitz, 16 Aug 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "Breakpoint.h"
#include "Exception.h"

#include <map>
//#include <utility>
//#include <vector>

//	begin namespace
namespace Loris {

class Partial_Iterator;
class Partial_ConstIterator;

// ---------------------------------------------------------------------------
//	class Partial
//
//	An instance of class Partial represents a single component in the
//	reassigned bandwidth-enhanced additive model. A Partial consists of a
//	chain of Breakpoints describing the time-varying frequency, amplitude,
//	and bandwidth (or noisiness) envelopes of the component, and a 4-byte
//	label. The Breakpoints are non-uniformly distributed in time. For more
//	information about Reassigned Bandwidth-Enhanced Analysis and the
//	Reassigned Bandwidth-Enhanced Additive Sound Model, refer to the Loris
//	website: www.cerlsoundgroup.org/Loris/.
//	
//	The constituent time-tagged Breakpoints are accessible through
//	Partial:iterator and Partial::const_iterator interfaces.
//	These iterator classes implement the interface for bidirectional
//	iterators in the STL, including pre and post-increment and decrement,
//	and dereferencing. Dereferencing a Partial::itertator or
//	Partial::const_itertator yields a reference to a Breakpoint. Additionally,
//	these iterator classes have breakpoint() and time() members, returning
//	the Breakpoint (by reference) at the current iterator position and the
//	time (by value) corresponding to that Breakpoint.
//	
//	Partial is a leaf class, do not subclass.
//
//	Most of the implementation of Partial delegates to a few
//	container-dependent members. The following members are
//	container-dependent, the other members are implemented in 
//	terms of these:
//		default construction
//		copy (construction)
//		operator= (assign)
//		operator== (equivalence)
//		size
//		insert( pos, Breakpoint )
//		erase( b, e )
//		findAfter( time )
//		begin (const and non-const)
//		end (const and non-const)
//		first (const and non-const)
//		last (const and non-const)
//
class Partial
{
//	-- public interface --
public:
//	-- types --
	typedef int label_type;	// should verify that this is a 32 bit type.
	
	typedef std::map< double, Breakpoint > container_type;
	//	typedef std::vector< std::pair< double, Breakpoint > > container_type;
	//	see Partial.C for a discussion of issues surrounding the 
	//	choice of std::map as a Breakpoint container.
	typedef Partial_Iterator iterator;
	typedef Partial_ConstIterator const_iterator;
	typedef container_type::size_type size_type;

//	-- container-dependent implementation --
	const_iterator begin( void ) const;
	iterator begin( void );
	/*	Return an iterator refering to the position of the first
		Breakpoint in this Partial's envelope.
		
		For const Partials, returns a const_iterator.
	 */
	 
	const_iterator end( void ) const;
	iterator end( void );
	/*	Return an iterator refering to the position past the last
		Breakpoint in this Partial's envelope. The iterator returned by
		end() (like the iterator returned by the end() member of any STL
		container) does not refer to a valid Breakpoint. 	

		For const Partials, returns a const_iterator.
	 */

	iterator erase( iterator beg, iterator end );
	/*	Breakpoint removal: erase the Breakpoints in the specified range,
		and return an iterator referring to the position after the,
		erased range.
	 */

	const_iterator findAfter( double time ) const;
	iterator findAfter( double time );
	/*	Return an iterator refering to the insertion position for a
		Breakpoint at the specified time (that is, the position of the first
		Breakpoint at a time later than the specified time).
		
		For const Partials, returns a const_iterator.
	 */

	iterator insert( double time, const Breakpoint & bp );
	/*	Breakpoint insertion: insert a copy of the specified Breakpoint in the
		parameter envelope at time (seconds), and return an iterator
		refering to the position of the inserted Breakpoint.
	 */

	size_type size( void ) const;
	/*	Return the number of Breakpoints in this Partial.
	 */
	
//	-- construction --
	Partial( void );
	/* 	Retun a new empty (no Breakpoints) Partial.
	 */
	 
	Partial( const_iterator beg, const_iterator end );
	/* 	Retun a new Partial from a half-open (const) iterator range 
		of time, Breakpoint pairs.
	 */
	 
	~Partial( void );
	/* 	Destroy this Partial.
	 */
	 
//	-- assignment and copy --
	Partial( const Partial & other );
	/*	Return a new Partial that is an exact copy (has an identical set
		of Breakpoints, at identical times, and the same label) of another 
		Partial.
	 */
	 
	 Partial & operator=( const Partial & other );
	/*	Make this Partial an exact copy (has an identical set of 
		Breakpoints, at identical times, and the same label) of another 
		Partial.
	 */
	
//	-- comparison --
	bool operator== ( const Partial & rhs ) const;
	/*	Equality operator: return true if this Partial has an identical
		parameter envelope (identical Breakpoints at identical times) and the
		same label as the Partial rhs, otherwise return false.
	 */
	 
//	-- access --
	double duration( void ) const;
	/*	Return the duration (in seconds) spanned by the Breakpoints in
		this Partial. Note that the synthesized onset time will differ,
		depending on the fade time used to synthesize this Partial (see
		class Synthesizer).
	 */
	 
	double endTime( void ) const;
	/*	Return the time (in seconds) of the last Breakpoint in this
		Partial. Note that the synthesized onset time will differ,
		depending on the fade time used to synthesize this Partial (see
		class Synthesizer).
	 */
	 
	Breakpoint & first( void );
	const Breakpoint & first( void ) const;
	/*	Return a reference to the first Breakpoint in the Partial's
		envelope. Raises InvalidPartial exception if there are no 
		Breakpoints.
	 */
	 
	double initialPhase( void ) const;
	/*	Return the phase (in radians) of this Partial at its start time
		(the phase of the first Breakpoint). Note that the initial
		synthesized phase will differ, depending on the fade time used
		to synthesize this Partial (see class Synthesizer).
	 */
	 	 
	label_type label( void ) const { return _label; }
	/*	Return the 32-bit label for this Partial as an integer.
	 */
	 
	Breakpoint & last( void );
	const Breakpoint & last( void ) const;
	/*	Return a reference to the last Breakpoint in the Partial's
		envelope. Raises InvalidPartial exception if there are no 
		Breakpoints.
	 */
	 
	size_type numBreakpoints( void ) const { return size(); }
	/*	Return the number of Breakpoints in this Partial.
	 */
	
	double startTime( void ) const;
	/*	Return the time (in seconds) of the first Breakpoint in this
		Partial. Note that the synthesized onset time will differ,
		depending on the fade time used to synthesize this Partial (see
		class Synthesizer).
	 */
	 
//	-- mutation --
	void absorb( const Partial & other );
	/*	Absorb another Partial's energy as noise (bandwidth), 
		by accumulating the other's energy as noise energy
		in the portion of this Partial's envelope that overlaps
		(in time) with the other Partial's envelope.
	 */

	void setLabel( label_type l ) { _label = l; }
	/*	Set the label for this Partial to the specified 32-bit value.
	 */

	iterator erase( iterator pos );
	/*	Breakpoint removal: erase the Breakpoint at the position of the given
		iterator (invalidating the iterator). Return a 
		iterator referring to the next valid position, or to
		the end of the Partial if the last Breakpoint is removed.
	 */
	 
	iterator findNearest( double time );
	const_iterator findNearest( double time ) const;
	/*	Return an iterator refering to the position of the
		Breakpoint in this Partial nearest the specified time.
		
		For const Partials, returns a const_iterator.
	 */
	 
	Partial split( iterator pos );
	/*	Break this Partial at the specified position (iterator).
		The Breakpoint at the specified position becomes the first
		Breakpoint in a new Partial. Breakpoints at the specified
		position and subsequent positions are removed from this
		Partial and added to the new Partial, which is returned.
	 */
	 
//	-- parameter interpolation/extrapolation --

	static const double ShortestSafeFadeTime;	//	1 nanosecond, see Partial.C
	/*	Define the default fade time for computing amplitude at the ends
		of a Partial. Floating point round-off errors make fadeTime == 0.0
		dangerous and unpredictable. 1 ns is short enough to prevent rounding
		errors in the least significant bit of a 48-bit mantissa for times
		up to ten hours.
	 */

	double amplitudeAt( double time, double fadeTime = ShortestSafeFadeTime ) const;
	/*	Return the interpolated amplitude of this Partial at the
		specified time. Throw an InvalidPartial exception if this 
		Partial has no Breakpoints. If non-zero fadeTime is specified, 
		then the amplitude at the ends of the Partial is coomputed using
		a linear fade. The default fadeTime is ShortestSafeFadeTime,
		see the definition of ShortestSafeFadeTime, above.
	 */

	double bandwidthAt( double time ) const;
	/*	Return the interpolated bandwidth (noisiness) coefficient of
		this Partial at the specified time. At times beyond the ends of
		the Partial, return the bandwidth coefficient at the nearest
		envelope endpoint. Throw an InvalidPartial exception if this
		Partial has no Breakpoints.
	 */
	 
	double frequencyAt( double time ) const;
	/*	Return the interpolated frequency (in Hz) of this Partial at the
		specified time. At times beyond the ends of the Partial, return
		the frequency at the nearest envelope endpoint. Throw an
		InvalidPartial exception if this Partial has no Breakpoints.
	 */
	 
	double phaseAt( double time ) const;
	/*	Return the interpolated phase (in radians) of this Partial at
		the specified time. At times beyond the ends of the Partial,
		return the extrapolated from the nearest envelope endpoint
		(assuming constant frequency, as reported by frequencyAt()).
		Throw an InvalidPartial exception if this Partial has no
		Breakpoints.
	 */

	Breakpoint parametersAt( double time, double fadeTime = ShortestSafeFadeTime ) const;
	/*	Return the interpolated parameters of this Partial at
		the specified time, same as building a Breakpoint from
		the results of frequencyAt, ampitudeAt, bandwidthAt, and
		phaseAt, but performs only one Breakpoint envelope search.
		Throw an InvalidPartial exception if this Partial has no
		Breakpoints. If non-zero fadeTime is specified, then the
		amplitude at the ends of the Partial is coomputed using a 
		linear fade. The default fadeTime is ShortestSafeFadeTime.
	 */

//	-- implementation --
private:
	label_type _label;
	container_type _breakpoints;	//	Breakpoint envelope
	 
};	//	end of class Partial

// ---------------------------------------------------------------------------
//	class Partial_Iterator
//
//	Non-const iterator for the Loris::Partial Breakpoint map. Wraps
//	the non-const iterator for the (time,Breakpoint) pair container
//	Partial::container_type. Partial_Iterator implements a
//	bidirectional iterator interface, and additionally offers time
//	and Breakpoint (reference) access through time() and breakpoint()
//	members.
//
class Partial_Iterator
{
//	-- instance variables --
	typedef Partial::container_type BaseContainer;
	typedef BaseContainer::iterator BaseIterator;
	BaseIterator _iter;
	
//	-- public interface --
public:
//	-- bidirectional iterator interface --
	typedef BaseIterator::iterator_category	iterator_category;
	typedef Breakpoint     					value_type;
	typedef BaseIterator::difference_type  	difference_type;
	typedef Breakpoint *					pointer;
	typedef Breakpoint &					reference;

//	construction:
//	(allow compiler to generate copy, assignment, and destruction):
	Partial_Iterator( void ) {}
	
//	pre-increment/decrement:
	Partial_Iterator& operator ++ () { ++_iter; return *this; }
	Partial_Iterator& operator -- () { --_iter; return *this; }

//	post-increment/decrement:
	Partial_Iterator operator ++ ( int ) { return Partial_Iterator( _iter++ ); } 
	Partial_Iterator operator -- ( int ) { return Partial_Iterator( _iter-- ); } 
	
//	dereference (for treating Partial like a 
//	STL collection of Breakpoints):
	const Breakpoint & operator * ( void ) const { return breakpoint(); }
	Breakpoint & operator * ( void ) { return breakpoint(); }
	const Breakpoint * operator -> ( void ) const  { return & breakpoint(); }
	Breakpoint * operator -> ( void )  { return & breakpoint(); }
		
//	comparison:
	friend bool operator == ( const Partial_Iterator & lhs, 
							  const Partial_Iterator & rhs )
		{ return lhs._iter == rhs._iter; }
	friend bool operator != ( const Partial_Iterator & lhs, 
							  const Partial_Iterator & rhs )
		{ return lhs._iter != rhs._iter; }
	
//	-- time and Breakpoint access --
	const Breakpoint & breakpoint( void ) const 
		{ return _iter->second; }
	Breakpoint & breakpoint( void ) 
		{ return _iter->second; }
	double time( void ) const  
		{ return _iter->first; }

//	-- BaseIterator conversions --
private:
	//	construction by GenericBreakpointContainer from a BaseIterator:
	Partial_Iterator( const BaseIterator & it ) :
		_iter(it) {}

	friend class Partial;
	
	//	befriend  Partial_ConstIterator, 
	//	for const construction from non-const:
	friend class Partial_ConstIterator;	
	
};	//	end of class Partial_Iterator

// ---------------------------------------------------------------------------
//	class Partial_ConstIterator
//
//	Const iterator for the Loris::Partial Breakpoint map. Wraps
//	the non-const iterator for the (time,Breakpoint) pair container
//	Partial::container_type. Partial_Iterator implements a
//	bidirectional iterator interface, and additionally offers time
//	and Breakpoint (reference) access through time() and breakpoint()
//	members.
//
class Partial_ConstIterator
{
//	-- instance variables --
	typedef Partial::container_type BaseContainer;
	typedef BaseContainer::const_iterator BaseIterator;
	BaseIterator _iter;
	
//	-- public interface --
public:
//	-- bidirectional iterator interface --
	typedef BaseIterator::iterator_category	iterator_category;
	typedef Breakpoint     					value_type;
	typedef BaseIterator::difference_type  	difference_type;
	typedef const Breakpoint *				pointer;
	typedef const Breakpoint &				reference;

//	construction:
//	(allow compiler to generate copy, assignment, and destruction):
	Partial_ConstIterator( void ) {}
	Partial_ConstIterator( const Partial_Iterator & other ) :
		_iter( other._iter ) {}
	
//	pre-increment/decrement:
	Partial_ConstIterator& operator ++ () { ++_iter; return *this; }
	Partial_ConstIterator& operator -- () { --_iter; return *this; }

//	post-increment/decrement:
	Partial_ConstIterator operator ++ ( int ) { return Partial_ConstIterator( _iter++ ); } 
	Partial_ConstIterator operator -- ( int ) { return Partial_ConstIterator( _iter-- ); } 
	
//	dereference (for treating Partial like a 
//	STL collection of Breakpoints):
	const Breakpoint & operator * ( void ) const { return breakpoint(); }
	const Breakpoint * operator -> ( void ) const { return & breakpoint(); }
	
//	comparison:
	friend bool operator == ( const Partial_ConstIterator & lhs, 
							  const Partial_ConstIterator & rhs )
		{ return lhs._iter == rhs._iter; }
	friend bool operator != ( const Partial_ConstIterator & lhs, 
							  const Partial_ConstIterator & rhs )
		{ return lhs._iter != rhs._iter; }
	
//	-- time and Breakpoint access --
	const Breakpoint & breakpoint( void ) const 
		{ return _iter->second; }
	double time( void ) const  
		{ return _iter->first; }
	
//	-- BaseIterator conversions --
private:
	//	construction by GenericBreakpointContainer from a BaseIterator:
	Partial_ConstIterator( BaseIterator it ) :
		_iter(it) {}
	
	friend class Partial;

};	//	end of class Partial_ConstIterator

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

}	//	end of namespace Loris

#endif /* ndef INCLUDE_PARTIAL_H */

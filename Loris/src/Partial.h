#ifndef INCLUDE_PARTIAL_H
#define INCLUDE_PARTIAL_H
/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
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
 * Definition of class Loris::Partial an associated iterators and exceptions.
 *
 * Kelly Fitz, 16 Aug 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <Exception.h>
#include <Breakpoint.h>
#include <iterator>
#include <map>

//	begin namespace
namespace Loris {

//	declare iterator types for class Partial:
class PartialIterator;
class PartialConstIterator;

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
//	Partial:iterator and Partial::const_iterator interfaces, which are
//	typedefs for PartialIterator and PartialConstIterator, respectively.
//	These iterator classes implement the interface for bidirectional
//	iterators in the STL, including pre and post-increment and decrement,
//	and dereferencing. Dereferencing a PartialIterator or
//	PartialConstIterator yields a reference to a Breakpoint. Additionally,
//	these iterator classes have breakpoint() and time() members, returning
//	the Breakpoint (by reference) at the current iterator position and the
//	time (by value) corresponding to that Breakpoint.
//	
//	Partial is a leaf class, do not subclass.
//
class Partial
{
//	-- public interface --
public:
//	-- types --
	typedef PartialIterator iterator;
	typedef PartialConstIterator const_iterator;
	typedef std::map< double, Breakpoint >::size_type size_type;
	typedef int label_type;	// should verify that this is a 32 bit type.
	
//	-- construction --
	Partial( void );
	/* 	Retun a new empty (no Breakpoints) Partial.
	 */
	 
	Partial( const_iterator beg, const_iterator end );
	/* 	Retun a new Partial from a half-open (const) iterator range 
		of time, Breakpoint pairs.
	 */
	 
	Partial( const Partial & other );
	/*	Return a new Partial that is an exact copy (has an identical set
		of Breakpoints, at identical times, and the same label) of another 
		Partial.
	 */
	 
	~Partial( void );
	/* 	Destroy this Partial.
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
	 
	double initialPhase( void ) const;
	/*	Return the phase (in radians) of this Partial at its start time
		(the phase of the first Breakpoint). Note that the initial
		synthesized phase will differ, depending on the fade time used
		to synthesize this Partial (see class Synthesizer).
	 */
	 	 
	label_type label( void ) const { return _label; }
	/*	Return the 32-bit label for this Partial as an integer.
	 */
	 
	size_type numBreakpoints( void ) const { return _bpmap.size(); }
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
	
//	-- iterator access --
	iterator begin( void );
	const_iterator begin( void ) const;
	/*	Return a PartialIterator refering to the position of the first
		Breakpoint in this Partial's envelope.
		
		For const Partials, returns a PartialConstIterator.
	 */
	 
	iterator end( void );
	const_iterator end( void ) const;
	/*	Return a PartialIterator refering to the position past the last
		Breakpoint in this Partial's envelope. The iterator returned by
		end() (like the iterator returned by the end() member of any STL
		container) does not refer to a valid Breakpoint. 	

		For const Partials, returns a PartialConstIterator.
	 */
	 
	iterator erase( iterator pos );
	/*	Breakpoint removal: erase the Breakpoint at the position of the given
		PartialIterator (invalidating the PartialIterator). Return a 
		PartialIterator referring to the next valid position, or to
		the end of the Partial if the last Breakpoint is removed.
	 */
	 
	iterator findAfter( double time );
	const_iterator findAfter( double time ) const;
	/*	Return a PartialIterator refering to the insertion position for a
		Breakpoint at the specified time (that is, the position of the first
		Breakpoint at a time later than the specified time).
		
		For const Partials, returns a PartialConstIterator.
	 */
	 
	iterator findNearest( double time );
	const_iterator findNearest( double time ) const;
	/*	Return a PartialIterator refering to the position of the
		Breakpoint in this Partial nearest the specified time.
		
		For const Partials, returns a PartialConstIterator.
	 */
	 
	iterator insert( double time, const Breakpoint & bp );
	/*	Breakpoint insertion: insert a copy of the specified Breakpoint in the
		parameter envelope at time (seconds), and return a PartialIterator
		refering to the position of the inserted Breakpoint.
	 */
	 
	Partial split( iterator pos );
	/*	Break this Partial at the specified position (iterator).
		The Breakpoint at the specified position becomes the first
		Breakpoint in a new Partial. Breakpoints at the specified
		position and subsequent positions are removed from this
		Partial and added to the new Partial, which is returned.
	 */
	 
//	-- parameter interpolation/extrapolation --
	double amplitudeAt( double time ) const;
	/*	Return the interpolated amplitude of this Partial at the
		specified time. At times beyond the ends of the Partial, return
		zero. Throw an InvalidPartial exception if this Partial has no
		Breakpoints.
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

	Breakpoint parametersAt( double time ) const;
	/*	Return the interpolated parameters of this Partial at
		the specified time, same as building a Breakpoint from
		the results of frequencyAt, ampitudeAt, bandwidthAt, and
		phaseAt, but performs only one Breakpoint envelope search.
		Throw an InvalidPartial exception if this Partial has no
		Breakpoints.
	 */

//	-- implementation --
private:
	std::map< double, Breakpoint > _bpmap;	//	Breakpoint envelope
	label_type _label;						//	label
	 
};	//	end of class Partial

// ---------------------------------------------------------------------------
//	class PartialIterator
//
//	Non-const iterator for the Loris::Partial Breakpoint map. Wraps
//	the non-const iterator for std::map< double, Breakpoint >. Implements
//	a bidirectional iterator interface, and additionally offers time
//	and Breakpoint (reference) access through time() and breakpoint()
//	memebrs.
//
class PartialIterator
{
//	-- instance variables --
	std::map< double, Breakpoint >::iterator _iter;
	
//	-- public interface --
public:
//	-- bidirectional iterator interface --
      typedef std::bidirectional_iterator_tag	iterator_category;
      typedef Breakpoint     					value_type;
      typedef std::map< double, Breakpoint >::difference_type  difference_type;
      typedef Breakpoint *			pointer;
      typedef Breakpoint &			reference;

//	construction:
//	(allow compiler to generate copy, assignment, and destruction):
	PartialIterator( void ) {}
	
//	pre-increment/decrement:
	PartialIterator& operator ++ () { ++_iter; return *this; }
	PartialIterator& operator -- () { --_iter; return *this; }

//	post-increment/decrement:
	PartialIterator operator ++ ( int ) { return PartialIterator( _iter++ ); } 
	PartialIterator operator -- ( int ) { return PartialIterator( _iter-- ); } 
	
//	dereference (for treating Partial like a 
//	STL collection of Breakpoints):
	const Breakpoint & operator * ( void ) const { return _iter->second; }
	Breakpoint & operator * ( void ) { return _iter->second; }
	const Breakpoint * operator -> ( void ) const { return & _iter->second; }
	Breakpoint * operator -> ( void ) { return & _iter->second; }
	
//	comparison:
	friend bool operator == ( const PartialIterator & lhs, const PartialIterator & rhs )
		{ return lhs._iter == rhs._iter; }
	friend bool operator != ( const PartialIterator & lhs, const PartialIterator & rhs )
		{ return lhs._iter != rhs._iter; }
	
//	-- time and Breakpoint access --
	const Breakpoint & breakpoint( void ) const { return _iter->second; }
	Breakpoint & breakpoint( void ) { return _iter->second; }
	double time( void ) const { return _iter->first; }	
	
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
//	the const iterator for std::map< double, Breakpoint >. Implements
//	a bidirectional iterator interface, and additionally offers time
//	and Breakpoint (reference) access through time() and breakpoint()
//	memebrs.
//
class PartialConstIterator
{
//	-- instance variables --
	std::map< double, Breakpoint >::const_iterator _iter;
	
//	-- public interface --
public:
//	-- bidirectional iterator interface --
      typedef std::bidirectional_iterator_tag	iterator_category;
      typedef Breakpoint     					value_type;
      typedef std::map< double, Breakpoint >::difference_type  difference_type;
      typedef const Breakpoint *			pointer;
      typedef const Breakpoint &			reference;

//	construction:
//	(allow compiler to generate copy, assignment, and destruction):
	PartialConstIterator( void ) {}
	PartialConstIterator( const PartialIterator & other ) : _iter( other._iter ) {}
	
//	pre-increment/decrement:
	PartialConstIterator& operator ++ () { ++_iter; return *this; }
	PartialConstIterator& operator -- () { --_iter; return *this; }

//	post-increment/decrement:
	PartialConstIterator operator ++ ( int ) { return PartialConstIterator( _iter++ ); } 
	PartialConstIterator operator -- ( int ) { return PartialConstIterator( _iter-- ); } 
	
//	dereference (for treating Partial like a 
//	STL collection of Breakpoints):
	const Breakpoint & operator * ( void ) const { return _iter->second; }
	const Breakpoint * operator -> ( void ) const { return & _iter->second; }
	
//	comparison:
	friend bool operator == ( const PartialConstIterator & lhs, const PartialConstIterator & rhs )
		{ return lhs._iter == rhs._iter; }
	friend bool operator != ( const PartialConstIterator & lhs, const PartialConstIterator & rhs )
		{ return lhs._iter != rhs._iter; }
	
//	-- time and Breakpoint access --
	const Breakpoint & breakpoint( void ) const { return _iter->second; }	
	double time( void ) const { return _iter->first; }	

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

}	//	end of namespace Loris

#endif /* ndef INCLUDE_PARTIAL_H */

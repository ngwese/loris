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
 * Definition of class Loris::Partial. The policy for storing Breakpoints has
 * been abstracted out of the Partial class itself. Partial inherits from a 
 * BreakpointContainer class, which specifies the storage implementation, and
 * the operations that are specific to that implementation. All other operations
 * are implemented in terms of a few BreakpointContainer pollicy operations. 
 * Ideally, Partial would be a template class (as shown in Alexandrescu, Modern
 * C++ Design, but that would necessitate implementing Partial entirely in the 
 * header, which is unacceptable. Ideally, the necessary operations for a 
 * Breakpoint container policy would be pure virtual members of an abstract 
 * interface class, but that would necessitate virtual functions, which is 
 * unacceptable overhead. The current implementation is all statically bound
 * so there is no performance penalty for factoring out the Breakpoint container
 * policy. There is no need for even a vitual destructor, because destruction is
 * protected in the base policy classes (a trick I learned from Alexandrescu).
 *
 * The no frills policy is basically the same as the old implementation, based on
 * std::map. the reference counted policy uses a reference counted pointer class
 * to manage a pointer to the std::map, and implements copy-on-write semantics. 
 * I thought this would speed things up by eliminating some copying of big 
 * Breakpoint maps, but in fact, it slowed things down! Must be that indirection
 * isn't free! So the no frills policy is the one we use. The reference 
 * counted one has been moved out to RefCountedBreakpointContainer.h.
 *
 * Kelly Fitz, 16 Aug 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <Exception.h>
#include <Breakpoint.h>
#include <GenericPartialIterator.h>
#include <map>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	Breakpoint container policy
//
//	responsibilities:
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
//		iterator and const_iterator types, and size_type
//		
class NoFrillsBreakpointContainer : public GenericBreakpointContainer
{
//	-- public interface --
public:
//	-- types --
	typedef std::map< double, Breakpoint > container_type;
	typedef GenericPartialIterator< NoFrillsBreakpointContainer > iterator;
	typedef GenericPartialConstIterator< NoFrillsBreakpointContainer > const_iterator;
	typedef container_type::size_type size_type;

//	-- construction --
	NoFrillsBreakpointContainer( void ) {}
	
//	-- compare --
	bool operator==( const NoFrillsBreakpointContainer & rhs ) const
		{ return _bpmap == rhs._bpmap; }

//	-- access opertions --
	const_iterator begin( void ) const { return makeIterator<const_iterator>( _bpmap.begin() ); }
	iterator begin( void ) { return makeIterator<iterator>( _bpmap.begin() ); }
	const_iterator end( void ) const { return makeIterator<const_iterator>( _bpmap.end() ); }
	iterator end( void ) { return makeIterator<iterator>( _bpmap.end() ); }
	size_type size( void ) const { 	return _bpmap.size(); }
	
	const_iterator findAfter( double time ) const
	{
		return makeIterator<const_iterator>( _bpmap.lower_bound( time ) );
	}
	iterator findAfter( double time ) 
	{
		return makeIterator<iterator>( _bpmap.lower_bound( time ) );
	}

//	-- mutating operations --
	iterator insert( double time, const Breakpoint & bp )
	{
		std::pair< container_type::iterator, bool > result = 
			_bpmap.insert( container_type::value_type(time, bp) );
		if ( ! result.second )
			result.first->second = bp;
		return makeIterator<iterator>( result.first );
	}
 
	iterator erase( iterator beg, iterator end )
	{
		//	This relies on the unsavory conversion from const_iterator 
		//	to container_type::const_iterator.
		_bpmap.erase( baseIterator(beg), baseIterator(end) );
		return end;
	}

protected:
	//	protect destruction
	~NoFrillsBreakpointContainer( void ) {}

	//	icky constructor for Partial to use:
	//	This relies on the unsavory conversion from const_iterator 
	//	to container_type::const_iterator.
	NoFrillsBreakpointContainer( const_iterator beg, const_iterator end ) 
		: _bpmap( baseIterator(beg), baseIterator(end) ) {}

//	-- implementation core --
private:
	container_type _bpmap;	//	Breakpoint envelope
	
};

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
//	types inherited from the BreakpointContainerPolicy.
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
class Partial : public NoFrillsBreakpointContainer
{
	typedef NoFrillsBreakpointContainer BreakpointContainerPolicy;

//	-- public interface --
public:
//	-- inherited from BreakpointContainerPolicy --
	using BreakpointContainerPolicy::begin;
	/*	Return an iterator refering to the position of the first
		Breakpoint in this Partial's envelope.
		
		For const Partials, returns a GenericPartialConstIterator.
	 */
	using BreakpointContainerPolicy::end;
	/*	Return an iterator refering to the position past the last
		Breakpoint in this Partial's envelope. The iterator returned by
		end() (like the iterator returned by the end() member of any STL
		container) does not refer to a valid Breakpoint. 	

		For const Partials, returns a GenericPartialConstIterator.
	 */
	using BreakpointContainerPolicy::erase;
	/*	Breakpoint removal: erase the Breakpoints in the specified range,
		and return an iterator referring to the position after the,
		erased range.
	 */
	using BreakpointContainerPolicy::findAfter;
	/*	Return an iterator refering to the insertion position for a
		Breakpoint at the specified time (that is, the position of the first
		Breakpoint at a time later than the specified time).
		
		For const Partials, returns a GenericPartialConstIterator.
	 */
	using BreakpointContainerPolicy::insert;
	/*	Breakpoint insertion: insert a copy of the specified Breakpoint in the
		parameter envelope at time (seconds), and return an iterator
		refering to the position of the inserted Breakpoint.
	 */
	using BreakpointContainerPolicy::size;
	/*	Return the number of Breakpoints in this Partial.
	 */
	
//	-- types --
	typedef int label_type;	// should verify that this is a 32 bit type.

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
		
		For const Partials, returns a GenericPartialConstIterator.
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
		
};	//	end of class InvalidPartial

}	//	end of namespace Loris

#endif /* ndef INCLUDE_PARTIAL_H */

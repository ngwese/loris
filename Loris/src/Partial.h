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
 * Definition of class Loris::Partial.
 *
 * Kelly Fitz, 16 Aug 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "Exception.h"
#include "Breakpoint.h"
#include <map>
//#include <list>

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
//	Loris Partials represent reassigned bandwidth-enhanced model components.
//	A Partial consists of a chain of Breakpoints describing the time-varying
//	frequency, amplitude, and bandwidth (noisiness) of the component.
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
	
//	static long DebugCounter;

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
	
//	pre-increment/decrement:
	PartialIterator& operator ++ () { ++_iter; return *this; }
	PartialIterator& operator -- () { --_iter; return *this; }

//	post-increment/decrement:
	PartialIterator operator ++ ( int ) { return PartialIterator( _iter++ ); } 
	PartialIterator operator -- ( int ) { return PartialIterator( _iter-- ); } 
	
//	time and Breakpoint access:
	const Breakpoint & breakpoint( void ) const { return _iter->second; }
	Breakpoint & breakpoint( void ) { return _iter->second; }
	double time( void ) const { return _iter->first; }	
	
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
	
//	pre-increment/decrement:
	PartialConstIterator& operator ++ () { ++_iter; return *this; }
	PartialConstIterator& operator -- () { --_iter; return *this; }

//	post-increment/decrement:
	PartialConstIterator operator ++ ( int ) { return PartialConstIterator( _iter++ ); } 
	PartialConstIterator operator -- ( int ) { return PartialConstIterator( _iter-- ); } 
	
//	time and Breakpoint access:
	const Breakpoint & breakpoint( void ) const { return _iter->second; }	
	double time( void ) const { return _iter->first; }	

//	dereference (for treating Partial like a 
//	STL collection of Breakpoints):
	const Breakpoint & operator * ( void ) const { return _iter->second; }
	const Breakpoint * operator -> ( void ) const { return & _iter->second; }
	
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

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_PARTIAL_H

#ifndef INCLUDE_PARTIALUTILS_H
#define INCLUDE_PARTIALUTILS_H
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
 * PartialUtils.h
 *
 *	A group of Partial utility function objects for use with STL 
 *	searching and sorting algorithms. PartialUtils is a namespace
 *	within the Loris namespace.
 *
 * This file defines three kinds of functors:
 * - Partial mutators
 * - predicates on Partials
 * - Partial comparitors
 *
 * Kelly Fitz, 6 July 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "Envelope.h"
#include "Partial.h"

#include <functional>
#include <utility>

//	begin namespace
namespace Loris {

namespace PartialUtils {

//	-- Partial mutating functors --

// ---------------------------------------------------------------------------
//	PartialMutator
//	
//! PartialMutator is an abstract base class for Partial mutators,
//! functors that operate on Partials according to a time-varying
//! envelope. The base class manages a polymorphic Envelope instance
//! that provides the time-varying mutation parameters.
//!
//! \invariant	env is a non-zero pointer to a valid instance of a 
//!            class derived from the abstract class Envelope.
class PartialMutator : public std::unary_function< Partial, void >
{
public:

	//! Construct a new PartialMutator from a constant mutation factor.
	PartialMutator( double x );

	//! Construct a new PartialMutator from an Envelope representing
	//! a time-varying mutation factor.
	PartialMutator( const Envelope & e );

	//! Construct a new PartialMutator that is a copy of another.
	PartialMutator( const PartialMutator & rhs );

	//! Destroy this PartialMutator, deleting its Envelope.
	virtual ~PartialMutator( void );
	
	//! Make this PartialMutator a duplicate of another one.
	//!
	//! \param	rhs is the PartialMutator to copy.
	PartialMutator & operator=( const PartialMutator & rhs );
	
	//! Function call operator: apply a mutation factor to the 
	//! specified Partial. Derived classes must implement this 
	//! member.
	virtual void operator()( Partial & p ) const = 0;

protected:
	Envelope * env;
};

// ---------------------------------------------------------------------------
//	AmplitudeScaler
//	
//! Scale the amplitude of the specified Partial according to
//! an envelope representing a time-varying amplitude scale value.
//
class AmplitudeScaler : public PartialMutator
{
public:

	//! Construct a new AmplitudeScaler from a constant scale factor.
	AmplitudeScaler( double x ) : PartialMutator( x ) {}
	
	//! Construct a new AmplitudeScaler from an Envelope representing
	//! a time-varying scale factor.
	AmplitudeScaler( const Envelope & e ) : PartialMutator( e ) {}
	
	//! Function call operator: apply a scale factor to the specified
	//! Partial.
	void operator()( Partial & p ) const;
};

// ---------------------------------------------------------------------------
//	scaleAmplitude
// ---------------------------------------------------------------------------
//! Scale the amplitude of the specified Partial according to
//! an envelope representing a amplitude scale value or envelope.
//!
//! \param	p is a Partial to mutate.
//! \param	arg is either a constant scale factor or an Envelope
//!			describing the time-varying scale factor.
//
template< class Arg >
void scaleAmplitude( Partial & p, const Arg & arg )
{
	AmplitudeScaler scaler( arg );
	scaler( p );
}

// ---------------------------------------------------------------------------
//	scaleAmplitude
// ---------------------------------------------------------------------------
//! Scale the amplitude of a sequence of Partials according to
//! an envelope representing a amplitude scale value or envelope.
//!
//! \param	b is the beginning of a sequence of Partials to mutate.
//! \param	e is the end of a sequence of Partials to mutate.
//! \param	arg is either a constant scale factor or an Envelope
//!			describing the time-varying scale factor.
//
template< class Iter, class Arg >
void scaleAmplitude( Iter b, Iter e, const Arg & arg )
{
	AmplitudeScaler scaler( arg );
	while ( b != e )
	{
		scaler( *b++ );
	}
}

// ---------------------------------------------------------------------------
//	BandwidthScaler
//	
//! Scale the bandwidth of the specified Partial according to
//! an envelope representing a time-varying bandwidth scale value.
//
class BandwidthScaler : public PartialMutator
{
public:

	//! Construct a new BandwidthScaler from a constant scale factor.
	BandwidthScaler( double x ) : PartialMutator( x ) {}

	//! Construct a new BandwidthScaler from an Envelope representing
	//! a time-varying scale factor.
	BandwidthScaler( const Envelope & e ) : PartialMutator( e ) {}
	
	//! Function call operator: apply a scale factor to the specified
	//! Partial.
	void operator()( Partial & p ) const;
};

// ---------------------------------------------------------------------------
//	scaleBandwidth
// ---------------------------------------------------------------------------
//! Scale the bandwidth of the specified Partial according to
//! an envelope representing a amplitude scale value or envelope.
//!
//! \param	p is a Partial to mutate.
//! \param	arg is either a constant scale factor or an Envelope
//!			describing the time-varying scale factor.
//
template< class Arg >
void scaleBandwidth( Partial & p, const Arg & arg )
{
	BandwidthScaler scaler( arg );
	scaler( p );
}

// ---------------------------------------------------------------------------
//	scaleBandwidth
// ---------------------------------------------------------------------------
//! Scale the bandwidth of a sequence of Partials according to
//! an envelope representing a amplitude scale value or envelope.
//!
//! \param	b is the beginning of a sequence of Partials to mutate.
//! \param	e is the end of a sequence of Partials to mutate.
//! \param	arg is either a constant scale factor or an Envelope
//!			describing the time-varying scale factor.
//
template< class Iter, class Arg >
void scaleBandwidth( Iter b, Iter e, const Arg & arg )
{
	BandwidthScaler scaler( arg );
	while ( b != e )
	{
		scaler( *b++ );
	}
}

/*	Scale the frequency of the specified Partial according to
	an envelope representing a time-varying frequency scale value.
 */
class FrequencyScaler : public PartialMutator
{
public:

	//	construction
	FrequencyScaler( double x ) : PartialMutator( x ) {}
	FrequencyScaler( const Envelope & e ) : PartialMutator( e ) {}
	
	//	function call operator
	void operator()( Partial & p ) const;
};

template< class Arg >
void scaleFrequency( Partial & p, const Arg & arg )
{
	FrequencyScaler scaler( arg );
	scaler( p );
}

template< class Iter, class Arg >
void scaleFrequency( Iter b, Iter e, const Arg & arg )
{
	FrequencyScaler scaler( arg );
	while ( b != e )
	{
		scaler( *b++ );
	}
}

/*	Scale the relative noise content of the specified Partial according 
	to an envelope representing a (time-varying) noise energy 
	scale value.
 */
class NoiseRatioScaler : public PartialMutator
{
public:

	//	construction
	NoiseRatioScaler( double x ) : PartialMutator( x ) {}
	NoiseRatioScaler( const Envelope & e ) : PartialMutator( e ) {}
	
	//	function call operator
	void operator()( Partial & p ) const;
};

template< class Arg >
void scaleNoiseRatio( Partial & p, const Arg & arg )
{
	NoiseRatioScaler scaler( arg );
	scaler( p );
}

template< class Iter, class Arg >
void scaleNoiseRatio( Iter b, Iter e, const Arg & arg )
{
	NoiseRatioScaler scaler( arg );
	while ( b != e )
	{
		scaler( *b++ );
	}
}

/*	Shift the pitch of the specified Partial according to
	the given pitch envelope. The pitch envelope is assumed to have 
	units of cents (1/100 of a halfstep).
 */
class PitchShifter : public PartialMutator
{
public:

	//	construction
	PitchShifter( double x ) : PartialMutator( x ) {}
	PitchShifter( const Envelope & e ) : PartialMutator( e ) {}
	
	//	function call operator
	void operator()( Partial & p ) const;
};

template< class Arg >
void shiftPitch( Partial & p, const Arg & arg )
{
	PitchShifter shifter( arg );
	shifter( p );
}

template< class Iter, class Arg >
void shiftPitch( Iter b, Iter e, const Arg & arg )
{
	PitchShifter shifter( arg );
	while ( b != e )
	{
		shifter( *b++ );
	}
}

//	These ones are not derived from PartialMutator, because
//	they don't use an Envelope and cannot be time-varying.

//! 	Trim a Partial by removing Breakpoints outside a specified time span.
//!	Insert a Breakpoint at the boundary when cropping occurs.
class Cropper
{
public:

	//	construction
	Cropper( double t1, double t2 ) : 
		minTime( std::min( t1, t2 ) ),
		maxTime( std::max( t1, t2 ) )
	{
	}
	
	//	function call operator
	void operator()( Partial & p ) const;
	
private:
	double minTime, maxTime;
};

inline
void crop( Partial & p, double t1, double t2 )
{
	Cropper cropper( t1, t2 );
	cropper( p );
}

template< class Iter >
void crop( Iter b, Iter e, double t1, double t2 )
{
	Cropper cropper( t1, t2 );
	while ( b != e )
	{
		cropper( *b++ );
	}
}

/*	Shift the time of all the Breakpoints in a Partial by a 
	constant amount.
 */
class TimeShifter
{
public:

	//	construction
	TimeShifter( double x ) : offset( x ) {}
	
	//	function call operator
	void operator()( Partial & p ) const;
	
private:
	double offset;
};

inline
void shiftTime( Partial & p, double arg )
{
	TimeShifter shifter( arg );
	shifter( p );
}

template< class Iter >
void shiftTime( Iter b, Iter e, double arg )
{
	TimeShifter shifter( arg );
	while ( b != e )
	{
		shifter( *b++ );
	}
}

	
/*	Return the time (in seconds) spanned by a specified half-open
	(STL-style) range of Partials as a std::pair composed of the earliest
	Partial start time and latest Partial end time in the range.
 */
template < typename Iterator >
std::pair< double, double > 
timeSpan( Iterator begin, Iterator end ) 
{
	double tmin = 0., tmax = 0.;
	if ( begin != end )
	{
		Iterator it = begin;
		tmin = it->startTime();
		tmax = it->endTime();
		while( it != end )
		{
			tmin = std::min( tmin, it->startTime() );
			tmax = std::max( tmax, it->endTime() );
			++it;
		}
	}
	return std::make_pair( tmin, tmax );
}
	
//	-- predicates --

/*	Predicate functor returning true if the label of its Partial argument is
	equal to the specified 32-bit label, and false otherwise.
 */
class isLabelEqual : public std::unary_function< const Partial, bool >
{
public:
	//	construction:
	isLabelEqual( int l ) : label(l) {}
	
	//	function call operators
	bool operator()( const Partial & p ) const 
		{ return p.label() == label; }
		
	bool operator()( const Partial * p ) const 
		{ return p->label() == label; }

private:	
	int label;
};
	
/*	Predicate functor returning true if the label of its Partial argument is
	greater than the specified 32-bit label, and false otherwise.
 */
class isLabelGreater : public std::unary_function< const Partial, bool >
{
public:
	//	construction:
	isLabelGreater( int l ) : label(l) {}
	
	//	function call operators
	bool operator()( const Partial & p ) const 
		{ return p.label() > label; }
		
	bool operator()( const Partial * p ) const 
		{ return p->label() > label; }

private:	
	int label;
};
		
/*	Predicate functor returning true if the label of its Partial argument is
	less than the specified 32-bit label, and false otherwise.
 */
class isLabelLess : public std::unary_function< const Partial, bool >
{
public:
	//	construction:
	isLabelLess( int l ) : label(l) {}
	
	//	function call operators
	bool operator()( const Partial & p ) const 
		{ return p.label() < label; }
		
	bool operator()( const Partial * p ) const 
		{ return p->label() < label; }

private:	
	int label;
};
		
//	-- comparitors --

/*	Comparitor (binary) functor returning true if its first Partial
	argument has a label whose 32-bit integer representation is less than
	that of the second Partial argument's label, and false otherwise.
	
	How about classes CompareLabelLess and IsLabelLess, etc?
	Make it work for Partial pointers too.
 */
class compareLabelLess : 
	public std::binary_function< const Partial, const Partial, bool >
{
public:
	bool operator()( const Partial & lhs, const Partial & rhs ) const 
		{ return lhs.label() < rhs.label(); }

	bool operator()( const Partial * lhs, const Partial * rhs ) const 
		{ return lhs->label() < rhs->label(); }
};


/*	Comparitor (binary) functor returning true if its first Partial
	argument has duration less than that of the second Partial
	argument, and false otherwise.
 */
class compareDurationLess : 
	public std::binary_function< const Partial, const Partial, bool >
{
public:
	bool operator()( const Partial & lhs, const Partial & rhs ) const 
		{ return lhs.duration() < rhs.duration(); }

	bool operator()( const Partial * lhs, const Partial * rhs ) const 
		{ return lhs->duration() < rhs->duration(); }
};

/*	Comparitor (binary) functor returning true if its first Partial
	argument has duration greater than that of the second Partial
	argument, and false otherwise.
 */
class compareDurationGreater : 
	public std::binary_function< const Partial, const Partial, bool >
{
public:
	bool operator()( const Partial & lhs, const Partial & rhs ) const 
		{ return lhs.duration() > rhs.duration(); }

	bool operator()( const Partial * lhs, const Partial * rhs ) const 
		{ return lhs->duration() > rhs->duration(); }
};


}	//	end of namespace PartialUtils

}	//	end of namespace Loris

#endif /* ndef INCLUDE_PARTIALUTILS_H */

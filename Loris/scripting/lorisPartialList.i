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
 *	lorisPartialList.i
 *
 *	SWIG interface file describing the PartialList class.
 *	A PartialList is a Loris::Handle< std::list< Loris::Partial > >.
 *	Include this file in loris.i to include the PartialList class
 *	interface in the scripting module. (Can be used with the 
 *	-shadow option to SWIG to build a PartialList class in the 
 *	scripting interface.) This file does not support exactly the 
 *	public interface of the C++ std::list class, but has been 
 *	modified to better support SWIG and scripting languages.
 *
 *	--- CHANGES ---
 *	This interface has been modified (March 2001) to wrap the list
 *	access with Loris::Handles, and to include iterators on the lists,
 *	called Partial in the interface, and iterators on the Partials, 
 *	called Breakpoint in the interface. The iterators retain a counted
 *	reference to their collections (PartialList or Partial), and this 
 *	is the reason for wrapping those container class with Handles, so
 *	that an iterator that outlives its container in the interpreter
 *	doesn't wind up refering to an object that got garbage-collected.
 *	In this implementation, all references in the interpreter and all
 *	references in the interpreter to iterators have to be deleted 
 *	before the collection is finally released.
 *
 *	(April 2001) Guaranteeing the safety and validity of all these 
 *	iterators and collections is so costly and complicated that it
 *	cannot be worth the headaches and performance penalty. So for now,
 *	they come with no guarantees, and clients just have o be responsible,
 *	as in the C++ STL, with the added excitement of garbage collection. (!)
 *
 * Kelly Fitz, 17 Nov 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

%{
#include "Partial.h"
#include "notifier.h"
#include <list>

using Loris::debugger;
using Loris::Partial;
using Loris::Breakpoint;

//	define the names of the classes that are 
//	wrapped by this interface file:
//	(additionally Partial and Breakpoint)
typedef std::list< Loris::Partial > PartialList;
typedef std::list< Loris::Partial >::iterator PartialListIterator;
typedef Loris::Partial::iterator PartialIterator;

%}

/*	PartialList

	A PartialList represents a collection of Bandwidth-Enhanced 
	Partials, each having a trio of synchronous, non-uniformly-
	sampled breakpoint envelopes representing the time-varying 
	frequency, amplitude, and noisiness of a single bandwidth-
	enhanced sinusoid.

	For more information about Bandwidth-Enhanced Partials and the  
	Reassigned Bandwidth-Enhanced Additive Sound Model, refer to
	the Loris website: www.cerlsoundgroup.org/Loris/
*/
class PartialList
{
public:
	//	construction:
#if 0
	// include these in the added methods for debugging:
	PartialList( void );
	/*	Return a new empty PartialList.
	 */
	
	~PartialList( void );
	/*	Destroy this PartialList.
	 */
	
#endif	
	
	//	C++ copy constructor has the wrong semantics 
	//	for the scripting interface, define a copy 
	//	member:
	%addmethods
	{
		%new PartialList * copy( void )
		{
			return new PartialList( *self );
		}
		/*	Return a new PartialList that is a copy of this 
			PartialList (i.e. has identical Partials).
		 */
	
		PartialList( void )
		{
			debugger << "creating an empty list of Partials" << Loris::endl;
			return new PartialList();
		}
		/*	Return a new empty PartialList.
		 */
		
		~PartialList( void )
		{
			debugger << "destroying  a list of " << self->size() << " Partials" << Loris::endl;
			delete self;
		}
		/*	Destroy this PartialList.
		 */
		
	}	//	end of added methods
	
	//	wrap std::list methods:
	//
	void clear( void );
	/*	Remove (and destroy) all the Partials from this PartialList,
		leaving it empty.
	 */
	
	unsigned long size( void );
	/*	Return the number of Partials in this PartialList.
	 */
		
	%new
	PartialListIterator begin( void );
	/*	Return an iterator refering to the first Partial in this PartialList.
	 */

	%new
	PartialListIterator end( void );
	/*	Return an iterator refering to the end of this PartialList (an invalid
		element after the last valid Partial).
	 */
	 
	%new
	PartialListIterator insert( PartialListIterator position, const Partial & partial );
	/*	Insert a copy of the given Partial into this PartialList at
		the position indicated by the PartialListIterator position.
		Returns a PartialListIterator refering to the position of the
		newly-inserted Partial.
	 */

	void erase( PartialListIterator position );
	/*	Remove the element at the position indicated by the 
		PartialListIterator, position. After this operation,
		the PartialListIterator position is invalid, but all
		other iterators refering to other positions in the
		PartialList are unaffected.
	 */
	 
	void splice( PartialListIterator position, PartialList & list );
	/*	Splice all the Partials in the specified PartialList into
		this PartialList at the position indicated by the PartialListIterator
		position. Leaves the other PartialList empty.
	 */
	
};	//	end of SWIG interface class PartialList

/*	PartialListIterator
	
	A PartialListIterator represents an iterator on a PartialList.
	Its interface reflects the interface of the underlying std::list
	iterator.
	
	PartialListIterators can only be obtained from their subject 
	PartialList, through the begin and end methods of PartialList, 
	and by copying, through the PartialListIterator copy method.
	PartialListIterators are used to access individual elements
	(Partials) of a PartialList.
	
	PartialListIterators are valid only as long as their subject
	PartialList exists. In languages with some form of grabage 
	collection, this means that a reference to the PartialList 
	must be maintained in order to iterate. If that reference is
	lost, the PartialList will be deleted, and the iterators will
	refer to free memory. 
	
	PartialListIterators are also invalidated when PartialList element 
	they reference is erased from the PartialList. Erasure does not 
	invalidate any other PartialListIterators, refering to other 
	elements. The validity of an iterator can be verified using the
	isInRange method.

 */
class PartialListIterator
{
public:

	//	most of the pointer semantics of std C++ iterators
	//	are inappropriate for the scripting interface (those
	//	languages don't have pointers), so the methods in the 
	//	interface all need to be added:
	%addmethods 
	{
		%new PartialListIterator * copy( void )
		{
			return new PartialListIterator( *self );
		}
		/*	Return a new PartialListIterator that is a copy of this 
			PartialListIterator (i.e. refers to the same position
			in the same PartialList).
		 */
		 
		PartialListIterator * next( void )
		{
			++(*self);
			return self;
		}
		/*	Increment (advance) this PartialListIterator. Return self.
		 */
		 
		PartialListIterator * prev( void )
		{
			--(*self);
			return self;
		}
		/*	Decrement (advance by -1) this PartialListIterator. Return self.
		 */
		 
		Partial * partial( void )
		{
			Partial & current = **self;
			return &current;
		}
		/*	Return (a reference to) the Partial at the position of this
			PartialListIterator.
		 */
		 
		int equals( PartialListIterator * other )
		{
			return *self == *other;
		}
		/*	Return true (1) if this PartialListIterator is equal to the
			other. PartialListIterators are equal is they refer to the
			same position in the same PartialList.
		 */
		 
		 int isInRange( const PartialListIterator * begin, const PartialListIterator * end )
		 {	
		 	PartialListIterator it;
		 	for ( it = *begin; it != *end; ++it )
		 	{
		 		if ( it == *self )
		 			return true;
		 	}
		 	return false;
		 }
		 /*	Return true (1) is this iterator is within the half-open iterator
		 	range bounded by begin and end, and false otherwise. This method
		 	can be used to check the validity of an iterator -- call with 
		 	begin and end methods of the PartialList as arguments.
		  */

	}	//	end of added methods

};	//	end of SWIG interface class PartialListIterator



/*	Partial
		
	A Partial represents a Reassigned Bandwidth-Enhanced model component.
	A Partial consists of a trio of synchronous, non-uniformly-
	sampled breakpoint envelopes representing the time-varying 
	frequency, amplitude, and noisiness of a single bandwidth-
	enhanced sinusoid.
	
	For more information about Bandwidth-Enhanced Partials and the  
	Reassigned Bandwidth-Enhanced Additive Sound Model, refer to
	the Loris website: www.cerlsoundgroup.org/Loris/
 */
class Partial
{
public:
//	construction:
//
	Partial( void );
	/*	Return a new, empty (no Breakpoints) Partial.
	 */
	
	~Partial( void );
	/*	Delete this Partial reference (doesn't remove it from the PartialList).
	 */
	
	
//	access members:
// 
	int label( void ) const { return _label; }
	/* 	Return this Partial's label.
	 */

	double initialPhase( void ) const;
	/* 	Return this Partial's starting phase.
	 */
	 
	double startTime( void ) const;
	/* 	Return this Partial's start time.
	 */

	double endTime( void ) const;
	/* 	Return this Partial's end time.
	 */
	 
	double duration( void ) const;
	/* 	Return this Partial's duration.
	 */
	
	long numBreakpoints( void ) const { return _bpmap.size(); }
	/* 	Return this Partial's number of Breakpoints.
	 */
	
//	mutation members:
//
	void setLabel( int l ) { _label = l; }
	/*	Assign a new label to this Partial.
	 */
		
//	iterator generation:
//
	%new
	PartialIterator begin( void );
	/*	Return a PartialIterator refering to the first Breakpoint 
		in this Partial.
	 */

	%new
	PartialIterator end( void );
	/*	Return a PartialIterator refering to the end of this Partial (an invalid
		element after the last valid Breakpoint).
	 */

//	collection access/mutation through iterators:
//	
	%new
	PartialIterator insert( double time, const Breakpoint & bp );
	/*	Make a copy of bp and insert it at time (seconds),
		return an iterator refering to the inserted Breakpoint.
	 */

	%new
	PartialIterator findAfter( double time );
	/*	Return the insertion position for a Breakpoint at
		the specified time (that is, the position of the first
		Breakpoint at a time later than the specified time).
	 */
	 
	%new
	PartialIterator findNearest( double time );
	/*	Return the insertion position for the Breakpoint nearest
		the specified time.
	 */
	
	void erase( PartialIterator & pos );
	/* 	Erase the Breakpoint at the position of the 
		given iterator (invalidating the iterator).
	 */
	
//	partial envelope interpolation/extrapolation:
//	Return the interpolated value of a partial parameter at
//	the specified time. At times beyond the ends of the
//	Partial, frequency and bandwidth hold their boundary values,
//	amplitude is zero, and phase is computed from frequency.
//	There is of sensible definition for any of these for Partials
//	having no Breakpoints, so they except (InvalidPartial) under 
//	that condition.
//
	double frequencyAt( double time ) const;
	/* 	Return this Partial's interpolated frequency at the specified time.
	 */

	double amplitudeAt( double time ) const;
	/* 	Return this Partial's interpolated amplitude at the specified time.
	 */

	double bandwidthAt( double time ) const;
	/* 	Return this Partial's interpolated bandwidth at the specified time.
	 */

	double phaseAt( double time ) const;
	/* 	Return this Partial's interpolated phase at the specified time.
	 */


//	add methods for copy and comparison members that, in C++ have
//	inappropriate semantics for the scripting interface:
//
	%addmethods
	{
		%new Partial * copy( void )
		{
			return new Partial( *self );
		}
		/*	Return a new Partial that is a copy of this 
			Partial (i.e. has he same label and an identical
			Breakpoint entvelope.
		 */
		 
		int equals( Partial * other )
		{
			return *self == *other;
		}
		/*	Return true (1) if this Partial is equal to the other. 
			Partials are equal is they have the same label and the
			same Breakpoint envelope.
		 */
	}	//	end of added methods
		
};	//	end of SWIG interface class Partial


/*	PartialIterator
	
	A PartialIterator represents an iterator on a Partial.
	
	PartialIterators can only be obtained from their subject 
	Partial, through the begin and end methods of Partial, 
	and by copying, through the PartialIterator copy method.
	PartialIterators are used to access individual Breakpoints
	in Partial Breakpoint envelopes, and the times of those 
	Breakpoints.
	
	PartialIterators are valid only as long as their subject
	Partial exists. In languages with some form of grabage 
	collection, this means that a reference to the Partial 
	must be maintained in order to iterate. If that reference is
	lost, the Partial will be deleted, and the iterators will
	refer to free memory. By extension, if the Partial is an
	element of a PartialList, a reference to the PartialList
	must be maintained.
	
	PartialIterators are also invalidated when the Breakpoint 
	they reference is erased from the Partial. Erasure does not 
	invalidate any other PartialIterators, refering to other 
	Breakpoints. The validity of an iterator can be verified using
	the isInRange method.
	

 */
class PartialIterator
{
public:
//	time and Breakpoint access:
//
#if 0
	//	this doesn't swig right, returning a reference
	//	causes the object (in Python) to wind up with
	//	ownership, which of course it should not. Why
	//	is this busted? Fix by adding method to return ptr.
	//	
	Breakpoint & breakpoint( void ) { return _iter->second; }
	/*	Return (a reference to) the Breakpoint at the position of this
		PartialIterator.
	 */
#endif
	 
	double time( void ) const { return _iter->first; }	
	/*	Return the time of the Breakpoint at the position of this
		PartialIterator.
	 */

	//	most of the pointer semantics of std C++ iterators
	//	are inappropriate for the scripting interface (those
	//	languages don't have pointers), so many methods in the 
	//	interface all need to be added:
	%addmethods 
	{
		//	this doesn't seem to swig correctly, Breakpoint
		//	winds up with ownership, try fixng it here:
	        Breakpoint * breakpoint( void ) 
		{ 
			return &(self->breakpoint());
		}
		/*      Return (a reference to) the Breakpoint at the position of this
			PartialIterator.
		 */


		%new PartialIterator * copy( void )
		{
			return new PartialIterator( *self );
		}
		/*	Return a new PartialIterator that is a copy of this 
			PartialIterator (i.e. refers to the same position
			in the same Partial).
		 */
		 
		PartialIterator * next( void )
		{
			++(*self);
			return self;
		}
		/*	Increment (advance) this PartialIterator. Return self.
		 */
		 
		PartialIterator * prev( void )
		{
			--(*self);
			return self;
		}
		/*	Decrement (advance by -1) this PartialIterator. Return self.
		 */
		 
		int equals( PartialIterator * other )
		{
			return *self == *other;
		}
		/*	Return true (1) if this PartialIterator is equal to the
			other. PartialIterators are equal is they refer to the
			same position (Breakpoint) in the same Partial.
		 */

		 int isInRange( const PartialIterator * begin, const PartialIterator * end )
		 {	
		 	PartialIterator it;
		 	for ( it = *begin; it != *end; ++it )
		 	{
		 		if ( it == *self )
		 			return true;
		 	}
		 	return false;
		 }
		 /*	Return true (1) is this iterator is within the half-open iterator
		 	range bounded by begin and end, and false otherwise. This method
		 	can be used to check the validity of an iterator -- call with 
		 	begin and end methods of the Partial as arguments.
		  */

	}	//	end of added methods

};	//	end of SWIG interface class PartialIterator


/*	Breakpoint
	
	A Breakpoint represents a single breakpoint in the time-varying
	frequency, amplitude, and bandwidth envelope of a Reassigned 
	Bandwidth-Enhanced Partial.
	
	Instantaneous phase is also stored, but is only used at the onset of 
	a partial, or when it makes a transition from zero to nonzero amplitude.

	A Partial represents a Reassigned Bandwidth-Enhanced model component.
	For more information about Bandwidth-Enhanced Partials and the  
	Reassigned Bandwidth-Enhanced Additive Sound Model, refer to
	the Loris website: www.cerlsoundgroup.org/Loris/
 */
class Breakpoint
{
public:	
//	construction:
//
	Breakpoint( double f, double a, double b, double p = 0. );
	/*	Return a new Breakpoint having the specified frequency
		amplitude, bandwidth, and (optionally, defaults to zero)
		phase.
	 */

	~Breakpoint( void );
	/*	Delete this Breakpoint.
	 */

//	attribute access:
//
	double frequency( void );
	/*	Return the frequency of this Breakpoint. 
	 */
	 
	double amplitude( void );
	/*	Return the amplitude of this Breakpoint. 
	 */
	 
	double bandwidth( void );
	/*	Return the bandwidth of this Breakpoint. 
	 */
	 
	double phase( void );
	/*	Return the phase of this Breakpoint. 
	 */
	 	
//	attribute mutation:
//
	void setFrequency( double x );
	/*	Assign the frequency of this Breakpoint. 
	 */
	 
	void setAmplitude( double x );
	/*	Assign the amplitude of this Breakpoint. 
	 */
	 
	void setBandwidth( double x );
	/*	Assign the bandwidth of this Breakpoint. 
	 */
	 
	void setPhase( double x );
	/*	Assign the phase of this Breakpoint. 
	 */
	 
	//	C++ copy constructor has the wrong semantics 
	//	for the scripting interface, define a copy 
	//	member:
	%addmethods
	{
		%new Breakpoint * copy( void )
		{
			return new Breakpoint( *self );
		}
		/*	Return a new Breakpoint that is a copy of this 
			Breakpoint (i.e. has identical parameter values).
		 */
		 
		int equals( Breakpoint * other )
		{
			return *self == *other;
		}
		/*	Return true (1) if this Breakpoint is equal to the
			other. Breakpoints are equal is they have identical 
			parameter values.
		 */

	}	//	end of added methods
	
};	//	//	end of SWIG interface class Breakpoint

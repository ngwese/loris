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

class PartialList
/*	A PartialList represents a collection of Bandwidth-Enhanced 
	Partials, each having a trio of synchronous, non-uniformly-
	sampled breakpoint envelopes representing the time-varying 
	frequency, amplitude, and noisiness of a single bandwidth-
	enhanced sinusoid.

	For more information about Bandwidth-Enhanced Partials and the  
	Reassigned Bandwidth-Enhanced Additive Sound Model, refer to
	the Loris website: www.cerlsoundgroup.org/Loris/
*/
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
	
	//	wrap std::list methods:
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
	 
	void splice( PartialListIterator position, PartialList & list );
	/*	Splice all the Partials in the specified PartialList into
		this PartialList at the position indicated by the PartialListIterator
		position. Leaves the other PartialList empty.
	 */
	
%addmethods
{
	PartialList( void )
	{
		debugger << "creating an empty list of Partials" << std::endl;
		return new PartialList();
	}
	/*	Return a new empty PartialList.
	 */
	
	~PartialList( void )
	{
		debugger << "destroying  a list of " << self->size() << " Partials" << std::endl;
		delete self;
	}
	/*	Destroy this PartialList.
	 */
	
	//	copy constructor:
	%new PartialList * copy( void )
	{
		return new PartialList( *self );
	}
	/*	Return a new PartialList that is a copy of this 
		PartialList (i.e. has identical Partials).
	 */

}
};	//	end of SWIG interface class PartialList

#if 0
%{
/*
	PartialListHandleIterator
	
	A class representing an iterator on a PartialList, composed of an iterator
	on a std::list of Partials and a Handle to the std::list itself. Even if the
	last reference (by handle, above) to the Partial list itself is lost, the 
	list will not be deleted as long as there are surviving iterators. 
 */
	class PartialListHandleIterator
	{
		PartialListHandle _list;
		std::list< Loris::Partial >::iterator _iter;
		
		public:
		//	construction:
		PartialListHandleIterator( PartialListHandle hlist, std::list< Loris::Partial >::iterator pos ) :
			_list( hlist ),
			_iter( pos )
		{
			debugger << "created an iterator on a list of " << _list->size() << " Partials" << std::std::endl;
		}
		
		PartialListHandleIterator( const PartialListHandleIterator & rhs ) :
			_list( rhs._list ),
			_iter( rhs._iter )
		{
			debugger << "copied an iterator on a list of " << _list->size() << " Partials" << std::std::endl;
		}
		
		PartialListHandleIterator & operator= ( const PartialListHandleIterator & rhs )
		{
			if ( &rhs != this )
			{
				_list = rhs._list;
				_iter = rhs._iter;
			}
			debugger << "assigned an iterator on a list of " << _list->size() << " Partials" << std::std::endl;
			
			return *this;
		}
			
		~PartialListHandleIterator( void )
		{
			debugger << "destroyed an iterator on a list of " << _list->size() << " Partials" << std::std::endl;
		}
		
		//	Iterator pattern:
		//	(except current(), which isn't useful here)
		void next( void )
		{
			if ( ! atEnd() )
				++_iter;
		}
		
		bool atEnd( void )
		{
			return _iter == _list->end();
		}
		
		Loris::Partial & current( void ) 
		{
			return *_iter;
		}
		
		//	easiest place to implement removal is in this class,
		//	not sure its the most logical place in the interface:
		//	Advances iterator, so this iterator is still valid,
		//	though it may be atEnd(), after removal.
		void removeFromList( void )
		{
			_iter = _list->erase( _iter );
		}
		
	};	//	end of class PartialListHandleIteratorHandle
	
	typedef Loris::Handle< PartialListHandleIterator > PartialListHandleIteratorHandle;
%}

/*
	Partial
	
	The Partial class in the Loris scripting interface represents both
	a Loris::Partial and an iterator on a list of Partials, so its interface
	includes access and mutation of Partials and iterator behavior, like
	next() and atEnd().
	
	Partials (iterators on PartialLists) can only be created using the 
	PartialList members first() and last() (a temporary condition, probably).
 */
%name( Partial )
class PartialListHandleIteratorHandle
{
public:
%addmethods
{
	//	construction:
	//	only construct from PartialList.	
	~PartialListHandleIteratorHandle( void )
	{
		delete self;
	}
	/*	Delete this Partial reference (doesn't remove it from the PartialList).
	 */
	
	//	Iterator pattern:
	void next( void )	{ (*self)->next(); }
	/*	Advance this PartialList iterator to the next position in the list.
	 */
	 
	bool atEnd( void )	{ return (*self)->atEnd(); }
	/*	Return true if this PartialList iterator refers to a position past
		the end of the list (does not refer to a valid Partial). Otherwise
		return false.
	 */

	//	Partial access:
	int label( void ) const 			{ return (*self)->current().label(); }
	/* 	Return this Partial's label.
	 */
	 
	double initialPhase( void ) const	{ return (*self)->current().initialPhase(); }
	/* 	Return this Partial's starting phase.
	 */
	 
	double startTime( void ) const 		{ return (*self)->current().startTime(); }
	/* 	Return this Partial's start time.
	 */

	double endTime( void ) const		{ return (*self)->current().endTime(); }
	/* 	Return this Partial's end time.
	 */
	 
	double duration( void ) const		{ return (*self)->current().duration(); }
	/* 	Return this Partial's duration.
	 */

	long countBreakpoints( void ) const { return (*self)->current().countBreakpoints(); }
	/* 	Return this Partial's number of Breakpoints.
	 */
	
	double frequencyAt( double time ) const	{ return (*self)->current().frequencyAt( time ); }
	/* 	Return this Partial's interpolated frequency at the specified time.
	 */

	double amplitudeAt( double time ) const	{ return (*self)->current().amplitudeAt( time ); }
	/* 	Return this Partial's interpolated amplitude at the specified time.
	 */

	double bandwidthAt( double time ) const	{ return (*self)->current().bandwidthAt( time ); }
	/* 	Return this Partial's interpolated bandwidth at the specified time.
	 */

	double phaseAt( double time ) const		{ return (*self)->current().phaseAt( time ); }
	/* 	Return this Partial's interpolated phase at the specified time.
	 */


	//	mutation:
	void setLabel( int l ) {  (*self)->current().setLabel( l ); }
	/*	Assign a new label to this Partial.
	 */
	
	//	removal from list:
	void removeFromList( void ) {  (*self)->removeFromList(); }
	/*	Remove this Partial from its PartialList, and advance the
		iterator, so that the iterator is still valid (represents
		a valid list position, though it may be atEnd) but refers 
		to the next Partial in the list, or is atEnd.
	 */
	
	//	BreakpointHandle access:
	%new
	BreakpointHandle * first( void )
	{
		return new BreakpointHandle( *self, (*self)->current().begin() );
	}
	/*	Return the first Breakpoint in this Partial.
	 */
	%new
	BreakpointHandle * last( void )
	{
		return new BreakpointHandle( *self, --( (*self)->current().end() ) );
	}
	/*	Return the last Breakpoint in this Partial.
	 */
}
	
};	//	end of SWIG interface class PartialListHandleIteratorHandle

%{	
/*
	BreakpointHandle
	
	A class representing an iterator on a Partial, composed of an iterator on a 
	Partial and a Handle to a PartialListHandleIterator, defined above. Even if 
	the last reference (by handle, above) to the Partial (PartialList iterator) 
	itself or even to the PartialList itself, is lost, the PartialList will not be 
	deleted as long as there are surviving Breakpoints. 
 */
	class BreakpointHandle
	{
		PartialListHandleIteratorHandle _partialH;
		Loris::PartialIterator _iter;
		
		public:
		//	construction:
		BreakpointHandle( PartialListHandleIteratorHandle subject, Loris::PartialIterator pos ) :
			_partialH( subject ),
			_iter( pos )
		{
			debugger << "created an iterator on a partial having " << _partialH->current().countBreakpoints()
					 << " breakpoints" << std::std::endl;
		}
		
		~BreakpointHandle( void )
		{
			debugger << "destroyed an iterator on a partial having " << _partialH->current().countBreakpoints()
					 << " breakpoints" << std::std::endl;
		}
		
		//	attribute access:
		double frequency( void ) const { return _iter.breakpoint().frequency(); }
		double amplitude( void ) const { return _iter.breakpoint().amplitude(); }
		double bandwidth( void ) const { return _iter.breakpoint().bandwidth(); }
		double phase( void ) const { return _iter.breakpoint().phase(); }
		
		//	attribute mutation:
		void setFrequency( double x ) { _iter.breakpoint().setFrequency(x); }
		void setAmplitude( double x ) { _iter.breakpoint().setAmplitude(x); }
		void setBandwidth( double x ) { _iter.breakpoint().setBandwidth(x); }
		void setPhase( double x ) { _iter.breakpoint().setPhase(x); }

		//	time:
		double time( void ) const { return _iter.time(); }
		
		//	iterator behavior:
		void next( void )
		{
			if ( ! atEnd() )
				++_iter;
		}
		
		bool atEnd( void )
		{
			return _iter == _partialH->current().end();
		}
		
	};	//	end of class BreakpointHandle
	
%}

/*
	Breakpoint
	
	The Breakpoint class in the Loris scripting interface represents both
	a Loris::Breakpoint and an iterator on a Loris Partial, so its interface
	includes access and mutation of Breakpoint data and iterator behavior, like
	next() and atEnd().
	
	Breakpoints (iterators on PartialLists) can only be created using the 
	Partial members first() and last().
 */
%name(Breakpoint)
class BreakpointHandle
{
	public:	
	~BreakpointHandle( void );
	/*	Delete this Breakpoint reference (does not remove from the Partial).
	 */

	//	attribute access:
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
	 
	//	time:
	double time( void );
	/*	Return the time of this Breakpoint (not mutable).
	 */
	
	//	iterator behavior:
	void next( void );
	/*	Advance this iterator to the next position (Breakpoint)
		in the Partial. 
	 */
	bool atEnd( void );
	/*	Return true if this iterator refers to a position past the end 
		of the Partial's envelope (does not refer to a valid Breakpoint).
		Otherwise return false.
	 */

};	//	end of class BreakpointHandle

#endif	//	REMOVED

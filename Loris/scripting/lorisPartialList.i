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
#include "Handle.h"
#include "Partial.h"
#include "notifier.h"
#include <list>

using Loris::debugger;
using Loris::endl;

typedef Loris::Handle< std::list< Loris::Partial > > PartialListHandle;

%}

%name(PartialList)
class PartialListHandle
/*	A PartialList represents a collection of Bandwidth-Enhanced 
	Partials, each having a trio of synchronous, non-uniformly-
	sampled breakpoint envelopes representing the time-varying 
	frequency, amplitude, and noisiness of a single bandwidth-
	enhanced sinusoid.

	For more information about Bandwidth-Enhanced Partials and the  
	Reassigned Bandwidth-Enhanced Additive Sound Model, refer to
	the Loris website: www.cerlsoundgroup.org/Loris/
*/
/*
	PartialList by handle:
 */
{
public:
%addmethods
{
//	PartialListHandle( const PartialList * pl )
	PartialListHandle( void )
	{
		// debugger << "creating a list of " << pl->size() << " Partials" << std::endl;
		debugger << "creating an empty list of Partials" << endl;
		return new PartialListHandle();
	}
	/*	Return a new empty PartialList.
	 */
	
	~PartialListHandle( void )
	{
		debugger << "destroying reference to a list of " << (*self)->size() << " Partials" << endl;
		delete self;
	}
	/*	Destroy this PartialList.
	 */
	
	void clear( void ) 
	{
		(*self)->clear();
	}
	/*	Remove (and destroy) all the Partials from this PartialList,
		leaving it empty.
	 */
	
	%new PartialListHandle * copy( void )
	{
		return new PartialListHandle( (*self)->begin(), (*self)->end() );
	}
	/*	Return a new PartialList that is a copy of this 
		PartialList (i.e. has identical Partials).
	 */
	
	unsigned long size( void )
	{
		return (*self)->size();
	}
	/*	Return the number of Partials in this PartialList.
	 */
		
	%new
	PartialListHandleIteratorHandle * first( void )
	{
		return new PartialListHandleIteratorHandle( *self, (*self)->begin() );
	}
	/*	Return an iterator refering to the first Partial in this PartialList.
	 */

	%new
	PartialListHandleIteratorHandle * last( void )
	{
		return new PartialListHandleIteratorHandle( *self, --(*self)->end() );
	}
	/*	Return an iterator refering to the last Partial in this PartialList.
	 */
	
	void append( PartialListHandleIteratorHandle * iter )
	{
		(*self)->push_back( (*iter)->partial() );
	}
	/*	Append a copy of the Partial referenced by the specified iterator
		to the end of this PartialList.
	 */
	 
	void splice( PartialListHandle * otherPartials )
	{
		if ( self == otherPartials )
		{
			std::string s( "Cannot splice a PartialList onto itself!" );
			throw s;
		}
		(*self)->splice( (*self)->end(), *otherPartials );
	}
	/*	Splice all the Partials in the other PartialList onto the end of
		this PartialList, leaving the other empty.
	 */
}
};	//	end of SWIG interface class PartialListHandle


%{
	class PartialListHandleIterator
	{
		PartialListHandle _list;
		std::list< Loris::Partial >::iterator _iter;
		
		//	no default constructor:
		PartialListHandleIterator( void );
		
		public:
		//	construction:
		PartialListHandleIterator( PartialListHandle hlist ) :
			_list( hlist ),
			_iter( hlist->begin() )
		{
			debugger << "created an iterator on a list of " << _list->size() << " Partials" << std::endl;
		}
		
		//	better be careful that pos is an iterator of *hlist!
		PartialListHandleIterator( PartialListHandle hlist, std::list< Loris::Partial >::iterator pos ) :
			_list( hlist ),
			_iter( pos )
		{
			debugger << "created an iterator on a list of " << _list->size() << " Partials" << std::endl;
		}
		
		PartialListHandleIterator( const PartialListHandleIterator & rhs ) :
			_list( rhs._list ),
			_iter( rhs._iter )
		{
			debugger << "copied an iterator on a list of " << _list->size() << " Partials" << std::endl;
		}
		
		PartialListHandleIterator & operator= ( const PartialListHandleIterator & rhs )
		{
			if ( &rhs != this )
			{
				_list = rhs._list;
				_iter = rhs._iter;
			}
			debugger << "assigned an iterator on a list of " << _list->size() << " Partials" << std::endl;
			
			return *this;
		}
			
		~PartialListHandleIterator( void )
		{
			debugger << "destroyed an iterator on a list of " << _list->size() << " Partials" << std::endl;
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
		
		//	easiest place to implement removal is in this class,
		//	not sure its the most logical place in the interface:
		//	Advances iterator, so this iterator is still valid,
		//	though it may be atEnd(), after removal.
		void removeFromList( void )
		{
			_iter = _list->erase( _iter );
		}
		
		//	Partial access:
		int label( void ) const 			{ return _iter->label(); }
		double initialPhase( void ) const	{ return _iter->initialPhase(); }
		double startTime( void ) const 		{ return _iter->startTime(); }
		double endTime( void ) const		{ return _iter->endTime(); }
		double duration( void ) const		{ return _iter->duration(); }
		
		long countBreakpoints( void ) const { return _iter->countBreakpoints(); }
		
		double frequencyAt( double time ) const	{ return _iter->frequencyAt( time ); }
		double amplitudeAt( double time ) const	{ return _iter->amplitudeAt( time ); }
		double bandwidthAt( double time ) const	{ return _iter->bandwidthAt( time ); }
		double phaseAt( double time ) const		{ return _iter->phaseAt( time ); }
	
		//	mutation:
		void setLabel( int l ) { _iter->setLabel( l ); }
		
		Loris::Partial & partial( void ) { return *_iter; }
	
	};	//	end of class PartialListHandleIteratorHandle
	
	typedef Loris::Handle< PartialListHandleIterator > PartialListHandleIteratorHandle;
%}

%name( Partial )
class PartialListHandleIteratorHandle
{
public:
%addmethods
{
	//	construction:
	//PartialListHandleIteratorHandle( PartialListHandle hlist );
	//	PartialListHandleIteratorHandle( const PartialListHandleIteratorHandle & rhs );
	~PartialListHandleIteratorHandle( void )
	{
		delete self;
	}
	
	//	Iterator pattern:
	void next( void )	{ (*self)->next(); }
	bool atEnd( void )	{ return (*self)->atEnd(); }

	//	Partial access:
	int label( void ) const 			{ return (*self)->label(); }
	double initialPhase( void ) const	{ return (*self)->initialPhase(); }
	double startTime( void ) const 		{ return (*self)->startTime(); }
	double endTime( void ) const		{ return (*self)->endTime(); }
	double duration( void ) const		{ return (*self)->duration(); }
	
	long countBreakpoints( void ) const { return (*self)->countBreakpoints(); }
	
	double frequencyAt( double time ) const	{ return (*self)->frequencyAt( time ); }
	double amplitudeAt( double time ) const	{ return (*self)->amplitudeAt( time ); }
	double bandwidthAt( double time ) const	{ return (*self)->bandwidthAt( time ); }
	double phaseAt( double time ) const		{ return (*self)->phaseAt( time ); }

	//	mutation:
	void setLabel( int l ) {  (*self)->setLabel( l ); }
	
	//	removal from list:
	//	Advances iterator, so this iterator is still valid,
	//	though it may be atEnd(), after removal.
	void removeFromList( void ) {  (*self)->removeFromList(); }
	
	//	BreakpointHandle access:
	%new
	BreakpointHandle * first( void )
	{
		return new BreakpointHandle( *self, (*self)->partial().begin() );
	}
	%new
	BreakpointHandle * last( void )
	{
		return new BreakpointHandle( *self, --( (*self)->partial().end() ) );
	}
	
}
	
};	//	end of SWIG interface class PartialListHandleIteratorHandle

%{	
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
			debugger << "created an iterator on a partial having " << _partialH->countBreakpoints()
					 << " breakpoints" << std::endl;
		}
		
		~BreakpointHandle( void )
		{
			debugger << "destroyed an iterator on a partial having " << _partialH->countBreakpoints()
					 << " breakpoints" << std::endl;
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
			return _iter == _partialH->partial().end();
		}
		
	};	//	end of class BreakpointHandle
	
%}

%name(Breakpoint)
class BreakpointHandle
{
	public:	
	~BreakpointHandle( void );

	//	attribute access:
	double frequency( void );
	double amplitude( void );
	double bandwidth( void );
	double phase( void );
	
	//	attribute mutation:
	void setFrequency( double x );
	void setAmplitude( double x );
	void setBandwidth( double x );
	void setPhase( double x );

	//	time:
	double time( void );
	
	//	iterator behavior:
	void next( void );
	bool atEnd( void );

};	//	end of class BreakpointHandle

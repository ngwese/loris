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
 *	A PartialList is a std::list< Loris::Partial >.
 *	Include this file in loris.i to include the PartialList class
 *	interface in the scripting module. (Can be used with the 
 *	-shadow option to SWIG to build an Analyzer class in the 
 *	Python interface.) This file does not support exactly the 
 *	public interface of the C++ std::list class, but has been 
 *	modified to better support SWIG and scripting languages.
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
#include <list>
typedef std::list< Loris::Partial > PartialList;
%}

// ---------------------------------------------------------------------------
//	class PartialList
//	
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
	PartialList( void );
	/*	Return a new empty PartialList.
	 */

	~PartialList( void );
	/*	Destroy this PartialList.
	 */

	void clear( void );
	/*	Remove (and destroy) all the Partials from this PartialList,
		leaving it empty.
	 */
	
	unsigned long size( void );
	/*	Return the number of Partials in this PartialList.
	 */
	
%addmethods
{
	void splice( PartialList & other )
	{
		if ( self == &other )
		{
			std::string s( "Cannot splice a PartialList onto itself!" );
			throw s;
		}
		self->splice( self->end(), other );
	}
	/*	Splice all the Partials in the other PartialList onto the end of
		this PartialList, leaving the other empty.
	 */
}

};	//	end of (SWIG) class PartialList

//	define a copy constructor:
//	(this should give the right documentation, the 
//	right ownership, the right function name in the
//	module, etc.)
%{
PartialList * PartialListCopy_( const PartialList * other )
{
	return new PartialList( *other );
}
%}

%name( PartialListCopy )  
%new PartialList * PartialListCopy_( const PartialList * other );
/*	Return a new PartialList that is a copy of this 
	PartialList (i.e. has identical Partials).
 */
 
/*
	EXPERIMENTAL JUNK:
 */
 
%{
	#include "notifier.h"
	using Loris::notifier;
	
	#include "Handle.h"
	typedef Loris::Handle< PartialList > PartialListH;
	
	class PartialListHIter_
	{
		PartialListH _list;
		PartialList::iterator _iter;
		
		//	no default constructor:
		PartialListHIter_( void );
		
		public:
		//	construction:
		PartialListHIter_( PartialListH hlist ) :
			_list( hlist ),
			_iter( hlist->begin() )
		{
			notifier << "created an iterator on a list of " << _list->size() << " Partials" << std::endl;
		}
		
		//	better be careful that pos is an iterator of *hlist!
		PartialListHIter_( PartialListH hlist, PartialList::iterator pos ) :
			_list( hlist ),
			_iter( pos )
		{
			notifier << "created a risky iterator on a list of " << _list->size() << " Partials" << std::endl;
		}
		
		PartialListHIter_( const PartialListHIter_ & rhs ) :
			_list( rhs._list ),
			_iter( rhs._iter )
		{
			notifier << "copied an iterator on a list of " << _list->size() << " Partials" << std::endl;
		}
		
		PartialListHIter_ & operator= ( const PartialListHIter_ & rhs )
		{
			if ( &rhs != this )
			{
				_list = rhs._list;
				_iter = rhs._iter;
			}
			notifier << "assigned an iterator on a list of " << _list->size() << " Partials" << std::endl;
			
			return *this;
		}
			
		~PartialListHIter_( void )
		{
			notifier << "destroyed an iterator on a list of " << _list->size() << " Partials" << std::endl;
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
	
	};	//	end of class PartialListHIter
	
	typedef Loris::Handle< PartialListHIter_ > PartialListHIter;
%}
class PartialListH
/*
	PartialList by handle:
 */
{
public:
%addmethods
{
	PartialListH( const PartialList * pl )
	{
		notifier << "creating a list of " << pl->size() << " Partials" << std::endl;
		return new PartialListH( pl->begin(), pl->end() );
	}
	
	~PartialListH( void )
	{
		notifier << "destroying a list of " << (*self)->size() << " Partials" << std::endl;
		delete self;
	}
	
	void clear( void ) 
	{
		(*self)->clear();
	}
	
	unsigned long size( void )
	{
		return (*self)->size();
	}
	
	%new
	PartialList * list( void )
	{
		return new PartialList( (*self)->begin(), (*self)->end() );
	}
	
	%new
	PartialListHIter * first( void )
	{
		return new PartialListHIter( *self, (*self)->begin() );
	}
	%new
	PartialListHIter * last( void )
	{
		return new PartialListHIter( *self, --(*self)->end() );
	}
	
	//	insertion of new Partials:
	void append( PartialListHIter * ph )
	{
		(*self)->push_back( (*ph)->partial() );
	}
}
};	//	end of SWIG interface class PartialListH

class PartialListHIter
{
public:
%addmethods
{
	//	construction:
	//PartialListHIter( PartialListH hlist );
	//	PartialListHIter( const PartialListHIter & rhs );
	~PartialListHIter( void )
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
	
	//	BP access:
	%new
	BP * first( void )
	{
		return new BP( *self, (*self)->partial().begin() );
	}
	%new
	BP * last( void )
	{
		return new BP( *self, --( (*self)->partial().end() ) );
	}
	
}
	
};	//	end of SWIG interface class PartialListHIter

%{	
	class BP
	{
		PartialListHIter _partialH;
		Loris::PartialIterator _iter;
		
		public:
		//	construction:
		BP( PartialListHIter subject, Loris::PartialIterator pos ) :
			_partialH( subject ),
			_iter( pos )
		{
			notifier << "created an iterator on a partial having " << _partialH->countBreakpoints()
					 << " breakpoints" << std::endl;
		}
		
		~BP( void )
		{
			notifier << "destroyed an iterator on a partial having " << _partialH->countBreakpoints()
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
		
	};	//	end of class BP
	
%}

class BP
{
	public:	
	~BP( void );

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

};	//	end of class BP

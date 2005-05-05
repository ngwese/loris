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
 *	lorisPartialList.i
 *
 *	SWIG interface file describing the PartialList class.
 *	A PartialList is a Loris::Handle<  >.
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
 
%newobject *::insert;
%newobject SwigPartialIterator::next;
	// but not SwigPListIterator::next
%newobject *::__iter__;
%newobject *::iterator;

%newobject *::findAfter;
%newobject *::findNearest;
	

/* ***************** inserted C++ code ***************** */
%{
#include <Partial.h>
#include <PartialList.h>
#include <Notifier.h>

using Loris::debugger;
using Loris::Partial;
using Loris::PartialList;
using Loris::PartialListIterator;
using Loris::Breakpoint;


/*	new iterator definitions

	These are much better than the old things, more like the 
	iterators in Python 2.2 and later, very much simpler.
*/
struct SwigPListIterator
{
	PartialList & subject;
	PartialList::iterator it;

	SwigPListIterator( PartialList & l ) : subject( l ), it ( l.begin() ) {}
	SwigPListIterator( PartialList & l, PartialList::iterator i ) : subject( l ), it ( i ) {}
	
	bool atEnd( void ) { return it == subject.end(); }
	bool hasNext( void ) { return !atEnd(); }

	Partial * next( void )
	{
		if ( atEnd() )
		{
			throw_exception("end of PartialList");
			return 0;
		}
		Partial * ret = &(*it);
		++it;
		return ret;
	}
};

typedef Partial::iterator BreakpointPosition;

struct SwigPartialIterator
{
	Partial & subject;
	Partial::iterator it;

	SwigPartialIterator( Partial & p ) : subject( p ), it ( p.begin() ) {}
	SwigPartialIterator( Partial & p, Partial::iterator i ) : subject( p ), it ( i ) {}
	
	bool atEnd( void ) { return it == subject.end(); }
	bool hasNext( void ) { return !atEnd(); }

	BreakpointPosition * next( void )
	{
		if ( atEnd() )
		{
			throw_exception("end of Partial");
			return 0;
		}
		BreakpointPosition * ret = new BreakpointPosition(it);
		++it;
		return ret;
	}
};

%}
/* ***************** end of inserted C++ code ***************** */

/* *********** exception handling for new iterators *********** */
/*	Exception handling code copied from the SWIG manual. 
	Tastes great, less filling.
	Defined in loris.i.
*/

%include exception.i
%exception next
{
    char * err;
    clear_exception();
    $action
    if ((err = check_exception()))
    {
#if defined(SWIGPYTHON)
		%#ifndef NO_PYTHON_EXC_STOPITER
		PyErr_SetString( PyExc_StopIteration, err );
		return NULL;
		%#else
		SWIG_exception( SWIG_ValueError, err );
		%#endif
#else
        SWIG_exception( SWIG_ValueError, err );
#endif
    }
}

%exception PartialList::erase
{
    char * err;
    clear_exception();
    $action
    if ((err = check_exception()))
    {
        SWIG_exception( SWIG_ValueError, err );
    }
}

#ifdef LEGACY_ITERATOR_BEHAVIOR
%exception SwigPListIterator::partial
{
    char * err;
    clear_exception();
    $action
    if ((err = check_exception()))
    {
        SWIG_exception( SWIG_ValueError, err );
    }
}
#endif
/* ******** end of exception handling for new iterators ******** */

/* ***************** new PartialList iterator ****************** */

%rename (PartialListIterator) SwigPListIterator;
%nodefault SwigPListIterator;
class SwigPListIterator
{
public:
	bool atEnd( void );
	Partial * next( void );
#ifdef SIWGPYTHON
    %extend
    {
        SwigPListIterator * __iter__( void )
        {
            return self;
        }

        SwigPListIterator * iterator( void )
        {
            return self;
        }
    }
#endif
#ifdef LEGACY_ITERATOR_BEHAVIOR
	%extend
	{
		Partial * partial( void )
		{
			if ( self->atEnd() )
			{
				throw_exception("end of PartialList");
				return 0;
			}			
			Partial & current = *(self->it);
			return &current;
		}
	}
#endif
};

/* ************** end of new PartialList iterator ************** */

/* ******************** new Partial iterator ******************* */

%rename (PartialIterator) SwigPartialIterator;
%nodefault SwigPartialIterator;
class SwigPartialIterator
{
public:
	bool atEnd( void );
	bool hasNext( void );
	BreakpointPosition * next( void );
#ifdef SIWGPYTHON
    %extend
    {
        SwigPartialIterator * __iter__( void )
        {
            return self;
        }

        SwigPartialIterator * iterator( void )
        {
            return self;
        }
    }
#endif
};

/* **************** end of new Partial iterator **************** */

/* ************************ PartialList ************************ */

%feature("docstring",
"A PartialList represents a collection of Bandwidth-Enhanced 
Partials, each having a trio of synchronous, non-uniformly-
sampled breakpoint envelopes representing the time-varying 
frequency, amplitude, and noisiness of a single bandwidth-
enhanced sinusoid.

For more information about Bandwidth-Enhanced Partials and the  
Reassigned Bandwidth-Enhanced Additive Sound Model, refer to
the Loris website: www.cerlsoundgroup.org/Loris/") PartialList;

class PartialList
{
public:
	//	PartialList construction:
	PartialList( void );
	PartialList( const PartialList & rhs );
	~PartialList( void );
	
	//	std::list methods:
	void clear( void );	
	unsigned long size( void );

	%extend
	{
		//	(new-style) iterator access:
		SwigPListIterator * iterator( void )
		{
			return new SwigPListIterator(*self);
		}
		#ifdef SWIGPYTHON
		SwigPListIterator * __iter__( void )
		{
			return new SwigPListIterator(*self);
		}

		unsigned long __len__( void ) 
		{
			return self->size();
		}
		#endif	
		
		//  append does not return position of inserted element:
		void append( Partial * partial )
		{
			self->insert( self->end(), *partial );
		}
		void append( PartialList * other )
		{
			self->insert( self->end(), other->begin(), other->end() );
		}
	
		//  implement erase using a linear search to find
		//  the Partial that should be removed -- slow and
		//  gross, but the only straightforward way to make
		//  erase play nice with the new iterator paradigm
		//  (especially in Python). Raise an exception if
		//  the specified Partial is not in the list.
		void erase( Partial * partial )
		{
			PartialList::iterator it = self->begin();
			while ( it != self->end() )
			{
				if ( &(*it) == partial )	// compare addresses
				{
					self->erase( it );
					return;
				}
				++it;
			}
			throw_exception( "PartialList.erase(p): p not in PartialList" );
		}
		 

		Partial * first( void )
		{
			if ( self->empty() )
			{
				return 0;
			}
			else
			{
				return &( self->front() );
			}
		}

		Partial * last( void )
		{
			if ( self->empty() )
			{
				return 0;
			}
			else
			{
				return &( self->back() );
			}
		}

	}	//	end of added methods

};

/* ********************* end of PartialList ********************* */

/* ************************** Partial *************************** */

%feature("docstring",
"A Partial represents a single component in the
reassigned bandwidth-enhanced additive model. A Partial consists of a
chain of Breakpoints describing the time-varying frequency, amplitude,
and bandwidth (or noisiness) envelopes of the component, and a 4-byte
label. The Breakpoints are non-uniformly distributed in time. For more
information about Reassigned Bandwidth-Enhanced Analysis and the
Reassigned Bandwidth-Enhanced Additive Sound Model, refer to the Loris
website: www.cerlsoundgroup.org/Loris/.
") Partial;

class Partial
{
public:
	//	Partial construction:
	Partial( void );
	Partial( const Partial & );
	~Partial( void );
	
	//	Partial access and mutation:
	int label( void );
	double initialPhase( void );
	double startTime( void );
	double endTime( void );
	double duration( void );
	long numBreakpoints( void );
	
	void setLabel( int l );
		
	//	partial envelope interpolation/extrapolation:
	//	Return the interpolated value of a partial parameter at
	//	the specified time. At times beyond the ends of the
	//	Partial, frequency and bandwidth hold their boundary values,
	//	amplitude is zero, and phase is computed from frequency.
	//	There is of sensible definition for any of these for Partials
	//	having no Breakpoints, so they except (InvalidPartial) under 
	//	that condition.
	double frequencyAt( double time );
	double amplitudeAt( double time );
	double bandwidthAt( double time );
	double phaseAt( double time );

	%extend
	{
		//	new iterator access:
		SwigPartialIterator * iterator( void )
		{
			return new SwigPartialIterator(*self);
		}
		#ifdef SWIGPYTHON
		SwigPartialIterator * __iter__( void )
		{
			return new SwigPartialIterator(*self);
		}
		#endif	

		//	erase works nicely with the new iterators:
		void erase( BreakpointPosition * pos )
		{
			if ( *pos != self->end() )
			{
				*pos = self->erase( *pos );
			}
		}

        Breakpoint * first( void )
        {
            if ( self->numBreakpoints() == 0 )
            {
                return 0;
            }
            else
            {
                return &( self->first() );
            }
        }

        Breakpoint * last( void )
        {
            if ( self->numBreakpoints() == 0 )
            {
                return 0;
            }
            else
            {
                return &( self->last() );
            }
        }

		SwigPartialIterator * insert( double time, const Breakpoint & bp )
		{
			return new SwigPartialIterator(*self, self->insert( time, bp ));
		}
		
		SwigPartialIterator * findAfter( double time )
		{
			return new SwigPartialIterator(*self, self->findAfter( time ));
		}
	
		SwigPartialIterator * findNearest( double time )
		{
			return new SwigPartialIterator(*self, self->findNearest( time ));
		}
	}		
};

/* *********************** end of Partial *********************** */

/* ************************* Breakpoint ************************* */

%feature("docstring",
"A Breakpoint represents a single breakpoint in the time-varying
frequency, amplitude, and bandwidth envelope of a Reassigned 
Bandwidth-Enhanced Partial.

Instantaneous phase is also stored, but is only used at the onset of 
a partial, or when it makes a transition from zero to nonzero amplitude.

A Partial represents a Reassigned Bandwidth-Enhanced model component.
For more information about Bandwidth-Enhanced Partials and the  
Reassigned Bandwidth-Enhanced Additive Sound Model, refer to
the Loris website: www.cerlsoundgroup.org/Loris/
") Breakpoint;

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

	Breakpoint( const Breakpoint & rhs );
	/*	Return a new Breakpoint that is a copy of this 
		Breakpoint (i.e. has identical parameter values).
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
	 	
};	//	//	end of SWIG interface class Breakpoint


%feature("docstring",
"A BreakpointPosition represents the position of a 
Breakpoint within a Partial." ) BreakpointPosition;

%nodefault BreakpointPosition;
class BreakpointPosition
{
public:
	%extend
	{
		double time( void ) 
		{ 
			return self->time(); 
		}
		Breakpoint * breakpoint( void ) 
		{ 
			return &(self->breakpoint());
		}
		
		//	duplicate the Breakpoint interface:
		//	(not sure yet whether this is the right way)
		//
		
		double frequency( void ) { return self->breakpoint().frequency(); }
		/*	Return the frequency of this Breakpoint. 
		*/
		
		double amplitude( void ) { return self->breakpoint().amplitude(); }
		/*	Return the amplitude of this Breakpoint. 
		*/
		
		double bandwidth( void ) { return self->breakpoint().bandwidth(); }
		/*	Return the bandwidth of this Breakpoint. 
		*/
		
		double phase( void ) { return self->breakpoint().phase(); }
		/*	Return the phase of this Breakpoint. 
		*/
			
		void setFrequency( double x ) { self->breakpoint().setFrequency( x ); }
		/*	Assign the frequency of this Breakpoint. 
		*/
		
		void setAmplitude( double x ) { self->breakpoint().setAmplitude( x ); }
		/*	Assign the amplitude of this Breakpoint. 
		*/
		
		void setBandwidth( double x ) { self->breakpoint().setBandwidth( x ); }
		/*	Assign the bandwidth of this Breakpoint. 
		*/
		
		void setPhase( double x ) { self->breakpoint().setPhase( x ); }
		/*	Assign the phase of this Breakpoint. 
		*/
		
	}
};


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
 * Channelizer.C
 *
 * Implementation of class Channelizer.
 *
 * Kelly Fitz, 21 July 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include <config.h>
#endif

#include<Channelizer.h>
#include<Envelope.h>
#include<Partial.h>
#include<Notifier.h>
#include <list>
#include <memory>


//	for debugging
#ifdef Debug_Loris
#include <set>
#endif

//	begin namespace
namespace Loris {


// ---------------------------------------------------------------------------
//	class Channelizer_imp
//
//	Implementation class for Channelizer insulating Channelizer interface.
//
class Channelizer_imp
{
public:
	const std::auto_ptr<Envelope> _refChannelFreq;
	int _refChannelLabel;

	Channelizer_imp( const Envelope & env, int label );
	~Channelizer_imp( void );
	
	void channelize( std::list< Partial >::iterator begin, std::list< Partial >::iterator end );
	void channelize_one( Partial & p, double tstart, double tend );

private:
	//	don't allow copy or assign:
	Channelizer_imp( const Channelizer_imp & );
	Channelizer_imp & operator=( const Channelizer_imp & );
};	//	end of class Channelizer_imp

// ---------------------------------------------------------------------------
//	Channelizer_imp constructor 
// ---------------------------------------------------------------------------
//
Channelizer_imp::Channelizer_imp( const Envelope & env, int label ) :
	_refChannelFreq( env.clone() ),
	_refChannelLabel( label )
{
	if ( label <= 0 )
		Throw( InvalidArgument, "Channelizer reference label must be positive." );
}

// ---------------------------------------------------------------------------
//	Channelizer_imp destructor
// ---------------------------------------------------------------------------
//
Channelizer_imp::~Channelizer_imp( void )
{
}

// ---------------------------------------------------------------------------
//	loudestAt (STATIC)
// ---------------------------------------------------------------------------
//	Helper for finding the time at which a Partial
//	attains its maximum amplitude.
//
//	Use sinusoidal amplitude, so that repeated channelizations and
//	distillations yield identical results.
//
//	This may not be used at all, we may instead use a weighted
//	average for determining channel number of a Partial, instead
//	of evaluating it at its loudest Breakpoint. Unresolved at this
//	time.
//
static double loudestAt( const Partial & p )
{
	PartialConstIterator env = p.begin();
	double maxAmp = env.breakpoint().amplitude() * std::sqrt( 1. - env.breakpoint().bandwidth() );
	double time = env.time();
	
	for ( ++env; env != p.end(); ++env ) 
	{
		double a = env.breakpoint().amplitude() * std::sqrt( 1. - env.breakpoint().bandwidth() );
		if ( a > maxAmp ) 
		{
			maxAmp = a;
			time = env.time();
		}
	}
	return time;
}

// ---------------------------------------------------------------------------
//	Channelizer_imp channelize
// ---------------------------------------------------------------------------
//	Two algorithms are defined here, the fancier one computes the best channel
//	label for each Partial by computing the amplitude-weighted average 
//	frequency channel for the Partial. The simple algorithm just evaluates the
//	frequency channel at the time of peak sinusoidal amplitude for each 
//	Partial. Still resolving which algorithm to use. Only matter for reference
//	envelopes which badly match the Partials to be channelized.
//	
void
Channelizer_imp::channelize( std::list< Partial >::iterator begin, std::list< Partial >::iterator end )
{
#ifdef Debug_Loris
	std::set<int> labelsfound;
#endif

	for ( std::list< Partial >::iterator it = begin; it != end; ++it ) 
	{

		//	just define these here for now, ultimately will be arguments:
		double tstart = it->startTime();
		double tend = it->endTime();

		channelize_one(*it, tstart, tend);
		
#ifdef Debug_Loris
		int label = it->label();
		if (label > 0)
			labelsfound.insert(label);
#endif
	}

#ifdef Debug_Loris
	debugger << "found " << labelsfound.size() << " non-empty channels" << endl;	
#endif
}

// ---------------------------------------------------------------------------
//	Channelizer_imp channelize_one
// ---------------------------------------------------------------------------
//	Helper function to channelize a single partial over a specified
//	time span, called by channelize() above.
//
void
Channelizer_imp::channelize_one( Partial & p, double tstart, double tend )
{
	//	bail if this Partial does not overlap the specified
	//	time span:
	if ( p.startTime() > tend || p.endTime() < tstart )
	{
		debugger << "channelizer skipping Partial spanning ";
		debugger << p.startTime() << "," << p.endTime() << " outside of span ";
		debugger << tstart << "," << tend << endl;
		return;
	}

	debugger << "channelizing Partial with " << p.numBreakpoints() << " Breakpoints" << endl;
		
	//	compute an amplitude-weighted average channel
	//	label for each Partial:
	double ampsum = 0.;
	double weightedlabel = 0.;
	PartialConstIterator bp;
	for ( bp = p.findAfter(tstart); bp != p.end(); ++bp )
	{
		//	bail if the current breakpoint is outside of the
		//	specified time span:
		if ( bp.time() > tend )
		{
			debugger << "channelizer reached end of time span" << endl;
			break;
		}
	
		//	use sinusoidal amplitude:
		double a = bp.breakpoint().amplitude() * std::sqrt( 1. - bp.breakpoint().bandwidth() );
		double f = bp.breakpoint().frequency();
		double t = bp.time();
		
		double refFreq = _refChannelFreq->valueAt( t ) / _refChannelLabel;
		weightedlabel += a * (f / refFreq);
		ampsum += a;
	}
	
	int label;
	if ( ampsum > 0. )	
		label = (int)((weightedlabel / ampsum) + 0.5);
	else	//	this should never happen, but just in case:
		label = 0;
	Assert( label >= 0 );

			
	//	assign label, and remember it, but
	//	only if it is a valid (positive) 
	//	distillation label:
	p.setLabel( label );

}	//	end of channelize_one

// ---------------------------------------------------------------------------
//	Channelizer constructor 
// ---------------------------------------------------------------------------
//	In fully-insulating interface constructors, with no subclassing (fully-
//	insulating interfaces have no virtual members), can safely initialize the
//	imp pointer this way because only that constructor could generate an 
//	exception, and in that case the associated memory would be released 
//	automatically. So there's no risk of a memory leak associated with this
//	pointer initialization.
//
Channelizer::Channelizer( const Envelope & refChanFreq, int refChanLabel ) :
	_imp( new Channelizer_imp( refChanFreq, refChanLabel ) )
{
}

// ---------------------------------------------------------------------------
//	Channelizer copy constructor 
// ---------------------------------------------------------------------------
//
Channelizer::Channelizer( const Channelizer & other ) :
	_imp( new Channelizer_imp( * other._imp->_refChannelFreq, other._imp->_refChannelLabel ) )
{
}

// ---------------------------------------------------------------------------
//	Channelizer assignment 
// ---------------------------------------------------------------------------
Channelizer & 
Channelizer::operator=( const Channelizer & rhs )
{
	if ( &rhs != this )
	{
		//	two different Channelizers had better
		//	never share an imp!
		if ( _imp != rhs._imp )
		{
			delete _imp;
			_imp = new Channelizer_imp( * rhs._imp->_refChannelFreq, rhs._imp->_refChannelLabel );
		}
		else
		{
			debugger << "Yikes! Two different Channelizers were sharing an implementation!" << endl;
		}
	}
	return *this;
}

// ---------------------------------------------------------------------------
//	Channelizer destructor
// ---------------------------------------------------------------------------
//
Channelizer::~Channelizer( void )
{
	delete _imp;
}

// ---------------------------------------------------------------------------
//	Channelizer channelize
// ---------------------------------------------------------------------------
//	Delegate to implementation.
//	
void
Channelizer::channelize( std::list< Partial >::iterator begin, std::list< Partial >::iterator end ) const
{
	_imp->channelize( begin, end );
}

}	//	end of namespace Loris

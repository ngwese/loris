/*
 * Copyright (c) 1999-2000 Kelly Fitz and Lippold Haken
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
#include "Channelizer.h"
#include "Handle.h"
#include "Envelope.h"
#include "Partial.h"
#include "notifier.h"

//	for debugging
#ifdef Debug_Loris
#include <set>
#endif

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class Channelizer_imp
//
//	Implementation class for Channelizer insulating Channelizer interface.
//
class Channelizer_imp
{
public:
	Handle<Envelope> _refChannelFreq;
	int _refChannelLabel;

	Channelizer_imp( Handle<Envelope> env, int label );
	~Channelizer_imp( void );
	
	void channelize( PartialList::iterator begin, PartialList::iterator end );

};	//	end of class Channelizer_imp

// ---------------------------------------------------------------------------
//	Channelizer_imp constructor 
// ---------------------------------------------------------------------------
//	Can verify in Meyer that when the exception is thrown, the Handle<>
//	destructor is guaranteed to be called.
//
Channelizer_imp::Channelizer_imp( Handle< Envelope > env, int label ) :
	_refChannelFreq( env ),
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
	double maxAmp = env->amplitude() * std::sqrt( 1. - env->bandwidth() );
	double time = env.time();
	
	for ( ++env; env != p.end(); ++env ) {
		double a = env->amplitude() * std::sqrt( 1. - env->bandwidth() );
		if ( a > maxAmp ) {
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
Channelizer_imp::channelize( PartialList::iterator begin, PartialList::iterator end )
{
#ifdef Debug_Loris
	std::set<int> labelsfound;
#endif

	for ( PartialList::iterator it = begin; it != end; ++it ) 
	{
		#define FANCY
		#ifdef FANCY

		//	compute an amplitude-weighted average channel
		//	label for each Partial:
		double ampsum = 0.;
		double weightedlabel = 0.;
		PartialConstIterator bp;
		for ( bp = it->begin(); bp != it->end(); ++bp )
		{
			//	use sinusoidal amplitude:
			double a = bp->amplitude() * std::sqrt( 1. - bp->bandwidth() );
			double f = bp->frequency();
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
		
		#else	//	not def FANCY

		//	less fancy, just use label calculated at 
		//	time of peak sinusoidal amplitude.
		//	
		//	calculate time of peak sunusoidal amplitude:
		double time = loudestAt( *it );
					
		//	get reference frequency at time:
		double refFreq = refFreqEnvelope->valueAt( time ) / refLabel;
		
		//	compute the label for this partial as 
		//	nearest integer multiple of reference 
		//	frequency at time:
		int label = (int)((it->frequencyAt( time ) / refFreq) + 0.5);
	
		#endif	// def FANCY
			
		//	assign label, and remember it, but
		//	only if it is a valid (positive) 
		//	distillation label:
		it->setLabel( label );

#ifdef Debug_Loris
		if (label > 0)
			labelsfound.insert(label);
#endif
	}

#ifdef Debug_Loris
	debugger << "found " << labelsfound.size() << " non-empty channels" << endl;	
#endif
}

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
Channelizer::Channelizer( Handle< Envelope > env, int label ) :
	_imp( new Channelizer_imp( env, label ) )
{
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
Channelizer::channelize( PartialList::iterator begin, PartialList::iterator end )
{
	_imp->channelize( begin, end );
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

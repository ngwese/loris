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
	#include "config.h"
#endif

#include "Channelizer.h"
#include "Envelope.h"
#include "Partial.h"
#include "PartialList.h"
#include "Notifier.h"

#include <cmath>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	Channelizer constructor 
// ---------------------------------------------------------------------------
//
Channelizer::Channelizer( const Envelope & env, int label ) :
	_refChannelFreq( env.clone() ),
	_refChannelLabel( label )
{
	if ( label <= 0 )
	{
		Throw( InvalidArgument, "Channelizer reference label must be positive." );
	}
}

// ---------------------------------------------------------------------------
//	Channelizer copy constructor 
// ---------------------------------------------------------------------------
//
Channelizer::Channelizer( const Channelizer & other ) :
	_refChannelFreq( other._refChannelFreq->clone() ),
	_refChannelLabel( other._refChannelLabel )
{
}

// ---------------------------------------------------------------------------
//	Channelizer assignment 
// ---------------------------------------------------------------------------
//
Channelizer & 
Channelizer::operator=( const Channelizer & rhs )
{
	if ( &rhs != this )
	{
		_refChannelFreq.reset( rhs._refChannelFreq->clone() );
		_refChannelLabel = rhs._refChannelLabel;
	}
	return *this;
}


// ---------------------------------------------------------------------------
//	Channelizer_imp destructor
// ---------------------------------------------------------------------------
//
Channelizer::~Channelizer( void )
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
	Partial::const_iterator env = p.begin();
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
//	channelize (one Partial)
// ---------------------------------------------------------------------------
//	Label a Partial with the number of the frequency channel containing
//	the greatest portion of its (the Partial's) energy.
//
void
Channelizer::channelize( Partial & p ) const
{
	debugger << "channelizing Partial with " << p.numBreakpoints() << " Breakpoints" << endl;
		
	//	compute an amplitude-weighted average channel
	//	label for each Partial:
	double ampsum = 0.;
	double weightedlabel = 0.;
	Partial::const_iterator bp;
	for ( bp = p.begin(); bp != p.end(); ++bp )
	{
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

}

}	//	end of namespace Loris

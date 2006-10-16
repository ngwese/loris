/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2006 by Kelly Fitz and Lippold Haken
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
//!	Construct a new Channelizer using the specified reference
//!	Envelope to represent the a numbered channel. If the sound
//! being channelized is known to have detuned harmonics, a 
//! stretching factor can be specified (defaults to 0 for no 
//! stretching). The stretching factor can be computed using
//! the static member computeStretchFactor.
//!	
//!	\param 	refChanFreq is an Envelope representing the center frequency
//!		    of a channel.
//!	\param  refChanLabel is the corresponding channel number (i.e. 1
//!		    if refChanFreq is the lowest-frequency channel, and all 
//!		    other channels are harmonics of refChanFreq, or 2 if  
//!		    refChanFreq tracks the second harmonic, etc.).
//! \param  stretchFactor is a stretching factor to account for detuned 
//!         harmonics, default is 0. 
//! \throw  InvalidArgument if refChanLabel is not positive.
Channelizer::Channelizer( const Envelope & refChanFreq, int refChanLabel, double stretchFactor ) :
	_refChannelFreq( refChanFreq.clone() ),
	_refChannelLabel( refChanLabel ),
	_stretchFactor( stretchFactor )
{
	if ( refChanLabel <= 0 )
	{
		Throw( InvalidArgument, "Channelizer reference label must be positive." );
	}
}

// ---------------------------------------------------------------------------
//	Channelizer copy constructor 
// ---------------------------------------------------------------------------
//!	Construct a new Channelizer that is an exact copy of another.
//!	The copy represents the same set of frequency channels, constructed
//!	from the same reference Envelope and channel number.
//!	
//!	\param other is the Channelizer to copy
//
Channelizer::Channelizer( const Channelizer & other ) :
	_refChannelFreq( other._refChannelFreq->clone() ),
	_refChannelLabel( other._refChannelLabel ),
	_stretchFactor( other._stretchFactor )
{
}

// ---------------------------------------------------------------------------
//	Channelizer assignment 
// ---------------------------------------------------------------------------
//!	Assignment operator: make this Channelizer an exact copy of another. 
//!	This Channelizer is made to represent the same set of frequency channels, 
//!	constructed from the same reference Envelope and channel number as @a rhs.
//!
//!	\param rhs is the Channelizer to copy
//
Channelizer & 
Channelizer::operator=( const Channelizer & rhs )
{
	if ( &rhs != this )
	{
		_refChannelFreq.reset( rhs._refChannelFreq->clone() );
		_refChannelLabel = rhs._refChannelLabel;
		_stretchFactor = rhs._stretchFactor;
	}
	return *this;
}


// ---------------------------------------------------------------------------
//	Channelizer destructor
// ---------------------------------------------------------------------------
//!	Destroy this Channelizer.
Channelizer::~Channelizer( void )
{
}

// ---------------------------------------------------------------------------
//	stretchFactor
// ---------------------------------------------------------------------------
//! Return the stretching factor used to account for detuned
//! harmonics, as in a piano tone. Normally set to 0 for 
//! in-tune harmonics.
double Channelizer::stretchFactor( void ) const
{
    return _stretchFactor;
}

// ---------------------------------------------------------------------------
//	setStretchFactor
// ---------------------------------------------------------------------------
//! Set the stretching factor used to account for detuned
//! harmonics, as in a piano tone. Normally set to 0 for 
//! in-tune harmonics.
void Channelizer::setStretchFactor( double stretch )
{
    _stretchFactor = stretch;
}

// ---------------------------------------------------------------------------
//	giveMeN (STATIC)
// ---------------------------------------------------------------------------
//  Compute the (fractional) channel number for a frequency given a 
//  reference frequency (corresponding to channel 1, the fundamental)
//  and a stretch factor.
//
static double giveMeN( double fn, double fref, double stretch )
{
    using std::sqrt;
    using std::pow;
    
    if ( 0 == stretch )
    {
        return fn / fref;
    }
    const double frefsqrd = fref*fref;
    double num = sqrt( (frefsqrd*frefsqrd) + (4*stretch*frefsqrd*fn*fn) ) - (frefsqrd);
    double denom = 2*stretch*frefsqrd;
    return sqrt( num / denom );
}

// ---------------------------------------------------------------------------
//	computeStretchFactor (STATIC)
// ---------------------------------------------------------------------------
double 
Channelizer::computeStretchFactor( double fref, double fn, double n )
{
    double num = (fn*fn) - (n*n*fref*fref);
    double denom = (n*n*n*n)*(fref*fref);
    return num / denom;
}

// ---------------------------------------------------------------------------
//	channelize (one Partial)
// ---------------------------------------------------------------------------
//!	Label a Partial with the number of the frequency channel corresponding to
//!	the average frequency over all the Partial's Breakpoints.
//!	
//!	\param partial is the Partial to label.
//
void
Channelizer::channelize( Partial & partial ) const
{
	debugger << "channelizing Partial with " << partial.numBreakpoints() << " Breakpoints" << endl;
			
	//	compute an amplitude-weighted average channel
	//	label for each Partial:
	//double ampsum = 0.;
	double weightedlabel = 0.;
	Partial::const_iterator bp;
	for ( bp = partial.begin(); bp != partial.end(); ++bp )
	{
		//	use sinusoidal amplitude:
		//double a = bp.breakpoint().amplitude() * std::sqrt( 1. - bp.breakpoint().bandwidth() );
		
		//  This used to be an amplitude-weighted avg, but for many sounds, 
		//  particularly those for which the weighted avg would be very
		//  different from the simple avg, the amplitude-weighted avg
		//  emphasized the part of the sound in which the frequency estimates
		//  are least reliable (e.g. a piano tone). The unweighted 
		//  average should give more intuitive results in most cases.
				
		double f = bp.breakpoint().frequency();
		double t = bp.time();
		
		double refFreq = _refChannelFreq->valueAt( t ) / _refChannelLabel;
		// weightedlabel += a * (f / refFreq);
		weightedlabel += giveMeN( f, refFreq, _stretchFactor );
		//ampsum += a;
	}
	
	int label;
	//if ( ampsum > 0. )	
	if ( 0 < partial.numBreakpoints() )
	{
		label = (int)((weightedlabel / partial.numBreakpoints()) + 0.5);
	}
	else	//	this should never happen, but just in case:
	{
		label = 0;
	}
	Assert( label >= 0 );
			
	//	assign label, and remember it, but
	//	only if it is a valid (positive) 
	//	distillation label:
	partial.setLabel( label );

}

}	//	end of namespace Loris

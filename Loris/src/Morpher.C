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
 * Morpher.C
 *
 * Implementation of class Morpher.
 *
 * Kelly Fitz, 15 Oct 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include "config.h"
#endif

#include "Morpher.h"
#include "Breakpoint.h"
#include "Envelope.h"
#include "Exception.h"
#include "Notifier.h"
#include "Partial.h"
#include "PartialList.h"
#include "PartialUtils.h"

#include <algorithm>
#include <memory>
#include <set>

//	begin namespace
namespace Loris {
	
// ---------------------------------------------------------------------------
//	Class Morpher
//
//!	@class Morpher Morpher.h loris/Morpher.h
//!
//!	Class Morpher performs sound morphing and Partial parameter
//!	envelope interpolation according to a trio of frequency, amplitude,
//!	and bandwidth morphing functions, described by Envelopes.
//!	Sound morphing is achieved by interpolating the time-varying 
//!	frequencies, amplitudes, and bandwidths of corresponding partials 
//!	obtained from reassigned bandwidth-enhanced analysis of the source 
//!	and target sounds. Partial correspondences may be established by 
//!	labeling, using instances of the Channelizer and Distiller classes.
//!
//!	The Morpher collects morphed Partials in a PartialList, that is
//!	accessible to clients.
//!
//!	For more information about sound morphing using 
//!	the Reassigned Bandwidth-Enhanced Additive Sound 
//!	Model, refer to the Loris website: 
//!	www.cerlsoundgroup.org/Loris/.
//!	
//!	Morpher is a leaf class, do not subclass.
//
	
#pragma mark -- construction --

// ---------------------------------------------------------------------------
//	Morpher constructor (single morph function)
// ---------------------------------------------------------------------------
//!	Construct a new Morpher using the same morphing envelope for 
//!	frequency, amplitude, and bandwidth (noisiness).
//
Morpher::Morpher( const Envelope & f ) :
	_freqFunction( f.clone() ),
	_ampFunction( f.clone() ),
	_bwFunction( f.clone() )
{
}

// ---------------------------------------------------------------------------
//	Morpher constructor (distinct morph functions)
// ---------------------------------------------------------------------------
//!	Construct a new Morpher using the specified morphing envelopes for
//!	frequency, amplitude, and bandwidth (noisiness).
//
Morpher::Morpher( const Envelope & ff, const Envelope & af, const Envelope & bwf ) :
	_freqFunction( ff.clone() ),
	_ampFunction( af.clone() ),
	_bwFunction( bwf.clone() )
{
}

// ---------------------------------------------------------------------------
//	Morpher destructor
// ---------------------------------------------------------------------------
//!	Destroy this Morpher.
//
Morpher::~Morpher( void )
{
}

#pragma mark -- morphed parameter computation --

// ---------------------------------------------------------------------------
//	morphParameters - Breakpoint to Breakpoint
// ---------------------------------------------------------------------------
//!	Compute morphed parameter values at the specified time, using
//!	the source and target Breakpoints (assumed to correspond exactly 
//!	to the specified time).
void
Morpher::morphParameters( const Breakpoint & srcBkpt, const Breakpoint & tgtBkpt, 
						  double time, Breakpoint & retBkpt )
{
	//	evaluate morphing functions at time:
	double alphaA = _ampFunction->valueAt( time );
	double alphaF = _freqFunction->valueAt( time );
	double alphaBW = _bwFunction->valueAt( time );

	//	compute weighted average parameters for 
	//	the return Breakpoint:	
	retBkpt.setFrequency( (alphaF * tgtBkpt.frequency()) + ((1.-alphaF) * srcBkpt.frequency()) );

#if !defined(LOG_AMP_MORPHING)
	retBkpt.setAmplitude( (alphaA * tgtBkpt.amplitude()) + ((1.-alphaA) * srcBkpt.amplitude()) );
#else	
	//	log-amplitude morphing:
	//	it is essential to add in a small Epsilon, so that 
	//	occasional zero amplitudes do not introduce artifacts
	//	(if amp is zero, then even if alpha is very small
	//	the effect is to multiply by zero, because 0^x = 0).
	using std::pow;
	static const double Epsilon = 1E-6;
	retBkpt.setAmplitude( pow(tgtBkpt.amplitude()+Epsilon, alphaA) * pow(srcBkpt.amplitude()+Epsilon, (1.-alphaA)) );
#endif	
	
	retBkpt.setBandwidth( (alphaBW * tgtBkpt.bandwidth()) + ((1.-alphaBW) * srcBkpt.bandwidth()) );
	retBkpt.setPhase( (alphaF * tgtBkpt.phase()) + ((1.-alphaF) * srcBkpt.phase()) );
}

// ---------------------------------------------------------------------------
//	morphParameters - Breakpoint to Partial
// ---------------------------------------------------------------------------
//!	Compute morphed parameter values at the specified time, using
//!	the source Breakpoint (assumed to correspond exactly to the
//!	specified time) and the target Partial (whose parameters are
//!	examined at the specified time).
//!
//!	The target Partial may be a dummy Partial (no Breakpoints).
//
void
Morpher::morphParameters( const Breakpoint & srcBkpt, const Partial & tgtPartial, 
						  double time, Breakpoint & retBkpt )
{
	//	if tgtPartial is a valid Partial, compute 
	//	weighted average parameters, otherwise just 
	//	fade the source:
	if ( tgtPartial.numBreakpoints() > 0 )
	{
		//	evalute the target Partial at time:
		Breakpoint tgtBkpt = tgtPartial.parametersAt( time );
		
		//	compute weighted average parameters for 
		//	the return Breakpoint:	
		morphParameters( srcBkpt, tgtBkpt, time, retBkpt );
	}
	else
	{
		//	its a dummy Partial, just fade (amplitude scale)
		//	the source Breakpoint:
		retBkpt = srcBkpt;
		retBkpt.setAmplitude( (1.-_ampFunction->valueAt( time )) * srcBkpt.amplitude() );
	}
}

// ---------------------------------------------------------------------------
//	morphParameters - Partial to Breakpoint
// ---------------------------------------------------------------------------
//!	Compute morphed parameter values at the specified time, using
//!	the source Partial (whose parameters are examined at the specified 
//!	time) and the target Breakpoint (assumed to correspond exactly to 
//!	the specified time).
//!
//!	The source Partial may be a dummy Partial (no Breakpoints).
//
void
Morpher::morphParameters( const Partial & srcPartial, const Breakpoint & tgtBkpt, 
						  double time, Breakpoint & retBkpt )
{
	//	if srcPartial is a valid Partial, compute 
	//	weighted average parameters, otherwise just 
	//	fade the source:
	if ( srcPartial.numBreakpoints() > 0 )
	{
		//	evalute the target Partial at time:
		Breakpoint srcBkpt = srcPartial.parametersAt( time );
		
		//	compute weighted average parameters for 
		//	the return Breakpoint:	
		morphParameters( srcBkpt, tgtBkpt, time, retBkpt );
	}
	else
	{
		//	its a dummy Partial, just fade (amplitude scale)
		//	the target Breakpoint:
		retBkpt = tgtBkpt;
		retBkpt.setAmplitude( _ampFunction->valueAt( time ) * tgtBkpt.amplitude() );
	}
}

// ---------------------------------------------------------------------------
//	morphParameters - Partial to Partial
// ---------------------------------------------------------------------------
//!	Compute morphed parameter values at the specified time, using
//!	the source  and target Partials, both of whose parameters are 
//!	examined at the specified time.
//!
//!	Either (or neither) the source or target Partial may be a dummy 
//!	Partial (no Breakpoints), but not both.
//
void
Morpher::morphParameters( const Partial & srcPartial, const Partial & tgtPartial, 
						  double time, Breakpoint & retBkpt )
{
	if ( srcPartial.numBreakpoints() == 0 && tgtPartial.numBreakpoints() == 0 )
		Throw( InvalidArgument, "Cannot morph a pair of dummy (empty) Partials." );
		
	// 	if the source Partial is valid, compute source parameters,
	//	otherwise compute target Partial parameters at the specified
	//	time, and scale the amplitude:
	Breakpoint srcBkpt;
	if ( srcPartial.numBreakpoints() > 0 )
	{
		//	compute interpolated values for srcPartial:
		srcBkpt  = srcPartial.parametersAt( time );
	}
	else
	{
		//	srcPartial is a dummy Partial, just fade (amplitude scale)
		//	the target Partial:
		retBkpt = tgtPartial.parametersAt( time );
		retBkpt.setAmplitude( _ampFunction->valueAt( time ) * retBkpt.amplitude() );
		return;
	}
	
	//	if the target Partial is valid, compute target parameters,
	//	otherwise use the source parameter values, and scale the
	//	amplitude:
	Breakpoint tgtBkpt;
	if ( tgtPartial.numBreakpoints() > 0 )
	{
		//	compute interpolated values for tgtPartial:
		tgtBkpt = tgtPartial.parametersAt( time );
	}
	else
	{
		//	tgtPartial is a dummy Partial, just fade (amplitude scale)
		//	the source Partial:
		retBkpt = srcPartial.parametersAt( time );
		retBkpt.setAmplitude( (1.-_ampFunction->valueAt( time )) * retBkpt.amplitude() );		
		return;
	}
			
	//	both Partials were valid, compute weighted average parametersfor 
	//	the return Breakpoint:	
	morphParameters( srcBkpt, tgtBkpt, time, retBkpt );
	
}

#pragma mark -- Partial morphing --

// ---------------------------------------------------------------------------
//	morphPartial
// ---------------------------------------------------------------------------
//!	Morph a pair of Partials to yield a new morphed Partial. 
//!	Dummy Partials (having no Breakpoints) don't contribute to the
//!	morph, except to cause their opposite to fade out. 
//!	Either (or neither) the source or target Partial may be a dummy 
//!	Partial (no Breakpoints), but not both. The morphed
//!	Partial has Breakpoints at times corresponding to every Breakpoint 
//!	in both source Partials. The morphed Partial is appended
//!	to the Morpher's PartialList, and a reference to it is returned.
//!	The morphed Partial is assigned the specified label.
//
Partial &
Morpher::morphPartial( const Partial & src, const Partial & tgt, int assignLabel )
{
	if ( (src.numBreakpoints() == 0) && (tgt.numBreakpoints() == 0) )
		Throw( InvalidArgument, "Cannot morph two empty Partials," );
		
	//	make a new Partial:
	Partial newp;
	newp.setLabel( assignLabel );
	
	//	loop over Breakpoints in first partial:
	Breakpoint tmpBkpt;
	for ( Partial::const_iterator iter = src.begin(); iter != src.end(); ++iter )
	{	
		morphParameters( iter.breakpoint(), tgt, iter.time(), tmpBkpt );
		newp.insert( iter.time(), tmpBkpt );
	}
	
	//	now do it for the other Partial:
	for ( Partial::const_iterator iter = tgt.begin(); iter != tgt.end(); ++iter )
	{
		morphParameters( src, iter.breakpoint(), iter.time(), tmpBkpt );
		newp.insert( iter.time(), tmpBkpt );
	}
		
	//	add the new partial to the collection,
	//	if it is valid, and it must be, unless two 
	//	dummy Partials were morphed:
	Assert( newp.numBreakpoints() > 0 );
	_partials.push_back( newp );
	return _partials.back();
}

// ---------------------------------------------------------------------------
//	crossfade
// ---------------------------------------------------------------------------
//!	Crossfade Partials with no correspondences.
//!
//!	Unlabeled Partials (having label 0) are considered to 
//!	have no correspondences, so they are just faded out, and not 
//!	actually morphed. This is the same as morphing each with an 
//!	empty dummy Partial (having no Breakpoints). 
//!
//!	The Partials in the first range are treated as components of the 
//!	source sound, corresponding to a morph function value of 0, and  
//!	those in the second are treated as components of the target sound, 
//!	corresponding to a morph function value of 1.
//!
//!	The crossfaded Partials are stored in the Morpher's PartialList.
//
void 
Morpher::crossfade( PartialList::const_iterator beginSrc, 
					PartialList::const_iterator endSrc,
					PartialList::const_iterator beginTgt, 
					PartialList::const_iterator endTgt )
{
	Partial nullPartial;
	debugger << "crossfading unlabeled (labeled 0) Partials" << endl;
	
	long debugCounter;

	//	crossfade Partials corresponding to a morph weight of 0:
	PartialList::const_iterator it;
	debugCounter = 0;
	for ( it = beginSrc; it != endSrc; ++it )
	{
		if ( it->label() == 0 )
		{
			morphPartial( *it, nullPartial, 0 );	
			++debugCounter;
		}
	}
	debugger << "there were " << debugCounter << " in sound 1" << endl;

	//	crossfade Partials corresponding to a morph weight of 1:
	debugCounter = 0;
	for ( it = beginTgt; it != endTgt; ++it )
	{
		if ( it->label() == 0 )
		{
			morphPartial( nullPartial, *it, 0 );
			++debugCounter;
		}
	}
	debugger << "there were " << debugCounter << " in sound 2" << endl;
}

// ---------------------------------------------------------------------------
//	morph
// ---------------------------------------------------------------------------
//!	Morph two sounds (collections of Partials labeled to indicate
//!	correspondences) into a single labeled collection of Partials.
//!	Unlabeled Partials (having label 0) are crossfaded. The morphed
//!	and crossfaded Partials are stored in the Morpher's PartialList.
//!
//!	The Partials in the first range are treated as components of the 
//!	source sound, corresponding to a morph function value of 0, and  
//!	those in the second are treated as components of the target sound, 
//!	corresponding to a morph function value of 1.
//
void 
Morpher::morph( PartialList::const_iterator beginSrc, 
				PartialList::const_iterator endSrc,
				PartialList::const_iterator beginTgt, 
				PartialList::const_iterator endTgt )
{
	//	collect the labels in the two Partial ranges, 
	//	object if the Partials have not been distilled,
	//	that is, if they contain multiple Partials having 
	//	the same non-zero label:
	std::set< int > labels, moreLabels;
	for ( PartialList::const_iterator it = beginSrc; it != endSrc; ++it ) 
	{
		//	don't add the crossfade label to the set:
		if ( it->label() != 0 )
		{
			//	set::insert() returns a pair, the second element
			//	of which is false if the insertion failed because
			//	the set already contained the insertee:
			if ( ! labels.insert(it->label()).second )
				Throw( InvalidObject, "Partials must be distilled before morphing." );
		}
	}
	for ( PartialList::const_iterator it = beginTgt; it != endTgt; ++it ) 
	{
		//	don't add the non-label, 0, to the set:
		if ( it->label() != 0 )
		{
			//	as above:
			if ( ! moreLabels.insert(it->label()).second )
				Throw( InvalidObject, "Partials must be distilled before morphing." );
		}
	}
	
	//	combine the label sets
	labels.insert( moreLabels.begin(), moreLabels.end() );
		
	//	loop over lots of labels and morph Partials
	//	having corresponding labels:
	std::set< int >::iterator labelIter;
	for ( labelIter = labels.begin(); labelIter != labels.end(); ++labelIter ) 
	{
		Assert( *labelIter != 0 );
		
		//	find source Partial 0:
		PartialList::const_iterator pSrc = 
			std::find_if( beginSrc, endSrc, PartialUtils::label_equals(*labelIter) );
				
		//	find source Partial 1:
		PartialList::const_iterator pTgt = 
			std::find_if( beginTgt, endTgt, PartialUtils::label_equals(*labelIter) );
		
		debugger << "morphing " << ((pSrc != endSrc)?(1):(0)) 
				 << " and " << ((pTgt != endTgt)?(1):(0)) 
				 <<	" partials with label " <<	*labelIter << endl;
		
		if ( pSrc == endSrc )
		{
			Assert( pTgt != endTgt );
			morphPartial( Partial(), *pTgt, *labelIter );
		}
		else if ( pTgt == endTgt )
		{
			Assert( pSrc != endSrc );
			morphPartial( *pSrc, Partial(), *labelIter );
		}
		else
		{			 
			morphPartial( *pSrc, *pTgt, *labelIter );
		}					
			
	}	//	end loop over labels
	
	//	crossfade the remaining unlabeled Partials:
	crossfade( beginSrc, endSrc, beginTgt, endTgt );
}

#pragma mark -- morphing function access/mutation --

// ---------------------------------------------------------------------------
//	setFrequencyFunction
// ---------------------------------------------------------------------------
//!	Assign a new frequency morphing envelope to this Morpher.
//
void
Morpher::setFrequencyFunction( const Envelope & f )
{
	_freqFunction.reset( f.clone() );
}

// ---------------------------------------------------------------------------
//	setAmplitudeFunction
// ---------------------------------------------------------------------------
//!	Assign a new amplitude morphing envelope to this Morpher.
//
void
Morpher::setAmplitudeFunction( const Envelope & f )
{
	_ampFunction.reset( f.clone() );
}

// ---------------------------------------------------------------------------
//	setBandwidthFunction
// ---------------------------------------------------------------------------
//!	Assign a new bandwidth morphing envelope to this Morpher.
//
void
Morpher::setBandwidthFunction( const Envelope & f )
{
	_bwFunction.reset( f.clone() );
}

// ---------------------------------------------------------------------------
//	frequencyFunction
// ---------------------------------------------------------------------------
//!	Return a reference to this Morpher's frequency morphing envelope.
//
const Envelope &
Morpher::frequencyFunction( void ) const 
{
	return * _freqFunction;
}

// ---------------------------------------------------------------------------
//	amplitudeFunction
// ---------------------------------------------------------------------------
//!	Return a reference to this Morpher's amplitude morphing envelope.
//
const Envelope &
Morpher::amplitudeFunction( void ) const 
{
	return * _ampFunction;
}

// ---------------------------------------------------------------------------
//	bandwidthFunction
// ---------------------------------------------------------------------------
//!	Return a reference to this Morpher's bandwidth morphing envelope.
//
const Envelope &
Morpher::bandwidthFunction( void ) const 
{
	return * _bwFunction;
}

#pragma mark -- PartialList access --

// ---------------------------------------------------------------------------
//	partials
// ---------------------------------------------------------------------------
//!	Return a reference to this Morpher's list of morphed Partials.
//
PartialList & 
Morpher::partials( void )
{ 
	return _partials; 
}

// ---------------------------------------------------------------------------
//	partials
// ---------------------------------------------------------------------------
//!	Return a const reference to this Morpher's list of morphed Partials.
//
const PartialList & 
Morpher::partials( void ) const 
{ 
	return _partials; 
}

}	//	end of namespace Loris

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
	#include <config.h>
#endif

#include <Morpher.h>
#include <Exception.h>
#include <Partial.h>
#include <PartialList.h>
#include <Breakpoint.h>
#include <Envelope.h>
#include <PartialUtils.h>
#include <Notifier.h>
#include <algorithm>
#include <cmath>
#include <memory>
#include <set>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	Morpher constructor (single morph function)
// ---------------------------------------------------------------------------
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
//
Morpher::~Morpher( void )
{
}

// ---------------------------------------------------------------------------
//	morphParameters - Breakpoint to Partial
// ---------------------------------------------------------------------------
//	Compute morphed parameter values at the specified time, using
//	the source Breakpoint (assumed to correspond exactly to the
//	specified time) and the target Partial (whose parameters are
//	examined at the specified time).
//
//	Need to check for tgtPartial being a dummy Partial.
//
void
Morpher::morphParameters( const Breakpoint & srcBkpt, const Partial & tgtPartial, 
							  double time, Breakpoint & retBkpt )
{
	double alphaA = _ampFunction->valueAt( time );

	//	if tgtPartial is a valid Partial, compute 
	//	weighted average parameters, otherwise just 
	//	fade the source:
	if ( tgtPartial.numBreakpoints() > 0 )
	{
		//	compute parameter weights:
		double alphaF = _freqFunction->valueAt( time );
		double alphaBW = _bwFunction->valueAt( time );
		
		//	compute interpolated values for tgtPartial:
		double tgtamp = tgtPartial.amplitudeAt( time );
		double tgtfreq = tgtPartial.frequencyAt( time );
		double tgtbw = tgtPartial.bandwidthAt( time );
		double tgtph = tgtPartial.phaseAt( time );
		
		//	create new weighted average parameters for 
		//	the return Breakpoint:	
		retBkpt.setFrequency( (alphaF * tgtfreq) + ((1.-alphaF) * srcBkpt.frequency()) );
		retBkpt.setAmplitude( (alphaA * tgtamp) + ((1.-alphaA) * srcBkpt.amplitude()) );
		retBkpt.setBandwidth( (alphaBW * tgtbw) + ((1.-alphaBW) * srcBkpt.bandwidth()) );
		retBkpt.setPhase( (alphaF * tgtph) + ((1.-alphaF) * srcBkpt.phase()) );
	}
	else
	{
		//	its a dummy Partial, just fade (amplitude scale)
		//	the source Breakpoint:
		retBkpt = srcBkpt;
		retBkpt.setAmplitude( (1.-alphaA) * srcBkpt.amplitude() );
	}
}

// ---------------------------------------------------------------------------
//	morphParameters - Partial to Breakpoint
// ---------------------------------------------------------------------------
//	Compute morphed parameter values at the specified time, using
//	the source Partial (whose parameters are examined at the specified 
//	time) and the target Breakpoint (assumed to correspond exactly to 
//	the specified time).
//
//	Need to check for srcPartial being a dummy Partial.
//
void
Morpher::morphParameters( const Partial & srcPartial, const Breakpoint & tgtBkpt, 
							  double time, Breakpoint & retBkpt )
{
	double alphaA = 1. - _ampFunction->valueAt( time );
	
	//	if srcPartial is a valid Partial, compute 
	//	weighted average parameters, otherwise just 
	//	fade the source:
	if ( srcPartial.numBreakpoints() > 0 )
	{
		//	compute parameter weights:
		double alphaF = 1. - _freqFunction->valueAt( time );
		double alphaBW = 1. - _bwFunction->valueAt( time );
		
		//	compute interpolated values for tgtPartial:
		double srcamp = srcPartial.amplitudeAt( time );
		double srcfreq = srcPartial.frequencyAt( time );
		double srcbw = srcPartial.bandwidthAt( time );
		double srcph = srcPartial.phaseAt( time );
		
		//	create new weighted average parameters for 
		//	the return Breakpoint:	
		retBkpt.setFrequency( (alphaF * srcfreq) + ((1.-alphaF) * tgtBkpt.frequency()) );
		retBkpt.setAmplitude( (alphaA * srcamp) + ((1.-alphaA) * tgtBkpt.amplitude()) );
		retBkpt.setBandwidth( (alphaBW * srcbw) + ((1.-alphaBW) * tgtBkpt.bandwidth()) );
		retBkpt.setPhase( (alphaF * srcph) + ((1.-alphaF) * tgtBkpt.phase()) );
	}
	else
	{
		//	its a dummy Partial, just fade (amplitude scale)
		//	the target Breakpoint:
		retBkpt = tgtBkpt;
		retBkpt.setAmplitude( (1.-alphaA) * tgtBkpt.amplitude() );
	}
}

// ---------------------------------------------------------------------------
//	morphParameters - Partial to Partial
// ---------------------------------------------------------------------------
//	Compute morphed parameter values at the specified time, using
//	the source  and target Partials, both of whose parameters are 
//	examined at the specified time.
//
//	Need to check for srcPartial or tgtPartial being a dummy Partial.
//
void
Morpher::morphParameters( const Partial & srcPartial, const Partial & tgtPartial, 
							  double time, Breakpoint & retBkpt )
{
	if ( srcPartial.numBreakpoints() == 0 && tgtPartial.numBreakpoints() == 0 )
		Throw( InvalidArgument, "Cannot morph a pair of dummy (empty) Partials." );
		
	double srcamp = 0, srcfreq = 0, srcbw = 0, srcph = 0;	//	computed below, if valid
	double tgtamp = 0, tgtfreq = 0, tgtbw = 0, tgtph = 0;	//	computed below, if valid
	
	//	compute parameter weights:
	double alphaA = _ampFunction->valueAt( time );
	double alphaF = _freqFunction->valueAt( time );
	double alphaBW = _bwFunction->valueAt( time );
	
	// 	if the source Partial is valid, compute source parameters,
	//	otherwise compute target Partial parameters at the specified
	//	time, and scale the amplitude:
	if ( srcPartial.numBreakpoints() > 0 )
	{
		//	compute interpolated values for srcPartial:
		srcamp = srcPartial.amplitudeAt( time );
		srcfreq = srcPartial.frequencyAt( time );
		srcbw = srcPartial.bandwidthAt( time );
		srcph = srcPartial.phaseAt( time );
	}
	else
	{
		//	srcPartial is a dummy Partial, just fade (amplitude scale)
		//	the target Partial:
		retBkpt.setFrequency( tgtPartial.frequencyAt( time ) );
		retBkpt.setAmplitude( alphaA * tgtPartial.amplitudeAt( time ) );
		retBkpt.setBandwidth( tgtPartial.bandwidthAt( time ) );
		retBkpt.setPhase( tgtPartial.phaseAt( time ) );
		
		return;
	}
	
	//	if the target Partial is valid, compute target parameters,
	//	otherwise use the source parameter values, and scale the
	//	amplitude:
	if ( tgtPartial.numBreakpoints() > 0 )
	{
		//	compute interpolated values for tgtPartial:
		double tgtamp = tgtPartial.amplitudeAt( time );
		double tgtfreq = tgtPartial.frequencyAt( time );
		double tgtbw = tgtPartial.bandwidthAt( time );
		double tgtph = tgtPartial.phaseAt( time );
	}
	else
	{
		//	tgtPartial is a dummy Partial, just fade (amplitude scale)
		//	the source Partial:
		retBkpt.setFrequency( srcPartial.frequencyAt( time ) );
		retBkpt.setAmplitude( (1.-alphaA) * srcPartial.amplitudeAt( time ) );
		retBkpt.setBandwidth( srcPartial.bandwidthAt( time ) );
		retBkpt.setPhase( srcPartial.phaseAt( time ) );
		
		return;
	}
			
	//	both Partials were valid, compute weighted average parametersfor 
	//	the return Breakpoint:	
	retBkpt.setFrequency( (alphaF * tgtfreq) + ((1.-alphaF) * srcfreq) );
	retBkpt.setAmplitude( (alphaA * tgtamp) + ((1.-alphaA) * srcamp) );
	retBkpt.setBandwidth( (alphaBW * tgtbw) + ((1.-alphaBW) * srcbw) );
	retBkpt.setPhase( (alphaF * tgtph) + ((1.-alphaF) * srcph) );
	
}

// ---------------------------------------------------------------------------
//	morphPartial
// ---------------------------------------------------------------------------
//	Basic morphing operation: either Partial may be a dummy with no 
//	Breakpoints. Partials with no duration don't contribute to the
//	morph, except to cause their opposite to fade out. The morphed
//	Partial has Breakpoints at times corresponding to every Breakpoint 
//	in both source Partials.
//
Partial &
Morpher::morphPartial( const Partial & p0, const Partial & p1, int assignLabel )
{
	if ( (p0.numBreakpoints() == 0) && (p1.numBreakpoints() == 0) )
		Throw( InvalidArgument, "Cannot morph two empty Partials," );
		
	//	make a new Partial:
	Partial newp;
	newp.setLabel( assignLabel );
	
	//	loop over Breakpoints in first partial:
	Breakpoint tmpBkpt;
	for ( PartialConstIterator iter = p0.begin(); iter != p0.end(); ++iter )
	{	
		morphParameters( iter.breakpoint(), p1, iter.time(), tmpBkpt );
		newp.insert( iter.time(), tmpBkpt );
	}
	
	//	now do it for the other Partial:
	for ( PartialConstIterator iter = p1.begin(); iter != p1.end(); ++iter )
	{
		morphParameters( p0, iter.breakpoint(), iter.time(), tmpBkpt );
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
//	Crossfade Partials with no correspondences.
//
//	Unlabeled Partials (having label 0) are considered to 
//	have no correspondences, so they are just faded out, and not 
//	actually morphed. This is the same as morphing each with an 
//	empty Partial. 
//
//	The Partials in the first range are treated as components of the 
//	sound corresponding to a morph function of 0, those in the second
//	are treated as components of the sound corresponding to a 
//	morph function of 1.
//
void 
Morpher::crossfade( PartialList::const_iterator begin0, 
						PartialList::const_iterator end0,
						PartialList::const_iterator begin1, 
						PartialList::const_iterator end1 )
{
	Partial nullPartial;
	debugger << "crossfading unlabeled (labeled 0) Partials" << endl;
	
	long debugCounter;

	//	crossfade Partials corresponding to a morph weight of 0:
	PartialList::const_iterator it;
	debugCounter = 0;
	for ( it = begin0; it != end0; ++it )
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
	for ( it = begin1; it != end1; ++it )
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
//	Morph two sounds (collections of Partials labeled to indicate
//	correspondences) into a single labeled collection of Partials.
//
void 
Morpher::morph( PartialList::const_iterator begin0, 
				PartialList::const_iterator end0,
				PartialList::const_iterator begin1, 
				PartialList::const_iterator end1 )
{
	//	collect the labels in the two Partial ranges, 
	//	object if the Partials have not been distilled,
	//	that is, if they contain multiple Partials having 
	//	the same non-zero label:
	std::set< int > labels, moreLabels;
	for ( PartialList::const_iterator it = begin0; it != end0; ++it ) 
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
	for ( PartialList::const_iterator it = begin1; it != end1; ++it ) 
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
		PartialList::const_iterator p0 = 
			std::find_if( begin0, end0, PartialUtils::label_equals(*labelIter) );
				
		//	find source Partial 1:
		PartialList::const_iterator p1 = 
			std::find_if( begin1, end1, PartialUtils::label_equals(*labelIter) );
		
		debugger << "morphing " << ((p0 != end0)?(1):(0)) 
				 << " and " << ((p1 != end1)?(1):(0)) 
				 <<	" partials with label " <<	*labelIter << endl;
		
		if ( p0 == end0 )
		{
			Assert( p1 != end1 );
			morphPartial( Partial(), *p1, *labelIter );
		}
		else if ( p1 == end1 )
		{
			Assert( p0 != end0 );
			morphPartial( *p0, Partial(), *labelIter );
		}
		else
		{			 
			morphPartial( *p0, *p1, *labelIter );
		}					
			
	}	//	end loop over labels
	
	//	crossfade the remaining unlabeled Partials:
	crossfade( begin0, end0, begin1, end1 );
}

// ---------------------------------------------------------------------------
//	setFrequencyFunction
// ---------------------------------------------------------------------------
//
void
Morpher::setFrequencyFunction( const Envelope & f )
{
	_freqFunction.reset( f.clone() );
}

// ---------------------------------------------------------------------------
//	setAmplitudeFunction
// ---------------------------------------------------------------------------
//
void
Morpher::setAmplitudeFunction( const Envelope & f )
{
	_ampFunction.reset( f.clone() );
}

// ---------------------------------------------------------------------------
//	setBandwidthFunction
// ---------------------------------------------------------------------------
//
void
Morpher::setBandwidthFunction( const Envelope & f )
{
	_bwFunction.reset( f.clone() );
}

// ---------------------------------------------------------------------------
//	frequencyFunction
// ---------------------------------------------------------------------------
//
const Envelope &
Morpher::frequencyFunction( void ) const 
{
	return * _freqFunction;
}

// ---------------------------------------------------------------------------
//	amplitudeFunction
// ---------------------------------------------------------------------------
//
const Envelope &
Morpher::amplitudeFunction( void ) const 
{
	return * _ampFunction;
}

// ---------------------------------------------------------------------------
//	bandwidthFunction
// ---------------------------------------------------------------------------
//
const Envelope &
Morpher::bandwidthFunction( void ) const 
{
	return * _bwFunction;
}

// ---------------------------------------------------------------------------
//	partials
// ---------------------------------------------------------------------------
//
PartialList & 
Morpher::partials( void )
{ 
	return _partials; 
}

// ---------------------------------------------------------------------------
//	partials
// ---------------------------------------------------------------------------
//
const PartialList & 
Morpher::partials( void ) const 
{ 
	return _partials; 
}

}	//	end of namespace Loris

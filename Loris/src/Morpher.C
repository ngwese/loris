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

//#undef LOG_AMP_MORPHING

//	begin namespace
namespace Loris {


#if defined(LOG_AMP_MORPHING)
static const Partial::label_type DefaultReferenceLabel = 1;
#else
static const Partial::label_type DefaultReferenceLabel = 0;	//	by default, don't use reference Partial
															//	(this is the traditional behavior or Loris)
#endif

	
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
	_bwFunction( f.clone() ),
	_refLabel0( DefaultReferenceLabel ),
	_refLabel1( DefaultReferenceLabel )
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
	_bwFunction( bwf.clone() ),
	_refLabel0( DefaultReferenceLabel ),
	_refLabel1( DefaultReferenceLabel )
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
	static const double Epsilon = 1E-12;
	double A0 = srcBkpt.amplitude();
	double A1 = tgtBkpt.amplitude();
	if ( A0 > Epsilon || A1 > Epsilon )
	{
		retBkpt.setAmplitude( pow( std::max( A0, Epsilon ), (1.-alphaA) ) *
							  pow( std::max( A1, Epsilon ), alphaA ) );		
	}
	else
	{
		retBkpt.setAmplitude( 0 );
	}
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
#if !defined(LOG_AMP_MORPHING)
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
#else
	//	the problem with the above method is that when one Partial
	//	is a dummy, then the morphing mechanism in the two-Breakpoint
	//	version of morphParameters is not used. It would be better if
	//	there were a single function that is always called to perform
	//	the morph, so that we can experiment with different techniques
	//	withouth having many pieces of code to modify.
	//
	Breakpoint tgtBkpt;
	if ( tgtPartial.numBreakpoints() > 0 )
	{
		//	compute interpolated values for tgtPartial:
		tgtBkpt = tgtPartial.parametersAt( time );
	}
	else
	{
		//	tgtPartial is a dummy Partial, use a zero-amplitude
		//	copy of the source parameters:
		tgtBkpt = srcBkpt;
		tgtBkpt.setAmplitude( 0 );		
	}

	//	compute weighted average parameters for 
	//	the return Breakpoint:	
	morphParameters( srcBkpt, tgtBkpt, time, retBkpt );
#endif	
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
#if !defined(LOG_AMP_MORPHING)
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
#else
	//	the problem with the above method is that when one Partial
	//	is a dummy, then the morphing mechanism in the two-Breakpoint
	//	version of morphParameters is not used. It would be better if
	//	there were a single function that is always called to perform
	//	the morph, so that we can experiment with different techniques
	//	withouth having many pieces of code to modify.
	//
	Breakpoint srcBkpt;
	if ( srcPartial.numBreakpoints() > 0 )
	{
		//	compute interpolated values for srcPartial:
		srcBkpt  = srcPartial.parametersAt( time );
	}
	else
	{
		//	srcPartial is a dummy Partial, use a zero-amplitude
		//	copy of the target parameters:
		srcBkpt = tgtBkpt;
		srcBkpt.setAmplitude( 0 );
	}

	//	compute weighted average parameters for 
	//	the return Breakpoint:	
	morphParameters( srcBkpt, tgtBkpt, time, retBkpt );
#endif	
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
	{
		Throw( InvalidArgument, "Cannot morph a pair of dummy (empty) Partials." );
	}
		
#if !defined(LOG_AMP_MORPHING)
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

#else
	//	the problem with the above method is that when one Partial
	//	is a dummy, then the morphing mechanism in the two-Breakpoint
	//	version of morphParameters is not used. It would be better if
	//	there were a single function that is always called to perform
	//	the morph, so that we can experiment with different techniques
	//	withouth having many pieces of code to modify.
	//
	Breakpoint srcBkpt;
	if ( srcPartial.numBreakpoints() > 0 )
	{
		//	compute interpolated values for srcPartial:
		srcBkpt  = srcPartial.parametersAt( time );
	}
	else
	{
		//	srcPartial is a dummy Partial, use a zero-amplitude
		//	copy of the target parameters:
		srcBkpt = tgtPartial.parametersAt( time );
		srcBkpt.setAmplitude( 0 );
	}
	
	Breakpoint tgtBkpt;
	if ( tgtPartial.numBreakpoints() > 0 )
	{
		//	compute interpolated values for tgtPartial:
		tgtBkpt = tgtPartial.parametersAt( time );
	}
	else
	{
		//	tgtPartial is a dummy Partial, use a zero-amplitude
		//	copy of the source parameters:
		tgtBkpt = srcPartial.parametersAt( time );
		tgtBkpt.setAmplitude( 0 );		
	}
	morphParameters( srcBkpt, tgtBkpt, time, retBkpt );
#endif	
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
	{
		Throw( InvalidArgument, "Cannot morph two empty Partials," );
	}
	
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
//!
//!	Throws InvalidArgument if either the source or target
//!	sequence is not distilled (contains more than one Partial having
//!	the same non-zero label).
//
//	Ugh! This ought to be a template function!
//
void 
Morpher::morph( PartialList::const_iterator beginSrc, 
				PartialList::const_iterator endSrc,
				PartialList::const_iterator beginTgt, 
				PartialList::const_iterator endTgt )
{
	//	build a PartialCorrespondence, a map of labels
	//	to pairs of pointers to Partials, by making every
	//	Partial in the source the first element of the
	//	pair at the corresponding label, and every Partial
	//	in the target the second element of the pair at
	//	the corresponding label. Pointers not assigned to
	//	point to a Partial in the source or target are 
	//	initialized to 0 in the correspondence map.
	PartialCorrespondence correspondence;
	
	//	add source Partials to the correspondence map:
	for ( PartialList::const_iterator it = beginSrc; it != endSrc; ++it ) 
	{
		//	don't add the crossfade label to the set:
		if ( it->label() != 0 )
		{
			PartialPtrPair & match = correspondence[ it->label() ];
			if ( match.first != 0 )
			{
				Throw( InvalidArgument, "Partials must be distilled before morphing." );
			}
			match.first = &(*it);
		}
	}
	
	//	add target Partials to the correspondence map:
	for ( PartialList::const_iterator it = beginTgt; it != endTgt; ++it ) 
	{
		//	don't add the crossfade label to the set:
		if ( it->label() != 0 )
		{
			PartialPtrPair & match = correspondence[ it->label() ];
			if ( match.second != 0 )
			{
				Throw( InvalidArgument, "Partials must be distilled before morphing." );
			}
			match.second = &(*it);
		}
	}
	
	//	morph corresponding labeled Partials:
	morph_aux( correspondence );
	
	//	crossfade the remaining unlabeled Partials:
	crossfade( beginSrc, endSrc, beginTgt, endTgt );
}

// ---------------------------------------------------------------------------
//	fix_frequencies
// ---------------------------------------------------------------------------
//	Local helper function to correct the frequencies of low-amplitude
//	breakpoints using a reference Partial.
//
static void fix_frequencies( Partial & fixme, const Partial & reference )
{
	//	nothing to do if fixme is the reference Partial:
	if ( &fixme != &reference )
	{
		//	compute absolute magnitude thresholds:
		const double ThresholdDB = -65;
		const double FadeRangeDB = 10;
		const double Threshold = std::pow( 10., 0.05 * ThresholdDB );
		const double BeginFade = std::pow( 10., 0.05 * (ThresholdDB+FadeRangeDB) );
		const double OneOverFadeSpan = 1. / ( BeginFade - Threshold );

		double fscale = (double)fixme.label() / reference.label();
		
		//	fix the frequency of every Breakpoint in fixme
		//	that has amplitude below the reference frequency
		//	threshold (called BeginFade).
		//
		//	invariant:
		//	bpPos is the position of a Breakpoint in fixme,
		//	all Breakpoints earlier in fixme than bpPos have
		//	amplitudes greater than the reference frequency
		//	threshold, or their frequencies have been "fixed".
		for ( Partial::iterator bpPos = fixme.begin(); bpPos != fixme.end(); ++bpPos )
		{
			if ( bpPos->amplitude() < BeginFade )
			{
				//	the new frequency is computed from a weighted
				//	average of the original analyzed frequency and
				//	the reference frequency:
				double alpha = std::max( ( BeginFade - bpPos->amplitude() ) * OneOverFadeSpan, 1. );
				double fRef = reference.frequencyAt( bpPos.time() );
				bpPos->setFrequency( ( alpha * ( fRef * fscale ) ) + 
									 ( (1 - alpha ) * bpPos->frequency() ) );
			}
		}				
		
		//	Breakpoints in the reference Partial that are before the
		//	start of fixme should introduce zero-amplitude Breakpoints
		//	in fixme.
		//
		//	invariant:
		//	earlyBpPos is the position of a Breakpoint in the reference
		//	Partial, all Breakpoints earlier than earlyBpPos have been 
		//	added to fix me as zero amplitude Breakpoints at the 
		//	appropriate frequency multiple (fscale, above)
		//
		//	Need to cache the start time of fixme, otherwise the
		//	loop will always terminate after the fist iteration,
		//	because the earliest Breakpoint is inserted first.
		Partial::const_iterator earlyBpPos = reference.begin();
		const double oldStartTime = fixme.startTime();
		while ( earlyBpPos.time() < oldStartTime )
		{
			Breakpoint silentBreakpoint = earlyBpPos.breakpoint();
			silentBreakpoint.setAmplitude( 0 );
			silentBreakpoint.setFrequency( silentBreakpoint.frequency() * fscale );
			fixme.insert( earlyBpPos.time(), silentBreakpoint );
			
			/*
			debugger << "inserting silent Breakpoint at time " << earlyBpPos.time()
					 << " and frequency " << silentBreakpoint.frequency()
					 << " at front of Partial labeled " << fixme.label() << endl;
			*/
			++earlyBpPos;
		}
		
		//	Breakpoints in the reference Partial that are after the
		//	end of fixme should introduce zero-amplitude Breakpoints
		//	in fixme.
		//
		//	invariant:
		//	lateBpPos is the position of a Breakpoint in the reference
		//	Partial, all Breakpoints later than lateBpPos have been 
		//	added to fix me as zero amplitude Breakpoints at the 
		//	appropriate frequency multiple (fscale, above)
		//
		//	Need to cache the end time of fixme, otherwise the
		//	loop will always terminate after the fist iteration,
		//	because the latest Breakpoint is inserted first.
		Partial::const_iterator lateBpPos = --( reference.end() );
		const double oldEndTime = fixme.endTime();
		while ( lateBpPos.time() > oldEndTime )
		{
			Breakpoint silentBreakpoint = lateBpPos.breakpoint();
			silentBreakpoint.setAmplitude( 0 );
			silentBreakpoint.setFrequency( silentBreakpoint.frequency() * fscale );
			fixme.insert( lateBpPos.time(), silentBreakpoint );
			
			/*
			debugger << "inserting silent Breakpoint at time " << lateBpPos.time()
					 << " and frequency " << silentBreakpoint.frequency()
					 << " at end of Partial labeled " << fixme.label() << endl;
			*/
			--lateBpPos;
		}
	}
}

// ---------------------------------------------------------------------------
//	morph_aux
// ---------------------------------------------------------------------------
//	Helper function that performs the morph between corresponding pairs
//	of Partials identified in a PartialCorrespondence. Called by the
//	morph() implementation accepting two sequences of Partials.
//
//	PartialCorrespondence represents a map from non-zero Partial 
//	labels to pairs of pointers to Partials that should be morphed 
//	into a single Partial that is assigned that label. 
//	PartialPtrPair is a pair of pointers to Partials that are
//	initialized to zero, and it is the element type for the
//	PartialCorrespondence map.
//
void Morpher::morph_aux( PartialCorrespondence & correspondence  )
{
	const Partial * ref0 = 0;
	const Partial * ref1 = 0;
	
	//	find reference Partials:
	if ( _refLabel0 != 0 )
	{
		PartialCorrespondence::iterator refPos = correspondence.find( _refLabel0 );
		if ( refPos == correspondence.end() )
		{
			Throw( InvalidObject, "no Partial in the morph source has the specified reference label" );
		}
		ref0 = refPos->second.first;
		Assert( ref0 != 0 );
	}
	if ( _refLabel1 != 0 )
	{
		PartialCorrespondence::iterator refPos = correspondence.find( _refLabel1 );
		if ( refPos == correspondence.end() )
		{
			Throw( InvalidObject, "no Partial in the morph target has the specified reference label" );
		}
		ref1 = refPos->second.second;
		Assert( ref1 != 0 );
	}

	PartialCorrespondence::const_iterator it;
	for ( it = correspondence.begin(); it != correspondence.end(); ++it )
	{
		Partial::label_type label = it->first;
		PartialPtrPair match = it->second;
		const Partial * p0 = match.first;
		const Partial * p1 = match.second;

		//	sanity check:
		//	one of those Partials must exist
		Assert( p0 != 0 || p1 != 0 );
					
		// construct source Partial for morph:
		Partial src;
		if ( p0 != 0 )
		{
			//	use the Partial in the correspondence
			src = *p0;
			
			if ( ref0 != 0 )
			{
				//	find quiet parts of the source Partial,
				//	and use scaled reference frequencies for 
				//	those Breakpoints:
				fix_frequencies( src, *ref0 );		
			}
		}
		else if ( ref0 != 0 )
		{
			//	fake it from the reference Partial:
			src = *ref0;
			double fscale = (double)label / 1;
			for ( Partial::iterator bpPos = src.begin(); bpPos != src.end(); ++bpPos )
			{
				bpPos->setAmplitude( 0 );
				bpPos->setFrequency( bpPos->frequency() * fscale );
			}
		}
		//	else src is a dummy
		
		// construct target Partial for morph:
		Partial tgt;
		if ( p1 != 0 )
		{
			//	use the Partial in the correspondence
			tgt = *p1;
			
			if ( ref1 != 0 )
			{
				//	find quiet parts of the target Partial,
				//	and use scaled reference frequencies for 
				//	those Breakpoints:
				fix_frequencies( tgt, *ref1 );
			}
		}
		else if ( ref1 != 0 )
		{
			//	fake it from the reference Partial:
			tgt = *ref1;
			double fscale = (double)label / 1;
			for ( Partial::iterator bpPos = tgt.begin(); bpPos != tgt.end(); ++bpPos )
			{
				bpPos->setAmplitude( 0 );
				bpPos->setFrequency( bpPos->frequency() * fscale );
			}
		}
		//	else tgt is a dummy
		
		debugger << "morphing " << ( ( 0 < src.size() )?( 1 ):( 0 ) )
				 << " and " << ( ( 0 < tgt.size() )?( 1 ):( 0 ) )
				 <<	" partials with label " <<	label << endl;
		morphPartial( src, tgt, label );
	}
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


#pragma mark -- reference Partial label access/mutation --

// ---------------------------------------------------------------------------
//	sourceReferenceLabel
// ---------------------------------------------------------------------------
//! 	Return the label of the Partial to be used as a reference
//!	Partial for the source sequence in a morph of two Partial
//!	sequences. The reference partial is used to compute 
//!	frequencies for very low-amplitude Partials whose frequency
//!	estimates are not considered reliable. The reference Partial
//!	is considered to have good frequency estimates throughout.
//!	The default label of 0 indicates that no reference Partial
//!	should be used for the source sequence.
//
Partial::label_type
Morpher::sourceReferenceLabel( void ) const
{
	return _refLabel0;
}

// ---------------------------------------------------------------------------
//	targetReferenceLabel
// ---------------------------------------------------------------------------
//! 	Return the label of the Partial to be used as a reference
//!	Partial for the target sequence in a morph of two Partial
//!	sequences. The reference partial is used to compute 
//!	frequencies for very low-amplitude Partials whose frequency
//!	estimates are not considered reliable. The reference Partial
//!	is considered to have good frequency estimates throughout.
//!	The default label of 0 indicates that no reference Partial
//!	should be used for the target sequence.
//
Partial::label_type
Morpher::targetReferenceLabel( void ) const
{
	return _refLabel1;
}
	
// ---------------------------------------------------------------------------
//	setSourceReferenceLabel
// ---------------------------------------------------------------------------
//! 	Set the label of the Partial to be used as a reference
//!	Partial for the source sequence in a morph of two Partial
//!	sequences. The reference partial is used to compute 
//!	frequencies for very low-amplitude Partials whose frequency
//!	estimates are not considered reliable. The reference Partial
//!	is considered to have good frequency estimates throughout.
//!	Setting the reference label to 0 indicates that no reference 
//!	Partial should be used for the source sequence.
//
void 
Morpher::setSourceReferenceLabel( Partial::label_type l )
{
	_refLabel0 = l;
}

// ---------------------------------------------------------------------------
//	setTargetReferenceLabel
// ---------------------------------------------------------------------------
//! 	Set the label of the Partial to be used as a reference
//!	Partial for the target sequence in a morph of two Partial
//!	sequences. The reference partial is used to compute 
//!	frequencies for very low-amplitude Partials whose frequency
//!	estimates are not considered reliable. The reference Partial
//!	is considered to have good frequency estimates throughout.
//!	Setting the reference label to 0 indicates that no reference 
//!	Partial should be used for the target sequence.
//
void 
Morpher::setTargetReferenceLabel( Partial::label_type l )
{
	_refLabel1 = l;
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

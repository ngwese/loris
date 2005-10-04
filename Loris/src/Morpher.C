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
#include <cmath>

#if defined(HAVE_M_PI) && (HAVE_M_PI)
    const double Pi = M_PI;
#else
    const double Pi = 3.14159265358979324;
#endif

//    begin namespace
namespace Loris {

// new morphing algorithms
#define MORPH_PHASE_TRAVEL 0
#define NEW_BW_MORPH 1

const double Morpher::DefaultFixThreshold = -90; // dB, very low by default

// shaping parameter, see interpolateLogAmplitudes:
// compile with LINEAR_AMP_MORPHS defined for
// legacy-style linear amplitude morphs by default.
// The default can always be overridden using 
// setAmplitudeShape.
//
#if !defined(LINEAR_AMP_MORPHS) || !LINEAR_AMP_MORPHS
   const double Morpher::DefaultAmpShape = 1E-5;    
#else  
   const double Morpher::DefaultAmpShape = 1E5;    
#endif
const double Morpher::DefaultBreakpointGap = 1E-4; // minimum time (sec) between Breakpoints in 
                                                   // morphed Partials

// helper declarations
static inline double interpolateFrequencies( double f0, double f1, double alpha );
static inline double interpolateLogAmplitudes( double a0, double a1, double alpha, double shape );
static inline double interpolateBandwidths( double bw0, double bw1, double alpha );
static inline double interpolatePhases( double phi0, double phi1, double alpha );
static inline double interpolateWholePeriods( double f0, double f1, double dt, double alpha );

static inline Breakpoint interpolateParameters( const Breakpoint & srcBkpt, const Breakpoint & tgtBkpt,
                                                double fweight, double aweight, double ashape, 
                                                double bweight );

static void fixPhaseTravel( const Breakpoint & bp0, Breakpoint & bp1, double dt, double alpha );
   
static double phaseTravel( double f0, double f1, double dt );
static double wrapPi( double x );
   
#pragma mark -- construction --

// ---------------------------------------------------------------------------
//    Morpher constructor (single morph function)
// ---------------------------------------------------------------------------
//    Construct a new Morpher using the same morphing envelope for 
//    frequency, amplitude, and bandwidth (noisiness).
//
Morpher::Morpher( const Envelope & f ) :
    _freqFunction( f.clone() ),
    _ampFunction( f.clone() ),
    _bwFunction( f.clone() ),
    _freqFixThresholdDb( DefaultFixThreshold ),
    _ampMorphShape( DefaultAmpShape ),
    _minBreakpointGapSec( DefaultBreakpointGap )
{
}

// ---------------------------------------------------------------------------
//    Morpher constructor (distinct morph functions)
// ---------------------------------------------------------------------------
//    Construct a new Morpher using the specified morphing envelopes for
//    frequency, amplitude, and bandwidth (noisiness).
//
Morpher::Morpher( const Envelope & ff, const Envelope & af, const Envelope & bwf ) :
    _freqFunction( ff.clone() ),
    _ampFunction( af.clone() ),
    _bwFunction( bwf.clone() ),
    _freqFixThresholdDb( DefaultFixThreshold ),
    _ampMorphShape( DefaultAmpShape ),
    _minBreakpointGapSec( DefaultBreakpointGap )
{
}

// ---------------------------------------------------------------------------
//    Morpher copy constructor 
// ---------------------------------------------------------------------------
//!    Construct a new Morpher that is a duplicate of rhs.
//!
//! \param  rhs is the Morpher to duplicate
Morpher::Morpher( const Morpher & rhs ) :
    _freqFunction( rhs._freqFunction->clone() ),
    _ampFunction( rhs._ampFunction->clone() ),
    _bwFunction( rhs._bwFunction->clone() ),
    _srcRefPartial( rhs._srcRefPartial ),
    _tgtRefPartial( rhs._tgtRefPartial ),
    _freqFixThresholdDb( rhs._freqFixThresholdDb ),
    _ampMorphShape( rhs._ampMorphShape ),
    _minBreakpointGapSec( rhs._minBreakpointGapSec )
{
}

// ---------------------------------------------------------------------------
//    Morpher destructor
// ---------------------------------------------------------------------------
//    Destroy this Morpher.
//
Morpher::~Morpher( void )
{
}

// ---------------------------------------------------------------------------
//    Morpher assignment operator
// ---------------------------------------------------------------------------
//! Make this Morpher a duplicate of rhs.
//!
//! \param  rhs is the Morpher to duplicate
Morpher & 
Morpher::operator= ( const Morpher & rhs )
{
    if ( &rhs != this )
    {
        _freqFunction.reset( rhs._freqFunction->clone() );
        _ampFunction.reset( rhs._ampFunction->clone() );
        _bwFunction.reset( rhs._bwFunction->clone() );
        
        _srcRefPartial = rhs._srcRefPartial;
        _tgtRefPartial = rhs._tgtRefPartial;
        
        _freqFixThresholdDb = rhs._freqFixThresholdDb;
        _ampMorphShape = rhs._ampMorphShape;
        _minBreakpointGapSec = rhs._minBreakpointGapSec;
    }
    return *this;
}



#pragma mark -- Partial morphing --

// ---------------------------------------------------------------------------
//    morphPartial
// ---------------------------------------------------------------------------
//!	Morph a pair of Partials to yield a new morphed Partial. 
//!	Dummy Partials (having no Breakpoints) don't contribute to the
//!	morph, except to cause their opposite to fade out. 
//!	Either (or neither) the source or target Partial may be a dummy 
//!	Partial (no Breakpoints), but not both. The morphed
//!	Partial has Breakpoints at times corresponding to every Breakpoint 
//!	in both source Partials, omitting Breakpoints that would be
//!   closer than the minBreakpointGap to their predecessor. 
//!	The new morphed Partial is assigned the specified label and returned.
//!
//!	\param 	src is the Partial corresponding to a morph function
//!		   	value of 0, evaluated at the specified time.
//!	\param 	tgt is the Partial corresponding to a morph function
//!		   	value of 1, evaluated at the specified time.
//!	\param 	assignLabel is the label assigned to the morphed Partial
//! \return the morphed Partial
//
Partial
Morpher::morphPartial( Partial src, Partial tgt, int assignLabel )
{  
    if ( (src.numBreakpoints() == 0) && (tgt.numBreakpoints() == 0) )
    {
        Throw( InvalidArgument, "Cannot morph two empty Partials," );
    }    
    
    Partial::const_iterator src_iter = src.begin();
    Partial::const_iterator tgt_iter = tgt.begin();
    
    // find the earliest time that a Breakpoint
    // could be added to the morph:
    double dontAddBefore = 0;
    if ( 0 < src.numBreakpoints() )
    {
        dontAddBefore = std::min( dontAddBefore, src_iter.time() );
    }
    if ( 0 < tgt.numBreakpoints() )
    {
        dontAddBefore = std::min( dontAddBefore, tgt_iter.time() );
    }
    
    //  make a new Partial:
    Partial newp;
    newp.setLabel( assignLabel );
    
    //  Merge Breakpoints from the two Partials,
    //  loop until there are no more Breakpoints to
    //  consider in either Partial.
    while ( src_iter != src.end() || tgt_iter != tgt.end() )
    {
        if ( ( tgt_iter == tgt.end() ) ||
             ( src_iter != src.end() && src_iter.time() < tgt_iter.time() ) )
        {
            //  Ran out of tgt Breakpoints, or 
            //  src Breakpoint is earlier, add it.
            //
            //  Don't insert Breakpoints arbitrarily close together, 
            //  only insert a new Breakpoint if it is later than
            //  the end of the new Partial by more than the gap time.
            if ( dontAddBefore <= src_iter.time() )
            {
                appendMorphedSrc( src_iter.breakpoint(), tgt, src_iter.time(), newp );
            }
            /*
            else
            {
                notifier << "skipping source Breakpoint at time " << t;
                notifier << " (label " << assignLabel << ")" << endl;
            }
            */
            ++src_iter;
        }
        else 
        {
            //  Ran out of src Breakpoints, or
            //  tgt Breakpoint is earlier add it.
            //
            //  Don't insert Breakpoints arbitrarily close together, 
            //  only insert a new Breakpoint if it is later than
            //  the end of the new Partial by more than the gap time.
            if ( dontAddBefore <= tgt_iter.time() )
            {
                appendMorphedTgt( tgt_iter.breakpoint(), src, tgt_iter.time(), newp );
            }
            /*
            else
            {
                notifier << "skipping target Breakpoint at time " << t;
                notifier << " (label " << assignLabel << ")" << endl;
            }
            */
            ++tgt_iter;
        }  
        
        if ( 0 != newp.numBreakpoints() )
        {
            // update the earliest time the next Breakpoint
            // could be added to the morph:
            dontAddBefore = newp.endTime() + _minBreakpointGapSec;
        }          
    }

    return newp;
}

// ---------------------------------------------------------------------------
//    crossfade
// ---------------------------------------------------------------------------
//    Crossfade Partials with no correspondences.
//
//    Unlabeled Partials (having label 0) are considered to 
//    have no correspondences, so they are just faded out, and not 
//    actually morphed. This is the same as morphing each with an 
//    empty dummy Partial (having no Breakpoints). 
//
//    The Partials in the first range are treated as components of the 
//    source sound, corresponding to a morph function value of 0, and  
//    those in the second are treated as components of the target sound, 
//    corresponding to a morph function value of 1.
//
//    The crossfaded Partials are stored in the Morpher's PartialList.
//
void 
Morpher::crossfade( PartialList::const_iterator beginSrc, 
                    PartialList::const_iterator endSrc,
                    PartialList::const_iterator beginTgt, 
                    PartialList::const_iterator endTgt,
                    Partial::label_type label /* default 0 */ )
{
    Partial nullPartial;
    debugger << "crossfading unlabeled (labeled 0) Partials" << endl;
    
    long debugCounter;

    //    crossfade Partials corresponding to a morph weight of 0:
    PartialList::const_iterator it;
    debugCounter = 0;
    for ( it = beginSrc; it != endSrc; ++it )
    {
        if ( it->label() == label && 0 != it->numBreakpoints() )
        {            
            Partial newp;
            newp.setLabel( label );
            double dontAddBefore = it->startTime();

            for ( Partial::const_iterator bpPos = it->begin(); 
                  bpPos != it->end(); 
                  ++bpPos )
            {        
                //  Don't insert Breakpoints arbitrarily close together, 
                //  only insert a new Breakpoint if it is later than
                //  the end of the new Partial by more than the gap time.
                if ( dontAddBefore <= bpPos.time() )
                {
                    newp.insert( bpPos.time(), 
                                 fadeSrcBreakpoint( bpPos.breakpoint(), bpPos.time() ) );
                    dontAddBefore = bpPos.time() + _minBreakpointGapSec;
                }
            }
            
            if ( newp.numBreakpoints() > 0 )
            {
                ++debugCounter;
                _partials.push_back( newp );
            }
        }
    }
    debugger << "kept " << debugCounter << " from sound 1" << endl;

    //    crossfade Partials corresponding to a morph weight of 1:
    debugCounter = 0;
    for ( it = beginTgt; it != endTgt; ++it )
    {
        if ( it->label() == label && 0 != it->numBreakpoints() )
        {
            Partial newp;
            newp.setLabel( label );
            double dontAddBefore = it->startTime();
                            
            for ( Partial::const_iterator bpPos = it->begin(); 
                  bpPos != it->end(); 
                  ++bpPos )
            {
                //  Don't insert Breakpoints arbitrarily close together, 
                //  only insert a new Breakpoint if it is later than
                //  the end of the new Partial by more than the gap time.
                if ( dontAddBefore <= bpPos.time() )
                {
                    newp.insert( bpPos.time(),
                                 fadeTgtBreakpoint( bpPos.breakpoint(), bpPos.time() ) );           
                    dontAddBefore = bpPos.time() + _minBreakpointGapSec;
                }
            }
            
            if ( newp.numBreakpoints() > 0 )
            {
                ++debugCounter;
                _partials.push_back( newp );
            }
        }
    }
    debugger << "kept " << debugCounter << " from sound 2" << endl;
}

// ---------------------------------------------------------------------------
//    morph
// ---------------------------------------------------------------------------
//    Morph two sounds (collections of Partials labeled to indicate
//    correspondences) into a single labeled collection of Partials.
//    Unlabeled Partials (having label 0) are crossfaded. The morphed
//    and crossfaded Partials are stored in the Morpher's PartialList.
//
//    The Partials in the first range are treated as components of the 
//    source sound, corresponding to a morph function value of 0, and  
//    those in the second are treated as components of the target sound, 
//    corresponding to a morph function value of 1.
//
//    Throws InvalidArgument if either the source or target
//    sequence is not distilled (contains more than one Partial having
//    the same non-zero label).
//
//    Ugh! This ought to be a template function!
// Ugh! But then crossfade needs to be a template function.
// Maybe need to do something different with crossfade first.
//
void 
Morpher::morph( PartialList::const_iterator beginSrc, 
                PartialList::const_iterator endSrc,
                PartialList::const_iterator beginTgt, 
                PartialList::const_iterator endTgt )
{
    //    build a PartialCorrespondence, a map of labels
    //    to pairs of pointers to Partials, by making every
    //    Partial in the source the first element of the
    //    pair at the corresponding label, and every Partial
    //    in the target the second element of the pair at
    //    the corresponding label. Pointers not assigned to
    //    point to a Partial in the source or target are 
    //    initialized to 0 in the correspondence map.
    PartialCorrespondence correspondence;
    
    //    add source Partials to the correspondence map:
    for ( PartialList::const_iterator it = beginSrc; it != endSrc; ++it ) 
    {
        //    don't add the crossfade label to the set:
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
    
    //    add target Partials to the correspondence map:
    for ( PartialList::const_iterator it = beginTgt; it != endTgt; ++it ) 
    {
        //    don't add the crossfade label to the set:
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
    
    //    morph corresponding labeled Partials:
    morph_aux( correspondence );
    
    //    crossfade the remaining unlabeled Partials:
    crossfade( beginSrc, endSrc, beginTgt, endTgt );
}

// ---------------------------------------------------------------------------
//    morphBreakpoints
// ---------------------------------------------------------------------------
//!    Compute morphed parameter values at the specified time, using
//!    the source and target Breakpoints (assumed to correspond exactly
//!    to the specified time).
//!
//!    \param  srcBkpt is the Breakpoint corresponding to a morph function
//!            value of 0.
//!    \param  tgtBkpt is the Breakpoint corresponding to a morph function
//!            value of 1.
//!    \param  time is the time corresponding to srcBkpt (used
//!            to evaluate the morphing functions and tgtPartial).
//!    \return the morphed Breakpoint
//
Breakpoint
Morpher::morphBreakpoints( Breakpoint srcBkpt, Breakpoint tgtBkpt, 
                           double time  ) const
{
   double fweight = _freqFunction->valueAt( time );
   double aweight = _ampFunction->valueAt( time );
   double bweight = _bwFunction->valueAt( time );

   // compute interpolated Breakpoint parameters:
   return interpolateParameters( srcBkpt, tgtBkpt, fweight, 
                                 aweight, _ampMorphShape, bweight );
}

// ---------------------------------------------------------------------------
//    morphSrcBreakpoint
// ---------------------------------------------------------------------------
//!    Compute morphed parameter values at the specified time, using
//!    the source Breakpoint (assumed to correspond exactly to the
//!    specified time) and the target Partial (whose parameters are
//!    examined at the specified time).
//!
//!    \pre    the target Partial may not be a dummy Partial (no Breakpoints).
//!
//!    \param  srcBkpt is the Breakpoint corresponding to a morph function
//!            value of 0.
//!    \param  tgtPartial is the Partial corresponding to a morph function
//!            value of 1, evaluated at the specified time.
//!    \param  time is the time corresponding to srcBkpt (used
//!            to evaluate the morphing functions and tgtPartial).
//!    \return the morphed Breakpoint
//
Breakpoint
Morpher::morphSrcBreakpoint( const Breakpoint & srcBkpt, const Partial & tgtPartial, 
                             double time  ) const
{
    if ( 0 == tgtPartial.numBreakpoints() )
    {
        Throw( InvalidArgument, "morphSrcBreakpoint cannot morph with empty Partial" );
    }
    
    Breakpoint tgtBkpt = tgtPartial.parametersAt( time );
    
    return morphBreakpoints( srcBkpt, tgtBkpt, time );
}

// ---------------------------------------------------------------------------
//    morphTgtBreakpoint
// ---------------------------------------------------------------------------
//!    Compute morphed parameter values at the specified time, using
//!    the target Breakpoint (assumed to correspond exactly to the
//!    specified time) and the source Partial (whose parameters are
//!    examined at the specified time).
//!
//!    \pre    the source Partial may not be a dummy Partial (no Breakpoints).
//!
//!    \param  tgtBkpt is the Breakpoint corresponding to a morph function
//!            value of 1.
//!    \param  srcPartial is the Partial corresponding to a morph function
//!            value of 0, evaluated at the specified time.
//!    \param  time is the time corresponding to srcBkpt (used
//!            to evaluate the morphing functions and srcPartial).
//!    \return the morphed Breakpoint
//
Breakpoint
Morpher::morphTgtBreakpoint( const Breakpoint & tgtBkpt, const Partial & srcPartial, 
                             double time  ) const
{
    if ( 0 == srcPartial.numBreakpoints() )
    {
        Throw( InvalidArgument, "morphTgtBreakpoint cannot morph with empty Partial" );
    }
    
    Breakpoint srcBkpt = srcPartial.parametersAt( time );
   
    return morphBreakpoints( srcBkpt, tgtBkpt, time );
}

// ---------------------------------------------------------------------------
//    fadeSrcBreakpoint
// ---------------------------------------------------------------------------
//! Compute morphed parameter values at the specified time, using
//! the source Breakpoint, assumed to correspond exactly to the
//! specified time, and assuming that there is no corresponding 
//! target Partial, so the source Breakpoint should be simply faded.
//!
//! \param  bp is the Breakpoint corresponding to a morph function
//!         value of 0.
//! \param  time is the time corresponding to bp (used
//!         to evaluate the morphing functions).
//! \return the faded Breakpoint
//
Breakpoint
Morpher::fadeSrcBreakpoint( Breakpoint bp, double time ) const
{
    double alpha = _ampFunction->valueAt( time );
    bp.setAmplitude( interpolateLogAmplitudes( bp.amplitude(), 0, 
                                               alpha, _ampMorphShape ) );
    return bp;
}

// ---------------------------------------------------------------------------
//    fadeTgtBreakpoint
// ---------------------------------------------------------------------------
//! Compute morphed parameter values at the specified time, using
//! the target Breakpoint, assumed to correspond exactly to the
//! specified time, and assuming that there is not corresponding 
//! source Partial, so the target Breakpoint should be simply faded.
//!
//! \param  bp is the Breakpoint corresponding to a morph function
//!         value of 1.
//! \param  time is the time corresponding to bp (used
//!         to evaluate the morphing functions).
//! \return the faded Breakpoint
//
Breakpoint
Morpher::fadeTgtBreakpoint( Breakpoint bp, double time ) const
{
    double alpha = _ampFunction->valueAt( time );
    bp.setAmplitude( interpolateLogAmplitudes( 0, bp.amplitude(), 
                                               alpha, _ampMorphShape ) );
    return bp;
}

#pragma mark -- morphing function access/mutation --

// ---------------------------------------------------------------------------
//    setFrequencyFunction
// ---------------------------------------------------------------------------
//    Assign a new frequency morphing envelope to this Morpher.
//
void
Morpher::setFrequencyFunction( const Envelope & f )
{
    _freqFunction.reset( f.clone() );
}

// ---------------------------------------------------------------------------
//    setAmplitudeFunction
// ---------------------------------------------------------------------------
//    Assign a new amplitude morphing envelope to this Morpher.
//
void
Morpher::setAmplitudeFunction( const Envelope & f )
{
    _ampFunction.reset( f.clone() );
}

// ---------------------------------------------------------------------------
//    setBandwidthFunction
// ---------------------------------------------------------------------------
//    Assign a new bandwidth morphing envelope to this Morpher.
//
void
Morpher::setBandwidthFunction( const Envelope & f )
{
    _bwFunction.reset( f.clone() );
}

// ---------------------------------------------------------------------------
//    frequencyFunction
// ---------------------------------------------------------------------------
//    Return a reference to this Morpher's frequency morphing envelope.
//
const Envelope &
Morpher::frequencyFunction( void ) const 
{
    return * _freqFunction;
}

// ---------------------------------------------------------------------------
//    amplitudeFunction
// ---------------------------------------------------------------------------
//    Return a reference to this Morpher's amplitude morphing envelope.
//
const Envelope &
Morpher::amplitudeFunction( void ) const 
{
    return * _ampFunction;
}

// ---------------------------------------------------------------------------
//    bandwidthFunction
// ---------------------------------------------------------------------------
//    Return a reference to this Morpher's bandwidth morphing envelope.
//
const Envelope &
Morpher::bandwidthFunction( void ) const 
{
    return * _bwFunction;
}


#pragma mark -- reference Partial label access/mutation --

// ---------------------------------------------------------------------------
//    sourceReferencePartial
// ---------------------------------------------------------------------------
//! Return the Partial to be used as a reference
//!	Partial for the source sequence in a morph of two Partial
//!	sequences. The reference partial is used to compute 
//!	frequencies for very low-amplitude Partials whose frequency
//!	estimates are not considered reliable. The reference Partial
//!	is considered to have good frequency estimates throughout.
//!	A default (empty) Partial indicates that no reference Partial
//!	should be used for the source sequence.
//
const Partial &
Morpher::sourceReferencePartial( void ) const
{
    return _srcRefPartial;
}

// ---------------------------------------------------------------------------
//    sourceReferencePartial
// ---------------------------------------------------------------------------
//! Return the Partial to be used as a reference
//!	Partial for the source sequence in a morph of two Partial
//!	sequences. The reference partial is used to compute 
//!	frequencies for very low-amplitude Partials whose frequency
//!	estimates are not considered reliable. The reference Partial
//!	is considered to have good frequency estimates throughout.
//!	A default (empty) Partial indicates that no reference Partial
//!	should be used for the source sequence.
//
Partial &
Morpher::sourceReferencePartial( void )
{
    return _srcRefPartial;
}

// ---------------------------------------------------------------------------
//    targetReferenceLabel
// ---------------------------------------------------------------------------
//! Return the Partial to be used as a reference
//!	Partial for the target sequence in a morph of two Partial
//!	sequences. The reference partial is used to compute 
//!	frequencies for very low-amplitude Partials whose frequency
//!	estimates are not considered reliable. The reference Partial
//!	is considered to have good frequency estimates throughout.
//!	A default (empty) Partial indicates that no reference Partial
//!	should be used for the target sequence.
//
const Partial &
Morpher::targetReferencePartial( void ) const
{
    return _tgtRefPartial;
}
    
// ---------------------------------------------------------------------------
//    targetReferenceLabel
// ---------------------------------------------------------------------------
//! Return the Partial to be used as a reference
//!	Partial for the target sequence in a morph of two Partial
//!	sequences. The reference partial is used to compute 
//!	frequencies for very low-amplitude Partials whose frequency
//!	estimates are not considered reliable. The reference Partial
//!	is considered to have good frequency estimates throughout.
//!	A default (empty) Partial indicates that no reference Partial
//!	should be used for the target sequence.
//
Partial &
Morpher::targetReferencePartial( void ) 
{
    return _tgtRefPartial;
}
    
// ---------------------------------------------------------------------------
//    setSourceReferencePartial
// ---------------------------------------------------------------------------
//! Specify the Partial to be used as a reference
//!	Partial for the source sequence in a morph of two Partial
//!	sequences. The reference partial is used to compute 
//!	frequencies for very low-amplitude Partials whose frequency
//!	estimates are not considered reliable. The reference Partial
//!	is considered to have good frequency estimates throughout.
//! The specified Partial must be labeled with its harmonic number.
//!	A default (empty) Partial indicates that no reference 
//!	Partial should be used for the source sequence.
//
void 
Morpher::setSourceReferencePartial( const Partial & p )
{
	if ( p.label() == 0 )
	{
		Throw( InvalidArgument, 
			   "the morphing source reference Partial must be "
			   "labeled with its harmonic number" );
	}
	_srcRefPartial = p;
}

// ---------------------------------------------------------------------------
//    setSourceReferencePartial
// ---------------------------------------------------------------------------
//! Specify the Partial to be used as a reference
//!	Partial for the source sequence in a morph of two Partial
//!	sequences. The reference partial is used to compute 
//!	frequencies for very low-amplitude Partials whose frequency
//!	estimates are not considered reliable. The reference Partial
//!	is considered to have good frequency estimates throughout.
//!	A default (empty) Partial indicates that no reference 
//!	Partial should be used for the source sequence.
//!
//!	\param	partials a sequence of Partials to search
//!			for the reference Partial
//!	\param	refLabel the label of the Partial in partials
//!			that should be selected as the reference
//
void 
Morpher::setSourceReferencePartial( const PartialList & partials, 
									Partial::label_type refLabel )
{
	if ( refLabel != 0 )
	{
		PartialList::const_iterator pos = 
			std::find_if( partials.begin(), partials.end(), 
						  PartialUtils::isLabelEqual( refLabel ) );
		if ( pos == partials.end() )
		{
			Throw( InvalidArgument, "no Partial has the specified reference label" );
		}
		_srcRefPartial = *pos;
	}
	else
	{
		_srcRefPartial = Partial();
	}
}

// ---------------------------------------------------------------------------
//    setTargetReferencePartial
// ---------------------------------------------------------------------------
//! Specify the Partial to be used as a reference
//!	Partial for the target sequence in a morph of two Partial
//!	sequences. The reference partial is used to compute 
//!	frequencies for very low-amplitude Partials whose frequency
//!	estimates are not considered reliable. The reference Partial
//!	is considered to have good frequency estimates throughout.
//! The specified Partial must be labeled with its harmonic number.
//!	A default (empty) Partial indicates that no reference 
//!	Partial should be used for the target sequence.
//
void 
Morpher::setTargetReferencePartial( const Partial & p )
{
	if ( p.label() == 0 )
	{
		Throw( InvalidArgument, 
			   "the morphing target reference Partial must be "
			   "labeled with its harmonic number" );
	}
	_tgtRefPartial = p;
}

// ---------------------------------------------------------------------------
//    setTargetReferencePartial
// ---------------------------------------------------------------------------
//! Specify the Partial to be used as a reference
//!	Partial for the target sequence in a morph of two Partial
//!	sequences. The reference partial is used to compute 
//!	frequencies for very low-amplitude Partials whose frequency
//!	estimates are not considered reliable. The reference Partial
//!	is considered to have good frequency estimates throughout.
//!	A default (empty) Partial indicates that no reference 
//!	Partial should be used for the target sequence.
//!
//!	\param	partials a sequence of Partials to search
//!			for the reference Partial
//!	\param	refLabel the label of the Partial in partials
//!			that should be selected as the reference
//
void 
Morpher::setTargetReferencePartial( const PartialList & partials, 
									Partial::label_type refLabel )
{
	if ( refLabel != 0 )
	{
		PartialList::const_iterator pos = 
			std::find_if( partials.begin(), partials.end(), 
						  PartialUtils::isLabelEqual( refLabel ) );
		if ( pos == partials.end() )
		{
			Throw( InvalidArgument, "no Partial has the specified reference label" );
		}
		_tgtRefPartial = *pos;
	}
	else
	{
		_tgtRefPartial = Partial();
	}
}


// ---------------------------------------------------------------------------
//    amplitudeShape
// ---------------------------------------------------------------------------
//    Return the shaping parameter for the amplitude moprhing
//    function (only used in new log-amplitude morphing).
//    This shaping parameter controls the 
//    slope of the amplitude morphing function,
//    for values greater than 1, this function
//    gets nearly linear (like the old amplitude
//    morphing function), for values much less 
//    than 1 (e.g. 1E-5) the slope is gently
//    curved and sounds pretty "linear", for 
//    very small values (e.g. 1E-12) the curve
//    is very steep and sounds un-natural because
//    of the huge jump from zero amplitude to
//    very small amplitude.
double Morpher::amplitudeShape( void ) const
{
    return _ampMorphShape;
}

// ---------------------------------------------------------------------------
//    setAmplitudeShape
// ---------------------------------------------------------------------------
//    Set the shaping parameter for the amplitude moprhing
//    function. This shaping parameter controls the 
//    slope of the amplitude morphing function,
//    for values greater than 1, this function
//    gets nearly linear (like the old amplitude
//    morphing function), for values much less 
//    than 1 (e.g. 1E-5) the slope is gently
//    curved and sounds pretty "linear", for 
//    very small values (e.g. 1E-12) the curve
//    is very steep and sounds un-natural because
//    of the huge jump from zero amplitude to
//    very small amplitude.
//
//    x is the new shaping parameter, it must be positive.
void Morpher::setAmplitudeShape( double x )
{
    if ( x <= 0. )
    {
        Throw( InvalidArgument, "the amplitude morph shaping parameter must be positive");
    }
    _ampMorphShape = x;
}

// ---------------------------------------------------------------------------
//    minBreakpointGap
// ---------------------------------------------------------------------------
//    Return the minimum time gap (secs) between two Breakpoints
//    in the morphed Partials. Morphing two
//    Partials can generate a third Partial having
//    Breakpoints arbitrarily close together in time,
//    and this makes morphs huge. Raising this 
//    threshold limits the Breakpoint density in
//    the morphed Partials. Default is 1/10 ms.
double Morpher::minBreakpointGap( void ) const
{
    return _minBreakpointGapSec;
}

// ---------------------------------------------------------------------------
//    setMinBreakpointGap
// ---------------------------------------------------------------------------
//    Set the minimum time gap (secs) between two Breakpoints
//    in the morphed Partials. Morphing two
//    Partials can generate a third Partial having
//    Breakpoints arbitrarily close together in time,
//    and this makes morphs huge. Raising this 
//    threshold limits the Breakpoint density in
//    the morphed Partials. Default is 1/10 ms.
//
//    x is the new minimum gap in seconds, it must be positive
//    
void Morpher::setMinBreakpointGap( double x )
{
    if ( x <= 0. )
    {
        Throw( InvalidArgument, "the minimum Breakpoint gap must be positive");
    }
    _minBreakpointGapSec = x;
}

#pragma mark -- PartialList access --

// ---------------------------------------------------------------------------
//    partials
// ---------------------------------------------------------------------------
//    Return a reference to this Morpher's list of morphed Partials.
//
PartialList & 
Morpher::partials( void )
{ 
    return _partials; 
}

// ---------------------------------------------------------------------------
//    partials
// ---------------------------------------------------------------------------
//    Return a const reference to this Morpher's list of morphed Partials.
//
const PartialList & 
Morpher::partials( void ) const 
{ 
    return _partials; 
}

#pragma mark -- helpers: morphed parameter computation --

// ---------------------------------------------------------------------------
//    adjustFrequency
// ---------------------------------------------------------------------------
// Leave the phase alone, because I don't know what we can do with it.
//
static void adjustFrequency( Breakpoint & bp, const Partial & ref, 
                             Partial::label_type harmonicNum,
                             double thresholdDb,
                             double time )
{
    if ( ref.numBreakpoints() != 0 )
    {
        //    compute absolute magnitude thresholds:
        static const double FadeRangeDB = 10;
        const double BeginFade = std::pow( 10., 0.05 * (thresholdDb+FadeRangeDB) );
                
        if ( bp.amplitude() < BeginFade )
        {
            const double Threshold = std::pow( 10., 0.05 * thresholdDb );
            const double OneOverFadeSpan = 1. / ( BeginFade - Threshold );

            double fscale = (double)harmonicNum / ref.label();

            double alpha = std::min( ( BeginFade - bp.amplitude() ) * OneOverFadeSpan, 1. );
            double fRef = ref.frequencyAt( time );
            bp.setFrequency( ( alpha * ( fRef * fscale ) ) + 
                             ( (1 - alpha) * bp.frequency() ) );
        }
    }
}

// ---------------------------------------------------------------------------
//    partial_is_nonnull
// ---------------------------------------------------------------------------
//  Helper function to examine a morphed Partial and determine whether 
//  it has any non-null Breakpoints. If not, there's no point in saving it.
//
static bool partial_is_nonnull( const Partial & p )
{
    for ( Partial::const_iterator it = p.begin(); it != p.end(); ++it )
    {
        if ( it.breakpoint().amplitude() != 0.0 )
        {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
//    morph_aux
// ---------------------------------------------------------------------------
//    Helper function that performs the morph between corresponding pairs
//    of Partials identified in a PartialCorrespondence. Called by the
//    morph() implementation accepting two sequences of Partials.
//
//    PartialCorrespondence represents a map from non-zero Partial 
//    labels to pairs of pointers to Partials that should be morphed 
//    into a single Partial that is assigned that label. 
//    PartialPtrPair is a pair of pointers to Partials that are
//    initialized to zero, and it is the element type for the
//    PartialCorrespondence map.
//
void Morpher::morph_aux( PartialCorrespondence & correspondence  )
{
    PartialCorrespondence::const_iterator it;
    for ( it = correspondence.begin(); it != correspondence.end(); ++it )
    {
        Partial::label_type label = it->first;
        PartialPtrPair match = it->second;
        const Partial * p0 = match.first;
        const Partial * p1 = match.second;

        //    sanity check:
        //    one of those Partials must exist
        Assert( p0 != 0 || p1 != 0 );
                    
        // construct source Partial for morph:
        Partial src;
        if ( p0 != 0 )
        {
            //    use the Partial in the correspondence
            src = *p0;
        }
        else if ( _srcRefPartial.numBreakpoints() != 0 )
        {
            //    fake it from the reference Partial:
            double fscale = double(label) / _srcRefPartial.label();
            src = makePartialFromReference( _srcRefPartial, fscale );
        }
        //    else src is a dummy
        
        // construct target Partial for morph:
        Partial tgt;
        if ( p1 != 0 )
        {
            //    use the Partial in the correspondence
            tgt = *p1;
        }
        else if ( _tgtRefPartial.numBreakpoints() != 0 )
        {
            //    fake it from the reference Partial:
            double fscale = double(label) / _tgtRefPartial.label();            
            tgt = makePartialFromReference( _tgtRefPartial, fscale );
        }
        //    else tgt is a dummy
        
        debugger << "morphing " << ( ( 0 < src.numBreakpoints() )?( 1 ):( 0 ) )
                   << " and " << ( ( 0 < tgt.numBreakpoints() )?( 1 ):( 0 ) )
                   << " partials with label " <<    label << endl;
                   
        //  &^)     HEY LOOKIE HERE!!!!!!!!!!!!!                   
        // try this kludge to solve the problem of Nulls 
        // getting left out of morphed Partials leading to
        // eroneous non-zero amplitude segments:
        if ( src.numBreakpoints() != 0 )
        {
            if ( src.first().amplitude() != 0.0 && src.startTime() > _minBreakpointGapSec )
            {
                double t = src.startTime() - _minBreakpointGapSec;
                Breakpoint null = src.parametersAt( t );
                src.insert( t, null );
            }
            if ( src.last().amplitude() != 0.0 )
            {
                double t = src.endTime() + _minBreakpointGapSec;
                Breakpoint null = src.parametersAt( t );
                src.insert( t, null );
            }
        }
        
        if ( tgt.numBreakpoints() != 0 )
        {            
            if ( tgt.first().amplitude() != 0.0 && tgt.startTime() > _minBreakpointGapSec )
            {
                double t = tgt.startTime() - _minBreakpointGapSec;
                Breakpoint null = tgt.parametersAt( t );
                tgt.insert( t, null );
            }
            if ( tgt.last().amplitude() != 0.0 )
            {
                double t = tgt.endTime() + _minBreakpointGapSec;
                Breakpoint null = tgt.parametersAt( t );
                tgt.insert( t, null );
            }
        }
                          
        Partial newp = morphPartial( src, tgt, label );
        if ( partial_is_nonnull( newp ) )
        {
            _partials.push_back( newp );
        }
    }
}


// ---------------------------------------------------------------------------
//    phaseTravel
// ---------------------------------------------------------------------------
//    Compute the sinusoidal phase travel due to sinusoidal oscillation
// beginning at frequency f0 and changing linearly to frequency f1
// over time dt. Return the total unwrapped phase travel.
//
static double  phaseTravel( double f0, double f1, double dt )
{
    double favg = .5 * ( f0 + f1 );
    return 2 * Pi * favg * dt;
}
 
// ---------------------------------------------------------------------------
//    wrapPi
// ---------------------------------------------------------------------------
//    Wrap an unwrapped phase value to the range (-pi,pi].
//
static double wrapPi( double x )
{
    x = std::fmod( x, 2*Pi );
    
    if ( x > Pi )
    {
        x = x - ( 2*Pi );
    }
    else if ( x < -Pi )
    {
        x = x + ( 2*Pi );
    }
    return x;
}

// ---------------------------------------------------------------------------
//    makePartialFromReference
// ---------------------------------------------------------------------------
// Helper function to construct a Partial for morphing by scaling
// the frequencies of the reference Partial. This is used when only
// one of the sources in a morph has a Partial with a particular label.
//
Partial
Morpher::makePartialFromReference( Partial scaleMe, double fscale )
{
   Partial::iterator prev = scaleMe.begin();    
   
   for ( Partial::iterator it = scaleMe.begin(); it != scaleMe.end(); ++it )
    {
        it.breakpoint().setAmplitude( 0 );
        it.breakpoint().setFrequency( it.breakpoint().frequency() * fscale );
        
        if ( it != prev )
        {
           double travel = phaseTravel( prev.breakpoint().frequency(), 
                                        it.breakpoint().frequency(),
                                        it.time() - prev.time() );
           it.breakpoint().setPhase( wrapPi( prev.breakpoint().phase() + travel ) );
        
           prev = it;
        }
    }
    return scaleMe;
}

    
// ---------------------------------------------------------------------------
//    Helper function for computing individual morphed amplitude values
//
static inline double 
interpolateLogAmplitudes( double a0, double a1, double alpha, double shape )
{    
    //    log-amplitude morphing:
    //    it is essential to add in a small Epsilon, so that 
    //    occasional zero amplitudes do not introduce artifacts
    //    (if amp is zero, then even if alpha is very small
    //    the effect is to multiply by zero, because 0^x = 0).
    //
    //    When Epsilon is very small, the curve representing the
    //    morphed amplitude is very steep, such that there is a 
    //    huge difference between zero amplitude and very small
    //    amplitude, and this causes audible artifacts. So instead
    //    use a larger value that shapes the curve more nicely. 
    //    Just have to subtract this value from the morphed 
    //    amplitude to avoid raising the noise floor a whole lot.
    using std::pow;
    static const double Epsilon = 1E-12;
    if ( ( a0 > Epsilon ) || ( a1 > Epsilon ) )
    {
        double newamp = ( pow( a0 + shape, (1.-alpha) ) * 
                              pow( a1 + shape, alpha ) ) - shape;
        return std::max( 0.0, newamp );        
        
    }
    else
    {
        return 0;
    }
}
#if 0

// ---------------------------------------------------------------------------
//    Helper function for computing individual morphed frequency values
//
static inline double 
interpolateFrequencies( double f0, double f1, double alpha )
{
    return (alpha * f1) + ((1.-alpha) * f0);
}

// ---------------------------------------------------------------------------
// Helper function for computing individual morphed bandwidth values.
// Instead of just interpolating the bandwidth envelopes, interpolate
// the amount of noise energy. This makes more intuitive sense.
//
static inline double 
interpolateBandwidths( const Breakpoint & bp0, const Breakpoint & bp1, 
                       double alpha )
{
   double noise0 = bp0.bandwidth() * bp0.amplitude();
   double noise1 = bp1.bandwidth() * bp1.amplitude();
   
   return 0; // (alpha * bw1) + ((1.-alpha) * bw0);
}
    
// ---------------------------------------------------------------------------
//    Helper function for computing individual morphed phase values
//
static inline double 
interpolatePhases( double phi0, double phi1, double alpha )
{
    //    try to wrap the phase so that they are
    //    as similar as possible:
    while ( ( phi0 - phi1 ) > Pi )
    {
        phi0 -= 2 * Pi;
    }
    while ( ( phi1 - phi0 ) > Pi )
    {
        phi0 += 2 * Pi;
    }

    return std::fmod( (alpha * phi1) + ((1.-alpha) * phi0), 2 * Pi );
}
#endif

// ---------------------------------------------------------------------------
//  Helper function for correcting the blandly-interpolated phase to 
//  account for different number of periods traveled over dt. This
//  is probably not the final word on how to do this correctly,
//  but probably we need something like this in order that
//  the simplest cases of morphing work intuitively.
static inline double 
interpolateWholePeriods( double f0, double f1, double dt, double alpha )
{
    double periods0 = 2 * Pi * int(f0 * dt);
    double periods1 = 2 * Pi * int(f1 * dt);
    
    return std::fmod( (alpha * periods1) + ((1.-alpha) * periods0), 2 * Pi );
}


// ---------------------------------------------------------------------------
//    Helper function for interpolating Breakpoint parameters
//
static inline Breakpoint 
interpolateParameters( const Breakpoint & srcBkpt, const Breakpoint & tgtBkpt,
                       double fweight, double aweight, double ashape, 
                       double bweight )
{
   using std::pow;
   using std::sqrt;

   // compute interpolated Breakpoint parameters:
   Breakpoint morphed;

   // interpolate frequencies:
   morphed.setFrequency( ((1.-fweight) *  srcBkpt.frequency()) + 
                         (fweight * tgtBkpt.frequency()) );

   // interpolate LOG amplitudes:
   //    it is essential to add in a small Epsilon, so that 
   //    occasional zero amplitudes do not introduce artifacts
   //    (if amp is zero, then even if alpha is very small
   //    the effect is to multiply by zero, because 0^x = 0).
   //
   //    When Epsilon is very small, the curve representing the
   //    morphed amplitude is very steep, such that there is a 
   //    huge difference between zero amplitude and very small
   //    amplitude, and this causes audible artifacts. So instead
   //    use a larger value that shapes the curve more nicely. 
   //    Just have to subtract this value from the morphed 
   //    amplitude to avoid raising the noise floor a whole lot.
   double morphedAmp = 0;
   static const double Epsilon = 1E-12;
   if ( ( srcBkpt.amplitude() > Epsilon ) || ( tgtBkpt.amplitude() > Epsilon ) )
   {
      morphedAmp = ( pow( srcBkpt.amplitude() + ashape, (1.-aweight) ) * 
                     pow( tgtBkpt.amplitude() + ashape, aweight ) ) - ashape;
      morphedAmp = std::max( 0.0, morphedAmp );  
     
   }
   morphed.setAmplitude( morphedAmp );
   
   // interpolate bandwidth: 
   // Instead of just interpolating the bandwidth envelopes, interpolate
   // the amount of noise energy. This makes more intuitive sense.
   // Well, sort of, but only if you also morph sinusoidal energy instead
   // of amplitude, and then we are really changing the way morphing works.
   // We are no longer interpolating the paramaters of our model at all!
   // Not sure that's what I want.
   //
   // probably morphing bandwidth logarithmically like ampliude is better.
   #if NEW_BW_MORPH
   double morphedBW =  0;
   if ( ( srcBkpt.bandwidth() > Epsilon ) || ( tgtBkpt.bandwidth() > Epsilon ) )
   {
      morphedBW = ( pow( srcBkpt.bandwidth() + ashape, (1.-bweight) ) * 
                     pow( tgtBkpt.bandwidth() + ashape, bweight ) ) - ashape;
      morphedBW = std::max( 0.0, morphedBW );  
     
   }
   #else
   double morphedBW = ((1.-bweight) * srcBkpt.bandwidth() ) + (bweight * tgtBkpt.bandwidth());
   #endif
   morphed.setBandwidth( morphedBW );
   
   // interpolate phase:
   // Interpolate raw absolute phase values. If the interpolated
   // phase matters at all (near the morphing function boudaries 0
   // and 1) then that will give a good target phase value, and the
   // frequency will be adjusted to match the phase. Otherwise,
   // the phase will just be recomputed to match the interpolated
   // frequency.
   //
   // Wrap the computed phase onto an appropriate range.
   // wrap the phases so that they are as similar as possible,
   // so that phase interpolation is shift-invariant.
   double srcphase = srcBkpt.phase();
   while ( ( srcphase - tgtBkpt.phase() ) > Pi )
   {
      srcphase -= 2 * Pi;
   }
   while ( ( tgtBkpt.phase() - srcphase ) > Pi )
   {
      srcphase += 2 * Pi;
   }
   double morphedPhase = ((1.-fweight) * srcphase) + (fweight * tgtBkpt.phase());
   morphed.setPhase( std::fmod( morphedPhase, 2 * Pi ) );

   return morphed;
}

// ---------------------------------------------------------------------------
//    fixPhaseTravel
// ---------------------------------------------------------------------------
// Helper function to update the frequency and phase of a morphed Breakpoint
// to achieve a desired weighted interpolation of phase travel. The desired
// travel is computed as a weighted function of the phase travel that would
// exactly match the phase of bp1 and the travel that would exactly match the 
// frequencies of the two breakpoints. These two values of the phase travel
// are weighted by the magnitude of the difference between the value of the
// frequency morph function and one half, so that a a halfway morph uses
// only the frequencies to determine the desired phase travel and a morph
// near one of the boundaries (1 or 0) relies only on the interpolated absolute
// phase to determine the desired phase travel. The frequency of bp1 is updated 
// according to this morphed phase travel, but the frequency correction is 
// clamped to at most MaxFixPct (0.2%). Finally, the phase of bp1 is updated to 
// match the phase travel between bp0 and bp1 with the updated frequency.
//
// bp0 is the Breakpoint that is already part of the morphed Partial
// bp1 is the new morphed Breakpoint that is being computed
// dt is the time between the two Breakpoints
// alpha is the value of the frequency morphing function. 
//
// It doesn't matter whether the new Breakpoint is derived from a 
// source or target Partial (0 or 1 value of morph function) since the 
// only use of alpha is a measure of the magnitude of the difference 
// from 0.5 (a halfway morph).
// 
static void
fixPhaseTravel( const Breakpoint & bp0, Breakpoint & bp1, double dt, double alpha )
{   
    // cannot do anything reasonable if dt == 0, since
    // there is no phase travel, should never happen:
    Assert( dt > 0 ); 
    alpha = std::max( 0., alpha );   
    alpha = std::min( 1., alpha );

    // compute the sinusoidal phase travel between the two Breakpoints:
    double travel = phaseTravel( bp0.frequency(), bp1.frequency(), dt ); 

    // Compute a morphed phase travel by weighting the error
    // correct by the distance from the nearest source, so that
    // near halfway (alpha == .5) we rely on the frequencies
    // to compute the morphed phase travel, and near the boundaries
    // of the morph (alpha near 1 or 0) we rely on the interpolated
    // absolute phases. Only try to match phase near the boundaries.
    static const double PHASE_MATCH_RANGE = 0.2;
    if ( std::fabs( 0.5 - alpha ) > ( 0.5 - PHASE_MATCH_RANGE ) )
    {
        double errorWeight = ( std::fabs( 0.5 - alpha ) - ( 0.5 - PHASE_MATCH_RANGE ) ) / PHASE_MATCH_RANGE;
   
        // compute the error between the predicted (due to phase travel) phase
        // and the phase of the target Breakpoint:
        double err = wrapPi( bp1.phase() - ( bp0.phase() + travel ) );

        // only try to correct HALF of the error, to avoid frequency oscillations
        travel += .5 * errorWeight * err;         

        // compute a new frequency for bp1 from the morphed
        // phase travel:
        double ftgt = ( travel / ( Pi * dt ) ) - bp0.frequency();

        //    the maximum amount by which frequency can be changed to 
        //    attempt to achieve correct phases:
        static const double MaxFixPct = .2;

        //    if bp1 is not a null breakpoint, may need to 
        //    clamp the amount of frequency correction:
        if ( bp1.amplitude() != 0. )
        {    
           ftgt = std::min( ftgt, bp1.frequency() * ( 1 + (MaxFixPct*.01) ) );
           ftgt = std::max( ftgt, bp1.frequency() * ( 1 - (MaxFixPct*.01) ) );
        }

        // update the morphed frequency to achieve the desired
        // morphed phase travel:
        bp1.setFrequency( ftgt );
    }
    
    //  recompute the phase travel according to the  
    //  frequency of bp1:
    travel = phaseTravel( bp0.frequency(), bp1.frequency(), dt );

    // update the phase of bp1 to be correct according
    // to the morphed frequencies, updated to achieve
    // the desired morphed phase travel: 
    bp1.setPhase( wrapPi( bp0.phase() + travel ) );
}

// ---------------------------------------------------------------------------
//    appendMorphedSrc
// ---------------------------------------------------------------------------
//! Compute morphed parameter values at the specified time, using
//! the source Breakpoint (assumed to correspond exactly to the
//! specified time) and the target Partial (whose parameters are
//! examined at the specified time). Append the morphed Breakpoint
//! to newp only if the source should contribute to the morph at
//! the specified time.
//!
//! If the target Partial is a dummy Partial (no Breakpoints), fade the
//! source instead of morphing.
//!
//! \param  srcBkpt is the Breakpoint corresponding to a morph function
//!         value of 0.
//! \param  tgtPartial is the Partial corresponding to a morph function
//!         value of 1, evaluated at the specified time.
//! \param  time is the time corresponding to srcBkpt (used
//!         to evaluate the morphing functions and tgtPartial).
//! \param  newp is the morphed Partial under construction, the morphed
//!         Breakpoint is added to this Partial.
//
void
Morpher::appendMorphedSrc( Breakpoint srcBkpt, const Partial & tgtPartial, 
                           double time, Partial & newp  )
{
    double fweight = _freqFunction->valueAt( time );
    double aweight = _ampFunction->valueAt( time );
    double bweight = _bwFunction->valueAt( time );
    
    //    Don't insert Breakpoints at src times if all 
    //    morph functions equal 1 (or > MaxMorphParam).
    const double MaxMorphParam = .9;
    if ( fweight < MaxMorphParam ||
         aweight < MaxMorphParam ||
         bweight < MaxMorphParam )
    {
        if ( 0 == tgtPartial.numBreakpoints() )
        {
            // hey, can this ever happen?
            // only if no tgt reference is used
            if ( 0 == _tgtRefPartial.numBreakpoints() )
            {
                newp.insert( time, fadeSrcBreakpoint( srcBkpt, time ) );
            }
           else
           {
//////////////////////////
#if 0
                //  THIS IS NEW AND UNTESTED, AND WON'T EVER 
                //  HAPPEN AS LONG AS MORPH_AUX IS CONSTRUCTING
                //  FAKE PARTIALS FROM THE REFERENCE PARTIAL
                Breakpoint tgtBkpt = _tgtRefPartial.parametersAt( time );
                tgtBkpt.setFrequency( newp.label() * tgtBkpt.frequency() ); // label had better be right!
                tgtBkpt.setAmplitude( 0 );
                tgtBkpt.setBandwidth( 0 );
                // no idea what phase should be! How about same as source????????
                tgtBkpt.setPhase( srcBkpt.phase() );

                
                adjustFrequency( srcBkpt, _srcRefPartial, newp.label(), _freqFixThresholdDb, time );
                
                
                // compute interpolated Breakpoint parameters:
                Breakpoint morphed = interpolateParameters( srcBkpt, tgtBkpt, fweight, 
                                                            aweight, _ampMorphShape, bweight );

    #if MORPH_PHASE_TRAVEL
                // correct phase travel:
                if ( 0 != newp.numBreakpoints() )
                {
                    double dt = ( time - newp.endTime() );
                    fixPhaseTravel( newp.last(), morphed, dt, fweight );
                }
    #endif
     
                newp.insert( time, morphed );
#endif            
////////////////////////////
            }
        }    
        else
        {
            Breakpoint tgtBkpt = tgtPartial.parametersAt( time );
            
            // adjust Breakpoint frequencies according to the reference
            // Partial (if a reference has been specified):
            adjustFrequency( srcBkpt, _srcRefPartial, newp.label(), _freqFixThresholdDb, time );
            adjustFrequency( tgtBkpt, _tgtRefPartial, newp.label(), _freqFixThresholdDb, time );
            
            // compute interpolated Breakpoint parameters:
            Breakpoint morphed = interpolateParameters( srcBkpt, tgtBkpt, fweight, 
                                                        aweight, _ampMorphShape, bweight );

#if MORPH_PHASE_TRAVEL
            // correct phase travel:
            // (This is identical in the function below.)
            //
            //  HEY
            //  maybe should toss out the interpolated phase and
            //  replace it with the nearest source phase before
            //  doing this fix?
            //
            //  Probably that makes no sense either.
            if ( 0 != newp.numBreakpoints() )
            {
                /*
                if ( fweight < .5 )
                {
                    morphed.setPhase( srcBkpt.phase() );
                }
                else
                {
                    morphed.setPhase( tgtBkpt.phase() );
                }
                */
                double dt = ( time - newp.endTime() );
                fixPhaseTravel( newp.last(), morphed, dt, fweight );
            }
#endif
 
            newp.insert( time, morphed );
        }
    }
}

// ---------------------------------------------------------------------------
//    appendMorphedTgt
// ---------------------------------------------------------------------------
//! Compute morphed parameter values at the specified time, using
//! the target Breakpoint (assumed to correspond exactly to the
//! specified time) and the source Partial (whose parameters are
//! examined at the specified time). Append the morphed Breakpoint
//! to newp only if the target should contribute to the morph at
//! the specified time.
//!
//! If the source Partial is a dummy Partial (no Breakpoints), fade the
//! target instead of morphing.
//!
//! \param  tgtBkpt is the Breakpoint corresponding to a morph function
//!         value of 1.
//! \param  srcPartial is the Partial corresponding to a morph function
//!         value of 0, evaluated at the specified time.
//! \param  time is the time corresponding to srcBkpt (used
//!         to evaluate the morphing functions and srcPartial).
//! \param  newp is the morphed Partial under construction, the morphed
//!         Breakpoint is added to this Partial.
//
void
Morpher::appendMorphedTgt( Breakpoint tgtBkpt, const Partial & srcPartial, 
                           double time, Partial & newp  )
{    
    double fweight = _freqFunction->valueAt( time );
    double aweight = _ampFunction->valueAt( time );
    double bweight = _bwFunction->valueAt( time );

    //    Don't insert Breakpoints at src times if all 
    //    morph functions equal 0 (or < MinMorphParam).
    const double MinMorphParam = .1;
    if ( fweight > MinMorphParam ||
         aweight > MinMorphParam ||
         bweight > MinMorphParam )
    {
        if ( 0 == srcPartial.numBreakpoints() )
        {
            // hey, can this ever happen?
            // only if no src reference is used.
            newp.insert( time, fadeTgtBreakpoint( tgtBkpt, time ) );
        }
        else
        {
            Breakpoint srcBkpt = srcPartial.parametersAt( time );

            // adjust Breakpoint frequencies according to the reference
            // Partial (if a reference has been specified):
            adjustFrequency( srcBkpt, _srcRefPartial, newp.label(), _freqFixThresholdDb, time );
            adjustFrequency( tgtBkpt, _tgtRefPartial, newp.label(), _freqFixThresholdDb, time );

            // compute interpolated Breakpoint parameters:           
            Breakpoint morphed = interpolateParameters( srcBkpt, tgtBkpt, fweight, 
                                                        aweight, _ampMorphShape, bweight );

#if MORPH_PHASE_TRAVEL
            // correct phase travel:
            // (This is identical in the function above.)
            //  HEY
            //  maybe should toss out the interpolated phase and
            //  replace it with the nearest source phase before
            //  doing this fix?
            //
            //  Probably that makes no sense either.
            if ( 0 != newp.numBreakpoints() )
            {
                /*
                if ( fweight < .5 )
                {
                    morphed.setPhase( srcBkpt.phase() );
                }
                else
                {
                    morphed.setPhase( tgtBkpt.phase() );
                }
                */
                double dt = ( time - newp.endTime() );
                fixPhaseTravel( newp.last(), morphed, dt, fweight );
            }
#endif

            newp.insert( time, morphed );
        }
    }
}

}    //    end of namespace Loris

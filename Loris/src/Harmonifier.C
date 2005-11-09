/* 
	Harmonifier.C

	Author:			
	Description:	<describe the Harmonifier class here>
*/

#include "Harmonifier.h"

#include "LinearEnvelope.h"

#include <cmath>    // for pow

using namespace Loris; 


// ---------------------------------------------------------------------------
//    Constructor
// ---------------------------------------------------------------------------
//! Construct a new Harmonifier that applies the specified 
//! reference Partial to fix the frequencies of Breakpoints
//! whose amplitude is below threshold_dB (0 by default,
//! to apply only to quiet Partials, specify a threshold,
//! like -90). 
//
Harmonifier::Harmonifier( const Partial & ref, double threshold_dB ) :
    _refPartial( ref ),
    _freqFixThresholdDb( threshold_dB ),
    _weight( createDefaultEnvelope() )
{
    if ( 0 == _refPartial.numBreakpoints() )
    {
        Throw( InvalidArgument, 
               "Cannot use an empty reference Partial in Harmonizer" );
    }
    if ( 0 == _refPartial.label() )
    {
        //  if the reference is unlabeled, assume that it is the fundamental
        _refPartial.setLabel( 1 );
    }
    
}

// ---------------------------------------------------------------------------
//    Constructor
// ---------------------------------------------------------------------------
//! Construct a new Harmonifier that applies the specified 
//! reference Partial to fix the frequencies of Breakpoints
//! whose amplitude is below threshold_dB (0 by default,
//! to apply only to quiet Partials, specify a threshold,
//! like -90). The Envelope is a time-varying weighting 
//! on the harmonifing process. When 1, harmonic frequencies
//! are used, when 0, breakpoint frequencies are unmodified. 
//
Harmonifier::Harmonifier( const Partial & ref, const Envelope & env, 
                          double threshold_dB ) :
    _refPartial( ref ),
    _freqFixThresholdDb( threshold_dB ),
    _weight( env.clone() )
{
    if ( 0 == _refPartial.numBreakpoints() )
    {
        Throw( InvalidArgument, 
               "Cannot use an empty reference Partial in Harmonizer" );
    }
    if ( 0 == _refPartial.label() )
    {
        //  if the reference is unlabeled, assume that it is the fundamental
        _refPartial.setLabel( 1 );
    }
    
}

// ---------------------------------------------------------------------------
//    Destructor
// ---------------------------------------------------------------------------
Harmonifier::~Harmonifier( void )
{
}

// ---------------------------------------------------------------------------
//    harmonify
// ---------------------------------------------------------------------------
//! Apply the reference envelope to a Partial.
//!
//! \pre    The Partial p must be labeled with its harmonic number.
//
void Harmonifier::harmonify( Partial & p ) const
{
    //    compute absolute magnitude thresholds:
    static const double FadeRangeDB = 10;
    const double BeginFade = std::pow( 10., 0.05 * (_freqFixThresholdDb+FadeRangeDB) );
    const double Threshold = std::pow( 10., 0.05 * _freqFixThresholdDb );
    const double OneOverFadeSpan = 1. / ( BeginFade - Threshold );

    double fscale = (double)p.label() / _refPartial.label();
    
    for ( Partial::iterator it = p.begin(); it != p.end(); ++it )
    {
        Breakpoint & bp = it.breakpoint();            
                
        if ( bp.amplitude() < BeginFade )
        {
            //  alpha is the harmonic frequency weighting:
            //  when alpha is 1, the harmonic frequency is used,
            //  when alpha is 0, the breakpoint frequency is
            //  unmodified.
            double alpha = 
                std::min( ( BeginFade - bp.amplitude() ) * OneOverFadeSpan, 1. );
                
            //  alpha is scaled by the weigthing envelope
            alpha *= _weight->valueAt( it.time() );
            
            double fRef = _refPartial.frequencyAt( it.time() );
            
            bp.setFrequency( ( alpha * ( fRef * fscale ) ) + 
                             ( (1 - alpha) * bp.frequency() ) );
        }

    }
}

// ---------------------------------------------------------------------------
//    createDefaultEnvelope (STATIC)
// ---------------------------------------------------------------------------
//! Return the default weighing envelope (always 1).
//! Used in template constructors.
//
Envelope * Harmonifier::createDefaultEnvelope( void )
{
    return new LinearEnvelope( 1 );
}    


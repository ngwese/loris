// ===========================================================================
//	Synthesizer.C
//	
//	Implementation of Loris::Synthesizer.
//	
//	Loris synthesis generates a buffer of samples from a 
//	collection of Partials. 
//
//	-kel 16 Aug 99
//
// ===========================================================================

#include "LorisLib.h"

#include "Synthesizer.h"
#include "Exception.h"
#include "SampleBuffer.h"
#include "Oscillator.h"
#include "Partial.h"
#include "Breakpoint.h"

Begin_Namespace( Loris )

#pragma mark -
#pragma mark construction

// ---------------------------------------------------------------------------
//	Synthesizer constructor
// ---------------------------------------------------------------------------
//	If no Oscillator (or a Null Oscillator) is specified, create a default
//	Oscillator.
//
Synthesizer::Synthesizer( SampleBuffer & buf, Double srate, Oscillator * osc ) :
	mSamples( buf ),
	mSampleRate( srate ),
	mOscillator( (osc != Null) ? (osc) : (Oscillator::Create()) )
{
	//	check to make sure that the sample rate is valid:
	Assert( mSampleRate > 0. );
}

// ---------------------------------------------------------------------------
//	Synthesizer destructor
// ---------------------------------------------------------------------------
//
Synthesizer::~Synthesizer( void )
{
	delete mOscillator;
}

#pragma mark -
#pragma mark synthesis

// ---------------------------------------------------------------------------
//	synthesizePartial
// ---------------------------------------------------------------------------
//	
void
Synthesizer::synthesizePartial( const Partial & p )
{
	if ( p.duration() == 0. )
		return;
		
//	reset the oscillator:
//	Remember that the oscillator only knows about radian frequency! Convert!
	const Breakpoint * bp = p.head();
	Double rads = radianFreq( bp->frequency() );
	mOscillator->reset( rads, 0., bp->bandwidth(), bp->phase() );

//	initialize sample offsets:
//	Keep track of fractional samples, so we don't loose samples 
//	due to rounding; fractionalSamples is the (fractional) number 
//	of samples that need to be generated.
	Uint bpSampleOffset = bp->time() * sampleRate() + 0.5;	// cheap, portable rounding

//	synthesize Partial turn-on if necessary:
	const Int rampLen = 0.001 * sampleRate() + 0.5;	//	1 ms
	
	if ( bp->amplitude() > 0. ) {
		//	roll back the phase so that it is correct
		//	at the time of the first real Breakpoint:
		mOscillator->setPhase( mOscillator->phase() - ( rads * rampLen ) );
		
		//	generate rampLen samples before bpSampleOffset
		//	targeting the radian frequency, amplitude, and 
		//	bandwidth of the first real Breakpoint (the 
		//	frequency and bandwidth have been reset to
		//	these same values already, the amplitude to 
		//	zero):
		mOscillator->generateSamples( mSamples, rampLen, bpSampleOffset - rampLen,
									  rads, bp->amplitude(), bp->bandwidth() );
	}

//	synthesize linear-frequency segments:
	for ( bp = bp->next(); bp != Null; bp = bp->next() ) {
		//	compute the number of samples to generate:
		//	By computing each Breakpoint offset this way, 
		//	(instead of computing nsamps from the time difference
		//	between consecutive envelope breakpoints) we
		//	obviate the running fractional sample total we
		//	used to need.
		Int nsamps = ( bp->time() * sampleRate() ) - bpSampleOffset + 0.5;
		Assert( nsamps >= 0 );
		
		//	generate nsamps samples starting at bpSampleOffset
		//	targeting the radian frequency, amplitude, and 
		//	bandwidth of the next Breakpoint bp:
		mOscillator->generateSamples( mSamples, nsamps, bpSampleOffset,
									  radianFreq( bp->frequency() ), 
									  bp->amplitude(), bp->bandwidth() );

		//	update the offset:	
		bpSampleOffset += nsamps;
	}
	
//	synthesize Partial turn-off if necessary:
	if ( bp->amplitude() > 0. ) {
		//	generate rampLen samples starting at bpSampleOffset
		//	targeting zero amplitude, and not changing the frequency
		//	or bandwidth:
		mOscillator->generateSamples( mSamples, rampLen, bpSampleOffset,
									  mOscillator->radianFreq(), 0., mOscillator->bandwidth() );
	}


}

#pragma mark -
#pragma mark synthesis

// ---------------------------------------------------------------------------
//	synthesizePartial
// ---------------------------------------------------------------------------
//	
inline Double 
Synthesizer::radianFreq( Double hz ) const
{
	return hz * TwoPi / sampleRate();
}

End_Namespace( Loris )

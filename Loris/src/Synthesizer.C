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
#include "PartialIterator.h"

Begin_Namespace( Loris )

#pragma mark -
#pragma mark construction

// ---------------------------------------------------------------------------
//	Synthesizer constructor
// ---------------------------------------------------------------------------
//	If no Oscillator (or a Null Oscillator) is specified, create a default
//	Oscillator.
//
Synthesizer::Synthesizer( SampleBuffer & buf, double srate ) :
	_samples( buf ),
	_sampleRate( srate ),
	_oscillator( Null ),
	_iterator( Null )
{
	//	check to make sure that the sample rate is valid:
	Assert( _sampleRate > 0. );
	
	//	initialize these:
	setOscillator();
	setIterator();
}

// ---------------------------------------------------------------------------
//	Synthesizer destructor
// ---------------------------------------------------------------------------
//
Synthesizer::~Synthesizer( void )
{
	delete _oscillator;
	delete _iterator;
}

// ---------------------------------------------------------------------------
//	setIterator
// ---------------------------------------------------------------------------
//	Default osc is Null, indicating that the default oscillator should be 
//	used.
//
void
Synthesizer::setOscillator( Oscillator * osc )
{	
	if ( osc == Null )
		osc = new Oscillator();

	delete _oscillator;
	_oscillator = osc;
}	

// ---------------------------------------------------------------------------
//	setIterator
// ---------------------------------------------------------------------------
//	Default iter is Null, indicating that the default iterator should be 
//	used.
//
void
Synthesizer::setIterator( PartialIterator * iter )
{	
	if ( iter == Null )
		iter = new PartialIterator();

	delete _iterator;
	_iterator = iter;
}	

#pragma mark -
#pragma mark synthesis
// ---------------------------------------------------------------------------
//	synthesizePartial
// ---------------------------------------------------------------------------
//	No clicks aloud!
//	All Partials begin and end at zero amplitude. If possible, the ramp
//	on or off is pasted on the beginning and end of the Partial, but if 
//	the Partial is too close to the buffer boundary (e.g. it starts at time
//	0.), then the head or tail of the Partial gets smooshed a little. Phase
//	is always corrected, and is never altered by the ramping. 
//	
void
Synthesizer::synthesizePartial( const Partial & p )
{
	if ( p.duration() == 0. )
		return;
		
//	reset the oscillator:
//	Remember that the oscillator only knows about radian frequency! Convert!
	_iterator->reset( p );
	double rads = radianFreq( _iterator->frequency() );
	_oscillator->reset( rads, _iterator->amplitude(), _iterator->bandwidth(), _iterator->phase() );

//	initialize sample offsets:
	ulong bpSampleOffset = _iterator->time() * sampleRate() + 0.5;	// cheap, portable rounding

//	synthesize Partial turn-on if necessary and possible:
	const int rampLen = 0.001 * sampleRate() + 0.5;	//	1 ms
	
	if ( _iterator->amplitude() > 0. ) {
		//	calculate the start time for the ramp, 
		int rampStart = bpSampleOffset - rampLen;
		if ( rampStart < 0 )
			rampStart = 0;
		
		//	roll back the phase so that it is correct
		//	at the time of the first real Breakpoint
		//	(not nec. the time that the top of the amplitude
		//	ramp is reached, if the Partial starts very 
		//	early):
		_oscillator->setPhase( _oscillator->phase() - ( rads * (bpSampleOffset - rampStart) ) );
		
		//	recalculate the bpSampleOffset if necessary:
		if ( bpSampleOffset < rampLen )
			bpSampleOffset = rampLen;
		
		//	ramp up from zero amplitude:
		_oscillator->setAmplitude( 0. );
		
		//	generate rampLen samples before bpSampleOffset
		//	targeting the radian frequency, amplitude, and 
		//	bandwidth of the first real Breakpoint (the 
		//	frequency and bandwidth have been reset to
		//	these same values already, the amplitude to 
		//	zero):
		_oscillator->generateSamples( _samples, rampLen, rampStart, rads,
									_iterator->amplitude( ), _iterator->bandwidth() );
	}

//	synthesize linear-frequency segments:
	//for ( bp = bp->next(); bp != Null; bp = bp->next() ) {
	for ( _iterator->advance(); ! _iterator->atEnd(); _iterator->advance() ) {
		//	compute the number of samples to generate:
		//	By computing each Breakpoint offset this way, 
		//	(instead of computing nsamps from the time difference
		//	between consecutive envelope breakpoints) we
		//	obviate the running fractional sample total we
		//	used to need.
		int nsamps = ( _iterator->time() * sampleRate() ) - bpSampleOffset + 0.5;
		Assert( nsamps >= 0 );
		
		//	don't generate samples all the way to the end of the
		//	buffer, make sure to leave enough for ramping off:
		if ( bpSampleOffset + nsamps + rampLen > _samples.size() )
			nsamps = _samples.size() - bpSampleOffset - rampLen;
		
		//	generate nsamps samples starting at bpSampleOffset
		//	targeting the radian frequency, amplitude, and 
		//	bandwidth of the next Breakpoint bp:
		_oscillator->generateSamples( _samples, nsamps, bpSampleOffset,
									  radianFreq( _iterator->frequency() ), 
									  _iterator->amplitude( ), _iterator->bandwidth() );

		//	update the offset:	
		bpSampleOffset += nsamps;
	}
	
//	synthesize Partial turn-off if necessary:
	if ( _oscillator->amplitude() > 0. ) {
		//	generate rampLen samples starting at bpSampleOffset
		//	targeting zero amplitude, and not changing the frequency
		//	or bandwidth:
		_oscillator->generateSamples( _samples, rampLen, bpSampleOffset,
									  _oscillator->radianFreq(), 0., _oscillator->bandwidth() );
	}


}

// ---------------------------------------------------------------------------
//	radianFreq
// ---------------------------------------------------------------------------
//	
inline double 
Synthesizer::radianFreq( double hz ) const
{
	return hz * TwoPi / sampleRate();
}

End_Namespace( Loris )

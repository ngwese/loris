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
#include "Oscillator.h"
#include "Partial.h"
#include "PartialIterator.h"
#include "Notifier.h"

#include <algorithm>
#include <vector>
using namespace std;

Begin_Namespace( Loris )

#pragma mark -
#pragma mark construction

// ---------------------------------------------------------------------------
//	Synthesizer constructor
// ---------------------------------------------------------------------------
//	Default offset and fade are 1ms, so that Partials beginning at time 
//	zero (like all Lemur 5 analyses) have time to ramp in.
//
//	Default osc is an auto_ptr with no reference, indicating that a default 
//	Oscillator should be created and used.
//
//	auto_ptr is used to submit the Oscillator argument to make explicit the
//	source/sink relationship between the caller and the Synthesizer. After
//	the call, the Synthesizer will own the Oscillator, and the client's auto_ptr
//	will have no reference (or ownership).
//
Synthesizer::Synthesizer( vector< double > & buf, double srate, 
						  auto_ptr< Oscillator > osc ) :
	_sampleRate( srate ),
	_offset( 0.001 ),
	_fadeTime( 0.001 ),
	_samples( buf )
{
	//	check to make sure that the sample rate is valid:
	if ( _sampleRate <= 0. ) {
		Throw( InvalidObject, "Synthesizer sample rate must be positive." );
	}
	
	//	initialize these:
	setOscillator( osc );
	setIterator();
}

// ---------------------------------------------------------------------------
//	Synthesizer copy constructor
// ---------------------------------------------------------------------------
//	This will need to be changed is Oscillator ever becomes a base class.
//
Synthesizer::Synthesizer( const Synthesizer & other ) :
	_sampleRate( other._sampleRate ),
	_offset( other._offset ),
	_fadeTime( other._fadeTime ),
	_samples( other._samples ),
	_oscillator( new Oscillator( * other._oscillator ) ),
	_iterator( other._iterator->clone() )
{
}

// ---------------------------------------------------------------------------
//	assignment operator
// ---------------------------------------------------------------------------
//	For best behavior, if any part of assignment fails, the object
//	should be unmodified. To that end, the iterator is cloned 
//	first, since that could potentially generate a low memory
//	exception, then all the assignments are made, and they cannot 
//	generate exceptions.
//
Synthesizer & 
Synthesizer::operator=( const Synthesizer & other )
{
	//	do nothing if assigning to self:
	if ( &other != this ) {	
		//	first do cloning:
		auto_ptr< Oscillator > osc( new Oscillator( * other._oscillator ) );
		auto_ptr< PartialIterator > it( other._iterator->clone() );

		//	try to reserve enough space to copy the 
		//	sample vector before attempting the copy:
		_samples.reserve( other._samples.size() );
				
		_sampleRate = other._sampleRate;
		_offset = other._offset;
		_fadeTime = other._fadeTime;
		_samples = other._samples;
		_oscillator = osc;
		_iterator = it;
		
	}
	
	return *this;
}

// ---------------------------------------------------------------------------
//	setOscillator
// ---------------------------------------------------------------------------
//	Default osc is an auto_ptr with no reference, indicating that a default 
//	Oscillator should be created and used.
//
//	auto_ptr is used to submit the Oscillator argument to make explicit
//	the source/sink relationship between the caller and the Synthesizer. After
//	the call, the Synthesizer will own the Oscillator, and the client's 
//	auto_ptr will have no reference (or ownership).
//
//
void
Synthesizer::setOscillator( auto_ptr< Oscillator > osc )
{	
	if ( ! osc.get() )
		osc.reset( new Oscillator() );

	_oscillator = osc;
}	

// ---------------------------------------------------------------------------
//	setIterator
// ---------------------------------------------------------------------------
//	Default iter is an auto_ptr with no reference, indicating that a default 
//	PartialIterator should be created and used.
//
//	auto_ptr is used to submit the PartialIterator argument to make explicit
//	the source/sink relationship between the caller and the Synthesizer. After
//	the call, the Synthesizer will own the PartialIterator, and the client's 
//	auto_ptr will have no reference (or ownership).
//
void
Synthesizer::setIterator( auto_ptr< PartialIterator > iter )
{	
	if ( ! iter.get() )
		iter.reset( new PartialIterator() );
	
	_iterator = iter;
}	

#pragma mark -
#pragma mark synthesis
// ---------------------------------------------------------------------------
//	synthesizePartial
// ---------------------------------------------------------------------------
//	Try to prevent Partial turnon/turnoff artifacts (clicks) by ramping 
//	Partials up from and down to zero amplitude. If possible, the ramp
//	on or off is pasted on the beginning and end of the Partial, but if 
//	the Partial is too close to the buffer boundary (e.g. it starts at time
//	0.), then the ramp at the head or tail of the Partial gets compressed. 
//	Phase is always corrected, and is never altered by the ramping. Samples
//	are never written outside the buffer boundaries.
//
//	A long Partial may generate samples at non-zero amplitude all the way 
//	to the end of the buffer. There's no guarantee that there won't be
//	a click at the end. Yet.
//	
void
Synthesizer::synthesizePartial( const Partial & p )
{
//	don't synthesize Partials having zero duration:
	if ( p.duration() == 0. )
		return;

/*
//	HEY do something better about this.
	//Assert( p.endTime() < _samples.size() / sampleRate() );
	if ( p.endTime() > _samples.size() / sampleRate() ) {
		debugger << "found Partial ending at " << p.endTime() 
					<< " but I only have a buffer of length " << _samples.size();
		debugger << " Having a go of it anyway." << endl;
		//return;
	}
*/
		
//	reset the oscillator:
//	Remember that the oscillator only knows about radian frequency! Convert!
	iterator().reset( p );
	_oscillator->reset( radianFreq( iterator().frequency() ), iterator().amplitude(), 
						iterator().bandwidth(), iterator().phase() );

//	initialize sample offset:
	long bpSampleOffset = (iterator().time() + offset()) * sampleRate();

//	synthesize Partial turn-on if necessary and possible;
	if ( iterator().amplitude() > 0. ) {
		synthesizeFadeIn( bpSampleOffset );
	}

//	synthesize linear-frequency segments until there aren't any more:
	for ( iterator().advance(); ! iterator().atEnd(); iterator().advance() ) {
		bpSampleOffset = synthesizeEnvelopeSegment( bpSampleOffset );
		
		if ( bpSampleOffset >= _samples.size() )
			break;
	}

//	synthesize Partial turn-off if necessary:
	if ( _oscillator->amplitude() > 0. ) {
		bpSampleOffset = synthesizeFadeOut( bpSampleOffset );
	}
}

// ---------------------------------------------------------------------------
//	synthesizeEnvelopeSegment
// ---------------------------------------------------------------------------
//	Synthesize a Partial envelope segment.
//	Return the new currentSampleOffset.
//
inline long
Synthesizer::synthesizeEnvelopeSegment( long currentSampleOffset )
{
	if ( currentSampleOffset < _samples.size() ) {
		//	compute the number of samples to generate:
		//	By computing each Breakpoint offset this way, 
		//	(instead of computing nsamps from the time difference
		//	between consecutive envelope breakpoints) we
		//	obviate the running fractional sample total we
		//	used to need.
		long nsamps = ((iterator().time() + offset()) * sampleRate()) - currentSampleOffset;
		
		//	Don't synthesize samples past the end of the buffer.
		nsamps = min( nsamps, long(_samples.size()) - currentSampleOffset );
		
		//	check sanity:
		Assert( nsamps >= 0 );
		Assert( nsamps + currentSampleOffset <= _samples.size() );
		
		//	generate nsamps samples starting at currentSampleOffset
		//	targeting the radian frequency, amplitude, and 
		//	bandwidth of the current Breakpoint:
		_oscillator->generateSamples( _samples, nsamps, currentSampleOffset,
									  radianFreq( iterator().frequency() ), 
									  iterator().amplitude(), iterator().bandwidth() );

		//	update the offset:	
		currentSampleOffset += nsamps;
	}
	
	return currentSampleOffset;
}

// ---------------------------------------------------------------------------
//	synthesizeFadeIn
// ---------------------------------------------------------------------------
//	Synthesize Partial turn-on if necessary and possible.
//	Ramp up Partials starting after time 0.
//	
inline long
Synthesizer::synthesizeFadeIn( long currentSampleOffset )
{
	const long rampLen = _fadeTime * sampleRate() /* + 0.5 */;
	
	if ( currentSampleOffset > 0 ) {
		//	calculate the start time for the ramp, 
		long rampStart = max( currentSampleOffset - rampLen, 0L );
		
		//	Remember that the oscillator only knows about radian frequency! Convert!
		double rads = radianFreq( iterator().frequency() );
		
		//	roll back the phase so that it is correct
		//	at the time of the first real Breakpoint
		//	(not nec. the time that the top of the amplitude
		//	ramp is reached, if the Partial starts very 
		//	early):
		_oscillator->setPhase( _oscillator->phase() - 
								(rads * (currentSampleOffset - rampStart)) );
		
		//	ramp up from zero amplitude:
		_oscillator->setAmplitude( 0. );
		
		//	generate samples before currentSampleOffset
		//	targeting the radian frequency, amplitude, and 
		//	bandwidth of the first real Breakpoint (the 
		//	frequency and bandwidth have been reset to
		//	these same values already, the amplitude to 
		//	zero):
		_oscillator->generateSamples( _samples, currentSampleOffset - rampStart, 
										rampStart, rads, iterator().amplitude(), 
										iterator().bandwidth() );
	}
	
	return currentSampleOffset;
}

// ---------------------------------------------------------------------------
//	synthesizeFadeOut
// ---------------------------------------------------------------------------
//	Synthesize Partial turn-off if necessary and possible.
//	Ramp down Partials ending before the end of the buffer.
//	Return the new currentSampleOffset.
//	
inline long
Synthesizer::synthesizeFadeOut( long currentSampleOffset )
{
	const long rampLen = _fadeTime * sampleRate() /* + 0.5 */;
	
	if ( currentSampleOffset < _samples.size() - 1 ) {
		//	make sure the ramp doesn't run off the end of the buffer:
		long rampEnd  = min( currentSampleOffset + rampLen, long(_samples.size()) - 1 );

		//	generate samples starting at currentSampleOffset
		//	targeting zero amplitude, and not changing the frequency
		//	or bandwidth:
		_oscillator->generateSamples( _samples, rampEnd - currentSampleOffset, 
									 	currentSampleOffset, _oscillator->radianFreq(), 
										0., _oscillator->bandwidth() );
		
		//	update currentSampleOffset:								
		currentSampleOffset = rampEnd;
	}

	return currentSampleOffset;
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

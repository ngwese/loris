#ifndef __Loris_synthesizer__
#define __Loris_synthesizer__

// ===========================================================================
//	Synthesizer.h
//	
//	Class definition for Loris::Synthesizer.
//	
//	Loris synthesis generates a buffer of samples from a 
//	collection of Partials. 
//
//	-kel 16 Aug 99
//
// ===========================================================================

#include "LorisLib.h"
//#include "Oscillator.h"
#include "PartialIterator.h"
#include <vector>
//#include <memory> 	//	for auto_ptr

Begin_Namespace( Loris )

class Partial;

// ---------------------------------------------------------------------------
//	class Synthesizer
//	
//	Definition of class of synthesizers of reassigned bandwidth-enhanced
//	partials. A Synthesizer generates samples for one partial at a time
//	at a specified sample rate into a specified sample buffer using an
//	Oscillator.
//
//	The Synthesizer does not own its sample vector, the client 
//	is responsible for its construction and destruction. 
//
//	Non-standard Oscillator and PartialIterator may be assigned,
// 	and are then owned by the Synthesizer, that is, they will be 
//	destroyed with the Synthesizer.
//
//	auto_ptr is used to pass some objects as arguments to make explicit
//	the source/sink relationship between the caller and the Synthesizer.
//	Synthesizer assumes ownership, and the client's auto_ptr
//	will have no reference (or ownership).
//
class Synthesizer : public PartialIteratorOwner
{
//	-- public interface --
public:
//	construction:
	Synthesizer( std::vector< double > & buf, double srate );
	Synthesizer( const Synthesizer & other );
	
	//~Synthesizer( void );	//	use compiler-generated destructor
	
	Synthesizer & operator= ( const Synthesizer & other );	//	 get rid of?

//	access:
	double sampleRate( void ) const { return _sampleRate; }
	/*
	double offset( void ) const { return _offset; }
	void setOffset( double x ) { _offset = x; }
	
	double fadeTime( void ) const { return _fadeTime; }
	void setFadeTime( double x ) { if (x >= 0.) _fadeTime = x; }
	*/
	std::vector< double > & samples( void ) { return _samples; }
	const std::vector< double > & samples( void ) const { return _samples; }
	//void setSampleBuffer( std::vector< double > & buf ) { _samples = buf; }

//	provide Oscillator access like iterator 
/*	access provided by PartialIteratorOwner:	
	const std::auto_ptr< Oscillator > & oscillator( void ) const { return _oscillator; }
	std::auto_ptr< Oscillator > 
	setOscillator( std::auto_ptr< Oscillator > osc = 
					std::auto_ptr< Oscillator >( new Oscillator() ) ) ;
*/
//	synthesis:
	void synthesize( const Partial & p );	
	
//	-- template member functions for synthesis --
//
//	Strictly speaking, we can do without these if necessary.
//
//	Either remove this or make it more like others, by allowing 
//	list<Partial> iterators if No_template_members.
//
//	Without these, one might use 
//		for_each( partials.begin(), partials.end(), synth );
//	to synthesize a collection of Partials, but note that this
//	makes two copies of the Synthesizer, one on the call to for_each,
//	and one for its return value. Using references doesn't seem to
//	get around that problem. It still works, because all the copies
//	share the same sample buffer (Synthesizer only has a reference
//	to its buffer), but if its too expensive or if the copying is
//	otherwise unacceptable, you'd have to write your own loop.
//
//	Should I write an extension to the STL that does the thing
//	we want? You'd think there'd be something, but there isn't.
//	Call it apply_to_each<InIter, Func>(InIter, InIter, Func &)?
//	
//
#if !defined(No_template_members)
	template < class Iterator >
	void synthesize( Iterator begin, Iterator end )
	{
		while( begin != end ) {
			synthesize( *(begin++) );
		}
	}
#endif 	//	template members allowed

//	-- private helpers --
private:
	//inline long synthesizeEnvelopeSegment( long currentSampleOffset );
	//inline long synthesizeFadeIn( long currentSampleOffset );
	//inline long synthesizeFadeOut( long currentSampleOffset );
	inline double radianFreq( double hz ) const;

//	-- instance variables --
	double _sampleRate;		//	in Hz
	//double _offset;			//	time offset for synthesized Partials, in seconds
	//double _fadeTime;		//	default (maximum) fade in/out time for Partials, in seconds
	
	std::vector< double > & _samples;	//	samples are computed and stored here
	
	//std::auto_ptr< Oscillator > _oscillator;	//	performs the sample computation
	
};	//	end of class Synthesizer

// ---------------------------------------------------------------------------
//	class SynthesisIterator
//
//	For best synthesis results, low-frequency Partials are synthesized without
//	their bandwidth energy, and Partials above the Nyquist frequency are
//	synthesized at zero amplitude. This iterator (the default iterator for
//	Synthesizer) makes both of these adjustments.
//
class SynthesisIterator : public PartialDecorIterator
{
public:
	SynthesisIterator( double fNyquist, double flow = 0. ) :
		_nyquistfreq( fNyquist ),
		_bwecutoff( flow )  {}
		
	SynthesisIterator( const SynthesisIterator & other ) :
		_nyquistfreq( other._nyquistfreq ), 
		_bwecutoff( other._bwecutoff ), 
		PartialDecorIterator( other ) {}
	
	//	cloning:
	//	In standard C++, an overriding member can return a type that
	//	is derived from the return type of the overidden member.
	//	But not in MIPSPro C++.
#if defined(__sgi) && ! defined(__GNUC__)
	PartialIterator * 
#else
	SynthesisIterator *	
#endif
		clone( void ) const { return new SynthesisIterator( *this ); }

	//	synthesize at zero amplitude above the Nyquist
	//	frequency, and without bandwidth enhancement below
	//	the specified cutoff frequency:
	double amplitude( void ) const;

	//	synthesize without bandwidth enhancement below
	//	the specified cutoff frequency, also clamp
	//	partial bandwidths to reasonable values:
	double bandwidth( void ) const;
	
private:
	//	helper
	static double bwclamp( double bw )
	{
		if( bw > 1. )
			return 1.;
		else if ( bw < 0. )
			return 0.;
		else
			return bw;
	}

	//	instance variables:
	double _nyquistfreq;
	double _bwecutoff;
	
};	//	end of class SynthesisIterator

End_Namespace( Loris )

#endif	// ndef __Loris_synthesizer__

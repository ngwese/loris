#ifndef INCLUDE_SPCFILE_H
#define INCLUDE_SPCFILE_H
// ===========================================================================
//	SpcFile.h
//	
//	Association of info	(number of partials, number of frames, frame rate, 
//	and midi pitch)	to completely specify an spc file.  The spc files are 
//	exported for real-time synthesis in Kyma.
//
//	-lip 6 Nov 99
//
// ===========================================================================
#include "Partial.h"
#include "Exception.h"
#include <vector>
#include <list>
#include <iosfwd>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class SpcFile
//	
class SpcFile
{
//	-- instance variables --
	int _partials;					// number of partials
	int _startFrame;				// first frame number to write to file
	int _endFrame;					// last frame number to write to file
	double _rate;					// frame rate in seconds
	double _midiPitch;				// midi note number
	int _susFrame, _rlsFrame;		// frame numbers for sustain and release start

public:
//	construction:
//	(let compiler generate destructor)
	SpcFile( int pars, int startFrame, int endFrame, double rate, double tuning );

//	markers:
	void setMarkers( double susTime, double releaseTime );
	
//	writing:
	void write( std::ostream & file,  const std::list<Partial> & plist, int refLabel );
	
//	-- helpers --
private:
	//	envelopes writing:
	void writeEnvelopeData( std::ostream & s, const std::list<Partial> & plist, int refLabel );
	void writeEnvelopes( std::ostream & s, const std::list<Partial> & plist, int refLabel );
	
	//	envelope writing helpers:
	int findRefPartial( const std::list<Partial> & plist );
	unsigned long packLeft( const Partial & p, double freqMult, double ampMult, double time );
	unsigned long packRight( const Partial & p, double noiseMagMult, double time );
	unsigned long envLog( double ) const;
	const Partial * select( const std::list<Partial> & partials, int label );
	
	//	chunk writing:
	void writeCommon( std::ostream & s );
	void writeContainer( std::ostream & s );
	void writeSosEnvelopesChunk( std::ostream & s );
	void writeInstrument( std::ostream & s );
	void writeMarker( std::ostream & s );
	
	//	data sizes:
	unsigned long sizeofCommon( void );
	unsigned long sizeofCkHeader( void );
	unsigned long sizeofSoundData( void );
	unsigned long sizeofSosEnvelopes( void );
	unsigned long sizeofInstrument( void );
	unsigned long sizeofMarker( void );
	
};	//	end of class SosFile

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif //	ndef INCLUDE_SPCFILE_H

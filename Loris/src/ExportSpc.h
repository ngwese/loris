#ifndef INCLUDE_EXPORTSPC_H
#define INCLUDE_EXPORTSPC_H
// ===========================================================================
//	SpcFile.h
//	
//	Association of info	(number of partials, number of frames, hop size, 
//	and midi pitch)	to completely specify an spc file.  The spc files are 
//	exported for real-time synthesis in Kyma.
//
//	-lip 6 Nov 99
//
// ===========================================================================
#include "Partial.h"
#include "Exception.h"
#include <vector>
#include <iosfwd>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class ExportSpc
//	
class ExportSpc
{
//	-- instance variables --
	int _partials;					// number of partials
	double _hop;					// frame hop size in seconds
	double _midiPitch;				// midi note number
	double _threshold;				// attack threshold (0.0 to keep all of attack)
	int _startFrame;				// first frame to keep (after _threshold crop)
	int _endFrame;					// last frame number to write to file
	int _markerFrame;				// frame number for a marker
	int _endApproachFrames;			// ease into final final spectrum over this many frames,
									// or 0 to disable this function (if final is quiet)
	int _enhanced;					// true for bandwidth-enhanced spc file format
									// false for pure sinusoidal spc file format
	
public:
//	construction:
//	(let compiler generate destructor)
	ExportSpc( int pars, double hop, double tuning, double thresh,
					double endt, double markert, double approacht);
	
//	writing:
	void write( std::ostream & file,  const std::list<Partial> & plist, int refLabel );
	
//	-- helpers --
private:
	//	envelopes writing:
	void writeEnvelopeData( std::ostream & s, const std::list<Partial> & plist, int refLabel );
	void writeEnvelopes( std::ostream & s, const std::list<Partial> & plist, int refLabel );
	
	//	envelope writing helpers:
	int findRefPartial( const std::list<Partial> & plist );
	unsigned long packLeft( const Partial & p, double freqMult, double ampMult, 
				double time1, double weightFactor, double time2 );
	unsigned long packRight( const Partial & p, double noiseMagMult,
				double time1, double weightFactor, double time2 );
	unsigned long envLog( double ) const;
	const Partial * select( const PartialList & partials, int label );
	double crop( const PartialList & partials );
	
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
	
};	//	end of class ExportSpc

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif //	ndef INCLUDE_EXPORTSPC_H

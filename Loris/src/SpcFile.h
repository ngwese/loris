#ifndef __Loris_spc_file__
#define __Loris_spc_file__

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

#include "LorisLib.h"
#include "Partial.h"

#include "LorisTypes.h"
#include "ieee.h"
#include "Exception.h"

#include <vector>
#include <list>

Begin_Namespace( Loris )

class BinaryFile;

// ---------------------------------------------------------------------------
//	class SpcFile
//	
class SpcFile
{
public:
//	construction:
	SpcFile( int pars, int startFrame, int endFrame, 
			 double rate, double tuning, int firstNoisePar );
	
//	defaults destructor is okay:
	//	~SosFile( void );

//	markers:
	void setMarkers( double susTime, double releaseTime );
	
//	writing:
	void write( BinaryFile & file,  const std::list<Partial> & plist );
	
//	-- chunk types --
private:
	enum { FileType = 'KYMs' };

	enum { 
		ContainerId = 'FORM', 
		AiffType = 'AIFF', 
		CommonId = 'COMM',
		SoundDataId = 'SSND',
		ApplicationSpecificId = 'APPL',
		SosEnvelopesId = 'SOSe',
		InstrumentId = 'INST',
		MarkerId = 'MARK'
	};
	
	struct CkHeader {
		Int_32 id;
		Uint_32 size;
	};
	
	struct ContainerCk
	{
		CkHeader header;
		Int_32 formType;
	};
	
	struct CommonCk
	{
		CkHeader header;
		Int_16 channels;			// number of channels 
		Int_32 sampleFrames;		// channel independent sample frames 
		Int_16 bitsPerSample;		// number of bits per sample 
		IEEE::extended80 srate;		// sampling rate IEEE 10 byte format 
	};
	
	struct SoundDataCk
	{
		CkHeader header;	
		Uint_32 offset;				
		Uint_32 blockSize;	
		//	sample frames follow
	};
	
	struct AIFFLoop {
		Int_16 	playMode;
		Int_16 	beginLoop;			// marker ID for beginning of loop
		Int_16 	endLoop;			// marker ID for ending of loop
	};

	struct InstrumentCk {
		CkHeader header;	
		char 	baseFrequency;		// integer midi note number
		char 	detune;				// negative of cents offset from midi note number
		char 	lowFrequency;
		char 	highFrequency;
		char 	lowVelocity;
		char 	highVelocity;
		short 	gain;
		struct AIFFLoop sustainLoop;
		struct AIFFLoop releaseLoop;
	};
	
	struct Marker3 {
		short 	id;
		long 	position;
		char 	markerName[4];		// 3 character name plus termination
	};

	struct Marker9 {
		short 	id;
		long 	position;
		char 	markerName[10];		// 9 character name plus termination
	};

	struct Marker7 {
		short 	id;
		long 	position;
		char 	markerName[8];		// 8 character name plus termination
	};

	struct MarkerCk {
		CkHeader header;	
		short 	numMarkers;
		Marker3	susMarker;			// array of markers starts here
		Marker9	loopStartMarker;	// second marker in every spc file
		Marker7	loopEndMarker;		// third marker in every spc file
	};

	struct SosEnvelopesCk
	{
		CkHeader header;	
		Int_32	signature;		// For SOS, should be 'SOSe'
		Int_32	frames;			// Total number of frames
		Int_32	validPartials;	// Number of partials with data in them; the file must
								// be padded out to the next higher 2**n partials
		Int_32	initPhase[512]; // obsolete initial phase array; was VARIABLE LENGTH array 
	//	Int_32	resolution;		// frame duration in microseconds 
		#define SOSresolution initPhase[MONOFILE ? 2 * _partials : _partials]	
								// follows the initPhase[] array
	//	Int_32	quasiHarmonic;	// how many of the partials are quasiharmonic
		#define SOSquasiHarmonic initPhase[MONOFILE ? 2 * _partials + 1 : _partials + 1]	
								// follows the initPhase[] array
	};  
	
//	data type for integer pcm samples of different sizes:
	typedef union {
		//	different size samples:
		Int_32 s32bits;						//	32 bits sample
		struct { char data[3]; }  s24bits;	//	24 bits sample
		Int_16 s16bits;						//	16 bits sample
		char s8bits;						//	8 bits sample
	} pcm_sample;

//	-- helpers --
	
	//	envelopes writing:
	void writeEnvelopeData( BinaryFile & file, const std::list<Partial> & plist );
	void writeEnvelopes( BinaryFile & file, const std::list<Partial> & plist );
	
	//	envelope writing helpers:
	int findRefPartial( const std::list<Partial> & plist );
	ulong packLeft( const Partial & p, double freqMult, double ampMult, double time );
	ulong packRight( const Partial & p, double noiseMagMult, double time );
	ulong envLog( double ) const;
	const Partial * select( const std::list<Partial> & partials, int label );
	
	//	chunk writing:
	void writeCommon( BinaryFile & file );
	void writeContainer( BinaryFile & file );
	void writeSosEnvelopesChunk( BinaryFile & file );
	void writeInstrument( BinaryFile & file );
	void writeMarker( BinaryFile & file );
	
	//	data sizes:
	Uint_32 sizeofCommon( void );
	Uint_32 sizeofCkHeader( void );
	Uint_32 sizeofSoundData( void );
	Uint_32 sizeofSosEnvelopes( void );
	Uint_32 sizeofInstrument( void );
	Uint_32 sizeofMarker( void );
	
//	-- instance variables --
	int _partials;					// number of partials
	int _startFrame;				// first frame number to write to file
	int _endFrame;					// last frame number to write to file
	double _rate;					// frame rate in seconds
	double _midiPitch;				// midi note number
	double _firstNoisePar;			// ignore bandwidth below this partial
	int _susFrame, _rlsFrame;		// frame numbers for sustain and release start

};	//	end of class SosFile

End_Namespace( Loris )

#endif //	ndef __Loris_spc_file__
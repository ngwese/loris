/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
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
 * ExportSpc.C
 *
 * Implementation of class ExportSpc, which exports spc files for
 * real-time synthesis in Kyma.
 *
 * Lippold Haken, 6 Nov 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "ExportSpc.h"
#include "Endian.h"
#include "Partial.h"
#include "notifier.h"
#include "ieee.h"
#include "LorisTypes.h"
#include "PartialUtils.h"

#include <algorithm>
#include <string>
#include <cmath>
#include <fstream>

// Temporary support for using old-style envelope reader for enhanced spc files in Kyma.
// Set MONO_ENH to 1 for old-style reader compatability, 
// set MONO_ENH to 0 for enhanced envelope reader.
// The issues for bandwidth-enhanced spc files are:
//		- file is mono, not stereo (despite the fact that it contain BW&phase)
//		- file claims to have twice the number of partials
//		- file claims to have twice the number of samples (2 mono samples = 1 stereo)
#define MONO_ENH 1

//	Pi:
static const double Pi = M_PI;
static const double TwoPi = 2. * M_PI;

using namespace std;

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

//	envelope writing
void writeEnvelopeData( std::ostream & s, const list<Partial> & plist, int refLabel,
				int startFrame, int endFrame, int partials, int enhanced,
				int endApproachFrames, double hop, double startFreqTime);
void writeEnvelopes( std::ostream & s, const list<Partial> & plist, int refLabel,
				int startFrame, int endFrame, int partials, int enhanced,
				int endApproachFrames, double hop, double startFreqTime);

// 	envelope writing helpers
unsigned long packLeft( const Partial & p, double freqMult, double ampMult, 
						double time1, double weightFactor, double time2, double startFreqTime );
unsigned long packRight( const Partial & p, double noiseMagMult, double time1,
						double weightFactor, double time2 );
unsigned long envLog( double floatingValue );
const Partial * select( const std::list< Partial > & partials, int label, int firstFrame = false );
double crop( const std::list< Partial > & partials, int endFrame, double hop, double threshold );

//	chunk wiriting helpers
void writeCommon( std::ostream & s, int startFrame, int endFrame, int partials, int enhanced );
void writeContainer( std::ostream & s, int startFrame, int endFrame, int partials, int enhanced, int markerFrame );
void writeInstrument( std::ostream & s, int midiPitch );
void writeMarker( std::ostream & s, int partials, int enhanced, int markerFrame );
void writeSosEnvelopesChunk( std::ostream & s, int startFrame, int endFrame, int partials, int enhanced, double hop );

//	chunk size helpers
unsigned long sizeofCkHeader( void );
unsigned long sizeofCommon( void );
unsigned long sizeofSosEnvelopes( void );
unsigned long sizeofInstrument( void );
unsigned long sizeofMarker( void );
unsigned long sizeofSoundData( int startFrame, int endFrame, int partials, int enhanced );


//	-- chunk types --
enum 
{ 
	FileType = 'KYMs',
	ContainerId = 'FORM', 
	AiffType = 'AIFF', 
	CommonId = 'COMM',
	SoundDataId = 'SSND',
	ApplicationSpecificId = 'APPL',
	SosEnvelopesId = 'SOSe',
	InstrumentId = 'INST',
	MarkerId = 'MARK'
};

struct CkHeader 
{
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

struct AIFFLoop 
{
	Int_16 	playMode;
	Int_16 	beginLoop;			// marker ID for beginning of loop
	Int_16 	endLoop;			// marker ID for ending of loop
};

struct InstrumentCk 
{
	CkHeader header;	
	char 	baseFrequency;		// integer midi note number
	char 	detune;				// negative of cents offset from midi note number
	char 	lowFrequency;
	char 	highFrequency;
	char 	lowVelocity;
	char 	highVelocity;
	Int_16 	gain;
	struct AIFFLoop sustainLoop;
	struct AIFFLoop releaseLoop;
};

struct Marker1 					// for single character marker name
{
	Int_16 	id;
	Int_32 	position;
	char 	markerName[2];		// 1 character name in pascal string
};

struct MarerKymaLoopStart 		// for Kyma's special "loopstart" marker
{
	Int_16 	id;
	Int_32 	position;
	char 	markerName[10];		// 9 character name in pascal string
};

struct MarkerKymaLoopEnd		// for Kyma's special "loopend" marker
{
	Int_16 	id;
	Int_32 	position;
	char 	markerName[8];		// 7 character name in pascal string
};

struct MarkerCk 
{
	CkHeader header;	
	Int_16 	numMarkers;
	Marker1	aMarker;			// array of markers starts here
};

struct SosEnvelopesCk
{
	CkHeader header;	
	Int_32	signature;		// For SOS, should be 'SOSe'
	Int_32	frames;			// Total number of frames
	Int_32	validPartials;	// Number of partials with data in them; the file must
							// be padded out to the next higher 2**n partials
#define initPhaseLth 520
	Int_32	initPhase[initPhaseLth]; // obsolete initial phase array; was VARIABLE LENGTH array 
							// this is big enough for a max of 512 partials plus values below
//	Int_32	resolution;		// frame duration in microseconds 
	#define SOSresolution(enh, pars)   initPhase[(enh && MONO_ENH) ? 2 * pars : pars]	
							// follows the initPhase[] array
//	Int_32	quasiHarmonic;	// how many of the partials are quasiharmonic
	#define SOSquasiHarmonic(enh, pars)  initPhase[(enh && MONO_ENH) ? 2 * pars + 1 : pars + 1]	
							// follows the initPhase[] array
};  




// ---------------------------------------------------------------------------
//	ExportSpc constructor from data in memory
// ---------------------------------------------------------------------------
//
ExportSpc::ExportSpc( int pars, double hop, double midiPitch, double thresh, double endt, 
				double markert, double startFreqTime, double endApproachTime) :
	_partials( pars ),
	_hop( abs(hop) ),
	_enhanced( hop > 0.0 ),
	_midiPitch( midiPitch ),
	_threshold( thresh ),
	_endFrame( endt / abs(hop) + 1 ),
	_markerFrame( markert / abs(hop) + 1 ),
	_startFreqTime( startFreqTime ),
	_endApproachFrames( endApproachTime / abs(hop) + 1 )
{
	// Max number of partials is due to (arbitrary) size of initPhase[].
	Assert( pars == 32 || pars == 64 || pars == 128 || pars == 256 
				|| (pars == 512 && (!_enhanced || !MONO_ENH)) );
	
	// Unlike sdif files, spc files always contain resampled envelope values.
	Assert( hop != 0.0 );
}

// ---------------------------------------------------------------------------
//	write
// ---------------------------------------------------------------------------
// The plist should be labeled and distilled before this is called.
//
void
ExportSpc::write( ostream & file, const list<Partial> & plist, int refLabel )
{
	
	// Find starting frame after cropping.
	_startFrame = crop( plist, _endFrame, _hop, _threshold ) / _hop + 1;

	debugger << "startFrame = " << _startFrame << " endFrame = " << _endFrame 
			 << " hop = " << _hop << " threshold = " << _threshold 
			 << " ref label = " << refLabel << endl;
	
	// Write out all the chunks.
	try 
	{
		writeContainer( file, _startFrame, _endFrame, _partials, _enhanced, _markerFrame );
		writeCommon( file, _startFrame, _endFrame, _partials, _enhanced );
		writeInstrument( file, _midiPitch );
		if (_markerFrame)
			writeMarker( file, _partials, _enhanced, _markerFrame );
		writeSosEnvelopesChunk( file, _startFrame, _endFrame, _partials, _enhanced, _hop );
		writeEnvelopeData( file, plist, refLabel, _startFrame, _endFrame, _partials, _enhanced,
				_endApproachFrames, _hop, _startFreqTime );
	}
	catch ( Exception & ex ) 
	{
		ex.append("Failed to write SPC file.");
		throw;
	}
}

#pragma mark -
#pragma mark envelope writing
// ---------------------------------------------------------------------------
//	writeEnvelopeData
// ---------------------------------------------------------------------------
//
void writeEnvelopeData( std::ostream & s, const list<Partial> & plist, int refLabel,
				int startFrame, int endFrame, int partials, int enhanced,
				int endApproachFrames, double hop, double startFreqTime)
{
	//	first build a Sound Data chunk, so that all the data sizes will 
	//	be correct:
	SoundDataCk ck;
	ck.header.id = SoundDataId;
	
	//	size is everything after the header:
	ck.header.size = sizeofSoundData( startFrame, endFrame, partials, enhanced ) - sizeofCkHeader();
				
	//	no block alignment:	
	ck.offset = 0.;
	ck.blockSize = 0;
	
	//	write it out:
	try 
	{
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.header.id );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.header.size );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.offset );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.blockSize );

	 	writeEnvelopes( s, plist, refLabel,
				startFrame, endFrame, partials, enhanced,
				endApproachFrames, hop, startFreqTime);
	}
	catch( FileIOException & ex ) 
	{
		ex.append("Failed to write SPC file SoundData chunk.");
		throw;
	}
}

// ---------------------------------------------------------------------------
//	writeEnvelopes
// ---------------------------------------------------------------------------
//	Let exceptions propogate.
//	The plist should be labeled and distilled before this is called.
//
void writeEnvelopes( std::ostream & s, const list<Partial> & plist, int refLabel,
				int startFrame, int endFrame, int partials, int enhanced,
				int endApproachFrames, double hop, double startFreqTime)
{	
	double freqMult;				// frequency multiplier for partial	
	double magMult;					// magnitude multiplier for partial	
	unsigned long left,right;		// packed 24-bit value for left, right channel in spc file
	
	// the label for the reference partial must be non-zero:
	if( refLabel == 0 )
		Throw( FileIOException, "Label for reference partial is zero." );
	
	// make sure the reference partial is there:
	const Partial * refPar = select( plist, refLabel );
	if ( refPar == NULL )
		Throw( FileIOException, "No partial has the reference partial label!" );
	if ( refPar->begin() == refPar->end() )
		Throw( FileIOException, "Reference partial has zero length." );

	// write out one frame at a time:
	for (long frame = startFrame; frame <= endFrame; ++frame ) 
	{
	
		//	for each frame, write one value for every partial:
		for (unsigned int label = 1; label <= partials; ++label ) 
		{

			// find partial with the correct label:
			// if no partial is found, frequency multiply the reference partial 
			const Partial * pcorrect = select( plist, label, frame == startFrame );
		
			if ( pcorrect == NULL || pcorrect->begin() == pcorrect->end() ) 
			{
				pcorrect = refPar;
				freqMult = (double) label / (double) refLabel; 
				magMult = 0.0;
			} 
			else if (!endApproachFrames && frame == endFrame) 
			{
				freqMult = 1.0;
				magMult = 0.0;	// last frame has zero amp, if not ending at static spectrum
			} 
			else 
			{
				freqMult = 1.0;
				magMult = 1.0;
			}
			
			//	Check for special processing for approach to static spectrum at end.
			//	Compute weighting factor between "normal" envelope point and static point.
			double weightFactor = 1.0;
			if (endApproachFrames && frame > endFrame - endApproachFrames)
				weightFactor = (endFrame - frame) / (double) endApproachFrames;

			//	pack log amplitude and log frequency into 24-bit left:
			//  The log frequency value sticks at the release frame's frequency value.
			left = packLeft(*pcorrect, freqMult, magMult, frame * hop, 
								weightFactor, endFrame * hop, startFreqTime);

			//	pack log bandwidth and phase into 24-bit right:
			right = packRight(*pcorrect, magMult, frame * hop, weightFactor, endFrame * hop);
	
			//	write integer samples without byte swapping,
			//	they are already correctly packed: 
			BigEndian::write( s, 3, 1, (char*)&left );
			if (enhanced)
				BigEndian::write( s, 3, 1, (char*)&right );
		}
	}
}

#pragma mark -
#pragma mark envelope writing helpers
// ---------------------------------------------------------------------------
//	envLog( )
// ---------------------------------------------------------------------------
//	For a range 0 to 1, this returns a log value, 0x0000 to 0xFFFF.
//
unsigned long envLog( double floatingValue )
{
	static double coeff = 65535.0 / log( 32768. );

	return coeff * log( 32768.0 * floatingValue + 1.0 );

}	//  end of envLog( )

// ---------------------------------------------------------------------------
//	packLeft
// ---------------------------------------------------------------------------
//	Pack two values into 24 bits, for interpretation by Envelope Reader sounds
//	in Kyma.  The log of the sine magnitude occupies the top 8 bits, the log of the
//	frequency occupies the bottom 16 bits.
//
unsigned long packLeft( const Partial & p, double freqMult, double ampMult, 
						double time1, double weightFactor, double time2, double startFreqTime )
{	

// Find amp, freq, and bw at time1.
	double amp = ampMult * p.amplitudeAt( time1 );
	double freq = freqMult * p.frequencyAt( time1 );
	double bw = p.bandwidthAt( time1 );

// Approach amp, freq, and bw values at time2, if necessary.	
// This avoids a sudden transition when using stick-at-end-frame sustains.
	if (weightFactor != 1.0) 
	{
		double amp2 = ampMult * p.amplitudeAt( time2 );
		double freq2 = freqMult * p.frequencyAt( time2 );
		double bw2 = p.bandwidthAt( time2 );
		amp = (amp * weightFactor + amp2 * (1.0 - weightFactor));
		freq = (freq * weightFactor + freq2 * (1.0 - weightFactor));
		bw = (bw * weightFactor + bw2 * (1.0 - weightFactor));
	}

// Approach frequency values at _startFreqTime, if necessary.	
	if (startFreqTime && time1 < startFreqTime)
	{
		double freq3 = freqMult * p.frequencyAt( startFreqTime );
		double fweight = (time1 < 0.0 ? 0.0 : time1 / startFreqTime);
		freq = (freq * fweight + freq3 * (1.0 - fweight));
	}
	
// 7 bits of log-sine-amplitude with 24 bits of zero to right.
	double theSineMag = amp * sqrt( 1. - bw );
	unsigned long theOutput	= ( envLog( theSineMag ) & 0xFE00 ) << 15;
	
// 16 bits of log-frequency with 8 bits of zero to right.
	double normalizedFreq	= freq / 22050.0;		// 0..1 , 1 is 22.050 kHz
	theOutput  |= ( envLog( normalizedFreq ) & 0xFFFF ) << 8;
	
// Our 24 bits are in the top of a 32-bit word.
	return theOutput;
}

// ---------------------------------------------------------------------------
//	packRight
// ---------------------------------------------------------------------------
//	Pack two values into 24 bits, for interpretation by Envelope Reader sounds
//	in Kyma.  The log of the noise magnitude occupies the top 8 bits, the scaled
//	linear phase occupies the bottom 16 bits.  
//
unsigned long packRight( const Partial & p, double noiseMagMult, double time1,
						double weightFactor, double time2 )
{	
	double amp = p.amplitudeAt( time1 );
	double phase = p.phaseAt( time1 );
	double bw = p.bandwidthAt( time1 );
	
	if (weightFactor != 1.0) 
	{
		double amp2 = p.amplitudeAt( time2 );
		double bw2 = p.bandwidthAt( time2 );
		amp = (amp * weightFactor + amp2 * (1.0 - weightFactor));
		bw = (bw * weightFactor + bw2 * (1.0 - weightFactor));
	}

	unsigned long	theOutput;
	
	phase = fmod( phase, TwoPi );
	if ( phase < 0. )
		phase += TwoPi;
	double zeroToOnePhase = phase / TwoPi;
	
	double theNoiseMag = noiseMagMult * 64.0 * amp * sqrt( bw );
	if (theNoiseMag > 1.0)
		theNoiseMag = 1.0;
	
// 7 bits of log-noise-amplitude with 24 bits of zero to right.
	theOutput	= ( envLog( theNoiseMag ) & 0xFE00 ) << 15;
	
// 16 bits of phase with 8 bits of zero to right.
	theOutput  |= ( (unsigned long) ( zeroToOnePhase * 0xFFFF ) ) << 8;
	
// Our 24 bits are in the top of a 32-bit word.
	return theOutput;
}

// ---------------------------------------------------------------------------
//	select
// ---------------------------------------------------------------------------
//	Special select function that returns a pointer to the Partial
//	having the specified label, or NULL if there is not such Partial
//	in the list. 
//
const Partial * select( const std::list< Partial > & partials, int label, int firstFrame )
{
	const Partial * ret = NULL;
	list< Partial >::const_iterator it = 
		std::find_if( partials.begin(), partials.end(), 
				 std::bind2nd(PartialUtils::label_equals(), label) );
		
	if ( it != partials.end() ) 
	{
		ret = &(*it);

		//	there should only be one of such Partial, verify this on first frame:
		if ( firstFrame )
		{
			if ( std::count_if( ++it, partials.end(), 
										std::bind2nd(PartialUtils::label_equals(), label) ) != 0)
				Throw( FileIOException, "Partials are not distilled." );
		}
	}
	
	return ret;
}

// ---------------------------------------------------------------------------
//	crop
// ---------------------------------------------------------------------------
//	Find first frame at which amplitude sum reaches a certain percentage of the
//	peak amplitude sum.
double crop( const std::list< Partial > & partials, int endFrame, double hop, double threshold )
{
	
	// Find max amp sum.
	double tim, ampsum;
	double maxampsum = 0.0;
	for ( tim = 0.0; tim < endFrame * hop; tim += hop ) 
	{
	
		// Compute sum of amp of all partials at this time.
		ampsum = 0.0;
		for ( std::list< Partial >::const_iterator it = partials.begin(); it != partials.end(); ++it ) 
		{
			if ( it->begin() != it->end() )
				ampsum += it->amplitudeAt( tim );
		}
		
		// Keep maximum of amp  values
		if (ampsum > maxampsum)
			maxampsum = ampsum;
	}

	// Find first time we get some percentage of the max amp.
	// This will be used to crop the attack.
	for (tim = 0.0; tim < endFrame * hop; tim += hop) 
	{

		// Compute sum of amp of all partials at this time.
		ampsum = 0.0;
		for ( std::list< Partial >::const_iterator it = partials.begin(); it != partials.end(); ++it ) 
		{
			if ( it->begin() != it->end() )
				ampsum += it->amplitudeAt( tim );
		}
		
		// If we hit the amp level, we will crop to here; exit.
		if (ampsum > threshold * maxampsum)
			return tim;
	}
	
	return 0.;
}

#pragma mark -
#pragma mark chunk writing helpers
// ---------------------------------------------------------------------------
//	writeCommon
// ---------------------------------------------------------------------------
//
void writeCommon( std::ostream & s, int startFrame, int endFrame, int partials, int enhanced )
{
	//	first build a Common chunk, so that all the data sizes will 
	//	be correct:
	CommonCk ck;
	ck.header.id = CommonId;
	
	//	size is everything after the header:
	ck.header.size = sizeofCommon() - sizeofCkHeader();

	if (enhanced) 			// bandwidth-enhanced spc file
	{					
		ck.channels = MONO_ENH ? 1 : 2;
		ck.sampleFrames = (endFrame - startFrame + 1) * partials * (MONO_ENH ? 2 : 1);
	}
	else					// pure sinusoidal spc file
	{					
		ck.channels = 1;
		ck.sampleFrames = (endFrame - startFrame + 1) * partials;
	}
	
	ck.bitsPerSample = 24;
	IEEE::ConvertToIeeeExtended( 44100, & ck.srate ); // bogus for SPC files
	
	//	write it out:
	try 
	{
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.header.id );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.header.size );
		BigEndian::write( s, 1, sizeof(Int_16), (char *)&ck.channels );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.sampleFrames );
		BigEndian::write( s, 1, sizeof(Int_16), (char *)&ck.bitsPerSample );
		//	don't let this get byte-reversed:
		BigEndian::write( s, 10, sizeof(char), (char *)&ck.srate );
	}
	catch( FileIOException & ex ) {
		ex.append( "Failed to write SPC file Common chunk." );
		throw;
	}
}

// ---------------------------------------------------------------------------
//	writeContainer
// ---------------------------------------------------------------------------
//
void writeContainer( std::ostream & s, int startFrame, int endFrame, int partials, int enhanced, int markerFrame )
{
	//	first build a Container chunk, so that all the data sizes will 
	//	be correct:
	ContainerCk ck;
	ck.header.id = ContainerId;
	
	//	size is everything after the header:
	ck.header.size = sizeof(Int_32) + sizeofCommon() 
				+ sizeofInstrument() 
				+ (markerFrame ? sizeofMarker() : 0)
				+ sizeofSosEnvelopes()
				+ sizeofSoundData( startFrame, endFrame, partials, enhanced );
	
	ck.formType = AiffType;
	
	//	write it out:
	try 
	{
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.header.id );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.header.size );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.formType );
	}
	catch( FileIOException & ex ) 
	{
		ex.append( "Failed to write SPC file Container chunk." );
		throw;
	}
}	

// ---------------------------------------------------------------------------
//	writeInstrument
// ---------------------------------------------------------------------------
//
void writeInstrument( std::ostream & s, int midiPitch )
{
	//	first build an Inst chunk, so that all the data sizes will 
	//	be correct:
	InstrumentCk ck;
	ck.header.id = InstrumentId;
	
	//	size is everything after the header:
	ck.header.size = sizeofInstrument() - sizeofCkHeader();
	
	ck.baseFrequency = long( midiPitch );
	ck.detune = ((long)(100 * midiPitch)) % 100;
	if (ck.detune > 50)
	{
		ck.baseFrequency++;
		ck.detune -= 100;
	}
	ck.detune *= -1;

	ck.lowFrequency = 0;	
	ck.highFrequency = 127;	
	ck.lowVelocity = 1;	
	ck.highVelocity = 127;	
	ck.gain = 0;	
	ck.sustainLoop.playMode = 0;		// Sustain looping done by name, not by this
	ck.sustainLoop.beginLoop = 0;
	ck.sustainLoop.endLoop = 0;
	ck.releaseLoop.playMode = 0;		// No Looping
	ck.releaseLoop.beginLoop = 0;
	ck.releaseLoop.endLoop = 0;
	
	//	write it out:
	try 
	{
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.header.id );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.header.size );

		BigEndian::write( s, 1, sizeof(char), (char *)&ck.baseFrequency );
		BigEndian::write( s, 1, sizeof(char), (char *)&ck.detune );
		BigEndian::write( s, 1, sizeof(char), (char *)&ck.lowFrequency );
		BigEndian::write( s, 1, sizeof(char), (char *)&ck.highFrequency );
		BigEndian::write( s, 1, sizeof(char), (char *)&ck.lowVelocity );
		BigEndian::write( s, 1, sizeof(char), (char *)&ck.highVelocity );
		BigEndian::write( s, 1, sizeof(Int_16), (char *)&ck.gain );

		//	AIFFLoop is three Int_16s:
		BigEndian::write( s, 3, sizeof(Int_16), (char *)&ck.sustainLoop );
		BigEndian::write( s, 3, sizeof(Int_16), (char *)&ck.releaseLoop );
	}
	catch( FileIOException & ex ) 
	{
		ex.append( "Failed to write SPC file Instrument chunk." );
		throw;
	}
}

// ---------------------------------------------------------------------------
//	writeMarker
// ---------------------------------------------------------------------------
//
void writeMarker( std::ostream & s, int partials, int enhanced, int markerFrame )
{
	//	first build a Marker chunk, so that all the data sizes will 
	//	be correct:
	MarkerCk ck;
	ck.header.id = MarkerId;
	
	//	size is everything after the header:
	ck.header.size = sizeofMarker() - sizeofCkHeader();
	
	ck.numMarkers = 1;					// one marker
	ck.aMarker.id = 1;					// first marker 
	ck.aMarker.position = markerFrame * partials * ((enhanced && MONO_ENH) ? 2 : 1); 
	ck.aMarker.markerName[0] = 1;		// 1 character long name
	ck.aMarker.markerName[1] = 'a';

	//	write it out:
	try 
	{
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.header.id );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.header.size );

		BigEndian::write( s, 1, sizeof(Int_16), (char *)&ck.numMarkers );

		BigEndian::write( s, 1, sizeof(Int_16), (char *)&ck.aMarker.id );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.aMarker.position );
		BigEndian::write( s, 2, sizeof(char), ck.aMarker.markerName );
	}
	catch( FileIOException & ex ) 
	{
		ex.append( "Failed to write SPC file Marker chunk." );
		throw;
	}
}

// ---------------------------------------------------------------------------
//	writeSosEnvelopesChunk
// ---------------------------------------------------------------------------
//
void writeSosEnvelopesChunk( std::ostream & s, int startFrame, int endFrame, int partials, int enhanced, double hop )
{
	//	first build an SOSe chunk, so that all the data sizes will 
	//	be correct:
	SosEnvelopesCk ck = { 0 };
	ck.header.id = ApplicationSpecificId;
	
	//	size is everything after the header:
	ck.header.size = sizeofSosEnvelopes() - sizeofCkHeader();
					
	ck.signature = SosEnvelopesId;
	ck.frames = endFrame - startFrame + 1;
	ck.validPartials = partials * ((enhanced && MONO_ENH) ? 2 : 1);
	
	ck.SOSresolution(enhanced,partials) = 1000000.0 * hop;				 // convert secs to microsecs
	ck.SOSquasiHarmonic(enhanced,partials) = partials * ((enhanced && MONO_ENH) ? 2 : 1); // all our partials quasiharmonic
	
	//	write it out:
	try 
	{
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.header.id );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.header.size );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.signature );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.frames );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.validPartials );
		
		// The SOSresultion and SOSquasiHarmonic fields are in the phase table memory.
		BigEndian::write( s, initPhaseLth, sizeof(Int_32), (char *)&ck.initPhase[0] );
	}
	catch( FileIOException & ex ) 
	{
		ex.append( "Failed to write SPC file SosEnvelopes chunk." );
		throw;
	}
}

#pragma mark -
#pragma mark chunk sizes
// ---------------------------------------------------------------------------
//	sizeofCkHeader
// ---------------------------------------------------------------------------
//
unsigned long sizeofCkHeader( void )
{
	return	sizeof(Int_32) +	//	id
			sizeof(Uint_32);	//	size
}

// ---------------------------------------------------------------------------
//	sizeofCommon
// ---------------------------------------------------------------------------
//
unsigned long sizeofCommon( void )
{
	return	sizeof(Int_32) +			//	id
			sizeof(Uint_32) +			//	size
			sizeof(Int_16) + 			//	num channels
			sizeof(Int_32) + 			//	num frames
			sizeof(Int_16) + 			//	bits per sample
			sizeof(IEEE::extended80);	//	sample rate
}

// ---------------------------------------------------------------------------
//	sizeofSOSe
// ---------------------------------------------------------------------------
//
unsigned long sizeofSosEnvelopes( void )
{
	return	sizeof(Int_32) +					//	id
			sizeof(Uint_32) +					//	size
			sizeof(Uint_32) + 					// signature
			sizeof(Uint_32) + 					// frames
			sizeof(Uint_32) + 					// validPartials
			initPhaseLth * sizeof(Int_32);		// initPhase[] et al
}

// ---------------------------------------------------------------------------
//	sizeofInstrument
// ---------------------------------------------------------------------------
//
unsigned long sizeofInstrument( void )
{
	return	sizeof(Int_32) +			//	id
			sizeof(Uint_32) +			//	size
			sizeof(char) +				// baseFrequency
			sizeof(char) +				// detune
			sizeof(char) +				// lowFrequency
			sizeof(char) +				// highFrequency
			sizeof(char) +				// lowVelocity
			sizeof(char) +				// highVelocity
			sizeof(short) +				// gain
			2 * sizeof(Int_16) +		// playmode for sustainLoop and releaseLoop
			2 * sizeof(Int_16) +		// beginLoop for sustainLoop and releaseLoop
			2 * sizeof(Int_16);			// loopEnd for sustainLoop and releaseLoop
}

// ---------------------------------------------------------------------------
//	sizeofMarker
// ---------------------------------------------------------------------------
//
unsigned long sizeofMarker( void )
{
	return	sizeof(Int_32) +			//	id
			sizeof(Uint_32) +			//	size
			sizeof(Int_16) +			// numMarkers
			sizeof(Int_16) +			// id for 1 standard marker
			sizeof(Int_32) +			// position for 1 standard marker
			2 * sizeof(char);			// characters in names for 1 marker
}

// ---------------------------------------------------------------------------
//	sizeofSoundData
// ---------------------------------------------------------------------------
//	No block alignment, the envelope samples start right after the 
//	chunk header info.
//
unsigned long sizeofSoundData( int startFrame, int endFrame, int partials, int enhanced )
{
	unsigned long dataSize = (endFrame - startFrame + 1) * partials * ( 24 / 8 )
							 * (enhanced ? 2 : 1);
	
	return	sizeof(Int_32) +	//	id
			sizeof(Uint_32) +	//	size
			sizeof(Uint_32) + 	//	offset
			sizeof(Uint_32) + 	//	block size
			dataSize;			//	envelope sample data
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

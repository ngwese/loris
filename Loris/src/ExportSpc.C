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


#pragma mark chunk types
// ---------------------------------------------------------------------------
//	Chunk Types
// ---------------------------------------------------------------------------
//
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
#define initPhaseLth (1024+8)
	Int_32	initPhase[initPhaseLth]; // obsolete initial phase array; was VARIABLE LENGTH array 
							// this is big enough for a max of 512 enhanced partials plus values below
//	Int_32	resolution;		// frame duration in microseconds 
	#define SOSresolution(enh, pars)   initPhase[(enh && MONO_ENH) ? 2 * (pars) : pars]	
							// follows the initPhase[] array
//	Int_32	quasiHarmonic;	// how many of the partials are quasiharmonic
	#define SOSquasiHarmonic(enh, pars)  initPhase[(enh && MONO_ENH) ? 2 * (pars) + 1 : pars + 1]	
							// follows the initPhase[] array
};  


#pragma mark -
#pragma mark chunk sizes
// ---------------------------------------------------------------------------
//	sizeofCkHeader
// ---------------------------------------------------------------------------
//
static unsigned long sizeofCkHeader( void )
{
	return	sizeof(Int_32) +	//	id
			sizeof(Uint_32);	//	size
}

// ---------------------------------------------------------------------------
//	sizeofCommon
// ---------------------------------------------------------------------------
//
static unsigned long sizeofCommon( void )
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
static unsigned long sizeofSosEnvelopes( void )
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
static unsigned long sizeofInstrument( void )
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
static unsigned long sizeofMarker( void )
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
static unsigned long sizeofSoundData( double startTime, double endTime, const ExportSpc & es )
{
	int frames = int((endTime - startTime) / es.hop()) + 1;
	unsigned long dataSize = frames * es.numPartials() * ( 24 / 8 ) * (es.enhanced() ? 2 : 1);
	
	return	sizeof(Int_32) +	//	id
			sizeof(Uint_32) +	//	size
			sizeof(Uint_32) + 	//	offset
			sizeof(Uint_32) + 	//	block size
			dataSize;			//	envelope sample data
}


#pragma mark -
#pragma mark chunk writing helpers
// ---------------------------------------------------------------------------
//	writeCommon
// ---------------------------------------------------------------------------
//
static void writeCommon( std::ostream & s, double startTime, double endTime, const ExportSpc & es )
{
	//	first build a Common chunk, so that all the data sizes will 
	//	be correct:
	CommonCk ck;
	ck.header.id = CommonId;
	
	//	size is everything after the header:
	ck.header.size = sizeofCommon() - sizeofCkHeader();

	int frames = int( (endTime - startTime) / es.hop() ) + 1;
	if ( es.enhanced() ) 			// bandwidth-enhanced spc file
	{					
		ck.channels = MONO_ENH ? 1 : 2;
		ck.sampleFrames = frames * es.numPartials() * (MONO_ENH ? 2 : 1);
	}
	else					// pure sinusoidal spc file
	{					
		ck.channels = 1;
		ck.sampleFrames = frames * es.numPartials();
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
		ex.append( " Failed to write SPC file Common chunk." );
		throw;
	}
}

// ---------------------------------------------------------------------------
//	writeContainer
// ---------------------------------------------------------------------------
//
static void writeContainer( std::ostream & s, double startTime, double endTime, const ExportSpc & es )
{
	//	first build a Container chunk, so that all the data sizes will 
	//	be correct:
	ContainerCk ck;
	ck.header.id = ContainerId;
	
	//	size is everything after the header:
	ck.header.size = sizeof(Int_32) + sizeofCommon() 
				+ sizeofInstrument() 
				+ (es.markerTime() ? sizeofMarker() : 0)
				+ sizeofSosEnvelopes()
				+ sizeofSoundData( startTime, endTime, es );
	
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
		ex.append( " Failed to write SPC file Container chunk." );
		throw;
	}
}	

// ---------------------------------------------------------------------------
//	writeInstrument
// ---------------------------------------------------------------------------
//
static void writeInstrument( std::ostream & s, const ExportSpc & es )
{
	//	first build an Inst chunk, so that all the data sizes will 
	//	be correct:
	InstrumentCk ck;
	ck.header.id = InstrumentId;
	
	//	size is everything after the header:
	ck.header.size = sizeofInstrument() - sizeofCkHeader();
	
	ck.baseFrequency = long( es.midiPitch() );
	ck.detune = long( 100 * es.midiPitch() ) % 100;
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
		ex.append( " Failed to write SPC file Instrument chunk." );
		throw;
	}
}

// ---------------------------------------------------------------------------
//	writeMarker
// ---------------------------------------------------------------------------
//
static void writeMarker( std::ostream & s, const ExportSpc & es )
{
	//	first build a Marker chunk, so that all the data sizes will 
	//	be correct:
	MarkerCk ck;
	ck.header.id = MarkerId;
	
	//	size is everything after the header:
	ck.header.size = sizeofMarker() - sizeofCkHeader();
	
	ck.numMarkers = 1;					// one marker
	ck.aMarker.id = 1;					// first marker 
	ck.aMarker.position = int( es.markerTime() / es.hop() ) *  es.numPartials() * ((es.enhanced() && MONO_ENH) ? 2 : 1); 
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
		ex.append( " Failed to write SPC file Marker chunk." );
		throw;
	}
}

// ---------------------------------------------------------------------------
//	writeSosEnvelopesChunk
// ---------------------------------------------------------------------------
//
static void writeSosEnvelopesChunk( std::ostream & s, double startTime, double endTime, const ExportSpc & es )
{
	//	first build an SOSe chunk, so that all the data sizes will 
	//	be correct:
	SosEnvelopesCk ck = { 0 };
	ck.header.id = ApplicationSpecificId;
	
	//	size is everything after the header:
	ck.header.size = sizeofSosEnvelopes() - sizeofCkHeader();
					
	ck.signature = SosEnvelopesId;

	ck.frames = int((endTime - startTime) / es.hop()) + 1;
	ck.validPartials = es.numPartials() * ((es.enhanced() && MONO_ENH) ? 2 : 1);
	
	//	resolution in microseconds
	ck.SOSresolution(es.enhanced(),es.numPartials()) = 1000000.0 * es.hop();
	
	//	all partials quasiharmonic
	ck.SOSquasiHarmonic(es.enhanced(),es.numPartials()) = 
					es.numPartials() * ((es.enhanced() && MONO_ENH) ? 2 : 1); 
	
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
		ex.append( " Failed to write SPC file SosEnvelopes chunk." );
		throw;
	}
}


#pragma mark -
#pragma mark envelope writing helpers
// ---------------------------------------------------------------------------
//	envLog( )
// ---------------------------------------------------------------------------
//	For a range 0 to 1, this returns a log value, 0x0000 to 0xFFFF.
//
static unsigned long envLog( double floatingValue )
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
static unsigned long packLeft( const Partial & p, double freqMult, double ampMult, 
						double time, double endApproachWeight, const ExportSpc & es )
{	
		
// Find amp, freq, and bw at time.
	double amp = ampMult * p.amplitudeAt( time );
	double freq = freqMult * p.frequencyAt( time );
	double bw = p.bandwidthAt( time );

// Approach amp, freq, and bw values at _endTime, if necessary.	
// This avoids a sudden transition when using stick-at-end-frame sustains.
	if (endApproachWeight != 1.0) 
	{
		double amp2 = ampMult * p.amplitudeAt( es.endTime() );
		double freq2 = freqMult * p.frequencyAt( es.endTime() );
		double bw2 = p.bandwidthAt( es.endTime() );
		amp = (amp * endApproachWeight + amp2 * (1.0 - endApproachWeight));
		freq = (freq * endApproachWeight + freq2 * (1.0 - endApproachWeight));
		bw = (bw * endApproachWeight + bw2 * (1.0 - endApproachWeight));
	}

// Approach frequency values at _startFreqTime, if necessary.	
	if (es.startFreqTime() && time < es.startFreqTime())
	{
		double freq3 = freqMult * p.frequencyAt( es.startFreqTime() );
		double fweight = (time < 0.0 ? 0.0 : time / es.startFreqTime());
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
static unsigned long packRight( const Partial & p, double noiseMagMult, double time,
						double endApproachWeight, const ExportSpc & es  )
{
// Find amp, bw, and phase at time.	
	double amp = p.amplitudeAt( time );
	double phase = p.phaseAt( time );
	double bw = p.bandwidthAt( time );
	
// Approach amp and bw values at _endTime, if necessary.	
// This avoids a sudden transition when using stick-at-end-frame sustains.
	if (endApproachWeight != 1.0) 
	{
		double amp2 = p.amplitudeAt( es.endTime() );
		double bw2 = p.bandwidthAt( es.endTime() );
		amp = (amp * endApproachWeight + amp2 * (1.0 - endApproachWeight));
		bw = (bw * endApproachWeight + bw2 * (1.0 - endApproachWeight));
	}

// Make phase into range 0..1.	
	if ( phase < 0. )
		phase += TwoPi; 
	double zeroToOnePhase = phase / TwoPi;

// Compute noise magnitude from amp and bw.	
	double theNoiseMag = noiseMagMult * 64.0 * amp * sqrt( bw );
	if (theNoiseMag > 1.0)
		theNoiseMag = 1.0;
	
// 7 bits of log-noise-amplitude with 24 bits of zero to right.
	unsigned long theOutput	= ( envLog( theNoiseMag ) & 0xFE00 ) << 15;
	
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
static const Partial * select( const std::list< Partial > & partials, int label, int firstFrameFlag = false );
static const Partial * select( const std::list< Partial > & partials, int label, int firstFrameFlag )
{
	const Partial * ret = NULL;
	list< Partial >::const_iterator it = 
		std::find_if( partials.begin(), partials.end(), 
				 std::bind2nd(PartialUtils::label_equals(), label) );
		
	if ( it != partials.end() ) 
	{
		ret = &(*it);

		//	there should only be one of such Partial, verify this on first frame:
		if ( firstFrameFlag )
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
static double crop( const std::list< Partial > & partials, double endTime, const ExportSpc & es )
{
	
	// Find max amp sum.
	double tim;
	double maxampsum = 0.0;
	for ( tim = 0.0; tim <= endTime; tim += es.hop() ) 
	{
	
		// Compute sum of amp of all partials at this time.
		double ampsum = 0.0;
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
	for ( tim = 0.0; tim <= endTime; tim += es.hop() ) 
	{

		// Compute sum of amp of all partials at this time.
		double ampsum = 0.0;
		for ( std::list< Partial >::const_iterator it = partials.begin(); it != partials.end(); ++it ) 
		{
			if ( it->begin() != it->end() )
				ampsum += it->amplitudeAt( tim );
		}
		
		// If we hit the amp level, we will crop to here; exit.
		if (ampsum > es.attackThreshold() * maxampsum)
			return tim;
	}
	
	return 0.;
}


#pragma mark -
#pragma mark envelope writing
// ---------------------------------------------------------------------------
//	writeEnvelopes
// ---------------------------------------------------------------------------
//	Let exceptions propogate.
//	The plist should be labeled and distilled before this is called.
//
static void writeEnvelopes( std::ostream & s, const list<Partial> & plist, 
				double startTime, double endTime, const ExportSpc & es)
{	
	
	// the label for the reference partial must be non-zero:
	if( es.refLabel() == 0 )
		Throw( FileIOException, "Label for reference partial is zero." );
	
	// make sure the reference partial is there:
	const Partial * refPar = select( plist, es.refLabel() );
	if ( refPar == NULL )
		Throw( FileIOException, "No partial has the reference partial label!" );
	if ( refPar->begin() == refPar->end() )
		Throw( FileIOException, "Reference partial has zero length." );

	// write out one frame at a time:
	for (double tim = startTime; tim <= endTime; tim += es.hop() ) 
	{
	
		//	for each frame, write one value for every partial:
		for (unsigned int label = 1; label <= es.numPartials(); ++label ) 
		{
			double freqMult;			// frequency multiplier for partial	
			double magMult;			// magnitude multiplier for partial	

			// find partial with the correct label:
			// if no partial is found, frequency multiply the reference partial 
			const Partial * pcorrect = select( plist, label, tim == startTime );
		
			if ( pcorrect == NULL || pcorrect->begin() == pcorrect->end() ) 
			{
				pcorrect = refPar;
				freqMult = (double) label / (double) es.refLabel(); 
				magMult = 0.0;
			} 
			else if (!es.endApproachTime() && tim > endTime - es.hop()) 
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
			double endApproachWeight = 1.0;
			if (!es.endApproachTime() && tim > endTime - es.endApproachTime())
				endApproachWeight = (endTime - tim) / es.endApproachTime();

			//	pack log amplitude and log frequency into 24-bit lval:
			//  The log frequency value sticks at the release frame's frequency value.
			unsigned long lval = packLeft( *pcorrect, freqMult, magMult, tim, endApproachWeight, es );

			//	pack log bandwidth and phase into 24-bit right:
			unsigned long rval = packRight( *pcorrect, magMult, tim, endApproachWeight, es );
	
			//	write integer samples without byte swapping,
			//	they are already correctly packed: 
			BigEndian::write( s, 3, 1, (char*)&lval );
			if (es.enhanced())
				BigEndian::write( s, 3, 1, (char*)&rval );
		}
	}
}

// ---------------------------------------------------------------------------
//	writeEnvelopeData
// ---------------------------------------------------------------------------
//
static void writeEnvelopeData( std::ostream & s, const list<Partial> & plist, 
					double startTime, double endTime, const ExportSpc & es )
{
	//	first build a Sound Data chunk, so that all the data sizes will 
	//	be correct:
	SoundDataCk ck;
	ck.header.id = SoundDataId;
	
	//	size is everything after the header:
	ck.header.size = sizeofSoundData( startTime, endTime, es ) - sizeofCkHeader();
				
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

	 	writeEnvelopes( s, plist, startTime, endTime, es);
	}
	catch( FileIOException & ex ) 
	{
		ex.append(" Failed to write SPC file SoundData chunk.");
		throw;
	}
}


#pragma mark -
#pragma mark export spc
// ---------------------------------------------------------------------------
//	configure
// ---------------------------------------------------------------------------
//	Compute default values for analysis parameters from the single core
//	parameter, the midi note number.
//
void
ExportSpc::configure( double midiPitch )
{
	//	note number (69.00 = A440) for spc file;
	//	this is the core parameter, others are, by default,
	//	computed from this one	
	_midiPitch = midiPitch;

	//	number of partials in spc file, may be 32, 64, 128, 256, or 512;
	//	this default assumes one partial per harmonic.
	if (_midiPitch > 72.5)
		_numPartials = 32;
	else if (_midiPitch > 60.5)
		_numPartials = 64;
	else if (_midiPitch > 48.5)
		_numPartials = 128;
	else if (_midiPitch > 36.5)
		_numPartials = 256;
	else 
		_numPartials = 512;
	
	// 	label for reference partial (1 indicates fundamental),
	// 	used for filling in frequencies during time gaps in other partials
	_refLabel = 2;	
			
	//	true for bandwidth-enhanced spc file format,
	//	false for pure sinusoidal spc file format
	_enhanced = true;		
		
	//	in seconds, frame duration for spc file
	_hop = 0.005;
	
	//	fraction of maximum amplitude of the sound, this provides an amplitude 
	//	threshold for cropping the start of the spc file, 
	//	0.0 indicates no cropping
	_attackThreshold = 0.0;
	
	//	in seconds, this indicates time at which frequency in attack is
	//	considered stable, frequencies before this time are modified in
	//	the spc file to avoid real-time morphing artifacts, 0.0 indicates
	//	the spc file is to have no attack frequencies modified
	 _startFreqTime = 0.0;		
	 
	//	in seconds, this indicates the time at which to truncate the end
	//	of the spc file; we set it to zero here, write() will use end of file
	//	if user does not overwrite this zero. 
	_endTime = 0.0;

	//	in seconds, this indicates how long before _endTime the
	//	amplitude, frequency, and bandwidth values are to be modified to
	//	make a gradual transition to the spectral content at _endTime,
	//	0.0 indicates no such modifications are to be don
	_endApproachTime = 0.0;
	
	//	in seconds, this indicates time at which a marker is inserted in the
	//	spc file, 0.0 indicates no marker is desired
	_markerTime = 0.0;		
}

// ---------------------------------------------------------------------------
//	write
// ---------------------------------------------------------------------------
// The plist should be labeled and distilled before this is called.
//
void
ExportSpc::write( const std::string & filename, const list<Partial> & plist )
{
	std::ofstream s;
	s.open( filename.c_str(), std::ios::out | std::ios::binary ); 
	
	write( s, plist );
}

// ---------------------------------------------------------------------------
//	write
// ---------------------------------------------------------------------------
// The plist should be labeled and distilled before this is called.
//
void
ExportSpc::write( ostream & file, const list<Partial> & plist )
{
	// Max number of partials is due to (arbitrary) size of initPhase[].
	if (_numPartials != 32 && _numPartials != 64 
				&& _numPartials != 128 && _numPartials != 256 && _numPartials != 512)
		Throw( FileIOException, "Number of partials must be 32, 64, 128, 256, or 512." );

	// Find the end time, if it is not set yet. 
	double endTime = _endTime;	
	if (!endTime)
	{
		for ( std::list< Partial >::const_iterator it = plist.begin(); it != plist.end(); ++it ) 
		{
			if ( it->begin() != it->end() && it->endTime() > endTime)
				endTime = it->endTime();
		}
	}
	
	// Find starting frame after cropping.
	double startTime = crop( plist, endTime, *this );

	debugger << "startTime = " << startTime << " endTime = " << endTime 
			 << " hop = " << _hop << " attackThreshold = " << _attackThreshold 
			 << " ref label = " << _refLabel << endl;
	
	// Write out all the chunks.
	try 
	{
		writeContainer( file, startTime, endTime, *this );
		writeCommon( file, startTime, endTime, *this );
		writeInstrument( file, *this );
		if (_markerTime)
			writeMarker( file, *this );
		writeSosEnvelopesChunk( file, startTime, endTime, *this );
		writeEnvelopeData( file, plist, startTime, endTime, *this );
	}
	catch ( Exception & ex ) 
	{
		ex.append(" Failed to write SPC file.");
		throw;
	}
}


// ---------------------------------------------------------------------------
//	ExportSpc constructor
// ---------------------------------------------------------------------------
//	The core analysis parameter is the midi note number. Configure all other
//	parameters according to this parameter, subsequent parameter mutations
//	will be independent.
//
ExportSpc::ExportSpc( double midiPitch )
{
	configure( midiPitch );
}


// ---------------------------------------------------------------------------
//	ExportSpc destructor
// ---------------------------------------------------------------------------
//	Destroy this ExportSpc instance.
//
ExportSpc::~ExportSpc( void )
{
}


#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif



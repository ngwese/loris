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
 * SpcFile.C
 *
 * Implementation of class SpcFile, which reads and writes SPC files.
 *
 * Lippold Haken, 6 Nov 1999, 14 Nov 2000, export spc
 * Lippold Haken, 4 Feb 2001, import spc
 * Lippold Haken, 19 Mar 2001, combine import and export
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include <config.h>
#endif

#include<SpcFile.h>
#include<AiffFile.h>
#include<Exception.h>
#include<Endian.h>
#include<Partial.h>
#include<PartialUtils.h>
#include<Notifier.h>
#include "ieee.h"
#include "Loris_types.h"
#include <climits>
#include <cmath>
#include <vector>
#include <list>
#include <algorithm>
#include <fstream>

#if HAVE_M_PI
	const double Pi = M_PI;
#else
	const double Pi = 3.14159265358979324;
#endif

using namespace std;

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif




#pragma mark spc helpers
// ---------------------------------------------------------------------------
//	fileNumPartials
// ---------------------------------------------------------------------------
//	Find number of partials in SOS file.  This is the actual number of partials,
//  plus padding to make a 2**n value. 
//
#define largestLabel 512	// max number of partials for SPC file
static int fileNumPartials( int partials )
{
	if ( partials <= 32 )	
		return 32;
	if ( partials <= 64 )	
		return 64;
	if ( partials <= 128 )	
		return 128;
	else if ( partials <= 256 )
		return 256;
	else if ( partials <= largestLabel )
		return largestLabel;

	Throw( FileIOException, "Too many SPC partials!" );
	return largestLabel;
}

// ---------------------------------------------------------------------------
//	envLog( )
// ---------------------------------------------------------------------------
//	For a range 0 to 1, this returns a log value, 0x0000 to 0xFFFF.
//
static unsigned long envLog( double floatingValue )
{
	static double coeff = 65535.0 / log( 32768. );

	return (unsigned long)( coeff * log( 32768.0 * floatingValue + 1.0 ) );

}	//  end of envLog( )

// ---------------------------------------------------------------------------
//	envExp( )
// ---------------------------------------------------------------------------
//	For a range 0x0000 to 0xFFFF, this returns an exponentiated value in the range 0..1.
//  This is the counterpart of SpcFile::envLog().
//
static double envExp( long intValue )
{
	static double coeff = 65535.0 / log( 32768. );

	return ( exp( intValue / coeff ) - 1.0 ) / 32768.0;

}	//  end of envExp( )





#pragma mark -
#pragma mark import spc
// ---------------------------------------------------------------------------
//	processPoint
// ---------------------------------------------------------------------------
//	Add breakpoint to existing Loris partials.
//
static void
processPoint( const int left, const int right, 
				const double frameTime, 
				Partial & par )
{
//
// Unpack values.  Counterpart to ExportSpc::packLeft() and ExportSpc::packRight().
//
	double freq = 		envExp( (left >> 8)   & 0xffff ) * 22050.0;
	double sineMag =	envExp( (left >> 15)  & 0xfe00 );
	double noiseMag =	envExp( (right >> 15) & 0xfe00 ) / 64.;
	double phase = 			  ( (right >> 8)  & 0xffff ) * ( 2. * Pi / 0xffff );
	
	double total = sineMag * sineMag + noiseMag * noiseMag;

	double amp = sqrt( total );
	
	double noise = 0.;
	if (total != 0.)
		noise = noiseMag * noiseMag / total;
	if (noise > 1.)
		noise = 1.;
	
	phase -= Pi / 2.;
	if (phase < 0.)
		phase += 2. * Pi;

//
// Create a new breakpoint and insert it.
//	
	Breakpoint newbp( freq, amp, noise, phase );
	par.insert( frameTime, newbp );
}



// ---------------------------------------------------------------------------
//	read
// ---------------------------------------------------------------------------
// Let exceptions propagate.
//
static void
read( const char *infilename, std::list<Partial> & partialsList )
{

	try
	{
		//	read spc aiff file
		AiffFile f( infilename );
		std::vector< double > v( f.sampleFrames() );
		f.getSamples( v.begin(), v.end() );
		
		//  mono file format has number of partials doubled
		//  stereo file format has proper number of partials
		int frames = f.frames();
		double hop = f.hop();
		int partials = (f.channels() == 1) ? f.partials() / 2 : f.partials();
		
		//  check for valid file
		if ( partials == 0 || f.sampleSize() != 24 )
				Throw( FileIOException, "Not an SPC file." );
		if ( partials < 32 || partials > largestLabel )
				Throw( FileIOException, "Bad number of partials in SPC file." );
		if ( fileNumPartials( partials ) * frames * 2 != v.end() - v.begin() )
				Throw( FileIOException, "Unexpected number of samples in SPC file." );
		
		//  allocate partialsVector
		std::vector< Partial > partialsVector;
		partialsVector.resize( partials );

		//  process SPC data points
		double *vpt = v.begin();
		for (int frame = 0; frame < frames; ++frame) 
		{
			for (int partial = 0; partial < fileNumPartials( partials ); ++partial)
			{
				int left = int ( LONG_MAX * *vpt++ + 0.5 );
				int right = int ( LONG_MAX * *vpt++ + 0.5 );
				if ( partial < partialsVector.size() )
					processPoint( left, right, frame * hop, partialsVector[partial] );
			}
		}
	
		//  copy partialsVector to partialsList
		for (int i = 0; i < partialsVector.size(); ++i)
		{
			partialsVector[i].setLabel( i + 1 );
			partialsList.push_back( partialsVector[i] );
		}
	}
	catch ( Exception & ex ) 
	{
		partialsList.clear();
		ex.append(" Failed to read SPC file.");
		throw;
	}
	
//
// Complain if no Partials were imported:
//
	if ( partialsList.size() == 0 )
	{
		notifier << "No Partials were imported from " << infilename << endl;
	}
	
}


// ---------------------------------------------------------------------------
//	SpcFile constructor from data in memory
// ---------------------------------------------------------------------------
//
SpcFile::SpcFile( const char *infilename ) 
{
	read( infilename, _partialsList );
}

#pragma mark -
#pragma mark -
#pragma mark export structures
// ---------------------------------------------------------------------------
//	Export Structures
// ---------------------------------------------------------------------------
//
#pragma mark spcEI
//  structure for export information
struct SpcExportInfo
{
	double midipitch;		//	note number (69.00 = A440) for spc file;
							//	this is the core parameter, others are, by default,
							//	computed from this one
	double endApproachTime;	//	in seconds, this indicates how long before the end of the sound the
							//	amplitude, frequency, and bandwidth values are to be modified to
							//	make a gradual transition to the spectral content at the end,
							//	0.0 indicates no such modifications are to be done
	int numPartials;		//	number of partials in spc file
	int fileNumPartials;	//	the actual number of partials plus padding to make a 2**n value
	int enhanced;			//	true for bandwidth-enhanced spc file, false for pure sines
	double startTime;		//  in seconds, time of first frame in spc file
	double endTime;			//	in seconds, this indicates the time at which to truncate the end
							//	of the spc file, 0.0 indicates no truncation
	double markerTime;		//	in seconds, this indicates time at which a marker is inserted in the
							//	spc file, 0.0 indicates no marker is desired
	double sampleRate;		//	in hertz, intended sample rate for synthesis of spc file
	double hop;				//  hop size, based on numPartials and sampleRate
	double ampEpsilon;		//  small amplitude value (related to lsb value in spc file log-amp)
};

static struct SpcExportInfo spcEI;		// spc Export information
	

// Temporary support for using old-style envelope reader for enhanced spc files in Kyma.
// Set MONO_ENH to 1 for old-style reader compatability, 
// set MONO_ENH to 0 for enhanced envelope reader.
// The issues for bandwidth-enhanced spc files are:
//		- file is mono, not stereo (despite the fact that it contain BW&phase)
//		- file claims to have twice the number of partials
//		- file claims to have twice the number of samples (2 mono samples = 1 stereo)
#define MONO_ENH 1


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
#define initPhaseLth ( 4*largestLabel + 8 )
	Int_32	initPhase[initPhaseLth]; // obsolete initial phase array; is VARIABLE LENGTH array 
							// this is big enough for a max of 512 enhanced partials plus values below
//	Int_32	resolution;		// frame duration in microseconds 
	#define SOSresolution( es )   initPhase[(spcEI.enhanced && MONO_ENH) \
											? 2 * spcEI.numPartials : spcEI.numPartials]	
							// follows the initPhase[] array
//	Int_32	quasiHarmonic;	// how many of the partials are quasiharmonic
	#define SOSquasiHarmonic( es )  initPhase[(spcEI.enhanced && MONO_ENH) \
											? 2 * spcEI.numPartials + 1 : spcEI.numPartials + 1]	
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
static unsigned long sizeofSoundData( )
{
	int frames = int( ( spcEI.endTime - spcEI.startTime ) / spcEI.hop ) + 1;
	unsigned long dataSize = frames * spcEI.fileNumPartials * ( 24 / 8 ) * (spcEI.enhanced ? 2 : 1);
	
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
static void writeCommon( std::ostream & s )
{
	//	first build a Common chunk, so that all the data sizes will 
	//	be correct:
	CommonCk ck;
	ck.header.id = CommonId;
	
	//	size is everything after the header:
	ck.header.size = sizeofCommon() - sizeofCkHeader();

	int frames = int( ( spcEI.endTime - spcEI.startTime ) / spcEI.hop ) + 1;
	if ( spcEI.enhanced ) 			// bandwidth-enhanced spc file
	{					
		ck.channels = (MONO_ENH ? 1 : 2);
		ck.sampleFrames = frames * spcEI.fileNumPartials * (MONO_ENH ? 2 : 1);
	}
	else							// pure sinusoidal spc file
	{					
		ck.channels = 1;
		ck.sampleFrames = frames * spcEI.fileNumPartials;
	}
	
	ck.bitsPerSample = 24;
	// The sample rate is not directly used in SPC files.  It indicates the
	// intended sample rate at which the SPC file should be synthesized.
	IEEE::ConvertToIeeeExtended( spcEI.sampleRate, & ck.srate );
	
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
static void writeContainer( std::ostream & s )
{
	//	first build a Container chunk, so that all the data sizes will 
	//	be correct:
	ContainerCk ck;
	ck.header.id = ContainerId;
	
	//	size is everything after the header:
	ck.header.size = sizeof(Int_32) + sizeofCommon() 
				+ sizeofInstrument() 
				+ (spcEI.markerTime ? sizeofMarker() : 0)
				+ sizeofSosEnvelopes()
				+ sizeofSoundData();
	
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
static void writeInstrument( std::ostream & s )
{
	//	first build an Inst chunk, so that all the data sizes will 
	//	be correct:
	InstrumentCk ck;
	ck.header.id = InstrumentId;
	
	//	size is everything after the header:
	ck.header.size = sizeofInstrument() - sizeofCkHeader();
	
	ck.baseFrequency = long( spcEI.midipitch );
	ck.detune = long( 100 * spcEI.midipitch ) % 100;
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
static void writeMarker( std::ostream & s )
{
	//	first build a Marker chunk, so that all the data sizes will 
	//	be correct:
	MarkerCk ck;
	ck.header.id = MarkerId;
	
	//	size is everything after the header:
	ck.header.size = sizeofMarker() - sizeofCkHeader();
	
	ck.numMarkers = 1;					// one marker
	ck.aMarker.id = 1;					// first marker 
	ck.aMarker.position = int( spcEI.markerTime / spcEI.hop ) *  spcEI.fileNumPartials 
							* ((spcEI.enhanced && MONO_ENH) ? 2 : 1); 
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
static void writeSosEnvelopesChunk( std::ostream & s )
{
	//	first build an SOSe chunk, so that all the data sizes will 
	//	be correct:
	SosEnvelopesCk ck = { 0 };
	ck.header.id = ApplicationSpecificId;
	
	//	size is everything after the header:
	ck.header.size = sizeofSosEnvelopes() - sizeofCkHeader();
					
	ck.signature = SosEnvelopesId;

	ck.frames = int(( spcEI.endTime - spcEI.startTime ) / spcEI.hop) + 1;
	ck.validPartials = spcEI.numPartials * ((spcEI.enhanced && MONO_ENH) ? 2 : 1);
	
	//	resolution in microseconds
	ck.SOSresolution( es ) = long( 1000000.0 * spcEI.hop );
	
	//	all partials quasiharmonic
	ck.SOSquasiHarmonic( es ) =  spcEI.numPartials * ((spcEI.enhanced && MONO_ENH) ? 2 : 1); 
	
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
//	getPhaseRefTime
// ---------------------------------------------------------------------------
//  Find the time at which to reference phase.
//	The time will be shortly after amplitude onset, if we are before the onset.
//
static double getPhaseRefTime( int label, const Partial & p, double time  )
{
// Keep array of previous values to optimize spc export.
// This depends on this routine being called in increasing-time order.
	static double prevPRT[largestLabel + 1];
	if ( prevPRT[label] > time && time > spcEI.startTime )
		return prevPRT[ label ]; 
			
// Go forward to nonzero amplitude.
	while ( p.amplitudeAt( time ) < spcEI.ampEpsilon && time < spcEI.endTime + spcEI.hop)
	{
		time += spcEI.hop;
	}

	prevPRT[ label ] = time;

// Use phase value at initial onset time.
	return time;
		
}

// ---------------------------------------------------------------------------
//	afbp
// ---------------------------------------------------------------------------
//	Find amplitude, frequency, bandwidth, phase value.  
//
static void afbp( const Partial & p, double time, double phaseRefTime,
						double magMult, double freqMult, 
						double & amp, double & freq, double & bw, double & phase)
{	
	
// Optional endApproachTime processing:
// Approach amp, freq, and bw values at endTime, and stick at endTime amplitude.
// We avoid a sudden transition when using stick-at-end-frame sustains.
// Compute weighting factor between "normal" envelope point and static point.
	if ( spcEI.endApproachTime && time > spcEI.endTime - spcEI.endApproachTime )
	{
		if ( time > p.endTime() && p.endTime() > spcEI.endTime - 2 * spcEI.hop)
			time = p.endTime();
		double wt = ( spcEI.endTime - time ) / spcEI.endApproachTime;
		amp   = magMult  * ( wt * p.amplitudeAt( time ) + (1.0 - wt) * p.amplitudeAt( spcEI.endTime )  );
		freq  = freqMult * ( wt * p.frequencyAt( time ) + (1.0 - wt) * p.frequencyAt( spcEI.endTime )  );
		bw    =            ( wt * p.bandwidthAt( time ) + (1.0 - wt) * p.bandwidthAt( spcEI.endTime )  );
		phase = p.phaseAt( time );
	}
	
// If we are before the phase reference time, or on the final frame,
// use zero amp and offset phase.
	else if ( time < phaseRefTime - spcEI.hop / 2 || time > spcEI.endTime - spcEI.hop / 2 )
	{
		amp = 0.;
		freq = freqMult * p.frequencyAt( phaseRefTime );
		bw = 0.;
		phase = p.phaseAt( phaseRefTime ) - 2. * Pi * (phaseRefTime - time) * freq;
	}
	
// Use envelope values at "time".
	else
	{
		amp = magMult * p.amplitudeAt( time );
		freq = freqMult * p.frequencyAt( time );
		bw = p.bandwidthAt( time );
		phase = p.phaseAt( time );
	}
}


// ---------------------------------------------------------------------------
//	pack
// ---------------------------------------------------------------------------
//	Pack envelope breakpoint value for interpretation by Envelope Reader sounds
//	in Kyma.  The packed result is two 24-bit quantities, lval and rval.
//
//  In lval, the log of the sine magnitude occupies the top 8 bits, the log of the
//	frequency occupies the bottom 16 bits.
//
//	In rval, the log of the noise magnitude occupies the top 8 bits, the scaled
//	linear phase occupies the bottom 16 bits.  
//
static void pack( double amp, double freq, double bw, double phase,
						unsigned long &lval, unsigned long &rval )
{	

// Set phase for one hop earlier, so that Kyma synthesis target phase is correct.
// Add offset to phase for difference between Kyma and Loris representation.
	phase -= 2. * Pi * spcEI.hop * freq; 
	phase += Pi / 2;

// Make phase into range 0..1.	
	phase = std::fmod( phase, 2. * Pi );
	if ( phase < 0. )
		phase += 2. * Pi; 
	double zeroToOnePhase = phase / (2. * Pi);

// Make frequency into range 0..1.
	double zeroToOneFreq = freq / 22050.0;		// 0..1 , 1 is 22.050 kHz

// Compute sine magnitude and noise magnitude from amp and bw.	
	double theSineMag = amp * sqrt( 1. - bw );
	double theNoiseMag = 64.0 * amp * sqrt( bw );
	if (theNoiseMag > 1.0)
		theNoiseMag = 1.0;

// Pack lval:	
// 7 bits of log-sine-amplitude with 24 bits of zero to right.
// 16 bits of log-frequency with 8 bits of zero to right.
	lval = ( envLog( theSineMag ) & 0xFE00 ) << 15;
	lval |= ( envLog( zeroToOneFreq ) & 0xFFFF ) << 8;

// Pack rval:
// 7 bits of log-noise-amplitude with 24 bits of zero to right.
// 16 bits of phase with 8 bits of zero to right.
	rval = ( envLog( theNoiseMag ) & 0xFE00 ) << 15;
	rval  |= ( (unsigned long) ( zeroToOnePhase * 0xFFFF ) ) << 8;
}


// ---------------------------------------------------------------------------
//	select
// ---------------------------------------------------------------------------
//	Special select function that returns a pointer to the Partial
//	having the specified label, or NULL if there is not such Partial
//	in the list. 
//
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



#pragma mark -
#pragma mark envelope writing
// ---------------------------------------------------------------------------
//	writeEnvelopes
// ---------------------------------------------------------------------------
//	Let exceptions propogate.
//	The plist should be labeled and distilled before this is called.
//
static void writeEnvelopes( std::ostream & s, const list<Partial> & plist)
{	
	const int refLabel = 1;			// label of reference partial; always use fundamental
	
	// make sure the reference partial is there:
	const Partial * refPar = select( plist, refLabel, false );
	if ( refPar == NULL )
		Throw( FileIOException, "You do not have a partial labeled 1." );
	if ( refPar->begin() == refPar->end() )
		Throw( FileIOException, "Partial labeled 1 has zero length." );

	// write out one frame at a time:
	for (double tim = spcEI.startTime; tim <= spcEI.endTime; tim += spcEI.hop ) 
	{
	
		//	for each frame, write one value for every partial:
		//  (this loop extends to the pad partials)
		for (unsigned int label = 1; label <= spcEI.fileNumPartials; ++label ) 
		{
			// find partial with the correct label
			const Partial * pcorrect = select( plist, label, tim == spcEI.startTime );
		
			// if no such partial, frequency multiply the reference partial
			double freqMult = 1.;			// frequency multiplier for partial	
			double magMult  = 1.;			// magnitude multiplier for partial	
			if ( pcorrect == NULL || pcorrect->begin() == pcorrect->end() ) 
			{
				pcorrect = refPar;
				freqMult = (double) label / (double) refLabel; 
				magMult = 0.0;
			} 
			
			//  find the reference time for the phase
			double phaseRefTime = getPhaseRefTime( label, *pcorrect, tim );
			
			//  find amplitude, frequency, bandwidth, phase value
			double amp, freq, bw, phase;
			afbp( *pcorrect, tim, phaseRefTime, magMult, freqMult, amp, freq, bw, phase);

			//	pack log amplitude and log frequency into 24-bit lval,
			//  log bandwidth and phase into 24-bit rval:
			unsigned long lval, rval;
			pack( amp, freq, bw, phase, lval, rval);
	
			//	write integer samples without byte swapping,
			//	they are already correctly packed: 
			BigEndian::write( s, 3, 1, (char*)&lval );
			if (spcEI.enhanced)
				BigEndian::write( s, 3, 1, (char*)&rval );
		}
	}
}

// ---------------------------------------------------------------------------
//	writeEnvelopeData
// ---------------------------------------------------------------------------
//
static void writeEnvelopeData( std::ostream & s, const list<Partial> & plist )
{
	//	first build a Sound Data chunk, so that all the data sizes will 
	//	be correct:
	SoundDataCk ck;
	ck.header.id = SoundDataId;
	
	//	size is everything after the header:
	ck.header.size = sizeofSoundData() - sizeofCkHeader();
				
	//	no block alignment:	
	ck.offset = 0L;
	ck.blockSize = 0;
	
	//	write it out:
	try 
	{
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.header.id );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.header.size );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.offset );
		BigEndian::write( s, 1, sizeof(Int_32), (char *)&ck.blockSize );

	 	writeEnvelopes( s, plist );
	}
	catch( FileIOException & ex ) 
	{
		ex.append(" Failed to write SPC file SoundData chunk.");
		throw;
	}
}



#pragma mark -
#pragma mark derived parameters


// ---------------------------------------------------------------------------
//	hop
// ---------------------------------------------------------------------------
//	Find the hop size, based on number of partials and sample rate. 
//
static double hop( int numPartials, double sampleRate )
{
	return 2 * numPartials / sampleRate;
}


// ---------------------------------------------------------------------------
//	startTime
// ---------------------------------------------------------------------------
//	Find the start time: the earliest time of any labeled partial.
//
static double startTime( const std::list<Partial> & pars )
{
	double startTime = 1000.;
	for ( std::list< Partial >::const_iterator pIter = pars.begin(); pIter != pars.end(); ++pIter )
		if ( pIter->begin() != pIter->end() && startTime > pIter->startTime() && pIter->label() > 0 )
			startTime = pIter->startTime();
	return startTime;
}


// ---------------------------------------------------------------------------
//	endTime
// ---------------------------------------------------------------------------
//	Find the end time: the latest time of any labeled partial.
//
static double endTime( const std::list<Partial> & pars )
{
	double endTime = -1000.;
	for ( std::list< Partial >::const_iterator pIter = pars.begin(); pIter != pars.end(); ++pIter )
		if ( pIter->begin() != pIter->end() && endTime < pIter->endTime()  && pIter->label() > 0 )
			endTime = pIter->endTime();
	return endTime;
}


// ---------------------------------------------------------------------------
//	numPartials
// ---------------------------------------------------------------------------
//	Find the number of partials.
//
static long numPartials( const std::list<Partial> & pars )
{

// We purposely consider partials with no breakpoints, to allow
// a larger number of partials than actually have data.
	int numPartials = 0;
	for ( std::list< Partial >::const_iterator pIter = pars.begin(); pIter != pars.end(); ++pIter )
		if ( numPartials < pIter->label() )
			numPartials = pIter->label();

// To ensure a reasonable hop time, make at least 32 partials.
	return numPartials ? max( 32, numPartials ) : 0;
}


#pragma mark -
#pragma mark export spc

// ---------------------------------------------------------------------------
//	Export
// ---------------------------------------------------------------------------
// The plist should be labeled and distilled before this is called.
//
// The last two arguments, enhanced and endApproachTime, are normally left at their
// default value (and not specified by the caller).
//
void
SpcFile::Export( const std::string & filename, const std::list<Partial> & plist, double midipitch, 
					int enhanced, double endApproachTime )
{
	std::ofstream s;
	s.open( filename.c_str(), std::ios::out | std::ios::binary ); 
	
	Export( s, plist, midipitch, enhanced, endApproachTime );
}

// ---------------------------------------------------------------------------
//	Export
// ---------------------------------------------------------------------------
// The plist should be labeled and distilled before this is called.
//
// The last two arguments, enhanced and endApproachTime, are normally left at their
// default value (and not specified by the caller).
//
void
SpcFile::Export( std::ostream & file, const std::list<Partial> & plist, double midipitch, 
						int enhanced, double endApproachTime )
{	
	//	note number (69.00 = A440) for spc file
	spcEI.midipitch = midipitch;
	
	//	enhanced indicates a bandwidth-enhanced spc file; by default it is true.
	//	if enhanced is false, no bandwidth or noise information is exported.
	spcEI.enhanced = enhanced;
	
	//	endApproachTime is in seconds; by default it is zero (and has no effect).
	//  a nonzero endApproachTime indicates that the plist does not include a
	//  release, but rather ends in a static spectrum corresponding to the final
	//  breakpoint values of the partials.  the endApproachTime specifies how
	//  long before the end of the sound the amplitude, frequency, and bandwidth
	//  values are to be modified to make a gradual transition to the static spectrum.
	spcEI.endApproachTime = endApproachTime;
	
	//  number of partials in spc file
	spcEI.numPartials = numPartials( plist );
	spcEI.fileNumPartials = fileNumPartials( spcEI.numPartials );
	
	//  start and end time of spc file
	spcEI.startTime = startTime( plist );
	spcEI.endTime = endTime( plist );

	//	in seconds, this indicates time at which a marker is inserted 
	//  in the spc file, 0.0 indicates no marker.  this is not being used currently.
	spcEI.markerTime = 0.;
		
	//	in hertz, intended sample rate for synthesis of spc file
	spcEI.sampleRate = 44100.;		
	
	//  compute hop size
	spcEI.hop = hop( spcEI.numPartials, spcEI.sampleRate );

	//  compute ampEpsilon, a small amplitude value twice the lsb value 
	//  of log amp in packed spc format. 
	spcEI.ampEpsilon = 2. * envExp( 0x200 );

	// Max number of partials is due to (arbitrary) size of initPhase[].
	if (spcEI.numPartials < 1 || spcEI.numPartials > largestLabel )
		Throw( FileIOException, "Partials must be distilled and labeled between 1 and 512." );

	debugger << "startTime = " << spcEI.startTime << " endTime = " << spcEI.endTime 
			 << " hop = " << spcEI.hop << " partials = " << spcEI.numPartials << endl;
	
	// Write out all the chunks.
	try 
	{
		writeContainer( file );
		writeCommon( file );
		writeInstrument( file );
		if (spcEI.markerTime)
			writeMarker( file );
		writeSosEnvelopesChunk( file );
		writeEnvelopeData( file, plist );
	}
	catch ( Exception & ex ) 
	{
		ex.append(" Failed to write SPC file.");
		throw;
	}
}


#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif



// ===========================================================================
//	SpcFile.C
//	
//	Association of info	(number of partials, number of frames, frame rate, 
//	and midiPitch)	to completely specify an spc file.  The spc files are exported for
//	real-time synthesis in Kyma.
//
//	-lip 6 Nov 99
//
// ===========================================================================

#include "LorisLib.h"

#include "SpcFile.h"
#include "BinaryFile.h"
#include "Partial.h"

#include "Morph.h"

#include "ieee.h"

#include <algorithm>
#include <string>

// Temporary hacks for using old-style envelope reader in Kyma.
// Set MONOFILE to 1 for hacks, set MONOFILE to 0 for new envelope reader.
// The hacks include:
//		- file is mono, not stereo (despite the fact that it contain BW&phase)
//		- file claims to have twice the number of partials
//		- file claims to have twice the number of samples (2 mono samples = 1 stereo)
#define MONOFILE 1


using namespace std;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	SpcFile constructor from data in memory
// ---------------------------------------------------------------------------
//
SpcFile::SpcFile( int pars, int frames, double rate, double midiPitch, double minNoiseFreq ) :
	_partials( pars ),
	_frames( frames ),
	_rate( rate ),
	_midiPitch( midiPitch ),
	_minNoiseFreq( minNoiseFreq )
{
	Assert( pars == 32 || pars == 64 || pars == 128 );
}

// ---------------------------------------------------------------------------
//	write
// ---------------------------------------------------------------------------
// The plist should be labeled and distilled before this is called.
//
void
SpcFile::write( BinaryFile & file, const list<Partial> & plist )
{
	
	try {
		//	rewind:
		file.seek(0);
		if( file.tell() != 0 )
			Throw( FileIOException, "Couldn't rewind SPC file (bad open mode?)." );
		file.setBigEndian();
		
		writeContainer( file );
		writeCommon( file );
		writeInstrument( file );
		writeSosEnvelopesChunk( file );
		writeEnvelopeData( file, plist );
	}
	catch ( Exception & ex ) {
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
void
SpcFile::writeEnvelopeData( BinaryFile & file, const list<Partial> & plist )
{
	//	first build a Sound Data chunk, so that all the data sizes will 
	//	be correct:
	SoundDataCk ck;
	ck.header.id = SoundDataId;
	
	//	size is everything after the header:
	ck.header.size = sizeofSoundData() - sizeofCkHeader();
				
	//	no block alignment:	
	ck.offset = 0.;
	ck.blockSize = 0;
	
	//	write it out:
	try {
		file.write( ck.header.id );
		file.write( ck.header.size );
		file.write( ck.offset );
		file.write( ck.blockSize );

		writeEnvelopes( file, plist );
	}
	catch( FileIOException & ex ) {
		ex.append("Failed to write SPC file Container chunk.");
		throw;
	}
}

// ---------------------------------------------------------------------------
//	writeEnvelopes
// ---------------------------------------------------------------------------
//	Let exceptions propogate.
//	The plist should be labeled and distilled before this is called.
//
void
SpcFile::writeEnvelopes( BinaryFile & file, const list<Partial> & plist )
{	
	double freqMult;				// frequency multiplier for partial	
	double ampMult;					// frequency multiplier for partial	
	pcm_sample left,right;			// packed value for left, right channel in spc file
	
	//	find reference partial label:
	//	the reference partial is used to fill in frequency data for missing partials.
	uint refLabel = findRefPartial( plist );
	Assert( refLabel != 0 );

	// write out one frame at a time:
	for (ulong frame = 0; frame < _frames; ++frame ) {
		cout << "writing frame " << frame << " of " << _frames << endl;
	
		//	for each frame, write one value for every partial:
		for (uint label = 1; label <= _partials; ++label ) {

			// find partial with the correct label:
			// if no partial is found, frequency multiply the reference partial 
			const Partial * pcorrect = select( plist, label );
		
			if ( pcorrect == Null || pcorrect->begin() == pcorrect->end() ) {
				pcorrect = select( plist, refLabel );
				freqMult = (double) label / (double) refLabel; 
				ampMult = 0;
			} else {
				freqMult = ampMult = 1.0;
			}
			const Partial & p = *pcorrect;

			//	pack log amplitude and log frequency into left:
			left.s32bits = packLeft(p, freqMult, ampMult, frame * _rate);

			//	pack log bandwidth and phase into right:
			right.s32bits = packRight(p, freqMult, ampMult, frame * _rate);
	
			//	write the sample:
			file.write( left.s24bits );
			file.write( right.s24bits );
			
			if ((left.s32bits & 0xff000000) != 0)
				left.s32bits += 1; // TEMPORARY
		}
	}
	
	//	except if there were any write errors:
	//	(better to check earlier?)
	if ( ! file.good() )
		Throw( FileIOException, "Failed to write SPC envelopes.");
}

#pragma mark -
#pragma mark envelope writing helpers
// ---------------------------------------------------------------------------
//	findRefPartial
// ---------------------------------------------------------------------------
//	The plist should be labeled and distilled before this is called.
//	Find the lowest labeled partial.
//
int
SpcFile::findRefPartial( const list<Partial> & plist )
{	
	for (uint label = 1; label <= _partials; ++label ) {
		// return if we can find a partial with this label:
		if ( select( plist, label ) )
			return label;
		
	}
	return 0;		// no labaled partial was found
}

// ---------------------------------------------------------------------------
//	envLog( )
// ---------------------------------------------------------------------------
//	For a range 0 to 1, this returns a log value, 0x0000 to 0xFFFF.
//
ulong
SpcFile::envLog( double floatingValue ) const
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
ulong
SpcFile::packLeft( const Partial & p, double freqMult, double ampMult, double time )
{	
	double amp = ampMult * p.amplitudeAt( time );
	double freq = freqMult * p.frequencyAt( time );
	double bw = p.bandwidthAt( time );

	ulong	theOutput;

	double normalizedFreq	= freq / 22050.0;		// 0..1 , 1 is 22.050 kHz
	
	double theSineMag = amp * sqrt( 1. - bw );
	
// 7 bits of log-sine-amplitude with 24 bits of zero to right.
	theOutput	= ( envLog( theSineMag ) & 0xFE00 ) << 15;
	
// 16 bits of log-frequency with 8 bits of zero to right.
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
ulong
SpcFile::packRight( const Partial & p, double freqMult, double ampMult, double time )
{	
	double amp = ampMult * p.amplitudeAt( time );
	double freq = freqMult * p.frequencyAt( time );
//	double phase = p.phaseAt( time );
	double bw = p.bandwidthAt( time );

	static double phase = 0;  phase += TwoPi / 4;  // TEMPORARY phase test

	ulong	theOutput;
	
	phase = fmod( phase, TwoPi );
	if ( phase < 0. )
		phase += TwoPi;
	double zeroToOnePhase = phase / TwoPi;
	
	double theNoiseMag = 64.0 * amp * sqrt( bw );
//	double theNoiseMag = 0.0;
//	for (int i = -6; i <= 6; i++)
//		theNoiseMag += 64.0 * amp * sqrt( bandwidthAtTime( p, time + 0.015 * i ) );
//	theNoiseMag /= 13.0;

	if (freq < _minNoiseFreq)
		theNoiseMag = 0;
	else if (theNoiseMag > 1.0)
		theNoiseMag = 1.0;
	
// 7 bits of log-noise-amplitude with 24 bits of zero to right.
	theOutput	= ( envLog( theNoiseMag ) & 0xFE00 ) << 15;
	
// 16 bits of phase with 8 bits of zero to right.
	theOutput  |= ( (ulong) ( zeroToOnePhase * 0xFFFF ) ) << 8;
	
// Our 24 bits are in the top of a 32-bit word.
	return theOutput;
}

// ---------------------------------------------------------------------------
//	select
// ---------------------------------------------------------------------------
//	Special select function that returns a pointer to the Partial
//	having the specified label, or Null if there is not such Partial
//	in the list. 
//
static struct LabelIs 
{
	LabelIs( int l ) : _l( l ) {}
	Boolean operator()( const Partial & p ) const { return p.label() == _l; }
	private:
		int _l;	//	the label to search for
};
	
const Partial *
SpcFile::select( const list<Partial> & partials, int label )
{
	const Partial * ret = Null;
	list< Partial >::const_iterator it = 
		find_if( partials.begin(), partials.end(), LabelIs( label ) );
		
	if ( it != partials.end() ) {
		ret = &(*it);
		#if Debug_Loris
		//	there should only be one of such Partial:
		Assert( find_if( ++it, partials.end(), LabelIs( label ) ) == partials.end() );
		#endif
	}
	
	return ret;
}

#pragma mark -
#pragma mark chunk writing helpers
// ---------------------------------------------------------------------------
//	writeCommon
// ---------------------------------------------------------------------------
//
void
SpcFile::writeCommon( BinaryFile & file )
{
	//	first build a Common chunk, so that all the data sizes will 
	//	be correct:
	CommonCk ck;
	ck.header.id = CommonId;
	
	//	size is everything after the header:
	ck.header.size = sizeofCommon() - sizeofCkHeader();
					
	ck.channels = MONOFILE ? 1 : 2;
	ck.sampleFrames = _frames * _partials * (MONOFILE ? 2 : 1);
	ck.bitsPerSample = 24;
	IEEE::ConvertToIeeeExtended( 44100, & ck.srate ); // bogus for SPC files
	
	//	write it out:
	try {
		file.write( ck.header.id );
		file.write( ck.header.size );
		file.write( ck.channels );
		file.write( ck.sampleFrames );
		file.write( ck.bitsPerSample );
		file.write( ck.srate );
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
void
SpcFile::writeContainer( BinaryFile & file )
{
	//	first build a Container chunk, so that all the data sizes will 
	//	be correct:
	ContainerCk ck;
	ck.header.id = ContainerId;
	
	//	size is everything after the header:
	ck.header.size = sizeof(Int_32) + sizeofCommon() + sizeofSosEnvelopes()
				+ sizeofInstrument() + sizeofSoundData();
	
	ck.formType = AiffType;
	
	//	write it out:
	try {
		file.write( ck.header.id );
		file.write( ck.header.size );
		file.write( ck.formType );
	}
	catch( FileIOException & ex ) {
		ex.append( "Failed to write SPC file Container chunk." );
		throw;
	}
}	

// ---------------------------------------------------------------------------
//	writeInstrument
// ---------------------------------------------------------------------------
//
void
SpcFile::writeInstrument( BinaryFile & file )
{
	//	first build an Inst chunk, so that all the data sizes will 
	//	be correct:
	InstrumentCk ck;
	ck.header.id = InstrumentId;
	
	//	size is everything after the header:
	ck.header.size = sizeofInstrument() - sizeofCkHeader();
	
	//	give the user a chance to get the right fundamental frequency:
	
	ck.baseFrequency = trunc( _midiPitch );
	ck.detune = ((long)(100 * _midiPitch)) % 100;
	
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
	ck.sustainLoop.playMode = 0;		// No Looping
	ck.sustainLoop.beginLoop = 0;
	ck.sustainLoop.endLoop = 0;
	ck.releaseLoop.playMode = 0;		// No Looping
	ck.releaseLoop.beginLoop = 0;
	ck.releaseLoop.endLoop = 0;
	
	//	write it out:
	try {
		file.write( ck.header.id );
		file.write( ck.header.size );

		file.write( ck.baseFrequency );
		file.write( ck.detune );
		file.write( ck.lowFrequency );
		file.write( ck.highFrequency );
		file.write( ck.lowVelocity );
		file.write( ck.highVelocity );
		file.write( ck.gain );

		file.write( ck.sustainLoop.playMode );
		file.write( ck.sustainLoop.beginLoop );
		file.write( ck.sustainLoop.endLoop );

		file.write( ck.releaseLoop.playMode );
		file.write( ck.releaseLoop.beginLoop );
		file.write( ck.releaseLoop.endLoop );
	}
	catch( FileIOException & ex ) {
		ex.append( "Failed to write SPC file Instrument chunk." );
		throw;
	}
}

// ---------------------------------------------------------------------------
//	writeSosEnvelopesChunk
// ---------------------------------------------------------------------------
//
void
SpcFile::writeSosEnvelopesChunk( BinaryFile & file )
{
	//	first build an SOSe chunk, so that all the data sizes will 
	//	be correct:
	SosEnvelopesCk ck = { 0 };
	ck.header.id = ApplicationSpecificId;
	
	//	size is everything after the header:
	ck.header.size = sizeofSosEnvelopes() - sizeofCkHeader();
					
	ck.signature = SosEnvelopesId;
	ck.frames = _frames;
	ck.validPartials = _partials * (MONOFILE ? 2 : 1);
	
	ck.SOSresolution = 1000000.0 * _rate;				 // convert secs to microsecs
	ck.SOSquasiHarmonic = _partials * (MONOFILE ? 2 : 1);// all our partials quasiharmonic
	
	//	write it out:
	try {
		file.write( ck.header.id );
		file.write( ck.header.size );
		file.write( ck.signature );
		file.write( ck.frames );
		file.write( ck.validPartials );
		
		// The SOSresultion and SOSquasiHarmonic fields are in the phase table memory.
		for ( int i = 0; i < sizeof( ck.initPhase ) / sizeof( ck.initPhase[0] ); i++)
			file.write( ck.initPhase[i] );
	}
	catch( FileIOException & ex ) {
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
Uint_32
SpcFile::sizeofCkHeader( void )
{
	return	sizeof(Int_32) +	//	id
			sizeof(Uint_32);	//	size
}

// ---------------------------------------------------------------------------
//	sizeofCommon
// ---------------------------------------------------------------------------
//
Uint_32
SpcFile::sizeofCommon( void )
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
Uint_32
SpcFile::sizeofSosEnvelopes( void )
{
	return	sizeof(Int_32) +			//	id
			sizeof(Uint_32) +			//	size
			sizeof(Uint_32) + 			// signature
			sizeof(Uint_32) + 			// frames
			sizeof(Uint_32) + 			// validPartials
			512 * sizeof(Uint_32);		// initPhase[] et al
}

// ---------------------------------------------------------------------------
//	sizeofInstrument
// ---------------------------------------------------------------------------
//
Uint_32
SpcFile::sizeofInstrument( void )
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
			2 * sizeof(Int_16);			// endLoop for sustainLoop and releaseLoop
}

// ---------------------------------------------------------------------------
//	sizeofSoundData
// ---------------------------------------------------------------------------
//	No block alignment, the envelope samples start right after the 
//	chunk header info.
//
Uint_32
SpcFile::sizeofSoundData( void )
{
	Uint_32 dataSize = _frames * _partials * 2 * ( 24 / 8 );
	
	return	sizeof(Int_32) +	//	id
			sizeof(Uint_32) +	//	size
			sizeof(Uint_32) + 	//	offset
			sizeof(Uint_32) + 	//	block size
			dataSize;			//	envelope sample data
}

End_Namespace( Loris )
/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2004 by Kelly Fitz and Lippold Haken
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
 * AiffFile.h
 *
 * Definition of AiffFile class for sample import and export in Loris.
 *
 * Kelly Fitz, 8 Jan 2003 
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "Marker.h"
#include "Synthesizer.h"

#if defined(NO_TEMPLATE_MEMBERS)
#include "PartialList.h"
#endif

#include <memory>
#include <string>
#include <vector>

//	begin namespace
namespace Loris {

class Partial;

// ---------------------------------------------------------------------------
//	class AiffFile
//
//!	Class AiffFile represents sample data in a AIFF-format samples 
//!	file, and manages file I/O and sample conversion. Since the sound
//!	analysis and synthesis algorithms in Loris and the reassigned
//!	bandwidth-enhanced representation are monaural, AiffFile manages
//!	only monaural (single channel) AIFF-format samples files.
//	
class AiffFile
{
//	-- public interface --
public:

//	-- types --

	//!	The type of the sample storage in an AiffFile.
	typedef std::vector< double > samples_type;
	
	//! The type of all size parameters for AiffFile.
	typedef samples_type::size_type size_type;
	
	//! The type of AIFF marker storage in an AiffFile.
	typedef std::vector< Marker > markers_type;

//	-- construction --

	//!	Initialize an instance of AiffFile by importing sample data from
	//!	the file having the specified filename or path.
	//!
	//!	@param filename is the name or path of an AIFF samples file
		explicit AiffFile( const std::string & filename );

	//!	Initialize an instance of AiffFile with samples rendered
	//!	from a sequnence of Partials. The Partials in the
	//!	specified half-open (STL-style) range are rendered at 
	//!	the specified sample rate, using the (optionally) 
	//!	specified Partial fade time (see Synthesizer.h
	//!	for an examplanation of fade time). 
	//!
	//!	@param begin_partials is the beginning of a sequence of Partials
	//!	@param end_partials is (one-past) the end of a sequence of
	//!	Partials
	//!	@param samplerate is the rate at which Partials are rendered
	//!	@param fadeTime is the Partial fade time for rendering
	//!	the Partials on the specified range. If unspecified, the
	//!	default fade time is 1 ms.
	//!
	//!	If compiled with NO_TEMPLATE_MEMBERS defined, this member accepts
	//!	only PartialList::const_iterator arguments.
#if !defined(NO_TEMPLATE_MEMBERS)
	template<typename Iter>
	AiffFile( Iter begin_partials, Iter end_partials, 
			  double samplerate, double fadeTime = .001 ); // default fade is 1ms
#else
	AiffFile( PartialList::const_iterator begin_partials, 
			  PartialList::const_iterator end_partials,
			  double samplerate, double fadeTime = .001 ); // default fade is 1ms
#endif

	//!	Initialize an instance of AiffFile having the specified sample 
	//!	rate, preallocating numFrames samples, initialized to zero.
	//!
	//!	@param samplerate is the rate at which Partials are rendered
	//!	@param numFrames is the initial number of (zero) samples. If
	//!	unspecified, no samples are preallocated.
	explicit AiffFile( double samplerate, size_type numFrames = 0 );
	
	//!	Initialize an instance of AiffFile from a buffer of sample
	//!	data, with the specified sample rate.
	//!
	//!	@param buffer is a pointer to a buffer of floating point samples.
	//!	@param bufferlength is the number of samples in the buffer.
	//!	@param samplerate is the sample rate of the samples in the buffer.
	AiffFile( const double * buffer, size_type bufferlength, double samplerate );
	 
	//!	Initialize an instance of AiffFile from a vector of sample
	//!	data, with the specified sample rate.
	//!
	//!	@param vec is a vector of floating point samples.
	//!	@param samplerate is the sample rate of the samples in the vector.
	AiffFile( const std::vector< double > & vec, double samplerate );
	 
	//!	Initialize this and AiffFile that is an exact copy, having
	//!	all the same sample data, as another AiffFile.
	//!
	//!	@param other is the AiffFile to copy
	AiffFile( const AiffFile & other );
	 
	//!	Assignment operator: change this AiffFile to be an exact copy
	//!	of the specified AiffFile, rhs, that is, having the same sample
	//!	data.
	//!
	//!	@param rhs is the AiffFile to replicate
	AiffFile & operator= ( const AiffFile & rhs );

//	-- access --
	//!	Return a reference to the Marker (see Marker.h) container 
	//!	for this AiffFile. 
	markers_type & markers( void );

	//!	Return a const reference to the Marker (see Marker.h) container 
	//!	for this AiffFile. 
	const markers_type & markers( void ) const;
	 
	//!	Return the fractional MIDI note number assigned to this AiffFile. 
	//!	If the sound has no definable pitch, note number 60.0 is used.
	double midiNoteNumber( void ) const;

	//!	Return the number of sample frames represented in this AiffFile.
	//!	A sample frame contains one sample per channel for a single sample
	//!	interval (e.g. mono and stereo samples files having a sample rate of
	//!	44100 Hz both have 44100 sample frames per second of audio samples).
 	size_type numFrames( void ) const;

	//!	Return the sampling freqency in Hz for the sample data in this
	//!	AiffFile.
 	double sampleRate( void ) const;
	
	//!	Return a reference to the vector containing
	//!	the floating-point sample data for this AiffFile.
	samples_type & samples( void );

	//!	Return a const reference to the vector containing
	//!	the floating-point sample data for this AiffFile.
	const samples_type & samples( void ) const;

//	-- mutation --

	//!	Render the specified Partial using the (optionally) specified
	//!	Partial fade time, and accumulate the resulting samples into
	//!	the sample vector for this AiffFile.
	//!
	//!	@param p is the partial to render into this AiffFile
	//!	@param fadeTime is the Partial fade time for rendering
	//!	the Partials on the specified range. If unspecified, the
	//!	default fade time is 1 ms.
	void addPartial( const Loris::Partial & p, double fadeTime = .001 /* 1 ms */ );
	 
	//!	Accumulate samples rendered from a sequence of Partials.
	//!	The Partials in the specified half-open (STL-style) range 
	//!	are rendered at this AiffFile's sample rate, using the (optionally) 
	//!	specified Partial fade time (see Synthesizer.h
	//!	for an examplanation of fade time). 
	//!
	//!	@param begin_partials is the beginning of a sequence of Partials
	//!	@param end_partials is (one-past) the end of a sequence of
	//!	Partials
	//!	@param fadeTime is the Partial fade time for rendering
	//!	the Partials on the specified range. If unspecified, the
	//!	default fade time is 1 ms.
	//!	
	//!	If compiled with NO_TEMPLATE_MEMBERS defined, this member accepts
	//!	only PartialList::const_iterator arguments.
#if !defined(NO_TEMPLATE_MEMBERS)
	template<typename Iter>
	void addPartials( Iter begin_partials, Iter end_partials, double fadeTime = .001 /* 1 ms */  );
#else
	void addPartials( PartialList::const_iterator begin_partials, 
					  PartialList::const_iterator end_partials,
					  double fadeTime = .001 /* 1 ms */  );
#endif

	//!	Set the fractional MIDI note number assigned to this AiffFile. 
	//!	If the sound has no definable pitch, use note number 60.0 (the default).
	//!
	//!	@param nn is a fractional MIDI note number, 60 is middle C.
	void setMidiNoteNumber( double nn );
	 
//	-- export --

	//!	Export the sample data represented by this AiffFile to
	//!	the file having the specified filename or path. Export
	//!	signed integer samples of the specified size, in bits
	//!	(8, 16, 24, or 32).
	//!
	//!	@param filename is the name or path of the AIFF samples file
	//!	to be created or overwritten.
	//!	@param bps is the number of bits per sample to store in the
	//!	samples file (8, 16, 24, or 32).If unspeicified, 16 bits
	//!	is assumed.
	//	is assumed.
	void write( const std::string & filename, unsigned int bps = 16 );

private:
//	-- implementation --
	double notenum_, rate_;		// MIDI note number and sample rate
	markers_type markers_;		// AIFF Markers
	samples_type samples_;		// floating point samples [-1.0, 1.0]

	std::auto_ptr< Synthesizer > psynth_;	//	Synthesizer for rendering Partials,
											//	lazy-initialized (not until needed)

//	-- helpers --

	//	Construct a Synthesizer, if necessary, for rendering Partials into
	//	samples, and set the fadeTime (in seconds) of the Synthesizer.
	//
	//	@param fadeTime is the time (in seconds) over which Partials fade in and
	//	out at the ends.
	void configureSynthesizer( double fadeTime );

	//	Read all the AIFF data chunks from a named filed and store the recognized
	//	data (samples, markers, and some MIDI information).
	//
	//	@param filename is the name of the file to read.
	//	@pre filename must be the name (or path) of a single-channel samples file
	//	in uncompressed AIFF format.
	//	@throws FileIOException if an error is encountered reading the file
	void readAiffData( const std::string & filename );

};	//	end of class AiffFile

#pragma mark -- template members --

// ---------------------------------------------------------------------------
//	constructor from Partial range
// ---------------------------------------------------------------------------
//	Initialize an instance of AiffFile having the specified sample 
//	rate, accumulating samples rendered at that sample rate from
//	all Partials on the specified half-open (STL-style) range with
//	the (optionally) specified Partial fade time (see Synthesizer.h
//	for an examplanation of fade time). 
//
//	begin_partials is the beginning of a sequence of Partials
//	end_partials is (one-past) the end of a sequence of
//	Partials
//	samplerate is the rate at which Partials are rendered
//	fadeTime is the Partial fade time for rendering
//	the Partials on the specified range. If unspecified, the
//	default fade time is 1 ms.
//
//	If compiled with NO_TEMPLATE_MEMBERS defined, this member accepts
//	only PartialList::const_iterator arguments.
#if !defined(NO_TEMPLATE_MEMBERS)
template< typename Iter >
 AiffFile::AiffFile( Iter begin_partials, Iter end_partials, 
					 double samplerate, double fadeTime ) : // default fade is 1ms
#else
 AiffFile::AiffFile( PartialList::const_iterator begin_partials, 
					 PartialList::const_iterator end_partials,
					 double samplerate, double fadeTime ) : // default fade is 1ms
#endif
//	initializers:
	notenum_( 60 ),
	rate_( samplerate )
{
	addPartials( begin_partials, end_partials, fadeTime );
}

// ---------------------------------------------------------------------------
//	addPartials 
// ---------------------------------------------------------------------------
//	Accumulate samples rendered from a sequence of Partials.
//	The Partials in the specified half-open (STL-style) range 
//	are rendered at this AiffFile's sample rate, using the (optionally) 
//	specified Partial fade time (see Synthesizer.h
//	for an examplanation of fade time). 
//
//	begin_partials is the beginning of a sequence of Partials
//	end_partials is (one-past) the end of a sequence of
//	Partials
//	fadeTime is the Partial fade time for rendering
//	the Partials on the specified range. If unspecified, the
//	default fade time is 1 ms.
//
//	If compiled with NO_TEMPLATE_MEMBERS defined, this member accepts
//	only PartialList::const_iterator arguments.
//
#if !defined(NO_TEMPLATE_MEMBERS)
template< typename Iter >
void 
 AiffFile::addPartials( Iter begin_partials, Iter end_partials, double fadeTime /*= .001  1 ms */  )
#else
void 
 AiffFile::addPartials( PartialList::const_iterator begin_partials, 
						PartialList::const_iterator end_partials,
						double fadeTime /* = .001 1 ms */  )
#endif
{ 
	configureSynthesizer( fadeTime );
	psynth_->synthesize( begin_partials, end_partials );
} 

}	//	end of namespace Loris

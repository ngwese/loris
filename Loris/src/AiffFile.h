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
#include <Marker.h>
#include <Synthesizer.h>

#if defined(NO_TEMPLATE_MEMBERS)
#include <PartialList.h>
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
//	Class AiffFile represents sample data in a AIFF-format samples 
//	file, and manages file I/O and sample conversion. Since the sound
//	analysis and synthesis algorithms in Loris and the reassigned
//	bandwidth-enhanced representation are monaural, AiffFile manages
//	only monaural (single channel) AIFF-format samples files.
//	
class AiffFile
{
//	-- public interface --
public:

//	-- types --
	typedef std::vector< double > samples_type;
	typedef samples_type::size_type size_type;
	typedef std::vector< Marker > markers_type;


//	-- construction --
 	explicit AiffFile( const std::string & filename );
	/*	Initialize an instance of AiffFile by importing sample data from
		the file having the specified filename or path.
	*/

#if !defined(NO_TEMPLATE_MEMBERS)
	template<typename Iter>
	AiffFile( Iter begin_partials, Iter end_partials, 
			  double samplerate, double fadeTime = .001 ); // default fade is 1ms
#else
	AiffFile( PartialList::const_iterator begin_partials, 
			  PartialList::const_iterator end_partials,
			  double samplerate, double fadeTime = .001 ); // default fade is 1ms
#endif
	/*	Initialize an instance of AiffFile having the specified sample 
		rate, accumulating samples rendered at that sample rate from
		all Partials on the specified half-open (STL-style) range with
		the (optionally) specified Partial fade time (see Synthesizer.h
		for an examplanation of fade time). 

		If compiled with NO_TEMPLATE_MEMBERS defined, this member accepts
		only PartialList::const_iterator arguments.
	*/

	explicit AiffFile( double samplerate, size_type numFrames = 0 );
	/*	Initialize an instance of AiffFile having the specified sample 
		rate, preallocating numFrames samples, initialized to zero.
	 */
	
	AiffFile( const double * buffer, size_type bufferlength, double samplerate );
	/*	Initialize an instance of AiffFile from a buffer of sample
		data, with the specified sample rate.
	 */
	 
	AiffFile( const std::vector< double > & vec, double samplerate );
	/*	Initialize an instance of AiffFile from a vector of sample
		data, with the specified sample rate.
	 */
	 
	AiffFile( const AiffFile & other );
	/*	Initialize this and AiffFile that is an exact copy, having
		all the same sample data, as another AiffFile.
	 */
	 
	AiffFile & operator= ( const AiffFile & rhs );
	/*	Assignment operator: change this AiffFile to be an exact copy
		of the specified AiffFile, rhs, that is, having the same sample
		data.
	 */	 

//	-- access --
	markers_type & markers( void );
	const markers_type & markers( void ) const;
	/*	Return a reference to the Marker (see Marker.h) container 
		for this AiffFile. 
	 */
	 
	double midiNoteNumber( void ) const;
	/*	Return the fractional MIDI note number assigned to this AiffFile. 
		If the sound has no definable pitch, note number 60.0 is used.
	 */

 	size_type numFrames( void ) const;
	/*	Return the number of sample frames represented in this AiffFile.
		A sample frame contains one sample per channel for a single sample
		interval (e.g. mono and stereo samples files having a sample rate of
		44100 Hz both have 44100 sample frames per second of audio samples).
	*/

 	double sampleRate( void ) const;
	/*	Return the sampling freqency in Hz for the sample data in this
		AiffFile.
	*/
	
	samples_type & samples( void );
	const samples_type & samples( void ) const;
	/*	Return a reference (or const reference) to the vector containing
		the floating-point sample data for this AiffFile.
	 */

//	-- mutation --
	void addPartial( const Loris::Partial & p, double fadeTime = .001 /* 1 ms */ );
	/*	Render the specified Partial using the (optionally) specified
		Partial fade time, and accumulate the resulting samples into
		the sample vector for this AiffFile.
	 */
	 
#if !defined(NO_TEMPLATE_MEMBERS)
	template<typename Iter>
	void addPartials( Iter begin_partials, Iter end_partials, double fadeTime = .001 /* 1 ms */  );
#else
	void addPartials( PartialList::const_iterator begin_partials, 
					  PartialList::const_iterator end_partials,
					  double fadeTime = .001 /* 1 ms */  );
#endif
	/*	Render all Partials on the specified half-open (STL-style) range
		with the (optionally) specified Partial fade time (see Synthesizer.h
		for an examplanation of fade time), and accumulate the resulting 
		samples. 
		
		If compiled with NO_TEMPLATE_MEMBERS defined, this member accepts
		only PartialList::const_iterator arguments.
	 */

	void setMidiNoteNumber( double nn );
	/*	Set the fractional MIDI note number assigned to this AiffFile. 
		If the sound has no definable pitch, use note number 60.0 (the default).
	 */
	 
//	-- export --
	void write( const std::string & filename, unsigned int bps = 16 );
	/*	Export the sample data represented by this AiffFile to
		the file having the specified filename or path. Export
		signed integer samples of the specified size, in bits
		(8, 16, 24, or 32).
	*/

private:
//	-- implementation --
	double notenum_, rate_;		// MIDI note number and sample rate
	markers_type markers_;		// AIFF Markers
	samples_type samples_;		// floating point samples [-1.0, 1.0]

	std::auto_ptr< Synthesizer > psynth_;	//	Synthesizer for rendering Partials,
											//	lazy-initialized (not until needed)

//	-- helpers --
	void configureSynthesizer( double fadeTime );
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
//	If compiled with NO_TEMPLATE_MEMBERS defined, this member accepts
//	only PartialList::const_iterator arguments.
//
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
//	Render all Partials on the specified half-open (STL-style) range
//	with the (optionally) specified Partial fade time (see Synthesizer.h
//	for an examplanation of fade time), and accumulate the resulting 
//	samples. 
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

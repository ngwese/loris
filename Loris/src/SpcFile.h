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
 * SpcFile.h
 *
 * Definition of SpcFile class for Partial import and export for
 * real-time synthesis in Kyma.
 *
 * Spc files always represent a number of Partials that is a power of
 * two. This is not necessary for purely-sinusoidal files, but might be
 * (not clear) for enhanced data to be properly processed in Kyma. 
 *
 * All of this is kind of disgusting right now. This code has evolved 
 * somewhat randomly, and we are awaiting full support for bandwidth-
 * enhanced data in Kyma..
 *
 * Kelly Fitz, 8 Jan 2003 
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include <Marker.h>
#include <Partial.h>

#if defined(NO_TEMPLATE_MEMBERS)
#include <PartialList.h>
#endif
 
#include <string>
#include <vector>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	class SpcFile
//
//	Class SpcFile represents a collection of reassigned bandwidth-enhanced
//	Partial data in a SPC-format envelope stream data file, used by the
//	real-time bandwidth-enhanced additive synthesizer implemented on the
//	Symbolic Sound Kyma Sound Design Workstation. Class SpcFile manages 
//	file I/O and conversion between Partials and envelope parameter streams.
//	
class SpcFile
{
//	-- public interface --
public:

//	-- types --
	typedef std::vector< Marker > markers_type;
	typedef std::vector< Partial > partials_type;

//	-- construction --
 	explicit SpcFile( const std::string & filename );
	/*	Initialize an instance of SpcFile by importing envelope parameter 
		streams from the file having the specified filename or path.
	*/

#if !defined(NO_TEMPLATE_MEMBERS)
	template<typename Iter>
	SpcFile( Iter begin_partials, Iter end_partials, double midiNoteNum = 60  );
#else
	SpcFile( PartialList::const_iterator begin_partials, 
			 PartialList::const_iterator end_partials,
			 double midiNoteNum = 60  );
#endif
	/*	Initialize an instance of SpcFile with copies of the Partials
		on the specified half-open (STL-style) range.

		If compiled with NO_TEMPLATE_MEMBERS defined, this member accepts
		only PartialList::const_iterator arguments.
	*/

	explicit SpcFile( double midiNoteNum = 60 );
	/*	Initialize an instance of SpcFile having the specified fractional
		MIDI note number, and no Partials (or envelope parameter streams). 
	*/
	 
	//	copy, assign, and delete are compiler-generated
	
//	-- access --
	markers_type & markers( void );
	const markers_type & markers( void ) const;
	/*	Return a reference to the MarkerContainer (see Marker.h) for this SpcFile. 
	 */
	 
	double midiNoteNumber( void ) const;
	/*	Return the fractional MIDI note number assigned to this SpcFile. 
		If the sound has no definable pitch, note number 60.0 is used.
	 */
	
	const partials_type & partials( void ) const;
	/*	Return a read-only (const) reference to the bandwidth-enhanced
		Partials represented by the envelope parameter streams in this SpcFile.
	 */
	
 	double sampleRate( void ) const;
	/*	Return the sampling freqency in Hz for the spc data in this
		SpcFile. This is the rate at which Kyma must be running to ensure
		proper playback of bandwidth-enhanced Spc data.
	*/
	
//	-- mutation --
	void addPartial( const Loris::Partial & p );
	/*	Add the specified Partial to the enevelope parameter streams
		represented by this SpcFile. 
		
		A SpcFile can contain only one Partial having any given (non-zero) 
		label, so an added Partial will replace a Partial having the 
		same label, if such a Partial exists.

		This may throw an InvalidArgument exception if an attempt is made
		to add unlabeled Partials, or Partials labeled higher than the
		allowable maximum.
	 */
	 
	void addPartial( const Loris::Partial & p, int label );
	/*	Add a Partial, assigning it the specified label (and position in the
		Spc data).
		
		A SpcFile can contain only one Partial having any given (non-zero) 
		label, so an added Partial will replace a Partial having the 
		same label, if such a Partial exists.

		This may throw an InvalidArgument exception if an attempt is made
		to add unlabeled Partials, or Partials labeled higher than the
		allowable maximum.
	 */
	 
#if !defined(NO_TEMPLATE_MEMBERS)
	template<typename Iter>
	void addPartials( Iter begin_partials, Iter end_partials  );
#else
	void addPartials( PartialList::const_iterator begin_partials, 
					  PartialList::const_iterator end_partials  );
#endif
	/*	Add all Partials on the specified half-open (STL-style) range
		to the enevelope parameter streams represented by this SpcFile. 
		
		A SpcFile can contain only one Partial having any given (non-zero) 
		label, so an added Partial will replace a Partial having the 
		same label, if such a Partial exists.

		If compiled with NO_TEMPLATE_MEMBERS defined, this member accepts
		only PartialList::const_iterator arguments.

		This may throw an InvalidArgument exception if an attempt is made
		to add unlabeled Partials, or Partials labeled higher than the
		allowable maximum.
	 */

	void setMidiNoteNumber( double nn );
	/*	Set the fractional MIDI note number assigned to this SpcFile. 
		If the sound has no definable pitch, use note number 60.0 (the default).
	 */
	 
	void setSampleRate( double rate );
	/*	Set the sampling freqency in Hz for the spc data in this
		SpcFile. This is the rate at which Kyma must be running to ensure
		proper playback of bandwidth-enhanced Spc data.
		
		The default sample rate is 44100 Hz.
	*/
	 	
//	-- export --
	void write( const std::string & filename, bool enhanced = true,
				double endApproachTime = 0 );
	/*	Export the envelope parameter streams represented by this SpcFile to
		the file having the specified filename or path. Export phase-correct 
		bandwidth-enhanced envelope parameter streams if enhanced is true 
		(the default), or pure sinsoidal streams otherwise.
	
		A nonzero endApproachTime indicates that the Partials do not include a
		release or decay, but rather end in a static spectrum corresponding to the
		final Breakpoint values of the partials. The endApproachTime specifies how
		long before the end of the sound the amplitude, frequency, and bandwidth
		values are to be modified to make a gradual transition to the static spectrum.
		
		If the endApproachTime is not specified, it is assumed to be zero, 
		corresponding to Partials that decay or release normally.
	*/

private:
//	-- implementation --
	partials_type partials_;		//	Partials to store in Spc format
	markers_type markers_;		// 	AIFF Markers

	double notenum_, rate_;		// MIDI note number and sample rate
	
	static const int MinNumPartials;	//	32
	static const double DefaultRate;	//	44kHz

//	-- helpers --
	void readSpcData( const std::string & filename );
	void growPartials( partials_type::size_type sz );
	
};	//	end of class SpcFile


// ---------------------------------------------------------------------------
//	constructor from Partial range
// ---------------------------------------------------------------------------
//	Initialize an instance of SpcFile with copies of the Partials
//	on the specified half-open (STL-style) range. If the MIDI
//	note number is not specified, then note number 60 is used.
//
//	If compiled with NO_TEMPLATE_MEMBERS defined, this member accepts
//	only PartialList::const_iterator arguments.
//
#if !defined(NO_TEMPLATE_MEMBERS)
template< typename Iter >
SpcFile::SpcFile( Iter begin_partials, Iter end_partials, double midiNoteNum  ) :
#else
SpcFile::SpcFile( PartialList::const_iterator begin_partials, 
				  PartialList::const_iterator end_partials,
				  double midiNoteNum ) :
#endif
//	initializers:
	notenum_( midiNoteNum ),
	rate_( DefaultRate )
{
	growPartials( MinNumPartials );
	addPartials( begin_partials, end_partials );
}

// ---------------------------------------------------------------------------
//	addPartials 
// ---------------------------------------------------------------------------
//	Add all Partials on the specified half-open (STL-style) range
//	to the enevelope parameter streams represented by this SpcFile. 
//	
//	A SpcFile can contain only one Partial having any given (non-zero) 
//	label, so an added Partial will replace a Partial having the 
//	same label, if such a Partial exists.
//
//	If compiled with NO_TEMPLATE_MEMBERS defined, this member accepts
//	only PartialList::const_iterator arguments.
//
//	This may throw an InvalidArgument exception if an attempt is made
//	to add unlabeled Partials, or Partials labeled higher than the
//	allowable maximum.
//
#if !defined(NO_TEMPLATE_MEMBERS)
template<typename Iter>
void SpcFile::addPartials( Iter begin_partials, Iter end_partials  )
#else
void SpcFile::addPartials( PartialList::const_iterator begin_partials, 
						   PartialList::const_iterator end_partials  )
#endif
{
	while ( begin_partials != end_partials )
		addPartial( *(begin_partials++) );
}

}	//	end of namespace Loris



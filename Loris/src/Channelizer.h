#ifndef INCLUDE_CHANNELIZER_H
#define INCLUDE_CHANNELIZER_H
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
 * Channelizer.h
 *
 * Definition of class Loris::Channelizer.
 *
 * Kelly Fitz, 21 July 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <PartialList.h>

//	begin namespace
namespace Loris {

class Channelizer_imp;
class Envelope;
class Partial;

// ---------------------------------------------------------------------------
//	class Channelizer
//
//	Class Channelizer represents an algorithm for automatic labeling of
//	a collection (STL range) of Partials. Partials must be labeled in
//	preparation for morphing (see Morpher) to establish correspondences
//	between Partials in the morph source and target sounds. 
//	
//	Channelized partials are labeled according to their adherence to a
//	harmonic frequency structure with a time-varying fundamental
//	frequency. The frequency spectrum is partitioned into
//	non-overlapping channels having time-varying center frequencies that
//	are harmonic (integer) multiples of a specified reference frequency
//	envelope, and each channel is identified by a unique label equal to
//	its harmonic number. Each Partial is assigned the label
//	corresponding to the channel containing the greatest portion of its
//	(the Partial's) energy. 
//	
//	A reference frequency Envelope for channelization and the channel
//	number to which it corresponds (1 for an Envelope that tracks the
//	Partial at the fundamental frequency) must be specified. The
//	reference Envelope can be constructed explcitly, point by point
//	(using, for example, the BreakpointEnvelope class), or constructed
//	automatically using the FrequencyReference class. 
//	
//	Channelizer is a leaf class, do not subclass.
//
class Channelizer
{
//	-- insulating implementaion --
	Channelizer_imp * _imp;
	
//	-- public interface --
public:
//	-- construction --
	Channelizer( const Envelope & refChanFreq, int refChanLabel );
	/*	Construct a new Channelizer using the specified reference frequency
		Envelope to represent the channel numbered refChanLabel.
	 */
	 
	Channelizer( const Channelizer & other );
	/*	Construct a new Channelizer that is an exact copy of this Channelizer
		(uses the same reference frequency Envelope to represent the same 
		channel).
	 */
	 
	~Channelizer( void );
	/*	Destroy this Channelizer.
	 */
	 
	Channelizer & operator=( const Channelizer & rhs );
	/*	Assignment operator: make this Channelizer an exact copy of rhs 
		(use the same set of frequency channels).
	 */
	 
//	-- channelizing --
	void channelize( PartialList::iterator begin, PartialList::iterator end ) const;
	/*	Assign each Partial in the specified half-open (STL-style) range
		the label corresponding to the frequency channel containing the
		greatest portion of its (the Partial's) energy.
	 */
	 
	void operator() ( PartialList::iterator begin, PartialList::iterator end ) const
		{ channelize( begin, end ); }
	/*	Function call operator: same as channelize().
	 */
	 
};	//	end of class Channelizer

}	//	end of namespace Loris

#endif /* ndef INCLUDE_CHANNELIZER_H */

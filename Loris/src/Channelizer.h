#ifndef INCLUDE_CHANNELIZER_H
#define INCLUDE_CHANNELIZER_H
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

#if defined(NO_TEMPLATE_MEMBERS)
#include "PartialList.h"
#endif

#include <memory>

//	begin namespace
namespace Loris {

class Envelope;
class Partial;

// ---------------------------------------------------------------------------
//	Channelizer
// ---------------------------------------------------------------------------
/**	\class Channelizer Channelizer.h loris/Channelizer.h
 *	
 *	Class Channelizer represents an algorithm for automatic labeling of
 *	a sequence of Partials. Partials must be labeled in
 *	preparation for morphing (see Morpher) to establish correspondences
 *	between Partials in the morph source and target sounds. 
 *	
 *	Channelized partials are labeled according to their adherence to a
 *	harmonic frequency structure with a time-varying fundamental
 *	frequency. The frequency spectrum is partitioned into
 *	non-overlapping channels having time-varying center frequencies that
 *	are harmonic (integer) multiples of a specified reference frequency
 *	envelope, and each channel is identified by a unique label equal to
 *	its harmonic number. Each Partial is assigned the label
 *	corresponding to the channel containing the greatest portion of its
 *	(the Partial's) energy. 
 *	
 *	A reference frequency Envelope for channelization and the channel
 *	number to which it corresponds (1 for an Envelope that tracks the
 *	Partial at the fundamental frequency) must be specified. The
 *	reference Envelope can be constructed explcitly, point by point
 *	(using, for example, the BreakpointEnvelope class), or constructed
 *	automatically using the FrequencyReference class. 
 *	
 *	Channelizer is a leaf class, do not subclass.
 */
class Channelizer
{
//	-- implementaion --
	std::auto_ptr< Envelope > _refChannelFreq;
	int _refChannelLabel;
	
//	-- public interface --
public:
//	-- construction --
	/**	Construct a new Channelizer using the specified reference
		\c Envelope to represent the a numbered channel. 
		\param refChanFreq is an Envelope representing the center frequency
		of a channel.
		\param refChanLabel is the corresponding channel number (i.e. 1
		if \a refChanFreq is the lowest-frequency channel, and all other channels
		are harmonics of \a refChanFreq, or 2 if \a refChanFreq tracks the second
		harmonic, etc.).
	 */
	Channelizer( const Envelope & refChanFreq, int refChanLabel );
	 
	/**	Construct a new Channelizer that is an exact copy of another.
		The copy represents the same set of frequency channels, constructed
		from the same reference Envelope and channel number.
		\param other is the Channelizer to copy
	 */
	Channelizer( const Channelizer & other );
	 
	/**	Assignment operator: make this Channelizer an exact copy of another. 
		This Channelizer is made to represent the same set of frequency channels, 
		constructed from the same reference Envelope and channel number as \a rhs.
		\param rhs is the Channelizer to copy
	 */
	Channelizer & operator=( const Channelizer & rhs );
	 
	/**	Destroy this Channelizer.
	 */
	~Channelizer( void );
	 
//	-- channelizing --
	/**	Label a Partial with the number of the frequency channel containing
		the greatest portion of its (the Partial's) energy.
		\param partial is the Partial to label.
	 */
	void channelize( Partial & partial ) const;

	/**	Assign each Partial in the specified half-open (STL-style) range
		the label corresponding to the frequency channel containing the
		greatest portion of its (the Partial's) energy.
		\param begin is the beginning of the range of Partials to channelize
		\param end is (one-past) the end of the range of Partials o channelize
		
		If compiled with \c NO_TEMPLATE_MEMBERS defined, then \a begin and \a end
		must be \c PartialList::iterators, otherwise they can be any type
		of iterators over a sequence of Partials.
	 */
#if ! defined(NO_TEMPLATE_MEMBERS)
	template<typename Iter>
	void channelize( Iter begin, Iter end ) const;
#else
	void channelize( PartialList::iterator begin, PartialList::iterator end ) const;
#endif	 

	/**	Function call operator: same as \c channelize().
	 */
#if ! defined(NO_TEMPLATE_MEMBERS)
	template<typename Iter>
	void operator() ( Iter begin, Iter end ) const
#else
	void operator() ( PartialList::iterator begin, PartialList::iterator end ) const
#endif
		{ channelize( begin, end ); }
	 
};	//	end of class Channelizer

// ---------------------------------------------------------------------------
//	channelize (sequence of Partials)
// ---------------------------------------------------------------------------
//	Assign each Partial in the specified half-open (STL-style) range
//	the label corresponding to the frequency channel containing the
//	greatest portion of its (the Partial's) energy.
//
#if ! defined(NO_TEMPLATE_MEMBERS)
template<typename Iter>
void Channelizer::channelize( Iter begin, Iter end ) const
#else
void Channelizer::channelize( PartialList::iterator begin, PartialList::iterator end ) const
#endif
{
	while ( begin != end )
	{
		channelize(*begin++);
	}
}

}	//	end of namespace Loris

#endif /* ndef INCLUDE_CHANNELIZER_H */

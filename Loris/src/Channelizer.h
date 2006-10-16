#ifndef INCLUDE_CHANNELIZER_H
#define INCLUDE_CHANNELIZER_H
/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2006 by Kelly Fitz and Lippold Haken
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
//	
//!	Class Channelizer represents an algorithm for automatic labeling of
//!	a sequence of Partials. Partials must be labeled in
//!	preparation for morphing (see Morpher) to establish correspondences
//!	between Partials in the morph source and target sounds. 
//!	
//!	Channelized partials are labeled according to their adherence to a
//!	harmonic frequency structure with a time-varying fundamental
//!	frequency. The frequency spectrum is partitioned into
//!	non-overlapping channels having time-varying center frequencies that
//!	are harmonic (integer) multiples of a specified reference frequency
//!	envelope, and each channel is identified by a unique label equal to
//!	its harmonic number. Each Partial is assigned the label
//!	corresponding to the channel containing the greatest portion of its
//!	(the Partial's) energy. 
//!	
//!	A reference frequency Envelope for channelization and the channel
//!	number to which it corresponds (1 for an Envelope that tracks the
//!	Partial at the fundamental frequency) must be specified. The
//!	reference Envelope can be constructed explcitly, point by point
//!	(using, for example, the BreakpointEnvelope class), or constructed
//!	automatically using the FrequencyReference class. 
//!
//! The Channelizer can be configured with a stretch factor, to accomodate
//! detuned harmonics, as in the case of piano tones. The static member
//! computeStretchFactor can compute the apppropriate stretch factor, given
//! a pair of partials. This computation is based on formulae given in 
//! "Understanding the complex nature of the piano tone" by Martin Keane
//! at the Acoustics Research Centre at the University of Aukland (Feb 2004).
//!	
//!	Channelizer is a leaf class, do not subclass.
//
class Channelizer
{
//	-- implementaion --
	std::auto_ptr< Envelope > _refChannelFreq;  //! the reference frequency envelope
	int _refChannelLabel;                       //! the channel number corresponding to the
	                                            //! reference frequency (1 for the fundamental)
	double _stretchFactor;                      //! stretching factor to account for 
	                                            //! detuned harmonics, as in the case of the piano; 
	                                            //! can be computed using the static member
	                                            //! computeStretchFactor. Should be 0 for most
	                                            //! (strongly harmonic) sounds.
	    
//	-- public interface --
public:
//	-- construction --

	//!	Construct a new Channelizer using the specified reference
	//!	Envelope to represent the a numbered channel. If the sound
	//! being channelized is known to have detuned harmonics, a 
	//! stretching factor can be specified (defaults to 0 for no 
	//! stretching). The stretching factor can be computed using
	//! the static member computeStretchFactor.
	//!	
	//!	\param 	refChanFreq is an Envelope representing the center frequency
	//!		    of a channel.
	//!	\param  refChanLabel is the corresponding channel number (i.e. 1
	//!		    if refChanFreq is the lowest-frequency channel, and all 
	//!		    other channels are harmonics of refChanFreq, or 2 if  
	//!		    refChanFreq tracks the second harmonic, etc.).
	//! \param  stretchFactor is a stretching factor to account for detuned 
	//!         harmonics, default is 0. 
    //! \throw  InvalidArgument if refChanLabel is not positive.
	Channelizer( const Envelope & refChanFreq, int refChanLabel, double stretchFactor = 0 );
	 
	//!	Construct a new Channelizer that is an exact copy of another.
	//!	The copy represents the same set of frequency channels, constructed
	//!	from the same reference Envelope and channel number.
	//!	
	//!	\param other is the Channelizer to copy
	Channelizer( const Channelizer & other );
	 
	//!	Assignment operator: make this Channelizer an exact copy of another. 
	//!	This Channelizer is made to represent the same set of frequency channels, 
	//!	constructed from the same reference Envelope and channel number as rhs.
	//!
	//!	\param rhs is the Channelizer to copy
	Channelizer & operator=( const Channelizer & rhs );
	 
	//!	Destroy this Channelizer.
	~Channelizer( void );
	 
//	-- channelizing --

	//!	Label a Partial with the number of the frequency channel containing
	//!	the greatest portion of its (the Partial's) energy.
	//!	
	//!	\param partial is the Partial to label.
	void channelize( Partial & partial ) const;

	//!	Assign each Partial in the specified half-open (STL-style) range
	//!	the label corresponding to the frequency channel containing the
	//!	greatest portion of its (the Partial's) energy.
	//!	
	//!	\param begin is the beginning of the range of Partials to channelize
	//!	\param end is (one-past) the end of the range of Partials to channelize
	//!	
	//!	If compiled with NO_TEMPLATE_MEMBERS defined, then begin and end
	//!	must be PartialList::iterators, otherwise they can be any type
	//!	of iterators over a sequence of Partials.
#if ! defined(NO_TEMPLATE_MEMBERS)
	template<typename Iter>
	void channelize( Iter begin, Iter end ) const;
#else
	void channelize( PartialList::iterator begin, PartialList::iterator end ) const;
#endif	 

	//!	Function call operator: same as channelize().
#if ! defined(NO_TEMPLATE_MEMBERS)
	template<typename Iter>
	void operator() ( Iter begin, Iter end ) const
#else
    inline
	void operator() ( PartialList::iterator begin, PartialList::iterator end ) const
#endif
		 { channelize( begin, end ); }
		 
//	-- access/mutation --
		 
    //! Return the stretching factor used to account for detuned
    //! harmonics, as in a piano tone. Normally set to 0 for 
    //! in-tune harmonics.
    double stretchFactor( void ) const;
        
    //! Set the stretching factor used to account for detuned
    //! harmonics, as in a piano tone. Normally set to 0 for 
    //! in-tune harmonics.
    void setStretchFactor( double stretch );    
    
		 
// -- static members --

	//! Static member that constructs an instance and applies
	//! it to a sequence of Partials. 
	//! Construct a Channelizer using the specified Envelope
	//! and reference label, and use it to channelize a
	//! sequence of Partials. 
	//!
	//! \param  begin is the beginning of a sequence of Partials to 
	//!         channelize.
	//! \param  end is the end of a sequence of Partials to 
	//!         channelize.
	//! \param 	refChanFreq is an Envelope representing the center frequency
	//!         of a channel.
	//! \param 	refChanLabel is the corresponding channel number (i.e. 1
	//!         if refChanFreq is the lowest-frequency channel, and all 
	//!         other channels are harmonics of refChanFreq, or 2 if  
	//!         refChanFreq tracks the second harmonic, etc.).
	//! \throw  InvalidArgument if refChanLabel is not positive.
	//!	
	//!	If compiled with NO_TEMPLATE_MEMBERS defined, then begin and end
	//!	must be PartialList::iterators, otherwise they can be any type
	//!	of iterators over a sequence of Partials.
#if ! defined(NO_TEMPLATE_MEMBERS)
	template< typename Iter >
	static 
	void channelize( Iter begin, Iter end, 
                     const Envelope & refChanFreq, int refChanLabel );
#else
	static inline 
	void channelize( PartialList::iterator begin, PartialList::iterator end,
                     const Envelope & refChanFreq, int refChanLabel );
#endif	 
	 
    //! Static member to compute the stretch factor for a sound having
    //! (consistently) detuned harmonics, like piano tones.
    //!
    //! \param      fref is the reference (fundamental) frequency from which
    //!             the harmonics are detuned.
    //! \param      fn is the frequency of the Nth stretched harmonic
    //! \param      n is the harmonic number of the harmonic whose frequnecy is fn
    //! \returns    the stretching factor, usually a very small positive
    //!             floating point number, or 0 for pefectly tuned harmonics
    //!             (that is, if fn = n*f1).
    //
    static double computeStretchFactor( double fref, double fn, double n );
	 
};	//	end of class Channelizer

// ---------------------------------------------------------------------------
//	channelize (sequence of Partials)
// ---------------------------------------------------------------------------
//!	Assign each Partial in the specified half-open (STL-style) range
//!	the label corresponding to the frequency channel containing the
//!	greatest portion of its (the Partial's) energy.
//!	
//!	\param begin is the beginning of the range of Partials to channelize
//!	\param end is (one-past) the end of the range of Partials o channelize
//!	
//!	If compiled with NO_TEMPLATE_MEMBERS defined, then begin and end
//!	must be PartialList::iterators, otherwise they can be any type
//!	of iterators over a sequence of Partials.
//
#if ! defined(NO_TEMPLATE_MEMBERS)
template<typename Iter>
void Channelizer::channelize( Iter begin, Iter end ) const
#else
inline
void Channelizer::channelize( PartialList::iterator begin, PartialList::iterator end ) const
#endif
{
	while ( begin != end )
	{
		channelize( *begin++ );
	}
}

// ---------------------------------------------------------------------------
//	channelize (static)
// ---------------------------------------------------------------------------
//!   Static member that constructs an instance and applies
//!   it to a sequence of Partials. 
//!   Construct a Channelizer using the specified Envelope
//!   and reference label, and use it to channelize a
//!   sequence of Partials. 
//!
//!   \param   begin is the beginning of a sequence of Partials to 
//!            channelize.
//!   \param   end is the end of a sequence of Partials to 
//!            channelize.
//!	  \param 	refChanFreq is an Envelope representing the center frequency
//!				of a channel.
//!	  \param 	refChanLabel is the corresponding channel number (i.e. 1
//!				if refChanFreq is the lowest-frequency channel, and all 
//!				other channels are harmonics of refChanFreq, or 2 if  
//!				refChanFreq tracks the second harmonic, etc.).
//!   \throw   InvalidArgument if refChanLabel is not positive.
//!	
//!	If compiled with NO_TEMPLATE_MEMBERS defined, then begin and end
//!	must be PartialList::iterators, otherwise they can be any type
//!	of iterators over a sequence of Partials.
//
#if ! defined(NO_TEMPLATE_MEMBERS)
template< typename Iter >
void Channelizer::channelize( Iter begin, Iter end, 
                              const Envelope & refChanFreq, int refChanLabel )
#else
inline
void Channelizer::channelize( PartialList::iterator begin, PartialList::iterator end,
                              const Envelope & refChanFreq, int refChanLabel )
#endif	 
{
   Channelizer instance( refChanFreq, refChanLabel );
	while ( begin != end )
	{
		instance.channelize( *begin++ );
	}
}



}	//	end of namespace Loris

#endif /* ndef INCLUDE_CHANNELIZER_H */

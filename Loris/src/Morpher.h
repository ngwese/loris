#ifndef INCLUDE_MORPHER_H
#define INCLUDE_MORPHER_H
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
 * Morpher.h
 *
 * Definition of class Morpher.
 *
 * Kelly Fitz, 15 Oct 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include<PartialList.h>
#include <memory>

//	begin namespace
namespace Loris {

class Envelope;
class Partial;

// ---------------------------------------------------------------------------
//	Class Morpher
//
//	Class Morpher performs sound morphing and Partial parameter
//	envelope interpolation according to a trio of frequency, amplitude,
//	and bandwidth morphing functions, described by Envelopes.
//	Sound morphing is achieved by interpolating the time-varying 
//	frequencies, amplitudes, and bandwidths of corresponding partials 
//	obtained from reassigned bandwidth-enhanced analysis of the source 
//	and target sounds. Partial correspondences may be established by 
//	labeling, using instances of the Channelizer and Distiller classes.
//
//	The Morpher collects morphed Partials in a PartialList, that is
//	accessible to clients.
//
//	For more information about sound morphing using 
//	the Reassigned Bandwidth-Enhanced Additive Sound 
//	Model, refer to the Loris website: 
//	www.cerlsoundgroup.org/Loris/.
//	
//	Morpher is a leaf class, do not subclass.
//
class Morpher
{
//	-- instance variables --
	std::auto_ptr< Envelope > _freqFunction;	//	frequency morphing function
	std::auto_ptr< Envelope > _ampFunction;		//	amplitude morphing function
	std::auto_ptr< Envelope > _bwFunction;		//	bandwidth morphing function
	
	PartialList _partials;						//	collect Partials here

//	-- public interface --
public:
//	-- construction --
//	Construct a new Morpher using the same morphing envelope for 
//	frequency, amplitude, and bandwidth (noisiness).
	Morpher( const Envelope & f );
//	Construct a new Morpher using the specified morphing envelopes for
//	frequency, amplitude, and bandwidth (noisiness).
	Morpher( const Envelope & ff, const Envelope & af, const Envelope & bwf );
//	Destroy this Morpher.
	~Morpher( void );
	
//	-- morphed parameter computation --
//	Morph Breakpoint to Breakpoint:
//	Compute morphed parameter values at the specified time, using
//	the source and target Breakpoints (assumed to correspond exactly 
//	to the specified time). Return the morphed Parameters in the
//	specified return Breakpoint.
	void morphParameters( const Breakpoint & srcBkpt, const Breakpoint & tgtBkpt, 
						  double time, Breakpoint & retBkpt );
						  
//	Morph Breakpoint to Partial:
//	Compute morphed parameter values at the specified time, using
//	the source Breakpoint (assumed to correspond exactly to the
//	specified time) and the target Partial (whose parameters are
//	examined at the specified time). Return the morphed Parameters 
//	in the specified return Breakpoint.
//
//	The target Partial may be a dummy Partial (no Breakpoints).
	void morphParameters( const Breakpoint & srcBkpt, const Partial & tgtPartial, 
						  double time, Breakpoint & retBkpt );
						  
//	Morph Partial to Breakpoint
//	Compute morphed parameter values at the specified time, using
//	the source Partial (whose parameters are examined at the specified 
//	time) and the target Breakpoint (assumed to correspond exactly to 
//	the specified time). Return the morphed Parameters in the
//	specified return Breakpoint.
//
//	The source Partial may be a dummy Partial (no Breakpoints).
	void morphParameters( const Partial & srcPartial, const Breakpoint & tgtBkpt, 
						  double time, Breakpoint & retBkpt );
						  
//	Morph Partial to Partial
//	Compute morphed parameter values at the specified time, using
//	the source  and target Partials, both of whose parameters are 
//	examined at the specified time. Return the morphed Parameters 
//	in the specified return Breakpoint.
//
//	Either (or neither) the source or target Partial may be a dummy 
//	Partial (no Breakpoints), but not both.
	void morphParameters( const Partial & srcPartial, const Partial & tgtPartial, 
						  double time, Breakpoint & retBkpt );


//	-- Partial morphing --
//	Morph a pair of Partials to yield a new morphed Partial. 
//	Dummy Partials (having no Breakpoints) don't contribute to the
//	morph, except to cause their opposite to fade out. 
//	Either (or neither) the source or target Partial may be a dummy 
//	Partial (no Breakpoints), but not both. The morphed
//	Partial has Breakpoints at times corresponding to every Breakpoint 
//	in both source Partials. The morphed Partial is appended
//	to the Morpher's PartialList, and a reference to it is returned.
//	The new morphed Partial is assigned the specified label.
	Partial & morphPartial( const Partial & src, const Partial & tgt, int assignLabel );

//	Morph two sounds (collections of Partials labeled to indicate
//	correspondences) into a single labeled collection of Partials.
//	Unlabeled Partials (having label 0) are crossfaded. The morphed
//	and crossfaded Partials are stored in the Morpher's PartialList.
//
//	The Partials in the first range are treated as components of the 
//	source sound, corresponding to a morph function value of 0, and  
//	those in the second are treated as components of the target sound, 
//	corresponding to a morph function value of 1.
	void morph( PartialList::const_iterator beginSrc, 
				PartialList::const_iterator endSrc,
				PartialList::const_iterator beginTgt, 
				PartialList::const_iterator endTgt );

//	Crossfade Partials with no correspondences.
//
//	Unlabeled Partials (having label 0) are considered to 
//	have no correspondences, so they are just faded out, and not 
//	actually morphed. This is the same as morphing each with an 
//	empty dummy Partial (having no Breakpoints). 
//
//	The Partials in the first range are treated as components of the 
//	source sound, corresponding to a morph function value of 0, and  
//	those in the second are treated as components of the target sound, 
//	corresponding to a morph function value of 1.
//
//	The crossfaded Partials are stored in the Morpher's PartialList.
	void crossfade( PartialList::const_iterator beginSrc, 
					PartialList::const_iterator endSrc,
					PartialList::const_iterator beginTgt, 
					PartialList::const_iterator endTgt );

//	-- morphing function access/mutation --
//	Assign a new frequency morphing envelope to this Morpher.
	void setFrequencyFunction( const Envelope & f );
//	Assign a new amplitude morphing envelope to this Morpher.
	void setAmplitudeFunction( const Envelope & f );
//	Assign a new bandwidth morphing envelope to this Morpher.
	void setBandwidthFunction( const Envelope & f );

//	Return a reference to this Morpher's frequency morphing envelope.
	const Envelope & frequencyFunction( void ) const;
//	Return a reference to this Morpher's amplitude morphing envelope.
	const Envelope & amplitudeFunction( void ) const;
//	Return a reference to this Morpher's bandwidth morphing envelope.
	const Envelope & bandwidthFunction( void ) const;
	
//	-- PartialList access --
//	Return a reference to this Morpher's list of morphed Partials.
	PartialList & partials( void ); 
//	Return a const reference to this Morpher's list of morphed Partials.
	const PartialList & partials( void ) const; 

					
//	-- unimplemented until useful --
private:
	Morpher( const Morpher & other );
	Morpher & operator= ( const Morpher & other );

};	//	end of class Morpher

}	//	end of namespace Loris

#endif /* ndef INCLUDE_MORPHER_H */

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
#include <Loris_prefix.h>
#include <list>

#if defined(__MWERKS__)
#include "Partial.h"
//	On Mac, can't get around including Partial.h unless make a 
//	class out of std::list< Partial > and its iterators that I can
//	forward-declare. CW seems not to be able to handle 
//	list<Partial>::iterator without the definition of Partial.
//	GNU, however, can handle it. Maybe in the new CW?
#endif

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

class Envelope;
class Partial;
class Morpher_imp;

// ---------------------------------------------------------------------------
//	class Morpher
//
//	Fully-insulating class encapsulating manipulations involving 
//	linear interpolation of Partial parameter envelopes. The implementation
//	is entirely defined in the Morpher_imp class, in Morpher.C.
//
//	The Morpher object performs sound morphing (cite Lip's papers, and the book)
//	by interpolating Partial parmeter envelopes of corresponding Partials in
//	a pair of source sounds. The correspondences are established by labeling.
//	The Morpher object collects morphed Partials in a list<Partial>, that can
//	be accessed by clients.
//
//	The Morpher interface is fully insulating, the implementation is 
//	defined entirely inthe Morpher_imp class in Morpher.C.
//	Morpher is a leaf class, do not subclass.
//
class Morpher
{
//	-- insulating implementation --
	Morpher_imp * _imp;

//	-- public interface --
public:
//	construction:
	Morpher( const Envelope & f );
	Morpher( const Envelope & ff, const Envelope & af, const Envelope & bwf );
	~Morpher( void );
	
//	morphing:
//	Morph two sounds (collections of Partials labeled to indicate
//	correspondences) into a single labeled collection of Partials.
	void morph( std::list< Partial >::const_iterator begin0, 
				std::list< Partial >::const_iterator end0,
				std::list< Partial >::const_iterator begin1, 
				std::list< Partial >::const_iterator end1 );

//	morphing functions access/mutation:	
	void setFrequencyFunction( const Envelope & f );
	void setAmplitudeFunction( const Envelope & f );
	void setBandwidthFunction( const Envelope & f );

	const Envelope & frequencyFunction( void ) const;
	const Envelope & amplitudeFunction( void ) const;
	const Envelope & bandwidthFunction( void ) const;
	
	
//	Partial list access:
	std::list< Partial > & partials( void ); 
	const std::list< Partial > & partials( void ) const; 

//	-- unimplemented until useful --
private:
	Morpher( const Morpher & other );
	Morpher & operator= ( const Morpher & other );

};	//	end of class Morpher

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif /* ndef INCLUDE_MORPHER_H */

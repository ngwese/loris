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

class Channelizer_imp;
class Envelope;
class Partial;

// ---------------------------------------------------------------------------
//	class Channelizer
//
//	Definition of a class for labeling Partials in a std::list< Partial >
//	according to a set of linearly-spaced, variable-frequency
//	channels (like a time-varying harmonic frequency relationship).
//
//	This class interface is fully insulating.
//	This class is a leaf class, not for subclassing (no virtual destructor). 
//
class Channelizer
{
//	-- insulating implementaion --
	Channelizer_imp * _imp;
	
//	-- public interface --
public:
	//	construction:
	Channelizer( const Envelope & refChanFreq, int refChanLabel );
	Channelizer( const Channelizer & other );
	~Channelizer( void );

	//	assignment:
	Channelizer & operator=( const Channelizer & rhs );

	//	channelizing:
	void channelize( std::list< Partial >::iterator begin, std::list< Partial >::iterator end );
	
};	//	end of class Channelizer

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif /* ndef INCLUDE_CHANNELIZER_H */

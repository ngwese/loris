#ifndef INCLUDE_MORPHER_H
#define INCLUDE_MORPHER_H
/*
 * Copyright (c) 1999-2000 Kelly Fitz and Lippold Haken
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
#include "Partial.h"	//	needed only for PartialList definition

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

template <class T> class Handle;	//	Batov's Handle<> template, in Handle.h
class Envelope;
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
	Morpher( Handle< Envelope > f );
	Morpher( Handle< Envelope > ff, Handle< Envelope > af, Handle< Envelope > bwf );
	~Morpher( void );
	
//	morphing:
//	Morph two sounds (collections of Partials labeled to indicate
//	correspondences) into a single labeled collection of Partials.
	void morph( PartialList::const_iterator begin0, 
				PartialList::const_iterator end0,
				PartialList::const_iterator begin1, 
				PartialList::const_iterator end1 );

//	morphing functions access/mutation:	
	void setFrequencyFunction( Handle< Envelope > f );
	void setAmplitudeFunction( Handle< Envelope > f );
	void setBandwidthFunction( Handle< Envelope > f );

	Handle< Envelope > frequencyFunction( void );
	Handle< const Envelope > frequencyFunction( void ) const;

	Handle< Envelope > amplitudeFunction( void );
	Handle< const Envelope > amplitudeFunction( void ) const;

	Handle< Envelope > bandwidthFunction( void );
	Handle< const Envelope > bandwidthFunction( void ) const;
	
	
//	PartialList access:
	PartialList & partials( void ); 
	const PartialList & partials( void ) const; 

//	-- unimplemented until useful --
private:
	Morpher( const Morpher & other );
	Morpher & operator= ( const Morpher & other );

};	//	end of class Morpher

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_MORPHER_H

#ifndef INCLUDE_ENVELOPE_H
#define INCLUDE_ENVELOPE_H
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
 * Envelope.h
 *
 * Definition of abstract interface class Envelope.
 *
 * Kelly Fitz, 21 July 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	class Envelope
//
//	Abstract base class, specifying interface ( valueAt() and clone() ).
//	Derived classes must implement valueAt() and clone(), the latter to 
//	support the Prototype pattern.
//
//	Clients of Envelope, like Morpher, use the Prototype pattern to safely
//	take ownership of their Envelopes. Formerly, Handle<>s were used for this
//	purpose, but using them polymorphically seemed to trigger random and 
//	diverse compiler problems. Handle<>s are no longer part of Loris.
//
//	Envelope is an abstract base class representing a generic real (double) 
//	function of one real (double) argument. 
//
class Envelope
{
//	-- public interface --
public:
	virtual double valueAt( double x ) const = 0;	
	virtual ~Envelope( void );
	
	virtual Envelope * clone( void ) const = 0;
	
//	-- protected interface --
protected:
	//	protect construction:
	Envelope( void );
	Envelope( const Envelope & );

};	//	end of abstract class Envelope

}	//	end of namespace Loris

#endif /* ndef INCLUDE_ENVELOPE_H */

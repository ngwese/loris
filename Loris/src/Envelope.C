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
 * Envelope.C
 *
 * Implementation of class Envelope.
 *
 * Kelly Fitz, 21 July 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include "config.h"
#endif

#include "Envelope.h"

//	Since Envelope is just an interface, there's nothing interesting in 
//	the implementation file.

//	begin namespace
namespace Loris {
	
// ---------------------------------------------------------------------------
//	class Envelope
//
//!	@class Envelope Envelope.h loris/Envelope.h
//!
//!	Class Envelope is an abstract base class, specifying interface for
//!	prototypable (clonable) objects representing generic, real-valued
//!	(double) functions of one real-valued (double) time argument. Derived
//!	classes (like BreakpointEnvelope) must implement valueAt() and
//!	clone(), the latter to support the Prototype pattern. Clients of
//!	Envelope, like Morpher and Distiller, can use prototype Envelopes to
//!	make their own private Envelopes.
//

// ---------------------------------------------------------------------------
//	constructor
// ---------------------------------------------------------------------------
//
Envelope::Envelope(void)
{
}

// ---------------------------------------------------------------------------
//	copy constructor
// ---------------------------------------------------------------------------
//
Envelope::Envelope(const Envelope &)
{
}

// ---------------------------------------------------------------------------
//	destructor
// ---------------------------------------------------------------------------
//
Envelope::~Envelope(void)
{
}

}	//	end of namespace Loris

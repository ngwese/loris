#ifndef INCLUDE_BREAKPOINTENVELOPE_H
#define INCLUDE_BREAKPOINTENVELOPE_H
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
 * BreakpointEnvelope.h
 *
 * Definition of class BreakpointEnvelope.
 *
 * Kelly Fitz, 21 July 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "Envelope.h"
#include <map>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class BreakpointEnvelope
//
//	BreakpointEnvelope specifyies a linear segment breakpoint function, 
//	and implements the Envelope interface. 
//
//	This class isn't insulating, clients that should be protected from 
//	the details should use the EnvelopeInterface.
//
//	Linear-segment breakpoint function with infinite extension at each end.
//
class BreakpointEnvelope : public Envelope
{
//	-- instance variables --
	std::map< double, double > _breakpoints;

//	-- public interface --
public:
	//	Envelope interface:
	virtual double valueAt( double x ) const;	
	
	//	construction:
	BreakpointEnvelope( void );
	BreakpointEnvelope( const BreakpointEnvelope & );
	virtual ~BreakpointEnvelope( void );
	
	//	compiler-generated assignment is okay
	//	BreakpointEnvelope & operator= ( const BreakpointEnvelope & );

	//	envelope composition:
	void insertBreakpoint( double x, double y );

};	//	end of abstract class BreakpointEnvelope

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_BREAKPOINTENVELOPE_H

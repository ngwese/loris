#ifndef INCLUDE_FREQUENCYREFERENCE_H
#define INCLUDE_FREQUENCYREFERENCE_H
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
 * FrequencyReference.h
 *
 * Definition of class FrequencyReference.
 *
 * Kelly Fitz, 3 Dec 2001
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <list>
#include <memory>

//	begin namespace
namespace Loris {

class BreakpointEnvelope;
class Partial; 

// ---------------------------------------------------------------------------
//	class FrequencyReference
//
//	A functor class that creates a reference frequency envelope from
//  the longest Partial found in a given iterator range and in a 
//  specified frequency range.
//
//
class FrequencyReference
{
//	-- instance variables --
private:
	std::auto_ptr< BreakpointEnvelope > _env;
	
//	-- public interface --
public:
	//	construction: 
	FrequencyReference( std::list<Partial>::const_iterator begin, 
						std::list<Partial>::const_iterator end, 
						double minFreq, double maxFreq, long numSamps );
	FrequencyReference( std::list<Partial>::const_iterator begin, 
						std::list<Partial>::const_iterator end, 
						double minFreq, double maxFreq );
	FrequencyReference( const FrequencyReference & other );
	FrequencyReference & operator= ( const FrequencyReference & other );
	~FrequencyReference();

	//	BreakpointEnvelope access:
	BreakpointEnvelope & envelope( void ) { return *_env; }
	const BreakpointEnvelope & envelope( void ) const { return *_env; }


};	// end of class FrequencyReference

}	//	end of namespace Loris

#endif	// ndef INCLUDE_FREQUENCYREFERENCE_H

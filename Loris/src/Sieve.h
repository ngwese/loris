#ifndef INCLUDE_SIEVE_H
#define INCLUDE_SIEVE_H
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
 * Sieve.h
 *
 * Definition of class Sieve.
 *
 * Lippold Haken, 20 Jan 2001
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <list>

//	begin namespace
namespace Loris {

class Partial;

// ---------------------------------------------------------------------------
//	class Sieve
//
//	Sift labeled Partials: 
//  If any two partials with same label overlap in time,
//  keep only the longer of the two partials.
//  Set the label of the shorter duration partial to zero.
//
//
class Sieve
{
//	-- instance variables --
	double _minGapTime;

//	-- public interface --
public:
//	construction:	
	explicit Sieve( double minGapTime = 0.0 );
	~Sieve( void );
	
//	sift:
	void sift( std::list<Partial> & sift_list );
	void sift( std::list<Partial> & sift_list, 
			   std::list< Partial >::iterator sieve_begin, 
			   std::list< Partial >::iterator sieve_end  );

//	-- unimplemented --
private:
	Sieve( const Sieve & other );
	Sieve & operator= ( const Sieve & other );
	
};	//	end of class Sieve

}	//	end of namespace Loris

#endif /* ndef INCLUDE_SIEVE_H */

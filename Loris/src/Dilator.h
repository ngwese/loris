#ifndef INCLUDE_DILATOR_H
#define INCLUDE_DILATOR_H
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
 * Dilator.h
 *
 * Definition of class Dilator.
 *
 * Kelly Fitz, 26 Oct 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <PartialList.h>
#include <vector>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	class Dilator
//
//	Dilator is a class of objects for temporally dilating and compressing
//	Partials by specifying initial and target times of temporal features.
//
class Dilator
{
//	-- implementation --
	std::vector< double > _initial, _target;	//	time points
	
//	-- public interface --
public:
//	construction from n time points:
	Dilator( const double * ibegin, const double * tbegin, int n );
	~Dilator( void );
	
//	dilation:
	void dilate( Partial & p );
	void dilate( PartialList::iterator begin, PartialList::iterator end );
	
//	function call operator:
//	(should this be for a single Partial, or the range?)
	void operator() ( PartialList::iterator begin, PartialList::iterator end )
		{ dilate( begin, end ); }
	
//	-- unimplemented until useful --
private:
	Dilator( const Dilator & );
	Dilator & operator= ( const Dilator & rhs );
	
};	//	end of class Dilator

}	//	end of namespace Loris

#endif /* ndef INCLUDE_DILATOR_H */

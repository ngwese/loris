#ifndef INCLUDE_PARTIALPTRS_H
#define INCLUDE_PARTIALPTRS_H
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
 *	PartialPtrs.h
 *
 *	Type definition of Loris::PartialPtrs.
 *
 *	PartialPtrs is a collection of pointers to Partials that
 *	can be used (among other things) for algorithms that operate
 *	on a range of Partials, but don't rely on access to their
 *	container.
 *
 * Kelly Fitz, 23 May 2002
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <Partial.h>
#include <iterator>
#include <vector>

//	begin namespace
namespace Loris {

//	PartialPtrs is a collection of pointers to Partials that
//	can be used (among other things) for algorithms that operate
//	on a range of Partials, but don't rely on access to their
//	container.
typedef std::vector< Loris::Partial * > PartialPtrs;
typedef std::vector< Loris::Partial * >::iterator PartialPtrsIterator;
typedef std::vector< Loris::Partial * >::const_iterator PartialPtrsConstIterator;

//	This is a generally useful operation that allows us to adapt our
//	algorithms to work with arbitrary containers of Partials without
//	exposing the algorithms themselves in the header files.
template <typename Iter>
void fillPartialPtrs( Iter begin, Iter end, PartialPtrs & fillme )
{
	fillme.reserve( std::distance( begin, end ) );
	fillme.clear();
	while ( begin != end )
		fillme.push_back( &(*begin++) );
}

}	//	end of namespace Loris

#endif /* ndef INCLUDE_PARTIALPTRS_H */

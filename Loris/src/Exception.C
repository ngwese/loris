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
 * Exception.C
 *
 * Implementation of class Exception, a generic exception class.
 *
 * Kelly Fitz, 17 Aug 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "Exception.h"
#include <string>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif


#pragma mark -
#pragma mark construction

// ---------------------------------------------------------------------------
//	Exception constructor
// ---------------------------------------------------------------------------
//	where defaults to empty.
//
Exception::Exception( const std::string & str, const std::string & where ) :
	_sbuf( str )
{
	_sbuf.append( where );
	_sbuf.append(" ");
}
	
// ---------------------------------------------------------------------------
//	append 
// ---------------------------------------------------------------------------
//
Exception & 
Exception::append( const std::string & str )
{
	_sbuf.append(str);
	return *this;
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#ifndef INCLUDE_SPCFILE_H
#define INCLUDE_SPCFILE_H
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
 * SpcFile.h
 *
 * Implementation of class SpcFile, which reads and writes SPC files.
 *
 * Lippold Haken, 6 Nov 1999, 14 Nov 2000, export spc
 * Lippold Haken, 4 Feb 2001, import spc
 * Lippold Haken, 19 Mar 2001, combine import and export
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 
#include <PartialList.h>
#include <iosfwd>
#include <string>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	class SpcFile
//	
class SpcFile
{
//	-- instance variables --
	PartialList _partialsList;	//	collect Partials here (import)

//	-- public interface --
public:
//	construction:
//	(let compiler generate destructor)
	SpcFile( const char *infilename );
		
//	PartialList access:
	PartialList & partials( void ) { return _partialsList; }
	const PartialList & partials( void ) const { return _partialsList; }

//	export:
//
//  the optional enhanced parameter;s default value is true (for bandwidth-enhanced spc files); it
//  can be specified false for pure-sines spc files.
//
//  it is normally left at its default 
//	the optional endApproachTime parameter is in seconds; its default value is zero (and has no effect).
//  a nonzero endApproachTime indicates that the plist does not include a release, but rather ends in a 
//  static spectrum corresponding to the final breakpoint values of the partials.  the endApproachTime
//  specifies how long before the end of the sound the amplitude, frequency, and bandwidth values are
//  to be modified to make a gradual transition to the static spectrum.
 	static void Export( const std::string & filename, const PartialList & plist, double midiPitch, 
				int enhanced = true, double endApproachTime = 0. );
	static void Export( std::ostream & file, const PartialList & plist, double midiPitch, 
				int enhanced = true, double endApproachTime = 0. );
	
//	-- private interface --
private:
	SpcFile( const SpcFile & other );
	SpcFile  & operator = ( const SpcFile & rhs );

};	//	end of class SpcFile

}	//	end of namespace Loris

#endif //	ndef INCLUDE_SPCFILE_H

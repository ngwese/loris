#ifndef INCLUDE_SDIFFILE_H
#define INCLUDE_SDIFFILE_H
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
 * SdifFile.h
 *
 * Definition of class SdifFile, which reads and writes SDIF files.
 *
 * Lippold Haken, 4 July 2000
 * Lippold Haken, 20 October 2000, using IRCAM SDIF library
 * Lippold Haken, 27 March 2001, write only 7-column 1TRC, combine reading and writing classes
 * Lippold Haken, 31 Jan 2002, write either 4-column 1TRC or 6-column RABP
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include <PartialList.h>
#include <string>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	class SdifFile
//	
//	Class SdifFile represents Spectral Description Interchange Format (SDIF)
//	data files, and manages file I/O and sample conversion. Construction of
//	an SdifFile from a stream or filename automatically imports the Partial
//	data. The static Export() members export Partials to a SDIF file using
//	the specified filename.
//	
//	Loris stores partials in SDIF RBEP and RBEL frames. The RBEP and RBEL
//	frame and matrix definitions are included in the SDIF file's header.
//	Each RBEP frame contains one RBEP matrix, and each row in a RBEP matrix
//	describes one breakpoint in a Loris partial. The data in RBEP matrices
//	are SDIF 32-bit floats.
//	
//	The six columns in an RBEP matrix are: partialIndex, frequency,
//	amplitude, phase, noise, timeOffset. The partialIndex uniquely
//	identifies a partial. When Loris exports SDIF data, each partial is
//	assigned a unique partialIndex. The frequency (Hz), amplitude (0..1),
//	phase (radians), and noise (bandwidth) are encoded the same as Loris
//	breakpoints. The timeOffset is an offset from the RBEP frame time,
//	specifying the exact time of the breakpoint. Loris always specifies
//	positive timeOffsets, and the breakpoint's exact time is always be
//	earlier than the next RBEP frame's time.
//	
//	Since reassigned bandwidth-enhanced partial breakpoints are
//	non-uniformly spaced in time, the RBEP frame times are also
//	non-uniformly spaced. Each RBEP frame will contain at most one
//	breakpoint for any given partial. A partial may extend over a RBEP frame
//	and have no breakpoint specified by the RBEP frame, as happens when one
//	active partial has a lower temporal density of breakpoints than other
//	active partials.
//	
//	If partials have nonzero labels in Loris, then a RBEL frame describing
//	the labeling of the partials will precede the first RBEP frame in the
//	SDIF file. The RBEL frame contains a single, two-column RBEL matrix The
//	first column is the partialIndex, and the second column specifies the
//	label for the partial.
//	
//	In addition to RBEP frames, Loris can also read and write SDIF 1TRC
//	frames (refer to IRCAM's SDIF web site, www.ircam.fr/sdif/, for
//	definitions of standard SDIF description types). Since 1TRC frames do
//	not represent bandwidth-enhancement or the exact timing of Loris
//	breakpoints, their use is not recommended. 1TRC capabilities are
//	provided in Loris to allow interchange with programs that are unable to
//	interpret RBEP frames.
//
class SdifFile
{
//	-- instance variables --
	PartialList _partials;	//	collect Partials for reading here

//	-- public interface --
public:
//	-- construction (import) --
	SdifFile( const std::string & infilename );
	/*	Initialize an instance of SdifFile by importing Partial data from
		the file having the specified filename.
	 */
	 
//	(let compiler generate destructor)
//	~SdifFile( void ) {}
		
//	-- Partial access --
	PartialList & partials( void ) { return _partials; }
	const PartialList & partials( void ) const { return _partials; }
	/*	Return a reference to this SdifFile's list of Partials, or, for const
		SdifFiles, a const reference to this SdifFile's list of Partials (const
		PartialList &).
	 */
	 
//	-- export --
	static void Export( const std::string & filename, 
						const PartialList & plist, const bool enhanced = true);
	/*	Export the Partials in the specified PartialList to a SDIF file having
		the specified file name or path. If enhanced is true (the default),
		reassigned bandwidth-enhanced Partial data are exported in the
		six-column RBEP format. Otherwise, the Partial data is exported as
		resampled sinusoidal analysis data in the 1TRC format.
	 */
	
//	-- private interface --
private:
	SdifFile( const SdifFile & other );
	SdifFile  & operator = ( const SdifFile & rhs );

};	//	end of class SdifFile

}	//	end of namespace Loris

#endif //	ndef INCLUDE_SDIFFILE_H

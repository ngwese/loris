#ifndef INCLUDE_EXPORTSPC_H
#define INCLUDE_EXPORTSPC_H
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
 * ExportSpc.h
 *
 * Definition of class ExportSpc, which exports spc files for
 * real-time synthesis in Kyma.
 *
 * Lippold Haken, 6 Nov 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "Partial.h"
#include "Exception.h"
#include <vector>
#include <list>
#include <iosfwd>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class ExportSpc
//	
class ExportSpc
{
//	-- instance variables --
	int _partials;					// number of partials
	double _hop;					// frame hop size in seconds
	double _midiPitch;				// midi note number
	double _threshold;				// attack threshold (0.0 to keep all of attack)
	int _startFrame;				// first frame to keep (after _threshold crop)
	int _endFrame;					// last frame number to write to file
	int _markerFrame;				// frame number for a marker
	int _endApproachFrames;			// ease into final final spectrum over this many frames,
									// or 0 to disable this function (if final is quiet)
	double _startFreqTime;			// ease into frequency values during attack if this is nonzero
	int _enhanced;					// true for bandwidth-enhanced spc file format
									// false for pure sinusoidal spc file format
	
public:
//	construction:
//	(let compiler generate destructor)
	ExportSpc( int pars, double hop, double tuning, double thresh, double endt, 
			double markert = 0.0, double startFreqTime = 0.0, double endApproachTime = 0.0);
	
//	writing:
	void write( std::ostream & file,  const std::list<Partial> & plist, int refLabel );
			
};	//	end of class ExportSpc

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif //	ndef INCLUDE_EXPORTSPC_H

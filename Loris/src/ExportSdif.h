#ifndef INCLUDE_EXPORTSDIF_H
#define INCLUDE_EXPORTSDIF_H
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
 * ExportSdif.h
 *
 * Definition of class ExportSdif, which exports the
 * 1TRC SDIF format.
 *
 * Lippold Haken, 4 July 2000
 * Lippold Haken, 20 October 2000, using IRCAM SDIF library
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "Partial.h"
#include <vector>
#include <list>

extern "C" {
#include <sdif.h>
}

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif


// ---------------------------------------------------------------------------
//	class ExportSdif
//	
class ExportSdif
{
//	-- instance variables --
	double _hop;		// frame rate in seconds, or 0.0 if we are not resampling envelopes

public:
//	construction:
//	(let compiler generate destructor)
	ExportSdif( const double hop = 0.0 );
	
//	writing:
	void write( const char *outfilename, const std::list<Partial> & partials );
	
};	//	end of class ExportSdif

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif //	ndef INCLUDE_EXPORTSDIF_H

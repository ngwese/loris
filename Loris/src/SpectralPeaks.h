#ifndef INCLUDE_SPECTRALPEAKS_H
#define INCLUDE_SPECTRALPEAKS_H
/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2004 by Kelly Fitz and Lippold Haken
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
 * SpectralPeaks.h
 *
 * Definition of a type representing a collection (vector) of 
 * reassigned spectral peaks or ridges. Shared by analysis policies.
 *
 * Kelly Fitz, 29 May 2003
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 
#include <utility>
#include <vector>
#include "Breakpoint.h"

//	begin namespace
namespace Loris {

//	define the structure used to collect spectral peaks:
typedef std::vector< std::pair< double, Breakpoint > > Peaks;

}	//	end of namespace Loris

#endif /* ndef INCLUDE_SPECTRALPEAKS_H */

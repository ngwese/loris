#ifndef INCLUDE_KAISERWINDOW_H
#define INCLUDE_KAISERWINDOW_H
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
 * KaiserWindow.h
 *
 * Definition of class Loris::KaiserWindow.
 *
 * Kelly Fitz, 14 Dec 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <vector>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	class KaiserWindow
//
//	Class KaiserWindow computes a Kaiser window function (see Kaiser and 
//	Schafer, 1980) for windowing FFT data.
//
//	This _should_ _not_ be a class. Or should it? Could be a namespace like
//	BreakpointUtils.
//
class KaiserWindow
{
//	-- public interface --
public:
	static void create( std::vector< double > & samples, double shape );
	
	static double computeShape( double atten );
	static long computeLength( double width, double alpha );

//	construction is not allowed:
private:
	KaiserWindow( void );
	
};	// end of class KaiserWindow

}	//	end of namespace Loris

#endif /* ndef INCLUDE_KAISERWINDOW_H */

#ifndef INCLUDE_LORISTYPES_H
#define INCLUDE_LORISTYPES_H
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
 * LorisTypes.h
 *
 *	Define some commonly-used data types so that we can have some 
 *	guarantee about their sizes. Most of the time it won't matter,
 *	so we will just use the built-in types. If it ever does matter, 
 *	use these types.
 *
 *	If it is necessary to make adjustments for other compilers or 
 *	machines, make them in here.
 *	
 * Kelly Fitz, 28 Sept 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#if defined( __cplusplus ) 
#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif
#endif	

typedef short 			Int_16;
typedef long 			Int_32;
typedef unsigned long 	Uint_32;
typedef float			Float_32;
typedef double			Double_64;

#if defined( __cplusplus )
#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif
#endif	

#endif	/*	ndef INCLUDE_LORISTYPES_H */

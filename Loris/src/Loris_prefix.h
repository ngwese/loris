#ifndef INCLUDE_LORIS_PREFIX_H
#define INCLUDE_LORIS_PREFIX_H
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
 *	Loris_prefix.h
 *
 *	Prefix file included in all Loris headers and sources. 
 *
 * Kelly Fitz, May 2001
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 

/*
 * 	This flag enables extra debugging reports and 
 *	sanity checks. 
 *
#define Debug_Loris 1
 
 */


/*
 * 	This flag disables the Loris namespace. Never use this.
 *
#define NO_LORIS_NAMESPACE 1
 
 */

/*
 *	Define the version number of this Loris distribution.
 *	Four hex digits: major version, minor version, A for
 *	alpha or B for beta or 0 for release, alpha or beta
 *	number or 0 for release. So release version 1.2 is
 *	0x1200, and beta 1.2b7 is 0x12B7.
 */
#define LORIS_VERSION 0x10B4

/*
 *	M_PI is apparently not available everywhere (e.g. Metrowerks).
 */
#if !defined(M_PI)
	#define M_PI 3.1415926535897932384626433L
#endif /* not defined M_PI */

/*
 * 	Include a file to define types having guaranteed sizes.
 */
#include <Loris_types.h>

#endif /* ndef INCLUDE_LORIS_PREFIX_H */

#ifndef INCLUDE_IEEE_H
#define INCLUDE_IEEE_H
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
 * ieee.h
 *
 * Prototypes for IEEE floating point conversions in ieee.c.
 *
 * Kelly Fitz, 28 Sept 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */


#ifdef __cplusplus
namespace IEEE {
extern "C" {
#endif

/*	struct extended80 defined below 	*/
#ifndef __cplusplus
typedef struct extended80 extended80;
#else
struct extended80;
#endif

/*	conversion functions				*/
extern void ConvertToIeeeExtended(double num, extended80 * x) ;
extern double ConvertFromIeeeExtended(extended80 x) ;

/*	struct extended80 definition, with 
	constructors and conversion to 
	double, if C++ 								
 */
struct extended80 {
	char data[10];
	
#ifdef __cplusplus
	extended80( double x = 0. ) { ConvertToIeeeExtended( x, this ); }
	operator double( void ) const { return ConvertFromIeeeExtended( *this ); }
#endif
	
};


#ifdef __cplusplus
}	//	end extern "C"
}	//	end namespace
#endif

#endif /* ndef INCLUDE_IEEE_H */

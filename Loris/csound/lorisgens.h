#ifndef INCLUDE_LORISGENS_H
#define INCLUDE_LORISGENS_H
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
 *	lorisgens.h
 *
 *	Header file containing structure definitions for Csound unit generators
 *	supporting bandwidth-enhanced synthesis using the Loris library.
 *
 *	The lorisplay module was originally written by Corbin Champion, 2002.
 *
 * Kelly Fitz, 9 May 2002
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "cs.h"

/* declare a structure holding private internal data */
typedef struct Lorisplay_priv Lorisplay_priv;

/*	Define a structure to hold parameters for the lorisplay module. */
typedef struct 
{
	/*	standard structure holding csoudn global data (esr, ksmps, etc.) */
	OPDS h;  	
	
	/* output */
	float *result;
	
	/* unit generator parameters/arguments */
	float *time, *ifilnam, *ampenv, *freqenv, *bwenv, *fadetime;    

	/* private internal data, used by generator */
	Lorisplay_priv *bwestore;
} LORISPLAY;

#endif	/* nef INCLUDE_LORISGENS_H */
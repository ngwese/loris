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
 *	lorisAiffFile.i
 *
 *	SWIG interface file describing the Loris::AiffFile class.
 *	Include this file in loris.i to include the AiffFile class
 *	interface in the scripting module. (Can be used with the 
 *	-shadow option to SWIG to build an AiffFile class in the 
 *	scripting interface.) This file does not support exactly the 
 *	public interface of the C++ class, but has been modified to
 *	better support SWIG and scripting languages.
 *
 *
 * Kelly Fitz, 4 March 2001
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
%{
#include "AiffFile.h"
using Loris::AiffFile;
%}

class AiffFile
{
public:
//	construction (import):
	AiffFile( const char * filename );
	~AiffFile( void );
	
//	parameter access:
	int channels( void ) const;
	unsigned long sampleFrames( void ) const;
	double sampleRate( void ) const;
	int sampleSize( void ) const;

%addmethods 
{
//	sample access:	
	%new SampleVector * samples( void )
	{
		SampleVector * vec = new SampleVector( self->sampleFrames(), 0. );
		self->getSamples( vec->begin(), vec->end() );
		return vec;
	}
}	//	end of added methods

};	//	end of class AiffFile

%inline 
%{
//	AIFF export:
	void exportAiffNEW( const char * path,
					 SampleVector * samples,
					 double samplerate, int nchannels, int bitsPerSamp )
	{		
		AiffFile::Export( path, samplerate, nchannels, bitsPerSamp, 
						 samples->begin(), samples->end() );
	}
%}



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

/*
	An AiffFile represents a sample file (on disk) in the Audio Interchange
	File Format. The file is read from disk and the samples stored in memory
	upon construction of an AiffFile instance. The samples are accessed by 
	the samples() method, which converts them to double precision floats and
	returns them in a SampleVector.
 */
class AiffFile
{
public:
//	construction (import):
	AiffFile( const char * filename );
	/*	Import an AIFF file with the specified file name
		or path.
	 */
	~AiffFile( void );
	/*	Delete this AIFF data (does not affect data on disk).
	 */
	
//	parameter access:
	int channels( void ) const;
	/*	Return the number of channels of sample data this AIFF file.
	 */
	 
	unsigned long sampleFrames( void ) const;
	/*	Return the number of frames of sample data this AIFF file.
	 */
	 
	double sampleRate( void ) const;
	/*	Return the sample rate in Hz of the sample data this AIFF file.
	 */
	 
	int sampleSize( void ) const;
	/*	Return the size in bits of the sample data this AIFF file.
	 */
	 
%addmethods 
{
//	sample access:	
	%new SampleVector * samples( void )
	{
		SampleVector * vec = new SampleVector( self->sampleFrames(), 0. );
		self->getSamples( vec->begin(), vec->end() );
		return vec;
	}
	/*	Return a SampleVector containing the AIFF samples from this AIFF 
		file as double precision floats on the range -1,1.
	 */
	 
}	//	end of added methods

};



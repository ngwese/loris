#ifndef INCLUDE_AIFFFILE_H
#define INCLUDE_AIFFFILE_H
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
 * AiffFile.h
 *
 * Definition of class AiffFile, used for importing and exporting 
 * sound sample data from AIFF-format files.
 *
 * Kelly Fitz, 28 Sept 99
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <vector>
#include <string>
#include <iosfwd>

//	begin namespace
namespace Loris {

struct CkHeader;

// ---------------------------------------------------------------------------
//	class AiffFile
//
//	Class AiffFile represents a AIFF-format samples file, and
//	manages file I/O and sample conversion. Construction of an 
//	AiffFile from a stream or filename automatically imports 
//	the sample data. The static Export() members export samples 
//	(doubles) to a AIFF file using the specified stream or filename.
//
//	This class could be made more insulating at some point.
//	
class AiffFile
{
//	-- instance variables --
	double _sampleRate;	//	in Hz
	int _nChannels;		//	samples per frame, usually one (mono) in Loris
	int _sampSize;		//	in bits
	double _hop;		//  hop size in seconds (for reading SPC files)
	int _partials;		// 	number of partials (for reading SPC files)
	int _frames;		//  frames (for reading SPC files)
	
	std::vector<unsigned char> _bytes;	//	buffer used for storing raw (integer) sample data
		
//	-- public interface --
public:
	//	construction (and import):
	//	(compiler can generate destructor, copy, and assignment)

/*	Initialize an instance of AiffFile by importing data from
	the file having the specified filename or path.

 */
 	AiffFile( const std::string & filename );

/*	Initialize an instance of AiffFile by importing data from
	the specified istream.

 */
 	AiffFile( std::istream & s );
	
	//	access:
/*	Return the number of channels of sample data represented in this
	AiffFile (e.g. 1 for mono, 2 for stereo, etc).

 */
 	int channels( void ) const;

/*	Return the number of sample frames represented in this AiffFile.
	A sample frame contains one sample per channel for a single sample
	interval (e.g. mono and stereo samples files having a sample rate of
	44100 Hz both have 44100 sample frames per second of audio samples).

 */
 	unsigned long sampleFrames( void ) const;

/*	Return the sampling freqency in Hz for the sample data in this
	AiffFile.

 */
 	double sampleRate( void ) const;

/*	Return the size in bits of a single, integer audio sample in this AiffFile.

 */
 	int sampleSize( void ) const;
	
	//	disgusting, these should not be here at all, just for Spc import:
	int partials( void ) const;
	int frames( void ) const;
	double hop( void ) const;
	
/*	Convert the integer sample data to doubles and store in on the half-open
	(STL-style) range [bufBegin, bufEnd). bufEnd represents a position after
	the last valid position in the buffer, no sample is written at bufEnd.
	To convert ten samples into a buffer, use
		myAiffFile.getSamples(myBuf, myBuf+10);

 */
 	void getSamples( double * bufBegin, double * bufEnd );	//	from raw data to doubles

	//	export:
/*	Export the sample data on the half-open (STL-style) range [bufBegin, bufEnd)
	to a AIFF samples file having the specified file name or path, using the 
	specified sample rate (in Hz), number of channels, and sample size (in bits).
	bufEnd represents a position after the last valid position in the buffer, no 
	sample is read from *bufEnd.

 */
 	static void Export( const std::string & filename, double rate, int chans, int bits, 
						const double * bufBegin, const double * bufEnd );

/*	Export the sample data on the half-open (STL-style) range [bufBegin, bufEnd)
	in the format of a AIFF samples file on the specified ostream, using the 
	specified sample rate (in Hz), number of channels, and sample size (in bits).
	bufEnd represents a position after the last valid position in the buffer, no 
	sample is read from *bufEnd.

 */
 	static void Export( std::ostream & s, double rate, int chans, int bits, 
						const double * bufBegin, const double * bufEnd );
	
//	-- helpers --
private:
	//	construct from data in memory and write (export):
	AiffFile( std::ostream & s, double rate, int chans, int bits, 
			  const double * bufBegin, const double * bufEnd );
	
	//	reading:
	void read( std::istream & s );
	void readChunkHeader( std::istream & s, CkHeader & h );
	void readContainer( std::istream & s );
	void readCommonData( std::istream & s );
	void readApplicationSpecifcData( std::istream & s, int length );
	void readSampleData( std::istream & s, unsigned long chunkSize );
	void readSamples( std::istream & s );
	//	writing:
	void write( std::ostream & s, const double * bufBegin, const double * bufEnd );
	void writeCommon( std::ostream & s );
	void writeContainer( std::ostream & s );
	void writeSampleData( std::ostream & s, const double * bufBegin, const double * bufEnd );
	void writeSamples( std::ostream & s, const double * bufBegin, const double * bufEnd );
	
	//	data sizes:
	unsigned long sizeofCommon( void );
	unsigned long sizeofCkHeader( void );
	unsigned long sizeofSoundData( void );

	//	parameter validation:
	void validateParams( void );

};	//	end of class AiffFile

}	//	end of namespace Loris

#endif /* ndef INCLUDE_AIFFFILE_H */

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
//	AiffFile manages a buffer of raw (integer sample data). Creating an 
//	AiffFile from a stream or filename automatically reads the sample data, 
//	which can then be converted and accessed using getSamples(). Use the 
//	static members Export() to export samples (doubles) to a AIFF file 
//	using the specified stream or filename.
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
//	construction (import):
//	(compiler can generate destructor, copy, and assignment)
	AiffFile( const std::string & filename );
	AiffFile( std::istream & s );
	
//	export:
	static void Export( std::ostream & s, double rate, int chans, int bits, 
						const double * bufBegin, const double * bufEnd );
	static void Export( const std::string & filename, double rate, int chans, int bits, 
						const double * bufBegin, const double * bufEnd );
	
//	access:
	int channels( void ) const;
	unsigned long sampleFrames( void ) const;
	double sampleRate( void ) const;
	int sampleSize( void ) const;
	int partials( void ) const;
	int frames( void ) const;
	double hop( void ) const;
	
	void getSamples( double * bufBegin, double * bufEnd );	//	from raw data to doubles

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

#ifndef INCLUDE_AIFFFILE_H
#define INCLUDE_AIFFFILE_H
/*
 * Copyright (c) 1999-2000 Kelly Fitz and Lippold Haken
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
 * Definition of class AiffFile.
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

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

struct CkHeader;

// ---------------------------------------------------------------------------
//	class AiffFile
//
//	The SampleBuffer must be provided by clients; it is not owned by 
//	the AiffFile. Streams passed to AIFFfiles also remain the responsibility
//	of the client.
//	
class AiffFile
{
//	-- instance variables --
	double _sampleRate;	//	in Hz
	int _nChannels;		//	samples per frame, usually one (mono) in Loris
	int _sampSize;		//	in bits
	
	std::vector< double > & _samples;
	
//	-- public interface --
public:
//	construction:
//	(compiler can generate destructor, copy, and assignment)
	AiffFile( double rate, int chans, int bits, std::vector< double > & buf );
	AiffFile( const std::string & filename, std::vector< double > & buf );
	AiffFile( std::istream & s, std::vector< double > & buf );
	
//	reading and writing:
	void read( const std::string & filename );
	void read( std::istream & s );
	void write( const std::string & filename );
	void write( std::ostream & s );
	
//	access/mutation:
	double sampleRate( void ) const { return _sampleRate; }
	int numChans( void ) const { return _nChannels; }
	int sampleSize( void ) const { return _sampSize; }
	
	void setSampleRate( double x ) { _sampleRate = x; }
	void setNumChannels( int n ) { _nChannels = n; }
	void setSampleSize( int n ) { _sampSize = n; }
	
	std::vector< double > & samples( void ) { return _samples; }
	const std::vector< double > & samples( void ) const { return _samples; }
	
//	-- helpers --
private:
	//	reading:
	void readChunkHeader( std::istream & s, CkHeader & h );
	void readContainer( std::istream & s );
	void readCommonData( std::istream & s );
	void readSampleData( std::istream & s );
	void readSamples( std::istream & s );

	//	writing:
	void writeCommon( std::ostream & s );
	void writeContainer( std::ostream & s );
	void writeSampleData( std::ostream & s );
	void writeSamples( std::ostream & s );
	
	//	data sizes:
	unsigned long sizeofCommon( void );
	unsigned long sizeofCkHeader( void );
	unsigned long sizeofSoundData( void );

	//	parameter validation:
	void validateParams( void );

};	//	end of class AiffFile

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif //	ndef INCLUDE_AIFFFILE_H


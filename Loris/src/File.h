#ifndef __Loris_basic_file__
#define __Loris_basic_file__

// ===========================================================================
//	File.h
//	
//	Wrapper class for stream-based binary file access.
//
//	-kel 22 Sept 99
//
// ===========================================================================


#include "LorisLib.h"

#include <string>		//	for path name
using std::string;

#include <fstream>		//	for file i/o
using std::fstream;

#include <algorithm>	//	for byte reversal
using std::reverse;

#include <vector>
using std::vector;

Begin_Namespace( Loris )

//	endian flag type:
typedef enum { BigEndian, LittleEndian } EndianFlag;

// ---------------------------------------------------------------------------
//	class File
//
//	Binary file i/o based on fstream. Performs byte reversal when needed.
//
//
class File
{
//	-- public interface --
public:
	//	construction, need a copy constructor?:
	File( const string & s, EndianFlag = BigEndian );
	File( const File & );	//	not defined
	virtual ~File( void );
	
	//	opening:
	void open( void );
	void openWrite( void );
	void close( void );
	
	//	positioning:
	int position( void );
	void offsetPosition( int offset );
	void setPosition( int pos );
	
	boolean atEOF( void );
	
	//	instance variable access:
	const string & name( void ) const { return _path; }
	boolean swapBytes( void ) const { return _swapBytes; }
	
	//	reading:
	//	(binary file, don't use formatted extractors)
	template< class T >
	void read( T & thing )
	{
		union {
			T t;
			char c[ sizeof(T) ];
		} x;
		
		stream().read( x.c, sizeof(T) );
		
		if ( swapBytes() ) {
			reverse( x.c, x.c + sizeof(T) );
		}
			
		thing = x.t;
	}
	
	//	writing:
	//	(binary file, don't use formatted extractors)
	template< class T >
	void write( const T & thing )
	{
		union {
			T t;
			char c[ sizeof(T) ];
		} x;
		
		x.t = thing;
		
		if ( swapBytes() ) {
			reverse( x.c, x.c + sizeof(T) );
		}
		
		stream().write( x.c, sizeof(T) );
	}
	
	void write( void * bytes, int howBig )
	{
		vector< char > v( (char *) bytes, (char *) bytes + howBig );
		
		if ( swapBytes() ) {
			reverse( v.begin(), v.end() );
		}
		
		for (int i = 0; i < howBig; ++i )
			stream().put( v[i] );
	}

	
//	restricted access to fstream:
//	Only File class should access the stream; use this
//	member in case the storage of the stream changes.
private:
	fstream & stream( void ) { return _stream; }
	
//	-- instance variables --
private:
	bool _swapBytes;
	const string _path;
	fstream _stream;

};	//	end of class File


End_Namespace( Loris )

#endif	// ndef __Loris_basic_file__

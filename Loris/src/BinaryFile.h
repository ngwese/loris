#ifndef __Loris_binary_file__
#define __Loris_binary_file__

// ===========================================================================
//	BinaryFile.h
//	
//	Wrapper class for stream-based binary file access.
//
//	-kel 5 Oct 99
//
// ===========================================================================

#include "LorisLib.h"

#include <string>
#include <iosfwd>	//	won't work for MIPSPro

Begin_Namespace( Loris )

//	endian flag type:
typedef enum { BigEndian, LittleEndian } EndianFlag;

// ---------------------------------------------------------------------------
//	class BinaryFile
//
class BinaryFile
{
//	-- public interface --
public:
	//	construction:
	BinaryFile( const std::string & path, std::ios::openmode flags, EndianFlag e = BigEndian );
	~BinaryFile( void );
	
	//	construction aids:
	static BinaryFile * Append( const std::string & path, EndianFlag e = BigEndian );
	static BinaryFile * Create( const std::string & path, EndianFlag e = BigEndian );
	static BinaryFile * Open( const std::string & path, EndianFlag e = BigEndian, 
								std::ios::openmode flags = std::ios::in | std::ios::out );
		
	//	reading:
	//	(binary file, don't use formatted extractors)
	template< class T >
	void read( T & thing, boolean dontSwap = false )
	{
		union {
			T t;
			char c[ sizeof(T) ];
		} x;
		
		readBytes( x.c, sizeof(T) );
		
		if ( swapBytes() && ! dontSwap ) {
			reverseBytes( x.c, sizeof(T) );
		}
			
		thing = x.t;
	}
	
	//	writing:
	//	(binary file, don't use formatted extractors)
	template< class T >
	void write( const T & thing, boolean dontSwap = false )
	{
		union {
			T t;
			char c[ sizeof(T) ];
		} x;
		
		x.t = thing;
		
		if ( swapBytes() && ! dontSwap ) {
			reverseBytes( x.c, sizeof(T) );
		}
		
		writeBytes( x.c, sizeof(T) );
	}
	
	//	position:
	std::streampos skip( std::streamoff offset );
	
	std::streampos position( void ) const;
	void setPosition( std::streampos pos );
	
	//	endian-ness:
	//	get this outta constructor,
	//	swap default to false
	//	and hey, put a freakin' endian check in
	//	ieee.c, duh! Make sure its big before 
	//	converting. DUH.
	setBigEndian(void);
	setLittleEndian(void);
	
//	-- helpers --
protected:
	boolean swapBytes( void ) const { return _swapBytes; }

	static EndianFlag machineType( void );
	static void reverseBytes( char * c, int n ); 
	
	void readBytes( char * bytes, int howmany );
	void writeBytes( char * bytes, int howmany );
	

//	-- instance variables --
private:
	boolean _swapBytes;
	std::filebuf * _buf;

};	//	end of class BinaryFile


End_Namespace( Loris )

#endif	// ndef __Loris_basic_file__

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
#include <string>
#include <iostream>
#include <fstream>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class BinaryFile
//
//	Inherit privately from iostream so that to make use of its interface
//	internally, but provide a simpler interace to clients of BinaryFile.
//
//	Unlike the standard i/o library streams, BinaryFile checks its state
//	after i/o and exceptions (FileIOException) if the read or write was
//	unsuccessful. 
//
//	Note: no check is made on positioning the stream pointer, behavior
//	is "undefined" when streams are positioned out of bounds. 
//
class BinaryFile : private std::iostream
{
//	-- public interface --
public:
	//	construction:
	BinaryFile( void );
	BinaryFile( const std::string & path );
	~BinaryFile( void ) { close(); }
	
	//	define the stream buffer type:
	//	For now, just use filebuf, but to have buffering, need to 
	//	derived our own file buffer class. The built-in filebuf
	//	class does no buffering, and doesn't support it.
	typedef std::filebuf buffer_type;
	
	//	file stream association:
	void append( const std::string & path );	//	append, create if nec.
	void create( const std::string & path );	//	read and write, create if nec.
	void edit( const std::string & path );		//	read and write, must exist
	void view( const std::string & path );		//	read only, must exist
	 void close( void );
	
	//	template i/o members allow arbtrary single objects 
	//	to be transfered, read/writeBytes() do all the work.
	//	reading:
	template< class T >
	void read( T & thing )
	{
		union {
			T t;
			char c[ sizeof(T) ];
		} x;
		
		readBytes( x.c, sizeof(T), swapBytes() );
		
		thing = x.t;
	}
	
	//	writing:
	template< class T >
	void write( const T & thing )
	{
		union {
			T t;
			char c[ sizeof(T) ];
		} x;
		
		x.t = thing;
		
		writeBytes( x.c, sizeof(T), swapBytes() );
	}
	
	//	absolute file stream position:
	//	(note: streampos is _not_ an integral type)
	//	(that is to say, it _should not_ be)
	std::streampos tell( void );
	void seek( std::streampos x );
	
	//	relative file stream position:
	//	(note: position offsets _are_ signed integral types)
	void offset( long x, std::ios::seekdir whence = std::ios::cur );
	
	//	import status access:	
	using std::iostream::clear;
	using std::iostream::eof;
	using std::iostream::fail;
	using std::iostream::good;
	
	//	endian-ness:
	void setBigEndian( void );
	void setLittleEndian( void );
	
//	-- helpers --
protected:
	//	instance variable access:
	bool swapBytes( void ) const { return _swapBytes; }
	buffer_type & buffer( void ) { return _buf; }

	//	low level file i/o:
	void readBytes( char * bytes, int howmany, bool swap );
	void writeBytes( char * bytes, int howmany, bool swap );
	
//	-- instance variables --
private:
	enum { op_wr, op_rd, op_seek } _prevOp;	//	was the previous operation read or write?
	
	bool _swapBytes;		//	should we swap the byte order of objects read/written?
	std::filebuf _buf;		//	associated file buffer 

};	//	end of class BinaryFile


class BigEndian
{
public:
	static void read( std::istream & s, int size, long howmany, char * putemHere );
	static void write( std::ostream & s, int size, long howmany, const char * stuff );
	
};	//	end of class BigEndian

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef __Loris_basic_file__

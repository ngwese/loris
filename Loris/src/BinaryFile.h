#ifndef __Loris_binary_file__
#define __Loris_binary_file__

// ===========================================================================
//	BinaryFile.h
//	
//	Wrapper class for stream-based binary file access.
//
//	-kel 29 Sept 99
//
// ===========================================================================

#include "LorisLib.h"

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class BinaryFile
//
class File
{
//	-- public interface --
public:
	//	construction:
	BinaryFile( const string & path );
	
	//	open/close:
	void open();
	void close( void );
	
	//	reading:
	template< class T >
	void read( T & thing );
	
	template< class T >
	void read( T * things, Int howMany );
	
	//	writing:
	template< class T >
	void write( const T & thing );
	
	template< class T >
	void write( const T * things, Int howMany );
	
	//	status:
	Boolean atEof( void ) const;
	
	//	access:
	const string & path( void ) const;
	
	//	position:
	void rewind( void );
	void skip( Int offset );
	void skipToEnd( void );


};	//	end of class BinaryFile


End_Namespace( Loris )

#endif	// ndef __Loris_basic_file__

#ifndef INCLUDE_ENDIAN_H
#define INCLUDE_ENDIAN_H
// ===========================================================================
//	Endian.h
//	
//	Wrappers for endian-independent stream (file) i/o.
//	LittleEndian has not been tested.
//
//	-kel 23 May 2000
//
// ===========================================================================
#include <iosfwd>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class BigEndian
//
class BigEndian
{
public:
	static void read( std::istream & s, long howmany, int size, char * putemHere );
	static void write( std::ostream & s, long howmany, int size, const char * stuff );	
};	//	end of class BigEndian

#if 0
// ---------------------------------------------------------------------------
//	class LittleEndian
//
class LittleEndian
{
public:
	static void read( std::istream & s, long howmany, int size, char * putemHere );
	static void write( std::ostream & s, long howmany, int size, const char * stuff );
};	//	end of class LittleEndian
#endif

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_ENDIAN_H

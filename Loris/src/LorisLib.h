#ifndef __Loris_library_prefix__
#define __Loris_library_prefix__

/* ===========================================================================
 *	LorisLib.h
 *
 *	Prefix file for all source files used to build the Loris libraries.
 *
 *	Policy:
 *
 *		Loris uses and supports namespaces and the STL, but is not as
 *		uniformly generic as the STL. Template versions of functions
 *		are provided when it seems useful, and when it doesn't cause 
 *		the header or class definition to be unwieldy.
 *
 *		Everything in the Loris library is part of the Loris namespace.
 *
 *		Loris uses built-in types (int, double, etc), except when size
 *		is critical, then use the size-checked types in LorisTypes.h.
 *
 *	Compatibility:
 *
 *		The following symbols are used to make the Loris library 
 *		easier to port to platforms with inferior compilers:
 *
 *		Deprecated_cstd_headers	( no cstdio, cmath, cstdlib, new )
 *		Deprecated_iostream_headers ( no iostream, ostream, streambuf )
 *		Deprecated_string_stream ( no sstream, use strstream? )
 *		Lacks_numeric_limits ( no limits, use limits.h, lacks numeric_limits )
 *
 *		Some implementations don't have all the new-style headers they
 *		are supposed to have, so we have to use the old-style .h versions
 *		of some of the standard includes files. 
 *
 *		This can also make problems with namespace specification, because
 *		the deprecated headers may not have things like ostream in the std 
 *		namespace where they belong.
 *
 *		No_template_members  ** Don't use this symbol. **
 *		Some implementations may not allow classes to have template
 *		members. But that's too damned bad. Some classes need template
 *		members, so if the compiler can't handle it, get a modern compiler.
 *		Some classes will not compile without template members.
 *		** Don't use this symbol. **
 *		Actually, I want to use this symbol, find a way.
 *
 *		Special MIPSPro kludges are flagged, for now, by the symbol __sgi.
 *		Specifically: defined(__sgi) && ! defined(__GNUC__).
 *
 *
 *	-kel 16 Aug 99
 *
 * ===========================================================================
 */

/* ---------------------------------------------------------------------------
 *	These make namespace definitions easier to read, only usable 
 *	under C++.
 */
#if defined( __cplusplus ) && ! defined( No_Loris_namespace )
	#define Begin_Namespace( x ) namespace x {
	#define End_Namespace( x ) }
#else
	#define Begin_Namespace( x )
	#define End_Namespace( x )
#endif

Begin_Namespace( Loris )

/* ---------------------------------------------------------------------------
 *	Macros:
 */
 
/* 	I freakin' hate all caps!	
 *	And anyway, it seems to be somewhat uncool to use the 
 *	c NULL in C++. This definition will always work.
 */
#define Null 0L

/* 	shorthand for types	*/
typedef unsigned long 	ulong;
typedef unsigned int 	uint;
typedef unsigned char 	uchar;

/*	may need to define this differently 
 *	for inferior compliers.
 *	Under C++, this will be defined in the Loris namespace,
 *	so we don't have to worry about name collisions. Under c,
 *	we should try to determine whether it is already defined,
 *	and if not, define it to int (or whatever).
 */
#ifdef __cplusplus
	typedef bool	boolean;
#elif !defined(boolean)	
	typedef short boolean;
	#if !defined(false)
		enum { false = 0, true = 1 };
	#endif
#endif

/* ---------------------------------------------------------------------------
 *	Constants:
 */
static const double Pi = 3.1415926535897932384626433L;
static const double TwoPi = 2.L * 3.1415926535897932384626433L;

End_Namespace( Loris )

#ifdef __cplusplus
#if 0
// ---------------------------------------------------------------------------
//	Loris initialization class.
//
//	Linking any C++ file in Loris will cause this class to be
//	instantiated. Its a Singleton, because we only need one of
//	them. Its sole purpose is to force initialization of the 
//	Loris library. See LorisInit.C for the initialization.
//
class Init_
{
//	only instance can instantiate:
	Init_( void );
	~Init_( void );
public:	
	static const Init_ & instance( void );
};	
#endif
#include "LorisInit.h"
	
//	reference to the sole istance:	
static const Loris::Init_ & _loris_lib_initializer = Loris::Init_::instance();

#endif


#endif	/*  ndef __Loris_library_prefix__ */

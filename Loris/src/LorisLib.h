#ifndef __Loris_library_prefix__
#define __Loris_library_prefix__

// ===========================================================================
//	LorisLib.h
//
//	Prefix file for all source files used to build the Loris libraries.
//
//	Policy:
//
//		Loris uses and supports namespaces and the STL.
//		Everything in the Loris library is part of the Loris namespace.
//
//	-kel 16 Aug 99
//
// ===========================================================================


// ---------------------------------------------------------------------------
//	Types:
//
//	Define commonly-used data types so that we can have some 
//	guarantee about their sizes. Most of the time it won't matter,
//	but if it ever does, it will be difficult to address the issue
//	retroactively.
//
typedef int 			Int;
typedef unsigned int 	Uint;
typedef double			Double;
typedef bool			Boolean;

// ---------------------------------------------------------------------------
//	Includes:
//
//	Some implementations don't have all the new-style headers they
//	are supposed to have, so we have to use the old-style .h versions
//	of some of the standard includes files. 
//
//	This can also make problems with namespace specification, because
//	the deprecated headers may not have things like ostream in the std 
//	namespace where they belong.
//
//#define USE_DEPRECATED_HEADERS

// ---------------------------------------------------------------------------
//	Macros:
//
#define Null 0L

//	These make namespace definitions easier to read.
#define Begin_Namespace( x ) namespace x {
#define End_Namespace( x ) }

// ---------------------------------------------------------------------------
//	Constants:
//
static const double Pi = 3.1415926535897932384626433L;
static const double TwoPi = 2.L * Pi;


#endif	// ndef __Loris_library_prefix__
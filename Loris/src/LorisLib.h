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
//	Compatibility:
//
//		The following symbols are used to make the Loris library 
//		easier to port to platforms with inferior compilers:
//
//		USE_DEPRECATED_HEADERS:
//		Some implementations don't have all the new-style headers they
//		are supposed to have, so we have to use the old-style .h versions
//		of some of the standard includes files. 
//
//		This can also make problems with namespace specification, because
//		the deprecated headers may not have things like ostream in the std 
//		namespace where they belong.
//
//		NO_TEMPLATE_MEMBERS:
//		Some implementations may not allow classes to have template
//		members. If this symbol is defined, template members are not
//		included in the class definitions.
//
//
//	-kel 16 Aug 99
//
// ===========================================================================


// ---------------------------------------------------------------------------
//	These make namespace definitions easier to read.
//
#define Begin_Namespace( x ) namespace x {
#define End_Namespace( x ) }

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	Types:
//
//	Define commonly-used data types so that we can have some 
//	guarantee about their sizes. Most of the time it won't matter,
//	but if it ever does, it will be difficult to address the issue
//	retroactively.
//
typedef long 			Int;
typedef unsigned long 	Uint;
typedef double			Double;
typedef bool			Boolean;

//	When size _really_ matters, use these, and
//	find a way to guarantee their sizes:
typedef short 			Int_16;
typedef long 			Int_32;
typedef unsigned long 	Uint_32;
typedef float			Float_32;
typedef double			Double_64;

// ---------------------------------------------------------------------------
//	Macros:
//
#define Null 0L

// ---------------------------------------------------------------------------
//	Constants:
//
static const double Pi = 3.1415926535897932384626433L;
static const double TwoPi = 2.L * Pi;

End_Namespace( Loris )

#endif	// ndef __Loris_library_prefix__
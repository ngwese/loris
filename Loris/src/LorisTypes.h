#ifndef __Loris_Types__
#define __Loris_Types__

/* ===========================================================================
 *	LorisTypes.h
 *
 *	Define some commonly-used data types so that we can have some 
 *	guarantee about their sizes. Most of the time it won't matter,
 *	so we will just use the built-in types. If it ever does matter, 
 *	use these types.
 *
 *	If it is necessary to make adjustments for other compilers or 
 *	machines, make them in here.
 *	
 */
 
 
#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif
	/* macro does nothing under c */

typedef short 			Int_16;
typedef long 			Int_32;
typedef unsigned long 	Uint_32;
typedef float			Float_32;
typedef double			Double_64;

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif		/* macro does nothing under c */

/*	
 *	type checking functions 
 */
#if defined( __cplusplus )
	#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

		bool check_types( void );			//	C++ types
		extern "C" int check_ctypes( void );	//	c types
	#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif
#else
	bool check_ctypes( void );
#endif

#endif	/*	ndef __Loris_Types__ */
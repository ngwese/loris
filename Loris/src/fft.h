#ifndef __fast_fft_implementation__
#define __fast_fft_implementation__
/* ===========================================================================
 *	fft.h
 *
 *
 *	-kel 24 Jan 00
 *
 * ===========================================================================
 */
#include "LorisLib.h"

//	keep this in the Loris namespace when compiling C++:
Begin_Namespace( Loris )

#ifdef __cplusplus
extern "C"
#endif
void butterfly( double * real, double * imaginary, long span, long length );

End_Namespace( Loris )

#endif	//	ndef __fast_fft_implementation__
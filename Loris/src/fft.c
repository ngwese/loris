/* ===========================================================================
 *	fft.c
 *
 *	Inscrutible but probably faster fft butterfly implementation.
 *
 *	-kel 24 Jan 00
 *
 * ===========================================================================
 */
#include "LorisLib.h"
#include "fft.h"

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
#else
	#include <math.h>
#endif

// ---------------------------------------------------------------------------
//	butterfly
// ---------------------------------------------------------------------------
//	decimationStep() performs one step of the decimation in time butterfly algorithm
//	for length a power of 2.  The input is assumed to be permuted
//	to reverse binary order, so the cosine and sine factors can
//	be generated and used in normal order.
//
void butterfly( double * real, double * imaginary, long span, long length )
{
	double angle;		/* for trig functions;  for last step, this is 2pi/n */
	double cosine;		/* for trig functions */
	double sine;		/* for trig functions */
	double tempi;		/* for swapping */
	double tempr;		/* for swapping */
	double termi;		/* for swapping */
	double termr;		/* for swapping */
	long i, j;			/* butterfly indices */
	
	double * realJPlusSpan;
	double * imagJPlusSpan;
	double * realJ;
	double * imagJ;

	const long twospan = span << 1;		/* for loop termination */
	const double dangle = Pi / span;
	
	for ( i = 0, angle = 0.0;  i < span;  ++i, angle += dangle ) {
		sine = sin(angle);
		cosine = cos(angle);
		
		realJPlusSpan = real + i + span; 		/* & (real[i + span]); */
		imagJPlusSpan = imaginary + i + span;	/* & (imaginary[i + span]); */
		realJ =	real + i;						/* & (real[i]); */
		imagJ =	imaginary + i;					/* & (imaginary[i]); */

		for ( j = i;  j < length;  j += twospan ) {												
			termr = cosine * (* realJPlusSpan) + sine * (* imagJPlusSpan);
			termi = - sine * (* realJPlusSpan) + cosine * (* imagJPlusSpan);
			
			tempr = (* realJ) - termr;
			tempi = (* imagJ) - termi;
			
			(* realJ) += termr;
			(* imagJ) += termi;
			
			(* realJPlusSpan) = tempr;
			(* imagJPlusSpan) = tempi;
			
			realJPlusSpan += twospan;
			imagJPlusSpan += twospan;
			realJ += twospan;
			imagJ += twospan;
		}
	}

}


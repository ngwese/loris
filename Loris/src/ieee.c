#include <math.h>
#include "ieee.h"

/*
 * C O N V E R T   T O   I E E E   E X T E N D E D
 */

/* 
 *
 * Machine-independent I/O routines for IEEE floating-point numbers.
 *
 * NaN's and infinities are converted to HUGE_VAL or HUGE, which
 * happens to be infinity on IEEE machines.  Unfortunately, it is
 * impossible to preserve NaN's in a machine-independent way.
 * Infinities are, however, preserved on IEEE machines.
 *
 * These routines have been tested on the following machines:
 *    Apple Macintosh, MPW 3.1 C compiler
 *    Apple Macintosh, THINK C compiler
 *    Silicon Graphics IRIS, MIPS compiler
 *    Cray X/MP and Y/MP
 *    Digital Equipment VAX
 *
 *
 * Implemented by Malcolm Slaney and Ken Turkowski.
 *
 * Malcolm Slaney contributions during 1988-1990 include big- and little-
 * endian file I/O, conversion to and from Motorola's extended 80-bit
 * floating-point format, and conversions to and from IEEE single-
 * precision floating-point format.
 *
 * In 1991, Ken Turkowski implemented the conversions to and from
 * IEEE double-precision format, added more precision to the extended
 * conversions, and accommodated conversions involving +/- infinity,
 * NaN's, and denormalized numbers.
 *
 */
 
 /*
  *	Kelly added the namespace and extern "C" wrapper, just in case
  * it ever gets compiled by a C++ compiler (it should agree with 
  *	the header ieee.h, or else it won't link).
  *	Also added a typedef for extended80, and used it in the conversion
  * routines. ConvertToIeeeExtended() should take a double arg, not 
  * an int. 
  *
  *	These routines assume that the the data is big endian.
  *
  *	Changes are copyright (c) 1999-2000 Kelly Fitz and Lippold Haken, and
  *	are distributed freely and without warranty under the terms of the GNU 
  *	General Public License. See the COPYRIGHT and LICENSE documents.
  *	
  *	Kelly Fitz, 28 Sept 1999
  *	
  */

#ifndef HUGE_VAL
# define HUGE_VAL HUGE
#endif							/* HUGE_VAL */

#ifdef __cplusplus
namespace IEEE {
extern "C" {
#endif

#define FloatToUnsigned(f)((int)(((int)(f - 2147483648.0)) + 2147483647L) + 1)

void ConvertToIeeeExtended(double num, extended80 * x)
{
	int sign;
	int expon;
	double fMant, fsMant;
	int hiMant, loMant;
	char * bytes = x->data;

	if (num < 0) {
		sign = 0x8000;
		num *= -1;
	} else
		sign = 0;

	if (num == 0) {
		expon = 0;
		hiMant = 0;
		loMant = 0;
	} else {
		fMant = frexp(num, &expon);
		if ((expon > 16384) || !(fMant < 1)) {	/* Infinity or NaN */
			expon = sign | 0x7FFF;
			hiMant = 0;
			loMant = 0;
		}
		 /* infinity */ 
		else {					/* Finite */
			expon += 16382;
			if (expon < 0) {	/* denormalized */
				fMant = ldexp(fMant, expon);
				expon = 0;
			}
			expon |= sign;
			fMant = ldexp(fMant, 32);
			fsMant = floor(fMant);
			hiMant = FloatToUnsigned(fsMant);
			fMant = ldexp(fMant - fsMant, 32);
			fsMant = floor(fMant);
			loMant = FloatToUnsigned(fsMant);
		}
	}

	bytes[0] = expon >> 8;
	bytes[1] = expon;

	bytes[2] = hiMant >> 24;
	bytes[3] = hiMant >> 16;
	bytes[4] = hiMant >> 8;
	bytes[5] = hiMant;

	bytes[6] = loMant >> 24;
	bytes[7] = loMant >> 16;
	bytes[8] = loMant >> 8;
	bytes[9] = loMant;
	
}

#ifndef HUGE_VAL
# define HUGE_VAL HUGE
#endif							/* HUGE_VAL */

# define UnsignedToFloat(u)(((double)((long)(u - 2147483647L - 1))) + 2147483648.0)

/****************************************************************
 * Extended precision IEEE floating-point conversion routine.
 ****************************************************************/

double ConvertFromIeeeExtended(extended80 x)
{								/* LCN */ /* ? */
	double f;
	int expon;
	int hiMant, loMant;
	char * bytes = x.data;

	expon = ((bytes[0] & 0x7F) << 8) | (bytes[1] & 0xFF);
	hiMant = ((int)(bytes[2] & 0xFF) << 24)
		| ((int)(bytes[3] & 0xFF) << 16)
		| ((int)(bytes[4] & 0xFF) << 8)
		| ((int)(bytes[5] & 0xFF));
	loMant = ((int)(bytes[6] & 0xFF) << 24)
		| ((int)(bytes[7] & 0xFF) << 16)
		| ((int)(bytes[8] & 0xFF) << 8)
		| ((int)(bytes[9] & 0xFF));

	if (expon == 0 && hiMant == 0 && loMant == 0)
		f = 0;
	else {
		if (expon == 0x7FFF)	/* Infinity or NaN */
			f = HUGE_VAL;
		else {
			expon -= 16383;
			f = ldexp(UnsignedToFloat(hiMant), expon -= 31);
			f += ldexp(UnsignedToFloat(loMant), expon -= 32);
		}
	}

	if (bytes[0] & 0x80)
		f = -f;
#if 0	//	CW6 optimizer doesn't like this, internal compiler error!
		return -f;
	else
		return f;
#endif

	return f;
}

#ifdef __cplusplus
}	//	end extern "C"
}	//	end namespace
#endif


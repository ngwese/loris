/*
 * Copyright (c) 1999-2000 Kelly Fitz and Lippold Haken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * KaiserWindow.C
 *
 * Implementation of class Loris::KaiserWindow.
 *
 * Kelly Fitz, 14 Dec 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "KaiserWindow.h"
#include "Exception.h"
#include "pi.h"
#include <cmath>

using namespace std;

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

//	prototypes for static helpers, defined below
static double factorial( double );
static double zeroethOrderBessel( double );

// ---------------------------------------------------------------------------
//	create
// ---------------------------------------------------------------------------
//	Compute a Kaiser window length samples long and using control parameter 
//	alpha. See Oppenheim and Schafer:  "Digital Signal Processing" (1975), 
//	p. 452 for further explanation of the Kaiser window. Also, see Kaiser 
//	and Schafer, 1980.
//
//	This could be a template function taking iterators in place of 
//	the vector.
//
void
KaiserWindow::create( vector< double > & samples, double shape )
{   
 	long length = samples.size();
 	if ( length == 0 ) 
 		return;
 	
//	Compute the window bounds:
	double offset = -0.5 * (length - 1);

//	Pre-compute the denominator in the kaiser equation.	
	double denom = zeroethOrderBessel( shape );
	
//	Pre-compute the square of half of one less than the window length.
	double Z( pow( ((length - 1.) / 2.), 2.) );

//	Compute the window samples.
	for ( long i = 0; i < length; ++i )
	{
		double x( i + offset );
		
		//	sanity:
		Assert( x * x <= Z );
		
		double arg = sqrt(1.0 - (x * x / Z));
		samples[i] = zeroethOrderBessel( shape * arg ) / denom;
	}
}

// ---------------------------------------------------------------------------
//	factorial
// ---------------------------------------------------------------------------
//
static double factorial( double val )
{
	double outval = 1.0;	//	could initialize with val--, right?
    while (val > 1) 
    	outval *= val--;
    return outval;
}

// ---------------------------------------------------------------------------
//	zeroethOrderBessel
// ---------------------------------------------------------------------------
//	Compute the zeroeth order Bessel function at val using the series expansion.
//
static double zeroethOrderBessel( double val )
{
	double besselValue = 1.0;
	double term;
	int	i = 1;

	do
	{
		term = pow(0.5 * val, double(i)) / factorial(i);
		besselValue += (term * term);
		i++;
	} while (term  > .000001 * besselValue);
	
	return besselValue;
}

// ---------------------------------------------------------------------------
//	computeShape
// ---------------------------------------------------------------------------
//	Compute the Kaiser window shaping parameter from the specified attenuation 
//	of side lobes. This algorithm is given in Kaiser an Schafer,1980 and is 
//	supposed to give better than 0.36% accuracy (Kaiser and Schafer 1980).
//
double
KaiserWindow::computeShape( double atten )
{
	double alpha;
	
	if ( atten > 60.0 )
	{
		alpha = 0.12438 * (atten + 6.3);
	}
	else if ( atten > 13.26 )
	{
		alpha =	0.76609L * ( pow((atten - 13.26), 0.4) ) + 
							0.09834L * (atten - 13.26L);
	}
	else
	{
		//	can't have less than 13dB.
		alpha = 0.0;
	}
	
	return alpha;
}
// ---------------------------------------------------------------------------
//	computeLength
// ---------------------------------------------------------------------------
//	Compute the length (in samples) of the Kaiser window from the desired 
//	(approximate) main lobe width and the control parameter. Of course, since 
//	the window must be an integer number of samples in length, your actual 
//	lobal mileage may vary. This equation appears in Kaiser and Schafer 1980.
//
//	The main width of the main lobe must be normalized by the sample rate,
//	that is, it is a fraction of the sample rate.
//
long
KaiserWindow::computeLength( double width, double atten )
{
	double alpha = computeShape( atten );

	//	The last 0.5 is cheap rounding.
	return long(1.0 + (2. * sqrt((Pi*Pi) + (alpha*alpha)) / (Pi * width)) + 0.5);
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif


// ===========================================================================
//	KaiserWindow.C
//	
//	Implementation of Loris::KaiserWindow.
//
//	-kel 14 Dec 99
//
// ===========================================================================
#include "KaiserWindow.h"
#include "Exception.h"

using namespace std;

Begin_Namespace( Loris )

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
void
KaiserWindow::create( vector< double > & samples, double shape )
{   
 	long length = samples.size();
 	if ( length == 0 ) 
 		return;
 	
//	Compute the window bounds:
	long lowerLimit, upperLimit;
	double offset;		
	if (length % 2)		// odd length
	{
		lowerLimit = - (length - 1) / 2;
		upperLimit = (length - 1) / 2;
		offset = - (length - 1) / 2;
	}
	else					// even length
	{
		//	need to be careful computing even length windows:
		lowerLimit = - length / 2;
		upperLimit = length / 2 - 1;
		offset = (- length / 2) + 0.5;
	}
	
//	sanity check:
	Assert( upperLimit - lowerLimit + 1 == length );

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

End_Namespace( Loris )


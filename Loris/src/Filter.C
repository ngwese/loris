// ---------------------------------------------------------------------------
//	Filter.C
//
//	Implementations of filters for bandwidth-enhanced partial synthesis.
//
//	This is only partly implemented, should clean it up.
//
//	Kelly Fitz 
//	May 1998
// ---------------------------------------------------------------------------

#include "LorisLib.h"
#include "Filter.h"
#include "Exception.h"

#include <algorithm>	//	for rotate()

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	Mkfilter constructor
// ---------------------------------------------------------------------------
//	Construct from coefficient vectors and gain.
//
Mkfilter::Mkfilter( const vector< Double > & vcx, const vector< Double > & vcy, Double gain ) : 
	xv( vcx.size(), 0. ), 
	yv( vcy.size(), 0. ),
	xCoeffs( vcx ),
	yCoeffs( vcy ),
	scale( 1. / gain )
{
}

// ---------------------------------------------------------------------------
//	nextSample
// ---------------------------------------------------------------------------
//	Implement recurrence relation. xCoeffs holds the MA coefficients, yCoeffs
//	holds the AR coeffs. The coefficient vectors and delay lines are ordered
//	by decreasing age, such that the new input sample goes at the end of xv
//	and the new output sample goes at the end of yv. This should be reversed.
//
Double
Mkfilter::nextSample( Double input )
{ 
	shift(xv);
	xv[ xv.size() - 1 ] = input * scale;
	shift(yv);

	Double output = 0.;
	for ( Int i = 0; i < xCoeffs.size() ; ++i )
		output += xCoeffs[i] * xv[i];
		
	for ( Int j = 0; j < yCoeffs.size() - 1; ++j )
		output += yCoeffs[j] * yv[j];
	             
	yv[ yv.size() - 1 ] = output;
	
	return output;
}

// ---------------------------------------------------------------------------
//		shift
// ---------------------------------------------------------------------------
//
inline void
Mkfilter::shift( vector< Double > & v )
{ 
	std::rotate( v.begin(), v.begin() + 1, v.end() );
}

#pragma mark - 
#pragma mark virtual constructors

// ---------------------------------------------------------------------------
//	Create
// ---------------------------------------------------------------------------
//	Chebychev order 3, cutoff 500, ripple -1.
//
Mkfilter *
Mkfilter::Create( void )
{
	const Double xCoeffs[] = { 1., 3., 3., 1. };
	const Double yCoeffs[] = { 0.9320209046, -2.8580608586, 2.9258684252, 0. };
	const Double gain = 4.663939184e+04;
	
	const Double extraScaling = 6.;

	vector< Double > vcx( xCoeffs, xCoeffs + 4 );
	vector< Double > vcy( yCoeffs, yCoeffs + 4 );

	try {
		return new Mkfilter( vcx, vcy, gain * extraScaling );
	}
	catch ( LowMemException & ex ) {
		ex.append(  "Failed to Create a Mkfilter." );
		throw;
		return Null; 	//	not reached
	}
}

// ---------------------------------------------------------------------------
//	CreateNarrow
// ---------------------------------------------------------------------------
//	Chebychev order 3, cutoff 200, ripple -0.1.
//
Mkfilter *
Mkfilter::CreateNarrow( void )
{
	const Double xCoeffs[] = { 1., 3., 3., 1. };
	const Double yCoeffs[] = { 0.9446013697, -2.8876354452, 2.9430115837, 0. };
	const Double gain = 2.169816230e+05;
	
	const Double extraScaling = 6.;

	vector< Double > vcx( xCoeffs, xCoeffs + 4 );
	vector< Double > vcy( yCoeffs, yCoeffs + 4 );

	try {
		return new Mkfilter( vcx, vcy, gain * extraScaling );
	}
	catch ( LowMemException & ex ) {
		ex.append(  "Failed to Create a Mkfilter." );
		throw;
		return Null; 	//	not reached
	}
}


// ---------------------------------------------------------------------------
//	CreateWide
// ---------------------------------------------------------------------------
//	Chebychev order 3, cutoff 1000, ripple -1.
//
Mkfilter *
Mkfilter::CreateWide( void )
{
	const Double xCoeffs[] = { 1., 3., 3., 1. };
	const Double yCoeffs[] = { 0.8687010111, -2.7146444787, 2.8446174086, 0. };
	const Double gain = 6.032914230e+03;
	
	const Double extraScaling = 5.;

	vector< Double > vcx( xCoeffs, xCoeffs + 4 );
	vector< Double > vcy( yCoeffs, yCoeffs + 4 );

	try {
		return new Mkfilter( vcx, vcy, gain * extraScaling );
	}
	catch ( LowMemException & ex ) {
		ex.append(  "Failed to Create a Mkfilter." );
		throw;
		return Null; 	//	not reached
	}
}

End_Namespace( Loris )

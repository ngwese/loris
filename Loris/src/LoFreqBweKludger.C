// ===========================================================================
//	LoFreqBweKludger.C
//
//	Implementation of Loris::LoFreqBweKludger.
//
//	BW enhanced synthesis still sounds bad if applied to low frequency
//	partials. For breakpoints below a certain cutoff, it is best to set
//	the bandwidth to zero, and adjust the amplitude to account for the
//	missing noise energy. 
//
//	This kludger does the trick. Inherits PartialIterator, and transforms
//	amplitudes and bandwidths of low frequency Partials.
//
//	-kel 7 Oct 99
//
// ===========================================================================

#include "LorisLib.h"
#include "LoFreqBweKludger.h"
#include "Breakpoint.h"

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
	using namespace std;	//	none of the other headers includes std
#else
	#include <math.h>
#endif


Begin_Namespace( Loris )
// ---------------------------------------------------------------------------
//	construction with Partial
// ---------------------------------------------------------------------------
//
LoFreqBweKludger::LoFreqBweKludger( const Partial & pin, double f ) :
	 _cutoff( f ), 
	 PartialDecorIterator( pin )
{
}

// ---------------------------------------------------------------------------
//	construction
// ---------------------------------------------------------------------------
//
LoFreqBweKludger::LoFreqBweKludger( double f ) : 
	_cutoff( f ), 
	PartialDecorIterator() 
{
}

// ---------------------------------------------------------------------------
//	amplitude
// ---------------------------------------------------------------------------
//
double
LoFreqBweKludger::amplitude( void ) const
{
	if ( iterator()->frequency() > _cutoff )
		return iterator()->amplitude();
	else
		return iterator()->amplitude() * sqrt(1. - bwclamp( iterator()->bandwidth() ) );
}

// ---------------------------------------------------------------------------
//	bandwidth
// ---------------------------------------------------------------------------
//
double
LoFreqBweKludger::bandwidth( void ) const
{
	if ( iterator()->frequency() > _cutoff )
		return bwclamp( iterator()->bandwidth() );
	else
		return 0.;
}

// ---------------------------------------------------------------------------
//	kludger bwclamp
// ---------------------------------------------------------------------------
//	(static)
//	
inline double
LoFreqBweKludger::bwclamp( double bw )
{
	if( bw > 1. )
		return 1.;
	else if ( bw < 0. )
		return 0.;
	else
		return bw;
}


End_Namespace( Loris )

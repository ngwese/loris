// ===========================================================================
//	Analyzer.C
//	
//	Implementation of class Loris::Analyzer.
//
//	-kel 5 Dec 99
//
// ===========================================================================
#include "Analyzer.h"

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	Analyzer constructor
// ---------------------------------------------------------------------------
//
Analyzer::Analyzer( void )
{
}

// ---------------------------------------------------------------------------
//	Analyzer destructor
// ---------------------------------------------------------------------------
//
Analyzer::~Analyzer( void )
{
}

// ---------------------------------------------------------------------------
//	analyze
// ---------------------------------------------------------------------------
//
void 
Analyzer::analyze( std::vector< double > & buf, double srate )
{
//	loop over short-time analysis frames:

	//	compute reassigned spectrum,
	//	identify ridges:
	_spectrum( buf, index, srate );

	//	perform bandwidth association:

	//	apply amplitude thresholds:

	//	match ridges identified in adjacent
	//	short-time frames:
	
	//	spawn Partials:
	
//	end of loop over short-time frames
}

End_Namespace( Loris )

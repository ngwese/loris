#ifndef __Loris_filter__
#define __Loris_filter__

// ===========================================================================
//	Filter.h
//	
//	Class definition for Loris::Filter.
//	
//	The modulator in the Bandwidth-Enhanced Oscillator uses lowpass-filtered
//	gaussian noise, Filter represents a lowpass filter that can be used for
//	that purpose. 
//
//	This version is not very reusable, it implements only a third order 
//	Chebychev, and the coefficients have to be passed in. This is copied
//	directly from a hack in Lemur, without the base class.
//
//	Check this out: http://www.cs.york.ac.uk/~fisher/mkfilter/
//	Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
//  Command line: /www/usr/fisher/helpers/mkfilter -Bu -Lp -o 4 -a 0.0113378685 0.0000000000 -l
//
//	-kel 1 Sep 99
//
// ===========================================================================


#include "LorisLib.h"

#include <vector>

Begin_Namespace( Loris )

using std::vector;

// ---------------------------------------------------------------------------
//	€ class Mkfilter
//	
class Mkfilter
{
//	-- public interface --
public:
//	construction:
	Mkfilter( const vector< Double > &, const vector< Double > &, Double );

//	next filtered sample from input sample:				
	Double nextSample( Double );
	
//	virtual constructors:
//	(three most commonly-used filters in Lemur)
	static Mkfilter * Create( void );
	static Mkfilter * CreateNarrow( void );
	static Mkfilter * CreateWide( void );
	
private:
//	state:
	vector< Double > xv, yv;
	vector< Double > xCoeffs, yCoeffs;
	Double scale;
	
static inline void shift( vector< Double > & );
	
};	//	end of class Mkfilter

End_Namespace( Loris )

#endif	// ndef __Loris_filter__
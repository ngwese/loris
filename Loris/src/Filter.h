#ifndef __Loris_filter__
#define __Loris_filter__
// ===========================================================================
//	Filter.h
//	
//	Class definition for Loris::Filter, a generic ARMA digital filter.
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
#include <utility>

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class Filter
//
//	Construct by passing in a vector of MA (x) coefficients and a vector
//	of AR (y) coefficients and (optionally) the filter gain, which is used
//	to normalize the fitler output, and which can, alternatively, be
//	incorporated into the MA coefficients. 
//
//	Filter is currently a leaf class (no virtual destructor, no virtual 
//	functions, no access to private instance variables), but someday
//	it can easily be altered to serve as a base class for other kinds
//	of filters, or for extension classes that compute the coefficients.  
//	
class Filter
{
//	-- public interface --
public:
//	construction:
	Filter( const std::vector< double > & vcx, 
			const std::vector< double > & vcy , 
			double gain = 1. );
	Filter( const Filter & other );
		
//	next filtered sample from input sample:				
	double nextSample( double );
	
//	coefficient computation:
//	Return a pair of coefficient vectors, first is ma, second is ar.
//	The gain and scaling are accounted for in the ma coefficients.
//	(These are three commonly-used filters from Lemur.)
	//	Chebychev order 3, cutoff 500, ripple -1.
	static std::pair< const std::vector< double >, const std::vector< double > >
		NormalCoefs( void );
		
	//	Chebychev order 3, cutoff 200, ripple -0.1.
	static std::pair< const std::vector< double >, const std::vector< double > >
		NarrowCoefs( void );
		
	static std::pair< const std::vector< double >, const std::vector< double > >
		WideCoefs( void );
	
private:
//	state:
	std::vector< double > _xv, _yv;
	std::vector< double > _maCoefs, _arCoefs;
	double _scale;
	
//	helper:
static inline void shift( std::vector< double > & );
	
};	//	end of class Filter

End_Namespace( Loris )

#endif	// ndef __Loris_filter__
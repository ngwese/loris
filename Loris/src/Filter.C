// ===========================================================================
//	Filter.C
//
//	Implementations of Loris::Filter, a generic ARMA digital filter.
//
//	Check this out: http://www.cs.york.ac.uk/~fisher/mkfilter/
//	Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
//  Command line: /www/usr/fisher/helpers/mkfilter -Bu -Lp -o 4 -a 0.0113378685 0.0000000000 -l
//
//	-kel 1 Sep 99
//
// ===========================================================================
#include "Filter.h"
#include "Exception.h"

#include <algorithm>	//	for rotate()
#include <vector>
using std::vector;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	Filter constructor
// ---------------------------------------------------------------------------
//	Construct by passing in a vector of MA (x) coefficients and a vector
//	of AR (y) coefficients and (optionally) the filter gain, which is used
//	to normalize the fitler output.
//
Filter::Filter( const vector< double > & vcx, const vector< double > & vcy, double gain ) : 
	_xv( vcx.size(), 0. ), 
	_yv( vcy.size(), 0. ),
	_maCoefs( vcx ),
	_arCoefs( vcy ),
	_scale( 1. / gain ),
	_p(0)
{
}

// ---------------------------------------------------------------------------
//	Filter copy constructor
// ---------------------------------------------------------------------------
//	Don't copy the filter state, that's silly.
//
Filter::Filter( const Filter & other ) : 
	_xv( other._maCoefs.size(), 0. ), 
	_yv( other._arCoefs.size(), 0. ),
	_maCoefs( other._maCoefs ),
	_arCoefs( other._arCoefs ),
	_scale( other._scale ),
	_p(0)
{
}

// ---------------------------------------------------------------------------
//	nextSample
// ---------------------------------------------------------------------------
//	Implement recurrence relation. _maCoefs holds the MA coefficients, _arCoefs
//	holds the AR coeffs. The coefficient vectors and delay lines are ordered
//	by decreasing age, such that the new input sample goes at the end of _xv
//	and the new output sample goes at the end of _yv. This is nice because it
//	makes no difference whether the client specifies the zeroeth (no delay) 
//	AR coefficient, which must be 0. anyway.
//
double
Filter::nextSample( double input )
{ 
	double output = 0.;
	
	shift(_xv);
	_xv.back() = input * _scale;
	shift(_yv);

	for ( int i = 0; i < _maCoefs.size(); ++i )
		output += _maCoefs[i] * _xv[i];
	for ( int j = 0; j < _arCoefs.size(); ++j )
		output += _arCoefs[j] * _yv[j];
	             
	_yv[ _yv.size() - 1 ] = output;
/*	
	_xv[_p] = input * _scale;
	for ( int i = _p; i < _maCoefs.size(); ++i )
		output += _maCoefs[i] * _xv[i-_p];
	for ( int i = 0; i < _p; ++i )
		output += _maCoefs[i] * _xv[i-_p+_maCoefs.size()];
	for ( int j = _p; j < _arCoefs.size(); ++j )
		output += _arCoefs[j] * _yv[j-_p];
	for ( int j = 0; j < _p; ++j )
		output += _arCoefs[j] * _yv[j-_p+_arCoefs.size()];
	_yv[_p] = output;
	++_p;
*/		
	return output;
}

// ---------------------------------------------------------------------------
//		shift
// ---------------------------------------------------------------------------
//
inline void
Filter::shift( vector< double > & v )
{ 
	std::rotate( v.begin(), v.begin() + 1, v.end() );
}


End_Namespace( Loris )

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

#include "LorisLib.h"
#include "Filter.h"
#include "Exception.h"

#include <algorithm>	//	for rotate()

#include <vector>
using std::vector;

#include <utility>
using std::pair;

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
	_scale( 1. / gain )
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
	_scale( other._scale )
{
}

// ---------------------------------------------------------------------------
//	nextSample
// ---------------------------------------------------------------------------
//	Implement recurrence relation. _maCoefs holds the MA coefficients, _arCoefs
//	holds the AR coeffs. The coefficient vectors and delay lines are ordered
//	by decreasing age, such that the new input sample goes at the end of _xv
//	and the new output sample goes at the end of _yv. This should be reversed.
//
double
Filter::nextSample( double input )
{ 
	shift(_xv);
	_xv.back() = input * _scale;
	shift(_yv);

	double output = 0.;
	for ( int i = 0; i < _maCoefs.size() ; ++i )
		output += _maCoefs[i] * _xv[i];
		
	for ( int j = 0; j < _arCoefs.size() - 1; ++j )	//	-1?
		output += _arCoefs[j] * _yv[j];
	             
	_yv[ _yv.size() - 1 ] = output;
	
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

#pragma mark - 
#pragma mark coefficient computation
// ---------------------------------------------------------------------------
//	NormalCoefs
// ---------------------------------------------------------------------------
//	Chebychev order 3, cutoff 500, ripple -1.
//	(static)
//	The filter gain and the extra scaling are incorporated in the
//	MA coefficients.
//
pair< const vector< double >, const vector< double > >
Filter::NormalCoefs( void )
{
	static const double gain = 4.663939184e+04;
	static const double extraScaling = 6.;
	static const double maCoefs[] = { 1. * extraScaling / gain, 3. * extraScaling / gain, 
									  3. * extraScaling / gain, 1. * extraScaling / gain };
	static const double arCoefs[] = { 0.9320209046, -2.8580608586, 2.9258684252, 0. };

	return std::make_pair( vector< double >( maCoefs, maCoefs + 4 ), 
						   vector< double >( arCoefs, arCoefs + 4 ) );
}

// ---------------------------------------------------------------------------
//	NarrowCoefs
// ---------------------------------------------------------------------------
//	Chebychev order 3, cutoff 200, ripple -0.1.
//	(static)
//	The filter gain and the extra scaling are incorporated in the
//	MA coefficients.
//
pair< const vector< double >, const vector< double > >
Filter::NarrowCoefs( void )
{
	static const double gain = 2.169816230e+05;
	static const double extraScaling = 6.;
	static const double maCoefs[] = { 1. * extraScaling / gain, 3. * extraScaling / gain, 
									  3. * extraScaling / gain, 1. * extraScaling / gain };
	static const double arCoefs[] = { 0.9446013697, -2.8876354452, 2.9430115837, 0. };

	return std::make_pair( vector< double >( maCoefs, maCoefs + 4 ), 
						   vector< double >( arCoefs, arCoefs + 4 ) );
}

// ---------------------------------------------------------------------------
//	WideCoefs
// ---------------------------------------------------------------------------
//	Chebychev order 3, cutoff 1000, ripple -1.
//	(static)
//	The filter gain and the extra scaling are incorporated in the
//	MA coefficients.
//
pair< const vector< double >, const vector< double > >
Filter::WideCoefs( void )
{
	static const double gain = 6.032914230e+03;
	static const double extraScaling = 5.;
	static const double maCoefs[] = { 1. * extraScaling / gain, 3. * extraScaling / gain, 
									  3. * extraScaling / gain, 1. * extraScaling / gain };
	static const double arCoefs[] = { 0.8687010111, -2.7146444787, 2.8446174086, 0. };

	return std::make_pair( vector< double >( maCoefs, maCoefs + 4 ), 
						   vector< double >( arCoefs, arCoefs + 4 ) );
}

End_Namespace( Loris )

/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2004 by Kelly Fitz and Lippold Haken
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
 * Filter.C
 *
 * Implementation of class Loris::Filter, a generic ARMA digital filter.
 *
 * Kelly Fitz, 1 Sept 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include "config.h"
#endif

#include "Filter.h"

#include <algorithm>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	default construction
// ---------------------------------------------------------------------------
//
Filter::Filter( void ) :
	_maCoefs( 1, 1.0 ),
	_arCoefs( 1, 1.0 ),
	_delayline( 1, 0 ),
	_gain( 1.0 )
{
}

// ---------------------------------------------------------------------------
//	copy construction
// ---------------------------------------------------------------------------
//	Do not copy delay line.
//
Filter::Filter( const Filter & other ) :
	_delayline( other._delayline.size(), 0. ),
	_maCoefs( other._maCoefs ),
	_arCoefs( other._arCoefs ),
	_gain( other._gain )
{
	Assert( _delayline.size() >= _maCoefs.size() - 1 );
	Assert( _delayline.size() >= _arCoefs.size() - 1 );
}

// ---------------------------------------------------------------------------
//	assignment
// ---------------------------------------------------------------------------
//	Do not copy delay line.
//
Filter &
Filter::operator=( const Filter & rhs )
{
	if ( &rhs != this )
	{
		_delayline.resize( rhs._delayline.size() );
		std::fill( _delayline.begin(), _delayline.end(), 0 );
		_maCoefs = rhs._maCoefs;
		_arCoefs = rhs._arCoefs;
		_gain = rhs._gain;

		Assert( _delayline.size() >= _maCoefs.size() - 1 );
		Assert( _delayline.size() >= _arCoefs.size() - 1 );
	}
	return *this;
}

// ---------------------------------------------------------------------------
//	clear
// ---------------------------------------------------------------------------
//	Clear the delay line.
//
void
Filter::clear( void )
{
	std::fill( _delayline.begin(), _delayline.end(), 0 );
	Assert( _delayline.size() >= _maCoefs.size() - 1 );
	Assert( _delayline.size() >= _arCoefs.size() - 1 );
}

// ---------------------------------------------------------------------------
//	sample
// ---------------------------------------------------------------------------
//	Implement recurrence relation. _maCoefs holds the MA coefficients, _arCoefs
//	holds the AR coeffs. The coefficient vectors and delay lines are ordered
//	by increasing age.
//
//
double
Filter::sample( double input )
{ 
	double wn = std::inner_product( _arCoefs.begin()+1, _arCoefs.end(), _delayline.begin(), input );
	_delayline.push_front( wn );
	double output = std::inner_product( _maCoefs.begin(), _maCoefs.end(), _delayline.begin(), 0. );
	_delayline.pop_back();
		
	return output * _gain;
}

}	//	end of namespace Loris

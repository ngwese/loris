/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
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
	#include <config.h>
#endif

#include <Filter.h>
#include <Exception.h>
#include <Notifier.h>

//	needed for constructor:
#if defined(NO_TEMPLATE_MEMBERS)
#include <algorithm>
#include <numeric>
#endif

//	begin namespace
namespace Loris {

#if defined(NO_TEMPLATE_MEMBERS)
// ---------------------------------------------------------------------------
//	Filter constructor
// ---------------------------------------------------------------------------
//	This constructor is only compiled if member templates are not 
//	allowed, otherwise the more versatile template constructor in
//	the class definition is used.
//
Filter::Filter( const double * ma_begin, const double * ma_end,
		const double * ar_begin, const double * ar_end,
		double gain = 1. ) :
	_maCoefs( ma_begin, ma_end ),
	_arCoefs( ar_begin, ar_end ),
	_delayline( std::max( ma_end-ma_begin, ar_end-ar_begin ) - 1, 0. ),
	_gain( gain )
{
	if ( *ar_begin == 0. )
	{
		Throw( InvalidObject, "Tried to create a Filter with zero AR coefficient at zero delay." );
	}

	debugger << "constructing a Filter with " << _maCoefs.size();
	debugger << " feed-forward coefficients and " << _arCoefs.size();
	debugger << " feedback coefficients, with a delay lines of length ";
	debugger << _delayline.size() << std::endl;

	if ( *ar_begin != 1. )
	{
		//	scale all filter coefficients by a[0]:
		std::transform( _maCoefs.begin(), _maCoefs.end(), _maCoefs.begin(),
						std::bind2nd( std::divides<double>(), *ar_begin ) );
		std::transform( _arCoefs.begin(), _arCoefs.end(), _arCoefs.begin(), 
						std::bind2nd( std::divides<double>(), *ar_begin ) );
		_arCoefs[0] = 1.;
	}	

	debugger << _maCoefs[0] << " " << _maCoefs[1] << " " << _maCoefs[2] << " " << _maCoefs[3] << " " << std::endl;
	debugger << _arCoefs[0] << " " << _arCoefs[2] << " " << _arCoefs[2] << " " << _arCoefs[3] << " " << std::endl;
}

#endif

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

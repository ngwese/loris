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
 * Dilator.C
 *
 * Implementation of class Dilator.
 *
 * Kelly Fitz, 26 Oct 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if HAVE_CONFIG_H
	#include <config.h>
#endif

#include <Dilator.h>
#include <Breakpoint.h>
#include <Exception.h>
#include <Notifier.h>
#include <Partial.h>
#include <PartialList.h>

#include <algorithm>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	constructor
// ---------------------------------------------------------------------------
//	Default constructor.
//
Dilator::Dilator( void )
{
}

// ---------------------------------------------------------------------------
//	destructor
// ---------------------------------------------------------------------------
//
Dilator::~Dilator(void)
{
}

// ---------------------------------------------------------------------------
//	insert
// ---------------------------------------------------------------------------
//	Insert a pair of initial and target time points. No need to worry 
//	about the sorting order, the time points will be sorted before 
//	they are used.
//
void
Dilator::insert( double i, double t )
{
	_initial.push_back(i);
	_target.push_back(t);
}

// ---------------------------------------------------------------------------
//	warpTime
// ---------------------------------------------------------------------------
//	Return the dilated time value corresponding to the specified initial time.
//
double
Dilator::warpTime( double currentTime )
{
    int idx = std::distance( _initial.begin(), 
                             std::lower_bound( _initial.begin(), _initial.end(), currentTime ) );
    Assert( idx == _initial.size() || currentTime <= _initial[idx] );
    
    //	compute a new time for the Breakpoint at pIter:
    double newtime = 0;
    if ( idx == 0 ) 
    {
        //	all time points in _initial are later than 
        //	the currentTime; stretch if no zero time 
        //	point has been specified, otherwise, shift:
        if ( _initial[idx] != 0. )
            newtime = currentTime * _target[idx] / _initial[idx];
        else
            newtime = _target[idx] + (currentTime - _initial[idx]);
    }
    else if ( idx == _initial.size() ) 
    {
        //	all time points in _initial are earlier than 
        //	the currentTime; shift:
        //
        //	note: size is already known to be > 0, so
        //	idx-1 is safe
        newtime = _target[idx-1] + (currentTime - _initial[idx-1]);
    }
    else 
    {
        //	currentTime is between the time points at idx and
        //	idx-1 in _initial; shift and stretch: 
        //
        //	note: size is already known to be > 0, so
        //	idx-1 is safe
        Assert( _initial[idx-1] < _initial[idx] );	//	currentTime can't wind up 
                                                    //	between two equal times
        
        double stretch = (_target[idx]	- _target[idx-1]) / (_initial[idx] - _initial[idx-1]);			
        newtime = _target[idx-1] + ((currentTime - _initial[idx-1]) * stretch);
    }
	
	return newtime;
}

// ---------------------------------------------------------------------------
//	dilate
// ---------------------------------------------------------------------------
//	Replace the Partial envelope with a new envelope having the
//	same Breakpoints at times computed to align temporal features
//	in _initial with their counterparts in _target.
//
//	Depending on the specification of initial and target time 
//	points, the dilated Partial may have Breakpoints at times
//	less than 0.
//
//	It is possible to have duplicate time points in either vector.
//	Duplicates in _initial result in very localized stretching.
//	Duplicates in _target result in very localized compression.
//
//	If all time points in _initial are greater than 0, then an implicit
//	time point at 0 is assumed in both _initial and _target, allowing
//	the onset of a sound to be stretched without explcitly specifying a 
//	zero point in each vector. This seems most intuitive, and only looks
//	like an inconsistency if clients are using negative time points in 
//	their Dilator, or Partials having Breakpoints before time 0, both 
//	of which are probably unusual circumstances. 
//	
void
Dilator::dilate( Partial & p )
{
	debugger << "dilating Partial having " << p.numBreakpoints() 
			 << " Breakpoints" << endl;

	//	sanity check:
	Assert( _initial.size() == _target.size() );
	
	//	don't dilate if there's no time points:
	if ( _initial.size() == 0 )
		return;

	//	sort the time points before dilating:
	std::sort( _initial.begin(), _initial.end() );
	std::sort( _target.begin(), _target.end() );

	//	create the new Partial:
	Partial newp;
	newp.setLabel( p.label() );
	
	//	timepoint index:
	int idx = 0;
	for ( Partial::const_iterator iter = p.begin(); iter != p.end(); ++iter )
	{
		//	find the first initial time point later 
		//	than the currentTime:
		double currentTime = iter.time();
        idx = std::distance( _initial.begin(), 
                             std::lower_bound( _initial.begin(), _initial.end(), currentTime ) );
        Assert( idx == _initial.size() || currentTime <= _initial[idx] );
        
		//	compute a new time for the Breakpoint at pIter:
		double newtime = 0;
		if ( idx == 0 ) 
		{
			//	all time points in _initial are later than 
			//	the currentTime; stretch if no zero time 
			//	point has been specified, otherwise, shift:
			if ( _initial[idx] != 0. )
				newtime = currentTime * _target[idx] / _initial[idx];
			else
				newtime = _target[idx] + (currentTime - _initial[idx]);
		}
		else if ( idx == _initial.size() ) 
		{
			//	all time points in _initial are earlier than 
			//	the currentTime; shift:
			//
			//	note: size is already known to be > 0, so
			//	idx-1 is safe
			newtime = _target[idx-1] + (currentTime - _initial[idx-1]);
		}
		else 
		{
			//	currentTime is between the time points at idx and
			//	idx-1 in _initial; shift and stretch: 
			//
			//	note: size is already known to be > 0, so
			//	idx-1 is safe
			Assert( _initial[idx-1] < _initial[idx] );	//	currentTime can't wind up 
														//	between two equal times
			
			double stretch = (_target[idx]	- _target[idx-1]) / (_initial[idx] - _initial[idx-1]);			
			newtime = _target[idx-1] + ((currentTime - _initial[idx-1]) * stretch);
		}
		
		//	add a Breakpoint at the computed time:
		newp.insert( newtime, iter.breakpoint() );
	}
	
	//	new Breakpoints need to be added to the Partial at times corresponding
	//	to all target time points that are after the first Breakpoint and
	//	before the last, otherwise, Partials may be briefly out of tune with
	//	each other, since our Breakpoints are non-uniformly distributed in time:
	for ( idx = 0; idx < _initial.size(); ++ idx )
	{
		if ( _initial[idx] < p.startTime() )
			continue;
		else if ( _initial[idx] > p.endTime() )
			break;
		else
		{
			newp.insert( _target[idx], 
						 Breakpoint( p.frequencyAt(_initial[idx]), p.amplitudeAt(_initial[idx]),
									 p.bandwidthAt(_initial[idx]), p.phaseAt(_initial[idx]) ) );
		}
	}
	
	//	store the new Partial:
	p = newp;
}

}	//	end of namespace Loris

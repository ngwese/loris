// ===========================================================================
//	Dilator.C
//	
//	Dilator is a class of objects for temporally dilating and compressing
//	Partials by specifying source and target times of temporal features.
//
//	-kel 26 Oct 99
//
// ===========================================================================
#include "Dilator.h"
#include "Partial.h"
#include "Breakpoint.h"
#include "Exception.h"
#include "notifier.h"
#include <algorithm>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif


// ---------------------------------------------------------------------------
//	constructor
// ---------------------------------------------------------------------------
//	Construct from n initial and n target time points.
//
Dilator::Dilator( const double * ibegin, const double * tbegin, int n ) :
	_initial( ibegin, ibegin+n ),
	_target( tbegin, tbegin+n )
{
	std::sort( _initial.begin(), _initial.end() );
	std::sort( _target.begin(), _target.end() );
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
//	Dilated Partials are collected (Dilator isa PartialCollector).
//	
void
Dilator::dilate( Partial & p )
{
	debugger << "dilating Partial having " << p.countBreakpoints() 
			 << " Breakpoints" << endl;

	//	sanity check:
	Assert( _initial.size() == _target.size() );
	
	//	don't dilate if there's no time points:
	if ( _initial.size() == 0 )
		return;
		
	//	create the new Partial:
	Partial newp;
	newp.setLabel( p.label() );
	
	//	timepoint index:
	int idx = 0;
	for ( PartialConstIterator iter = p.begin(); iter != p.end(); ++iter )
	{
		//	find the first initial time point later 
		//	than the currentTime:
		double currentTime = iter.time();
		while ( idx < _initial.size() && currentTime > _initial[idx] )
		{
			++idx;
		}
	
		//	compute a new time for the Breakpoint at pIter:
		double newtime = 0;
		if ( idx == 0 ) {
			//	all time points in _initial are later than 
			//	the currentTime; stretch if no zero time 
			//	point has been specified, otherwise, shift:
			if ( _initial[idx] != 0. )
				newtime = currentTime * _target[idx] / _initial[idx];
			else
				newtime = _target[idx] + (currentTime - _initial[idx]);
		}
		else if ( idx == _initial.size() ) {
			//	all time points in _initial are earlier than 
			//	the currentTime; shift:
			//
			//	note: size is already known to be > 0, so
			//	idx-1 is safe
			newtime = _target[idx-1] + (currentTime - _initial[idx-1]);
		}
		else {
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
		newp.insert( newtime, Breakpoint( iter->frequency(), iter->amplitude(), 
										  iter->bandwidth(), iter->phase() ) );
	}
	
	//	store the new Partial:
	partials().push_back( newp );
}

// ---------------------------------------------------------------------------
//	dilate (range)
// ---------------------------------------------------------------------------
//	dilate() each Partial in the specified half-open range.
//
void 
Dilator::dilate( PartialList::iterator begin, PartialList::iterator end )
{
	while ( begin != end )
		dilate( *(begin++) );
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

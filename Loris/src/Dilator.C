// ===========================================================================
//	Dilator.C
//	
//	Dilator is a class of objects for temporally dilating and compressing
//	Partials by specifying source and target times of temporal features.
//
//	-kel 26 Oct 99
//
// ===========================================================================

#include "LorisLib.h"
#include "Dilator.h"
#include "Partial.h"
#include "PartialIterator.h"
#include "Breakpoint.h"
#include "Exception.h"
#include "Notifier.h"

#include <algorithm>

using namespace std;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	constructor
// ---------------------------------------------------------------------------
Dilator::Dilator( const vector< double > & init, 
				  const vector< double > & tgt )
{
	//	time point vectors have to be the same size:
	if ( init.size() != tgt.size() ) {
		Throw( InvalidObject, 
				"Dilator must have the same number of initial and target time points." );
	}

	setTimePoints( init.begin(), tgt.begin(), init.size() );
}

// ---------------------------------------------------------------------------
//	dilate
// ---------------------------------------------------------------------------
//	Dilate a Partial in-place, and return a reference to it.
//
//	The Partial envelope is replaced with a new envelope having the
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
//	
Partial &
Dilator::dilate( Partial & p ) const
{
	//	sanity check:
	Assert( _initial.size() == _target.size() );
		
	//	Nothing to do if there are no time points:
	if ( _initial.size() == 0 )
		return p;
	
	//	create the new Partial:
	Partial newp;
	newp.setLabel( p.label() );
	
	//	use iterators here instead of index, so that the algorithm
	//	is independent of the kind of container I choose for the time
	//	points:
	typedef multiset< double >::const_iterator IterType;
	IterType iterInit( _initial.begin() ), iterTgt( _target.begin() );
	
	for ( PartialIterator pIter(p); ! pIter.atEnd(); pIter.advance() ) {
		//	find the first initial time point later than pIter:
		while ( iterInit != _initial.end() && pIter.time() > *iterInit ) {
			++iterInit;
			++iterTgt;
		}
	
		//	compute a new time for the Breakpoint at pIter:
		double newtime = 0;
		if ( iterInit == _initial.begin() ) {
			//	all time points in _initial are later than 
			//	the time of pIter; stretch if no zero time 
			//	point has been specified, otherwise, shift:
			if ( *iterInit != 0. )
				newtime = pIter.time() * (*iterTgt) / (*iterInit);
			else
				newtime = (*iterTgt) + ( pIter.time() - (*iterInit) );
		}
		else if ( iterInit == _initial.end() ) {
			//	all time points in _initial are earlier than 
			//	the time of pIter; shift:
			IterType prevTgt = iterTgt; --prevTgt;
			IterType prevInit = iterInit; --prevInit;
			newtime = *prevTgt + ( pIter.time() - (*prevInit) );
		}
		else {
			IterType prevTgt = iterTgt; --prevTgt;
			IterType prevInit = iterInit; --prevInit;
			
			//	pIter is between the time points at index and
			//	index-1 in _initial; shift and stretch: 
			Assert( *iterInit > *prevInit );	//	pIter can't wind up 
												//	between two equal times
			newtime = *prevTgt + 
					  ( (pIter.time() - (*prevInit)) * 
						( (*iterTgt) - (*prevTgt) ) /
						( (*iterInit) - (*prevInit) ) );
		}
		
		//	add a Breakpoint at the computed time:
		newp.insert( newtime, Breakpoint( pIter.frequency(), pIter.amplitude(), 
										  pIter.bandwidth(), pIter.phase() ) );
	}
	
	//	assign the new Partial:
	p = newp;
	return p;
}

End_Namespace( Loris )

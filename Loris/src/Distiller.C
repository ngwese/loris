// ===========================================================================
//	Distiller.C
//	
//	A group of Partials that logically represent a single component
//	can be distilled into a single Partial using a Distiller. 
//
//	-kel 20 Oct 99
//
// ===========================================================================
#include "Distiller.h"
#include "Partial.h"
#include "Breakpoint.h"
#include "Exception.h"
#include "PartialUtils.h"
#include "notifier.h"
#include <cmath>
#include <list>
#include <vector>
#include <set>
#include <algorithm>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class Distiller_imp
//
//	Implementation class for the fully-insulating interface Distiller.
//
struct Distiller_imp
{
//	-- instance variables --
	PartialList _partials;	//	collect Partials here
			
//	-- "public" interface --
//	construction:	
	Distiller_imp( void ) {}
	~Distiller_imp( void ) {}
	
//	distillation:
	void distill_all( PartialList::const_iterator start,
		 			  PartialList::const_iterator end );
	void distill_one( PartialList::const_iterator start,
		 			  PartialList::const_iterator end,
		  			  int assignLabel );

//	-- helpers --
	void distill_aux( const Partial & src, 
				 	  Partial & dest, 
				 	  PartialList::const_iterator start,
				 	  PartialList::const_iterator end );

	void fixGaps( Partial & dest, 
				  PartialList::const_iterator start,
				  PartialList::const_iterator end );
				   
};	//	end of class Distiller_imp

// ---------------------------------------------------------------------------
//	Distiller_imp distill_all
// ---------------------------------------------------------------------------
//	Distill all the Partials in the half open range start,end by label.
//	Unlabeled (label 0) Partials are ignored.
//
void 
Distiller_imp::distill_all( PartialList::const_iterator start,
	 						PartialList::const_iterator end )
{
	std::set<int> ignorelabels;
	ignorelabels.insert(0);

	//	set pos to first Partial with non-zero label:
	PartialList::const_iterator pos = 
		std::find_if( start, end, 
					  std::not1( std::bind2nd( PartialUtils::label_equals(), 0 ) ) );

	//	it and pos are stupid variable names!
	while ( pos != end )
	{
		int label = pos->label();
		debugger << "distilling Partials labeled " << label << endl;
		ignorelabels.insert(label);
		
		PartialList::const_iterator it = pos;
		PartialList dothese;
		/*
		std::copy_if( pos, end, 
					  	   std::bind2nd( PartialUtils::label_equals(), label ),
						   dothese.begin() );
		*/
		dothese.push_back( *pos );
		while( ++it != end )
		{
			if ( it->label() == label )
			{
				dothese.push_back( *it );
			}
		}
		debugger << "distilled " << dothese.size() << " Partials labeled " << label << endl;
		distill_one( dothese.begin(), dothese.end(), label );

		//	advance pos until a new non-zero label is found:
		while( ++pos != end )
		{
			if ( ignorelabels.find(pos->label()) == ignorelabels.end() )
				break;
		} 
	}
}

// ---------------------------------------------------------------------------
//	Distiller_imp distill_one
// ---------------------------------------------------------------------------
//	Distill the Partials in a range into a single Partial, 
//	add the result to the collection of distilled Partials.
//
//	(start, end) _must_ be a valid range in a list< Partial >...or else!
//
void 
Distiller_imp::distill_one( PartialList::const_iterator start,
	 						PartialList::const_iterator end, 
	 						int assignLabel )
{
	if ( assignLabel <= 0 )
		Throw( InvalidArgument, "distillation label must be positive" );

	//	create the resulting distilled partial:
	Partial newp;
	newp.setLabel( assignLabel );
	
	//	iterate over range:
	for ( PartialList::const_iterator it = start; it != end; ++it )
	{
		distill_aux( *it, newp, start, end );
	}
	
	//	fill in gaps:
	fixGaps( newp, start, end );

	//	add the newly-distilled partial to the collection:
	_partials.push_back( newp );
}

// ---------------------------------------------------------------------------
//	Distiller_imp distill_aux
// ---------------------------------------------------------------------------
//	Mother of all helper functions, distillation core: 
//	Distill a single source Partial into the destination Partial, 
//	inserting Breakpoints only at times when the source Partial
//	is louder than every other Partial.
//
//	For every Breakpoint in every Partial in the list, determine whether 
//	that Breakpoint's Partial is the loudest (greatest total amplitude -or- 
//	greatest sine energy, which one?) Partial at the time of that Breakpoint.
//	If not, forget it and go on to the next Breakpoint. If so, copy that
//	Breakpoint, add all other Partials' energies as Bandwidth, and insert 
//	the copy in the new distilled Partial. If there are gaps, time when no
//	Partial in the list is active, ramp the Partial out and back in again at
//	the end of the gap.
//
//	This algorithm is neither particularly time- nor space-efficient.
//
//	(start, end) _must_ be a valid range in a list< Partial >...or else!
//	
void
Distiller_imp::distill_aux( const Partial & src, 
					   Partial & dest, 
					   PartialList::const_iterator start,
					   PartialList::const_iterator end  )
{
	//	iterate over the source Partial:
	for ( PartialConstIterator pIter = src.begin(); pIter != src.end(); ++pIter )
	{
		//	iterate over all Partials in the range and compute the
		//	bandwidth energy contribution at the time of bp
		//	due to all the other Partials:
		double xse = 0.;
		PartialList::const_iterator it;
		for ( it = start; it != end; ++it ) 
		{
			//	skip the source Partial:
			//	(identity test: compare addresses)
			if ( &(*it) == &src )
				continue;

			//	accumulate energy to be added as bandwidth:
			//	(this should use the same algorithm as the energy
			//	redistribution in the analysis)
			//	If this Partial is louder than is represented
			//	by bp at the time of bp, then break out of
			//	this loop, this Breakpoint will not be part of 
			//	the distilled Partial.
			double a = it->amplitudeAt( pIter.time() );
			if ( a > pIter->amplitude() ) {
				break;	
			}
			else {
				xse += a*a;
			}
		}	//	end iteration over Partial range
		
		//	if all Partials were examined, then
		//	src is the loudest Partial at the time of bp,
		//	and the bandwidth energy contributed by the 
		//	other Partials is xse.
		//	Create a new Breakpoint and add it to dest:
		if ( it == end ) 
		{
			//	create and insert the new Breakpoint:
			Breakpoint newBp( *pIter );
			newBp.addNoise( xse );
			dest.insert( pIter.time(), newBp );
			
		}	//	end if all other Partials are quieter at time of pIter
		
	}	//	end iteration over source Partial
}

// ---------------------------------------------------------------------------
//	Distiller_imp fixGaps
// ---------------------------------------------------------------------------
//	Look for gaps between Partials in the half-open interval [start,end)
//	and make sure that the new distilled Partial (dest) ramps to zero
//	amplitude during those gaps.
//
void 
Distiller_imp::fixGaps( Partial & dest, 
				  	PartialList::const_iterator start,
				  	PartialList::const_iterator end )
{
	//	make a list of segments:
	std::list< std::pair<double, double> > segments;
	for ( PartialList::const_iterator p = start; p != end; ++p )
	{	
		segments.push_back( std::make_pair( p->startTime(), p->endTime() ) );
	}
	//debugger << "found " << segments.size() << " segments." << endl;
	
	//	sort the list and collapse overlapping segments
	//	to leave a list of non-overlapping segments:
	segments.sort(); 	//	uses op < ( pair<>, pair<> ), does what you think
	std::list< std::pair<double, double> >::iterator curseg;
	for ( curseg = segments.begin(); curseg != segments.end(); ++curseg )
	{
		//	curseg absorbs all succeeding segments that
		//	begin before it ends:
		std::list< std::pair<double, double> >::iterator nextseg = curseg;
		for ( ++nextseg; nextseg != segments.end(); ++nextseg ) 
		{
			if ( nextseg->first > curseg->second )
				break;
			//	else absorb:
			curseg->second = std::max( curseg->second, nextseg->second );
		}
		
		//	nextseg now begins after curseg ends,
		//	remove all segments after curseg and before
		//	nextseg (list<> erasure is guaranteed not to 
		//	invalidate curseg or any iterators refering to
		//	elements that aren't deleted):
		std::list< std::pair<double, double> >::iterator del = curseg;
		++del;
		segments.erase( del, nextseg );
	}
	//debugger << "collapsed to " << segments.size() << " segments" << endl;
	
	//	fill in gaps between segments:
	if ( segments.size() > 1 )
	{
		std::list< std::pair<double, double> >::iterator seg = segments.begin();
		std::list< std::pair<double, double> >::iterator nextseg = seg;	
		++nextseg;
		while ( nextseg != segments.end() )
		{	
			double gap = nextseg->first - seg->second;
			
			//	sanity check:
			Assert( gap > 0. );
			
			//	fill in the gap if its big enough:
			if ( gap > 2. * Partial::FadeTime() )
			{
				//
				//	HEY should we use zero BW or dest BW?
				//
				double time = seg->second + Partial::FadeTime();
				double freq = dest.frequencyAt( seg->second );
				double phase = dest.phaseAt(seg->second) +  (freq * Partial::FadeTime()); 
				dest.insert( time, Breakpoint( freq, 0., 0., phase ) );
				
				time = nextseg->first - Partial::FadeTime();
				freq = dest.frequencyAt( nextseg->first );
				phase = dest.phaseAt(nextseg->first) - (freq * Partial::FadeTime());
				dest.insert( time, Breakpoint( freq, 0., 0., phase ) );
			} 
			
			//	advance iterators:
			++seg;
			++nextseg;
		}
	}
	
	//	make the dest Partial have 0 amplitude
	//	Breakpoints at its head and tail (VERY 
	//	important for morphing):
	if ( dest.amplitudeAt( dest.startTime() ) > 0. )
	{
		double time = dest.startTime() - Partial::FadeTime();
		double freq = dest.frequencyAt(time);
		double phase = dest.phaseAt(time);
		dest.insert( time, Breakpoint( freq, 0., 0., phase ) );		
	}
	
	if ( dest.amplitudeAt( dest.endTime() ) > 0. )
	{
		double time = dest.endTime() + Partial::FadeTime();
		double freq = dest.frequencyAt(time);
		double phase = dest.phaseAt(time);
		dest.insert( time, Breakpoint( freq, 0., 0., phase ) );		
	}
}

// ---------------------------------------------------------------------------
//	Distiller constructor
// ---------------------------------------------------------------------------
//
Distiller::Distiller( void ):
	_imp( new Distiller_imp() )
{
}

// ---------------------------------------------------------------------------
//	Distiller destructor
// ---------------------------------------------------------------------------
//
Distiller::~Distiller( void )
{
	delete _imp;
}

// ---------------------------------------------------------------------------
//	distill
// ---------------------------------------------------------------------------
//	Distill the Partials in a range into a single Partial, 
//	add the result to the collection of distilled Partials.
//
//	(start, end) _must_ be a valid range in a list< Partial >...or else!
//
void 
Distiller::distill( PartialList::const_iterator start,
	 				PartialList::const_iterator end, 
	 				int assignLabel )
{
	_imp->distill_one( start, end, assignLabel );
}

// ---------------------------------------------------------------------------
//	distillAll
// ---------------------------------------------------------------------------
//	Distill the Partials in a range into a single Partial, 
//	add the result to the collection of distilled Partials.
//
//	(start, end) _must_ be a valid range in a list< Partial >...or else!
//
void 
Distiller::distillAll( PartialList::const_iterator start,
	 				   PartialList::const_iterator end )
{
	_imp->distill_all( start, end );
}

// ---------------------------------------------------------------------------
//	partials
// ---------------------------------------------------------------------------
//
PartialList & 
Distiller::partials( void )
{ 
	return _imp->_partials; 
}

// ---------------------------------------------------------------------------
//	partials
// ---------------------------------------------------------------------------
//
const PartialList & 
Distiller::partials( void ) const 
{ 
	return _imp->_partials; 
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

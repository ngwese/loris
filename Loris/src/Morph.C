// ===========================================================================
//	Morph.C
//	
//	The Morph object performs sound morphing (cite Lip's papers, and the book)
//	by interpolating Partial parmeter envelopes of corresponding Partials in
//	a pair of source sounds. The correspondences are established by labeling.
//	The Morph object collects morphed Partials in a PartialList, that can
//	be accessed by clients.
//
//	-kel 15 Oct 99
//
// ===========================================================================
#include "Morph.h"
#include "Synthesizer.h"
#include "Exception.h"
#include "Partial.h"
#include "PartialIterator.h"
#include "Breakpoint.h"
#include "Distiller.h"
#include "Map.h"
#include "notifier.h"
#include <set>

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
#else
	#include <math.h>
#endif

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

//	declarations of local helpers, defined at bottom:
static void collectLabels( PartialList::const_iterator begin, 
						   PartialList::const_iterator end, 
						   std::set<int> & labels );
static void collectByLabel( PartialList::const_iterator start, 
							PartialList::const_iterator end, 
							PartialList & collector, 
							int label );
							
// ---------------------------------------------------------------------------
//	Morph default constructor
// ---------------------------------------------------------------------------
//
Morph::Morph( void ) :
	_freqFunction( new BreakpointMap() ),
	_ampFunction( new BreakpointMap() ),
	_bwFunction( new BreakpointMap() )
{
}

// ---------------------------------------------------------------------------
//	Morph constructor (single morph function)
// ---------------------------------------------------------------------------
//
Morph::Morph( const Map & f ) :
	_freqFunction( f.clone() ),
	_ampFunction( f.clone() ),
	_bwFunction( f.clone() )
{
}

// ---------------------------------------------------------------------------
//	Morph constructor (distinct morph functions)
// ---------------------------------------------------------------------------
//
Morph::Morph( const Map & ff, const Map & af, const Map & bwf ) :
	_freqFunction( ff.clone() ),
	_ampFunction( af.clone() ),
	_bwFunction( bwf.clone() )
{
}

// ---------------------------------------------------------------------------
//	morph
// ---------------------------------------------------------------------------
//	Morph two sounds (collections of Partials labeled to indicate
//	correspondences) into a single labeled collection of Partials.
//
void 
Morph::morph( PartialList::const_iterator begin0, 
			  PartialList::const_iterator end0,
			  PartialList::const_iterator begin1, 
			  PartialList::const_iterator end1 )
{
	//	find label range:
	std::set< int > labels;
	collectLabels( begin0, end0, labels );
	collectLabels( begin1, end1, labels );
	const int CROSSFADE_LABEL = 0;
	
	//	loop over lots of labels:
	for ( std::set< int >::iterator it = labels.begin(); it != labels.end(); ++it ) {
		int label = *it;
		
		//	collect Partials in plist1:
		PartialList sublist1;
		collectByLabel( begin0, end0, sublist1, label );

		//	collect Partials in plist2:
		PartialList sublist2;
		collectByLabel( begin1, end1, sublist2, label );
		
		if ( label == CROSSFADE_LABEL ) {
			debugger << "crossfading Partials labeled " << label << endl;
			crossfadeLists( sublist1, sublist2 );
		}
		else {
			debugger << "morphing " << sublist1.size() << " and "
					 << sublist2.size() << " partials with label " << label << endl;
					 
			morphLists( sublist1, sublist2, label );
		}
	}
}

// ---------------------------------------------------------------------------
//	setFrequencyFunction
// ---------------------------------------------------------------------------
//
void
Morph::setFrequencyFunction( const Map & f )
{
	_freqFunction.reset( f.clone() );
}

// ---------------------------------------------------------------------------
//	setAmplitudeFunction
// ---------------------------------------------------------------------------
//
void
Morph::setAmplitudeFunction( const Map & f )
{
	_ampFunction.reset( f.clone() );
}

// ---------------------------------------------------------------------------
//	setBandwidthFunction
// ---------------------------------------------------------------------------
//
void
Morph::setBandwidthFunction( const Map & f )
{
	_bwFunction.reset( f.clone() );
}

// ---------------------------------------------------------------------------
//	morphLists
// ---------------------------------------------------------------------------
//	Distill and morph. Either distillation could yield an empty Partial, 
//	but morphPartial can deal with it.
//
void
Morph::morphLists( const PartialList & fromlist, const PartialList & tolist, 
				   int assignLabel /* default = 0 */ )
{
	//	don't bother if there's no Partials:
	if ( fromlist.size() == 0 && tolist.size() == 0 ) {
		//	this shouldn't happen:
		debugger << "Morph::morphLists() got two empty lists!" << endl;
		return;
	}
	
	Distiller still;
	morphPartial( still.distill( fromlist.begin(), fromlist.end() ), 
				  still.distill( tolist.begin(), tolist.end() ),
				  assignLabel );
}

// ---------------------------------------------------------------------------
//	morphPartial
// ---------------------------------------------------------------------------
//	Basic morphing operation: either Partial may be a dummy with no 
//	Breakpoints. Partials with no duration don't contribute to the
//	morph, except to cause their opposite to fade out. The morphed
//	Partial has Breakpoints at times corresponding to every Breakpoint 
//	in both source Partials.
//
void 
Morph::morphPartial( const Partial & p0, const Partial & p1, int assignLabel /* default = 0 */ )
{
	//	make a new Partial:
	Partial newp;
	newp.setLabel( assignLabel );
	
	//	loop over Breakpoints in first partial:
	for ( BasicPartialIterator iter(p0); ! iter.atEnd(); iter.advance() ) {
		double alphaF = frequencyFunction().valueAt( iter.time() );
		double alphaA = amplitudeFunction().valueAt( iter.time() );
		double alphaBW = bandwidthFunction().valueAt( iter.time() );
		
		double amp2 = ( p1.duration() > 0. ) ? 
			p1.amplitudeAt( iter.time() ) : 0.;
		double freq2 = ( p1.duration() > 0. ) ? 
			p1.frequencyAt( iter.time() ) : iter.frequency();
		double bw2 = ( p1.duration() > 0. ) ? 
			p1.bandwidthAt( iter.time() ) : iter.bandwidth();
		double theta2 = ( p1.duration() > 0. ) ? 
			p1.phaseAt( iter.time() ) : iter.phase();
			
		Breakpoint newbp( (alphaF * freq2) + ((1.-alphaF) * iter.frequency()),
						   (alphaA * amp2) + ((1.-alphaA) * iter.amplitude()),
						   (alphaBW * bw2) + ((1.-alphaBW) * iter.bandwidth()),
						   (alphaF * theta2) + ((1.-alphaF) * iter.phase()) );
		
		/*
		debugger << "time " << iter.time() << ":" << newbp.frequency() << "," <<
					newbp.amplitude() << "," << newbp.bandwidth() << "," << 
					newbp.phase() << endl;
		*/
		newp.insert( iter.time(), newbp );
	}
	
	//	now do it for the other Partial:
	for ( BasicPartialIterator iter(p1); ! iter.atEnd(); iter.advance() ) {
		double alphaF = 1. - frequencyFunction().valueAt( iter.time() );
		double alphaA = 1. - amplitudeFunction().valueAt( iter.time() );
		double alphaBW = 1. - bandwidthFunction().valueAt( iter.time() );
		
		double amp1 = ( p0.duration() > 0. ) ? 
			p0.amplitudeAt( iter.time() ) : 0.;
		double freq1 = ( p0.duration() > 0. ) ? 
			p0.frequencyAt( iter.time() ) : iter.frequency();
		double bw1 = ( p0.duration() > 0. ) ? 
			p0.bandwidthAt( iter.time() ) : iter.bandwidth();
		double theta1 = ( p0.duration() > 0. ) ? 
			p0.phaseAt( iter.time() ) : iter.phase();
			
		Breakpoint newbp( (alphaF * freq1) + ((1.-alphaF) * iter.frequency()),
						   (alphaA * amp1) + ((1.-alphaA) * iter.amplitude()),
						   (alphaBW * bw1) + ((1.-alphaBW) * iter.bandwidth()),
						   (alphaF * theta1) + ((1.-alphaF) * iter.phase()) );
		/*
		debugger << "time " << iter.time() << ":" << newbp.frequency() << "," <<
					newbp.amplitude() << "," << newbp.bandwidth() << "," << 
					newbp.phase() << endl;
		*/
		newp.insert( iter.time(), newbp );
	}
	
		
	//	add the new partial to the collection,
	//	if it is valid:
	if ( newp.begin() != newp.end() ) {
		partials().push_back( newp );
	}

}

// ---------------------------------------------------------------------------
//	crossfadeLists
// ---------------------------------------------------------------------------
//	Crossfade Partials with no correspondences.
//
//	The Partials in the  specified range are considered to have 
//	no correspondences, so they are just faded out, and not 
//	actually morphed. This is the same as morphing each with an 
//	empty Partial. 
//
//	The Partials in fromlist are treated as components of the 
//	sound corresponding to a morph function of 0, those in tolist
//	are treated as components of the sound corresponding to a 
//	morph function of 1.
//
void
Morph::crossfadeLists( const PartialList & fromlist, 
					   const PartialList & tolist )
{
	//	crossfade Partials corresponding to a morph weight of 0:
	for ( PartialList::const_iterator it = fromlist.begin();
		  it != fromlist.end();
		  ++it )
	{
		morphPartial( *it, Partial() );	
	}

	//	crossfade Partials corresponding to a morph weight of 1:
	for ( PartialList::const_iterator it = tolist.begin();
		  it != tolist.end();
		  ++it )
	{
		morphPartial( Partial(), *it );
	}
}

// ---------------------------------------------------------------------------
//	collectLabels
// ---------------------------------------------------------------------------
//	Collect Partial labels in a set.	
//
//	local helper
//
static void collectLabels( PartialList::const_iterator begin, 
						   PartialList::const_iterator end, 
						   std::set<int> & labels )
{
	while ( begin != end ) {
		labels.insert( begin->label() );
		++begin;
	}
}

// ---------------------------------------------------------------------------
//	collectByLabel
// ---------------------------------------------------------------------------
//	Copy all Partials in the range [start, end) having the specified label 
//	into collector. It would be more efficient to splice the element from
//	the original list, but those lists are immutable, so just copy it.
//
//	local helper
//
static void collectByLabel( PartialList::const_iterator start, 
							PartialList::const_iterator end, 
							PartialList & collector, 
							int label )
{
	while ( start != end ) {
		if ( start->label() == label ) {
			collector.push_back( *start );
		}
		++start;
	}
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

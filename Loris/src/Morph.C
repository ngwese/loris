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
#include "Breakpoint.h"
#include "Map.h"
#include "PartialUtils.h"
#include "notifier.h"
#include <set>
#include <cmath>
#include <algorithm>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

//	declarations of local helpers, defined at bottom:
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
//	Morph destructor
// ---------------------------------------------------------------------------
//
Morph::~Morph( void )
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
	//	collect the labels in the two Partial ranges, 
	//	object if the Partials have not been distilled,
	//	that is, if they contain multiple Partials having 
	//	the same non-zero label:
	std::set< int > labels, moreLabels;
	for ( PartialList::const_iterator it = begin0; it != end0; ++it ) 
	{
		//	don't add the crossfade label to the set:
		if ( it->label() != 0 )
		{
			//	set::insert() returns a pair, the second element
			//	of which is false if the insertion failed because
			//	the set already contained the insertee:
			if ( ! labels.insert(it->label()).second )
				Throw( MorphException, "Partials must be distilled before morphing." );
		}
	}
	for ( PartialList::const_iterator it = begin1; it != end1; ++it ) 
	{
		//	don't add the non-label, 0, to the set:
		if ( it->label() != 0 )
		{
			//	as above:
			if ( ! moreLabels.insert(it->label()).second )
				Throw( MorphException, "Partials must be distilled before morphing." );
		}
	}
	
	//	combine the label sets
	labels.insert( moreLabels.begin(), moreLabels.end() );
		
	//	loop over lots of labels and morph Partials
	//	having corresponding labels:
	std::set< int >::iterator labelIter;
	for ( labelIter = labels.begin(); labelIter != labels.end(); ++labelIter ) 
	{
		Assert( *labelIter != 0 );
		
		//	find source Partial 0:
		PartialList::const_iterator p0 = 
			std::find_if( begin0, end0, 
						  std::bind2nd(PartialUtils::label_equals(), *labelIter) );
				
		//	find source Partial 1:
		PartialList::const_iterator p1 = 
			std::find_if( begin1, end1, 
						  std::bind2nd(PartialUtils::label_equals(), *labelIter) );
		
		debugger << "morphing " << ((p0 != end0)?(1):(0)) 
				 << " and " << ((p1 != end1)?(1):(0)) 
				 <<	" partials with label " <<	*labelIter << endl;
				 
		if ( p0 == end0 )
		{
			Assert( p1 != end1 );
			morphPartial( Partial(), *p1, *labelIter );
		}
		else if ( p1 == end1 )
		{
			Assert( p0 != end0 );
			morphPartial( *p0, Partial(), *labelIter );
		}
		else
		{			 
			morphPartial( *p0, *p1, *labelIter );
		}
		
	}	//	end loop over labels
	
	//	crossfade the remaining unlabeled Partials:
	crossfade( begin0, end0, begin1, end1 );
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
	for ( PartialConstIterator iter = p0.begin(); iter != p0.end(); ++iter )
	{
		double alphaF = frequencyFunction().valueAt( iter.time() );
		double alphaA = amplitudeFunction().valueAt( iter.time() );
		double alphaBW = bandwidthFunction().valueAt( iter.time() );
		
		double amp2 = ( p1.duration() > 0. ) ? 
			p1.amplitudeAt( iter.time() ) : 0.;
		double freq2 = ( p1.duration() > 0. ) ? 
			p1.frequencyAt( iter.time() ) : iter->frequency();
		double bw2 = ( p1.duration() > 0. ) ? 
			p1.bandwidthAt( iter.time() ) : iter->bandwidth();
		double theta2 = ( p1.duration() > 0. ) ? 
			p1.phaseAt( iter.time() ) : iter->phase();
			
		Breakpoint newbp( (alphaF * freq2) + ((1.-alphaF) * iter->frequency()),
						   (alphaA * amp2) + ((1.-alphaA) * iter->amplitude()),
						   (alphaBW * bw2) + ((1.-alphaBW) * iter->bandwidth()),
						   (alphaF * theta2) + ((1.-alphaF) * iter->phase()) );
		
		/*
		debugger << "time " << iter.time() << ":" << newbp.frequency() << "," <<
					newbp.amplitude() << "," << newbp.bandwidth() << "," << 
					newbp.phase() << endl;
		*/
		newp.insert( iter.time(), newbp );
	}
	
	//	now do it for the other Partial:
	for ( PartialConstIterator iter = p1.begin(); iter != p1.end(); ++iter )
	{
		double alphaF = 1. - frequencyFunction().valueAt( iter.time() );
		double alphaA = 1. - amplitudeFunction().valueAt( iter.time() );
		double alphaBW = 1. - bandwidthFunction().valueAt( iter.time() );
		
		double amp1 = ( p0.duration() > 0. ) ? 
			p0.amplitudeAt( iter.time() ) : 0.;
		double freq1 = ( p0.duration() > 0. ) ? 
			p0.frequencyAt( iter.time() ) : iter->frequency();
		double bw1 = ( p0.duration() > 0. ) ? 
			p0.bandwidthAt( iter.time() ) : iter->bandwidth();
		double theta1 = ( p0.duration() > 0. ) ? 
			p0.phaseAt( iter.time() ) : iter->phase();
			
		Breakpoint newbp( (alphaF * freq1) + ((1.-alphaF) * iter->frequency()),
						   (alphaA * amp1) + ((1.-alphaA) * iter->amplitude()),
						   (alphaBW * bw1) + ((1.-alphaBW) * iter->bandwidth()),
						   (alphaF * theta1) + ((1.-alphaF) * iter->phase()) );
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
//	crossfade
// ---------------------------------------------------------------------------
//	Crossfade Partials with no correspondences.
//
//	Unlabeled Partials (having label 0) are considered to 
//	have no correspondences, so they are just faded out, and not 
//	actually morphed. This is the same as morphing each with an 
//	empty Partial. 
//
//	The Partials in the first range are treated as components of the 
//	sound corresponding to a morph function of 0, those in the second
//	are treated as components of the sound corresponding to a 
//	morph function of 1.
//
void 
Morph::crossfade( PartialList::const_iterator begin0, 
				  PartialList::const_iterator end0,
				  PartialList::const_iterator begin1, 
				  PartialList::const_iterator end1 )
{
	Partial nullPartial;
	debugger << "crossfading unlabeled (labeled 0) Partials" << endl;

	//	crossfade Partials corresponding to a morph weight of 0:
	PartialList::const_iterator it;
	for ( it = begin0; it != end0; ++it )
	{
		if ( it->label() == 0 )
			morphPartial( *it, nullPartial );	
	}

	//	crossfade Partials corresponding to a morph weight of 1:
	for ( it = begin1; it != end1; ++it )
	{
		if ( it->label() == 0 )
			morphPartial( nullPartial, *it );
	}
}

// ---------------------------------------------------------------------------
//	crossfadeLists
// ---------------------------------------------------------------------------
//	Crossfade Partials with no correspondences.
//
//	The Partials in the specified range are considered to have 
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
//	collectByLabel
// ---------------------------------------------------------------------------
//	Static member for accessing the label for 
//	crossfaded Partials (0):
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

// ---------------------------------------------------------------------------
//	CrossfadeLabel
// ---------------------------------------------------------------------------
//	Static member for accessing the label for 
//	crossfaded Partials (0):
int 
Morph::CrossfadeLabel(void)
{
	const int CROSSFADE_LABEL = 0;
	return CROSSFADE_LABEL;
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

// ===========================================================================
//	Morph.C
//	
//	The Morph object performs sound morphing (cite Lip's papers, and the book)
//	by interpolating Partial parmeter envelopes of corresponding Partials in
//	a pair of source sounds. The correspondences are established by labeling.
//	The Morph object collects morphed Partials in a list<Partial>, that can
//	be accessed by clients.
//
//	-kel 15 Oct 99
//
// ===========================================================================
#include "LorisLib.h"
#include "Morph.h"
#include "Synthesizer.h"
#include "Exception.h"
#include "Partial.h"
#include "PartialIterator.h"
#include "Breakpoint.h"
#include "Distiller.h"
#include "Map.h"
#include "notifier.h"

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
#else
	#include <math.h>
#endif

#include <algorithm>

using namespace std;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	Morph constructor (single morph function)
// ---------------------------------------------------------------------------
//
Morph::Morph( const Map & f ) :
_freqFunction( f.clone() ),
	_ampFunction( f.clone() ),
	_bwFunction( f.clone() ),
	_crossfadelabel( 0 )
{
}

// ---------------------------------------------------------------------------
//	Morph default constructor
// ---------------------------------------------------------------------------
//
Morph::Morph( void ) :
	_freqFunction( new BreakpointMap() ),
	_ampFunction( new BreakpointMap() ),
	_bwFunction( new BreakpointMap() ),
	_crossfadelabel( 0 )
{
}
// ---------------------------------------------------------------------------
//	Morph constructor (distinct morph functions)
// ---------------------------------------------------------------------------
//
Morph::Morph( const Map & ff, const Map & af, const Map & bwf ) :
	_freqFunction( ff.clone() ),
	_ampFunction( af.clone() ),
	_bwFunction( bwf.clone() ),
	_crossfadelabel( 0 )
{
}

// ---------------------------------------------------------------------------
//	Morph copy constructor
// ---------------------------------------------------------------------------
//
Morph::Morph( const Morph & other ) :
	//_partials( other._partials ),
	_freqFunction( other.frequencyFunction().clone() ),
	_ampFunction( other.amplitudeFunction().clone() ),
	_bwFunction( other.bandwidthFunction().clone() ),
	_crossfadelabel( 0 ),
	PartialCollector( other )
{
}

// ---------------------------------------------------------------------------
//	assignment operator
// ---------------------------------------------------------------------------
//	For best behavior, if any part of assignment fails, the object
//	should be unmodified. To that end, the morphing functions are 
//	cloned first, since that could potentially generate a low memory
//	exception, then the partials list is copied, which could also
//	generate an exception, finally all the other assignments are made, 
//	and they cannot generate exceptions.
//
Morph & 
Morph::operator=( const Morph & other )
{
	//	do nothing if assigning to self:
	if ( &other != this ) {	
		//	first do cloning:
		auto_ptr< Map > ff( other.frequencyFunction().clone() );
		auto_ptr< Map > af( other.amplitudeFunction().clone() );
		auto_ptr< Map > bwf( other.bandwidthFunction().clone() );
		
		_partials = other._partials;
		_freqFunction = ff;
		_ampFunction = af;
		_bwFunction = bwf;
	}
	
	return *this;
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
		
		newp.insert( iter.time(), newbp );
	}
	
		
	//	add the new partial to the collection,
	//	if it is valid:
	if ( newp.begin() != newp.end() ) {
		_partials.push_back( newp );
	}

}

// ---------------------------------------------------------------------------
//	collectByLabel
// ---------------------------------------------------------------------------
//	Copy all Partials in the range [start, end) having the specified label 
//	into collector. It would be more efficient to splice the element from
//	the original list, but those lists are immutable, so just copy it.
//
//  function object for selecting by label:
//	(this should be local to collectByLabel, but MIPSPro
//	doesn't allow template args to reference local types)
struct LabelIs
{
	int _x;
	LabelIs( int x ) : _x(x) {}
	boolean operator() (const Partial & p) const {
		return p.label() == _x;
	}
};

void
Morph::collectByLabel( list<Partial>::const_iterator start, 
					   list<Partial>::const_iterator end, 
					   list<Partial> & collector, 
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
//	morphLists
// ---------------------------------------------------------------------------
//	Distill and morph. Either distillation could yield an empty Partial, 
//	but morphPartial can deal with it.
//
void
Morph::morphLists( const list<Partial> & fromlist, const list<Partial> & tolist, 
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
//	crossfadeLists
// ---------------------------------------------------------------------------
//
void
Morph::crossfadeLists( const list<Partial> & fromlist, const list<Partial> & tolist )
{
	crossfadeFromPartials( fromlist.begin(), fromlist.end() );
	crossfadeToPartials( tolist.begin(), tolist.end() );
}

End_Namespace( Loris )

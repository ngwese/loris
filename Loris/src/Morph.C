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
#include "Notifier.h"

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
	_minlabel( 0 ),
	_maxlabel( 200 ),
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
	_minlabel( 0 ),
	_maxlabel( 200 ),
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
	_minlabel( 0 ),
	_maxlabel( 200 ),
	_crossfadelabel( 0 )
{
}

// ---------------------------------------------------------------------------
//	Morph copy constructor
// ---------------------------------------------------------------------------
//
Morph::Morph( const Morph & other ) :
	_partials( other._partials ),
	_freqFunction( other.frequencyFunction().clone() ),
	_ampFunction( other.amplitudeFunction().clone() ),
	_bwFunction( other.bandwidthFunction().clone() ),
	_minlabel( other._minlabel ),
	_maxlabel( other._maxlabel ),
	_crossfadelabel( other._crossfadelabel )
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
		_minlabel = other._minlabel;
		_maxlabel = other._maxlabel;
		_crossfadelabel = other._crossfadelabel;
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
//	setRange
// ---------------------------------------------------------------------------
void 
Morph::setRange( int min, int max )
{
	if ( min > max ) 
		swap( min, max );
	_minlabel = min;
	_maxlabel = max;
}

// ---------------------------------------------------------------------------
//	crossfadePartials
// ---------------------------------------------------------------------------
//	The Partials in the two lists identified by the specified label (default
//	is 0) are considered to have no correspondences, so they are just faded 
//	in and out, and not actually morphed. This is the same as morphing each 
//	with an empty Partial.
//
void 
Morph::crossfadePartials( const list<Partial> & plist1, 
						  const list<Partial> & plist2, 
						  int label )
{
	debugger << "crossfading Partials labeled " << label << endl;

	//	fade Partials in the first list (the morph source, corresponding
	//	to a morph weight of 0):
	for ( list< Partial >::const_iterator it = plist1.begin(); it != plist1.end(); ++it ) {
		if ( (*it).label() == label ) {
			morphPartial( *it, Partial() );
		}
	}
	
	//	fade Partials in the second list (the morph destination, corresponding
	//	to a morph weight of 1):
	for ( list< Partial >::const_iterator it = plist2.begin(); it != plist2.end(); ++it ) {
		if ( (*it).label() == label ) {
			morphPartial( Partial(), *it );
		}
	}
}

// ---------------------------------------------------------------------------
//	morph
// ---------------------------------------------------------------------------
//
void
Morph::morph( const list<Partial> & plist1, const list<Partial> & plist2 )
{
	Distiller distill1, distill2;
	
	//	loop over lots of labels:
	for ( int label = _minlabel; label < _maxlabel; ++label ) {
		if ( label == _crossfadelabel ) {
			//	do the unlabeled Partials:
			crossfadePartials( plist1, plist2, _crossfadelabel );
		}
		else {
			//	collect Partials in plist1:
			list<Partial> sublist1;
			collectByLabel( plist1.begin(), plist1.end(), sublist1, label );

			//	collect Partials in plist2:
			list<Partial> sublist2;
			collectByLabel( plist2.begin(), plist2.end(), sublist2, label );
			
			//	don't bother if there's no Partials:
			if ( sublist1.size() == 0 && sublist2.size() == 0 ) {
				continue;
			}
			
			notifier << "morphing " << sublist1.size() << " and "
				<< sublist2.size() << " partials with label " << label << endl;
			
			//	distill and morph:	
			//	(either distillation could yield an empty Partial, 
			//	but morphPartial can deal with it)
			morphPartial( distill1.distill( sublist1.begin(), sublist1.end(), label ), 
						  distill2.distill( sublist2.begin(), sublist2.end(), label ) );
		}
	}
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
Morph::morphPartial( const Partial & p0, const Partial & p1 )
{
	//	make a new Partial:
	Partial newp;
	
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

int
Morph::collectByLabel( const list<Partial>::const_iterator & start, 
					   const list<Partial>::const_iterator & end, 
					   list<Partial> & collector, 
					   int label ) const
{
	int n = 0;
	for ( list< Partial >::const_iterator it = find_if( start, end, LabelIs( label ) );
			  it != end; 
			  it = find_if( ++it, end, LabelIs( label ) ) ) {
		collector.push_back( *it );
		++n;
	}
	
	//cout << "found " << n << " partials labeled " << label << endl;
	return n;
}



End_Namespace( Loris )

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

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
#else
	#include <math.h>
#endif

#if !defined( Deprecated_iostream_headers)
	#include <iostream>
#else
	#include <iostream.h>
#endif

#if !defined( Deprecated_cstd_headers )
	#include <cstdio>
#else
	#include <stdio.h>
#endif

using namespace std;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	Morph constructor (single morph function)
// ---------------------------------------------------------------------------
//	Default f is an auto_ptr with no reference, indicating that a default 
//	Map should be created and used.
//
//	auto_ptr is used to submit the Map argument to make explicit the
//	source/sink relationship between the caller and the Morph. After
//	the call, the Morph will own the Map, and the client's auto_ptr
//	will have no reference (or ownership).
//
//
Morph::Morph( auto_ptr< Map > f ) :
	_minlabel( 0 ),
	_maxlabel( 200 ),
	_crossfadelabel( 0 )
{
	//	initialize morphing functions:
	if ( ! f.get() ) {
		//	need to do this here, because clone() can't
		//	be called on a Null (auto) pointer:
		f = defaultMap();
	}
		
	setFrequencyFunction( auto_ptr< Map >( f->clone() ) );
	setAmplitudeFunction( auto_ptr< Map >( f->clone() ) );
	setBandwidthFunction( auto_ptr< Map >( f->clone() ) );
}

// ---------------------------------------------------------------------------
//	Morph constructor (distinct morph functions)
// ---------------------------------------------------------------------------
//	auto_ptr is used to submit the Map argument to make explicit the
//	source/sink relationship between the caller and the Morph. After
//	the call, the Morph will own the Map, and the client's auto_ptr
//	will have no reference (or ownership).
//
Morph::Morph( auto_ptr< Map > ff, 
			  auto_ptr< Map > af, 
			  auto_ptr< Map > bwf ) :
	_minlabel( 0 ),
	_maxlabel( 200 ),
	_crossfadelabel( 0 )
{
	//	initialize morphing functions:
	setFrequencyFunction( ff );
	setAmplitudeFunction( af );
	setBandwidthFunction( bwf );
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
//	Default f is an auto_ptr with no reference, indicating that a default 
//	Map should be created and used.
//
//	auto_ptr is used to submit the Map argument to make explicit the
//	source/sink relationship between the caller and the Morph. After
//	the call, the Morph will own the Map, and the client's auto_ptr
//	will have no reference (or ownership).
//
//
void
Morph::setFrequencyFunction(  auto_ptr< Map > f )
{
	if ( ! f.get() ) 
		f = defaultMap();

	_freqFunction = f;
}

// ---------------------------------------------------------------------------
//	setAmplitudeFunction
// ---------------------------------------------------------------------------
//	Default f is an auto_ptr with no reference, indicating that a default 
//	Map should be created and used.
//
//	auto_ptr is used to submit the Map argument to make explicit the
//	source/sink relationship between the caller and the Morph. After
//	the call, the Morph will own the Map, and the client's auto_ptr
//	will have no reference (or ownership).
//
void
Morph::setAmplitudeFunction(  auto_ptr< Map > f )
{
	if ( ! f.get() ) 
		f = defaultMap();

	_ampFunction = f;
}

// ---------------------------------------------------------------------------
//	setBandwidthFunction
// ---------------------------------------------------------------------------
//	Default f is an auto_ptr with no reference, indicating that a default 
//	Map should be created and used.
//
//	auto_ptr is used to submit the Map argument to make explicit the
//	source/sink relationship between the caller and the Morph. After
//	the call, the Morph will own the Map, and the client's auto_ptr
//	will have no reference (or ownership).
//
void
Morph::setBandwidthFunction(  auto_ptr< Map > f )
{
	if ( ! f.get() ) 
		f = defaultMap();

	_bwFunction = f;
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
			
			cout << "morphing " << sublist1.size() << " and "
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
Morph::morphPartial( const Partial & p1, const Partial & p2 )
{
	//	make a new Partial:
	Partial newp;
	
	//	loop over Breakpoints in first partial:
	for ( PartialIterator it(p1); !it.atEnd(); it.advance() ) {
		double alphaF = frequencyFunction().valueAt( it.time() );
		double alphaA = amplitudeFunction().valueAt( it.time() );
		double alphaBW = bandwidthFunction().valueAt( it.time() );
		
		double amp2 = ( p2.duration() > 0. ) ? 
			p2.amplitudeAt( it.time() ) : 0.;
		double freq2 = ( p2.duration() > 0. ) ? 
			p2.frequencyAt( it.time() ) : it.frequency();
		double bw2 = ( p2.duration() > 0. ) ? 
			p2.bandwidthAt( it.time() ) : it.bandwidth();
		double theta2 = ( p2.duration() > 0. ) ? 
			p2.phaseAt( it.time() ) : it.phase();
			
		Breakpoint newbp( (alphaF * freq2) + ((1.-alphaF) * it.frequency()),
						   (alphaA * amp2) + ((1.-alphaA) * it.amplitude()),
						   (alphaBW * bw2) + ((1.-alphaBW) * it.bandwidth()),
						   (alphaF * theta2) + ((1.-alphaF) * it.phase()) );
		
		newp.insert( it.time(), newbp );
	}
	
	//	now do it for the other Partial:
	for ( PartialIterator it(p2); !it.atEnd(); it.advance() ) {
		double alphaF = 1. - frequencyFunction().valueAt( it.time() );
		double alphaA = 1. - amplitudeFunction().valueAt( it.time() );
		double alphaBW = 1. - bandwidthFunction().valueAt( it.time() );
		
		double amp1 = ( p1.duration() > 0. ) ? 
			p1.amplitudeAt( it.time() ) : 0.;
		double freq1 = ( p1.duration() > 0. ) ? 
			p1.frequencyAt( it.time() ) : it.frequency();
		double bw1 = ( p1.duration() > 0. ) ? 
			p1.bandwidthAt( it.time() ) : it.bandwidth();
		double theta1 = ( p1.duration() > 0. ) ? 
			p1.phaseAt( it.time() ) : it.phase();
			
		Breakpoint newbp( (alphaF * freq1) + ((1.-alphaF) * it.frequency()),
						   (alphaA * amp1) + ((1.-alphaA) * it.amplitude()),
						   (alphaBW * bw1) + ((1.-alphaBW) * it.bandwidth()),
						   (alphaF * theta1) + ((1.-alphaF) * it.phase()) );
		
		newp.insert( it.time(), newbp );
	}
	
		
	//	add the new partial to the collection,
	//	if it is valid:
	if ( newp.head() ) {
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
int
Morph::collectByLabel( const list<Partial>::const_iterator & start, 
					   const list<Partial>::const_iterator & end, 
					   list<Partial> & collector, 
					   int label ) const
{
	//	function object for selecting by label:
	struct LabelIs
	{
		int _x;
		LabelIs( int x ) : _x(x) {}
		boolean operator() (const Partial & p) const {
			return p.label() == _x;
		}
	};
		
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

// ---------------------------------------------------------------------------
//	defaultMap
// ---------------------------------------------------------------------------
//	Static member for creating a default morphing function. By default,
//	use a BreakpointMap that makes a constant 50% morph.
//
auto_ptr< Map > 
Morph::defaultMap( void )
{
	BreakpointMap * m = new BreakpointMap();
	//m->insertBreakpoint( 0., 0.5 );
	return auto_ptr< Map >( m );
}



End_Namespace( Loris )

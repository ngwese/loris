// ===========================================================================
//	Morph.C
//	
//	Stuff related to morphing.
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

#pragma mark -
#pragma mark stuff


// ---------------------------------------------------------------------------
//	dilate
// ---------------------------------------------------------------------------
//	Not in-place temporal dilation/compression. Return a new dilated partial.
//	
Partial 
dilate( const Partial & p, const vector< double > & current, 
		const vector< double > & desired )
{
	Assert( current.front() > 0. );
	Assert( current.size() == desired.size() );
	
	//	create the new Partial:
	Partial newp;
	newp.setLabel( p.label() );
	
	int index = 0;
	const Breakpoint * bp;
	for ( bp = p.head(); bp != Null; bp = bp->next() ) {
		while ( index < current.size() && bp->time() > current[index] ) {
			++index;
		}
		
		double newtime = 0;
		if ( index == 0 ) {
			newtime = bp->time() * desired[index] / current[index];
		}
		else if ( index > current.size() ) {
			newtime = desired[index-1] + 
					( bp->time() - current[index - 1] );
		}
		else {
			newtime = desired[index-1] + 
					( (bp->time() - current[index - 1]) * 
						( desired[index] - desired[index - 1] ) /
						( current[index] - current[index - 1] ) );
		}
		
		//	add a Breakpoint at the computed time:
		newp.insert( newtime, Breakpoint(*bp) );
	}
	
	return newp;
}

#pragma mark -
#pragma mark weighting function
// ---------------------------------------------------------------------------
//	insertBreakpoint
// ---------------------------------------------------------------------------
//	
void
WeightFunction::insertBreakpoint( double time, double weight )
{
	struct LaterThan
	{
		LaterThan( double z ) : _z( z ) {}
		boolean operator () ( const pair<double, double> & bp ) const { 
			return bp.second > _z;
		}
		
	private:
		double _z;
	};
		
	BreakpointsVector::iterator it = 
		find_if( _breakpoints.begin(), _breakpoints.end(), LaterThan(time) );
	_breakpoints.insert( it, std::make_pair( time, weight ) );
}

// ---------------------------------------------------------------------------
//	weightAtTime
// ---------------------------------------------------------------------------
//
double
WeightFunction::weightAtTime( double time ) const
{
	if ( _breakpoints.size() == 0 ) {
		return 0.5;
	}
	
	if ( time <= _breakpoints.front().first ) {
		return _breakpoints.front().second;
	}
	else if ( time >= _breakpoints.back().first ) {
		return _breakpoints.back().second;
	}
	else {
		std::vector< std::pair< double, double > >::const_iterator it;
		for (it = _breakpoints.begin(); it != _breakpoints.end(); ++it ) {
			if ((*it).first > time) {
			//	interpolate between it and its predecessor:
				double alpha = (time - (*(it-1)).first) / ((*it).first - (*(it-1)).first);
				return (alpha * (*it).second) + ((1. - alpha) * (*(it-1)).second);
			}
		}
	}	
	
	//	not reached:
	return 0.;	
}


#pragma mark -
#pragma mark construction

// ---------------------------------------------------------------------------
//	Morph constructor (single morph function)
// ---------------------------------------------------------------------------
//	Use auto_ptrs instead to clean up this obscene mess.
//
Morph::Morph( const Map & f ) :
	_freqFunction( Null ),
	_ampFunction( Null ),
	_bwFunction( Null ),
	_minlabel( 0 ),
	_maxlabel( 200 ),
	_crossfadelabel( 0 )
{
	try {
		setFrequencyFunction( f );
		setAmplitudeFunction( f );
		setBandwidthFunction( f );
	}
	catch (...) {
		delete _freqFunction;
		delete _ampFunction;
		delete _bwFunction;
		throw;
	}
}

// ---------------------------------------------------------------------------
//	Morph constructor (distinct morph functions)
// ---------------------------------------------------------------------------
//	Use auto_ptrs instead to clean up this obscene mess.
//
Morph::Morph( const Map & ff, 
			  const Map & af, 
			  const Map & bwf ) :
	_freqFunction( Null ),
	_ampFunction( Null ),
	_bwFunction( Null ),
	_minlabel( 0 ),
	_maxlabel( 200 ),
	_crossfadelabel( 0 )
{
	try {
		setFrequencyFunction( ff );
		setAmplitudeFunction( af );
		setBandwidthFunction( bwf );
	}
	catch (...) {
		delete _freqFunction;
		delete _ampFunction;
		delete _bwFunction;
		throw;
	}
}

// ---------------------------------------------------------------------------
//	Morph copy constructor
// ---------------------------------------------------------------------------
//	Use auto_ptrs instead to clean up this obscene mess.
//
Morph::Morph( const Morph & other ) :
	_partials( other._partials ),
	_freqFunction( Null ),
	_ampFunction( Null ),
	_bwFunction( Null ),
	_minlabel( other._minlabel ),
	_maxlabel( other._maxlabel ),
	_crossfadelabel( other._crossfadelabel )
{
	try {
		setFrequencyFunction( other.frequencyFunction() );
		setAmplitudeFunction( other.amplitudeFunction() );
		setBandwidthFunction( other.bandwidthFunction() );
	}
	catch (...) {
		delete _freqFunction;
		delete _ampFunction;
		delete _bwFunction;
		throw;
	}
}

// ---------------------------------------------------------------------------
//	Morph destructor
// ---------------------------------------------------------------------------
//
Morph::~Morph( void )
{
	delete _freqFunction;
	delete _ampFunction;
	delete _bwFunction;
}

// ---------------------------------------------------------------------------
//	setFrequencyFunction
// ---------------------------------------------------------------------------
//
void
Morph::setFrequencyFunction( const Map & f )
{
	if ( _freqFunction != &f ) {
		delete _freqFunction;
		_freqFunction = f.clone();
	}
}

// ---------------------------------------------------------------------------
//	setAmplitudeFunction
// ---------------------------------------------------------------------------
//
void
Morph::setAmplitudeFunction( const Map & f )
{
	if ( _ampFunction != &f ) {
		delete _ampFunction;
		_ampFunction = f.clone();
	}
}

// ---------------------------------------------------------------------------
//	setBandwidthFunction
// ---------------------------------------------------------------------------
//
void
Morph::setBandwidthFunction( const Map & f )
{
	if ( _bwFunction != &f ) {
		delete _bwFunction;
		_bwFunction = f.clone();
	}
}

// ---------------------------------------------------------------------------
//	frequencyFunction
// ---------------------------------------------------------------------------
//
inline const Map & 
Morph::frequencyFunction(void) const 
{ 
//	make sure the morphing function has been specified:
	Assert( _freqFunction != Null );
	
	return * _freqFunction; 
}

// ---------------------------------------------------------------------------
//	amplitudeFunction
// ---------------------------------------------------------------------------
//
inline const Map & 
Morph::amplitudeFunction(void) const
{ 
//	make sure the morphing function has been specified:
	Assert( _ampFunction != Null );
	
	return * _ampFunction; 
}

// ---------------------------------------------------------------------------
//	bandwidthFunction
// ---------------------------------------------------------------------------
//
inline const Map & 
Morph::bandwidthFunction(void) const
{ 
//	make sure the morphing function has been specified:
	Assert( _bwFunction != Null );
	
	return * _bwFunction; 
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
//	This should use PartialIterators instead of directly using Breakpoints.
//
void 
Morph::morphPartial( const Partial & p1, const Partial & p2 )
{
	//	make a new Partial:
	Partial newp;
	
	//	loop over Breakpoints in first partial:
	for ( const Breakpoint * bp1 = p1.head(); bp1 != Null; bp1 = bp1->next() ) {
		double alphaF = frequencyFunction().valueAt( bp1->time() );
		double alphaA = amplitudeFunction().valueAt( bp1->time() );
		double alphaBW = bandwidthFunction().valueAt( bp1->time() );
		
		double amp2 = ( p2.duration() > 0. ) ? 
			p2.amplitudeAt( bp1->time() ) : 0.;
		double freq2 = ( p2.duration() > 0. ) ? 
			p2.frequencyAt( bp1->time() ) : bp1->frequency();
		double bw2 = ( p2.duration() > 0. ) ? 
			p2.bandwidthAt( bp1->time() ) : bp1->bandwidth();
		double theta2 = ( p2.duration() > 0. ) ? 
			p2.phaseAt( bp1->time() ) : bp1->phase();
			
		Breakpoint newbp( (alphaF * freq2) + ((1.-alphaF) * bp1->frequency()),
						   (alphaA * amp2) + ((1.-alphaA) * bp1->amplitude()),
						   (alphaBW * bw2) + ((1.-alphaBW) * bp1->bandwidth()),
						   (alphaF * theta2) + ((1.-alphaF) * bp1->phase()) );
		
		newp.insert( bp1->time(), newbp );
	}
	
	//	now do it for the other Partial:
	for ( const Breakpoint * bp2 = p2.head(); bp2 != Null; bp2 = bp2->next() ) {
		double alphaF = 1. - frequencyFunction().valueAt( bp2->time() );
		double alphaA = 1. - amplitudeFunction().valueAt( bp2->time() );
		double alphaBW = 1. - bandwidthFunction().valueAt( bp2->time() );
		
		double amp1 = ( p1.duration() > 0. ) ? 
			p1.amplitudeAt( bp2->time() ) : 0.;
		double freq1 = ( p1.duration() > 0. ) ? 
			p1.frequencyAt( bp2->time() ) : bp2->frequency();
		double bw1 = ( p1.duration() > 0. ) ? 
			p1.bandwidthAt( bp2->time() ) : bp2->bandwidth();
		double theta1 = ( p1.duration() > 0. ) ? 
			p1.phaseAt( bp2->time() ) : bp2->phase();
			
		Breakpoint newbp( (alphaF * freq1) + ((1.-alphaF) * bp2->frequency()),
						   (alphaA * amp1) + ((1.-alphaA) * bp2->amplitude()),
						   (alphaBW * bw1) + ((1.-alphaBW) * bp2->bandwidth()),
						   (alphaF * theta1) + ((1.-alphaF) * bp2->phase()) );
		
		newp.insert( bp2->time(), newbp );
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

End_Namespace( Loris )

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
#pragma mark value at time 
// ---------------------------------------------------------------------------
//	amplitudeAtTime
// ---------------------------------------------------------------------------
//	
double
amplitudeAtTime( const Partial & p, double time )
{
	Assert( p.head() != Null );
	
	const Breakpoint * bp = p.find(time);
	if ( !bp ) {
	//	time is before the onset of the Partial:
		return 0.;
	}
	else if (! bp->next() ){
	//	time is past the end of the Partial:
		return 0.;
	}
	else {
	//	interpolate between bp and its successor:
		double alpha = (time - bp->time()) / (bp->next()->time() - bp->time());
		return (alpha * bp->next()->amplitude()) + ((1. - alpha) * bp->amplitude());
	}
}
// ---------------------------------------------------------------------------
//	frequencyAtTime
// ---------------------------------------------------------------------------
//	
double
frequencyAtTime( const Partial & p, double time )
{
	Assert( p.head() != Null );
	
	const Breakpoint * bp = p.find(time);
	if ( !bp ) {
	//	time is before the onset of the Partial:
		return p.head()->frequency();
	}
	else if (! bp->next() ){
	//	time is past the end of the Partial:
		return bp->frequency();
	}
	else {
	//	interpolate between bp and its successor:
		double alpha = (time - bp->time()) / (bp->next()->time() - bp->time());
		return (alpha * bp->next()->frequency()) + ((1. - alpha) * bp->frequency());
	}
}
// ---------------------------------------------------------------------------
//	phaseAtTime
// ---------------------------------------------------------------------------
//	
double
phaseAtTime( const Partial & p, double time )
{
	Assert( p.head() != Null );
	
	const Breakpoint * bp = p.find(time);
	if ( !bp ) {
	//	time is before the onset of the Partial:
		double dp = TwoPi * (p.head()->time() - time) * p.head()->frequency();
		return fmod( p.head()->phase() - dp, TwoPi);
	}
	else if (! bp->next() ){
	//	time is past the end of the Partial:
		double dp = TwoPi * (time - bp->time()) * bp->frequency();
		return fmod( bp->phase() + dp, TwoPi );
	}
	else {
	//	interpolate between bp and its successor:
	//	(compute the frequency halfway between bp and time,
	//	and use that average frequency to compute the phase
	//	travel from bp to time)
		double alpha = (time - bp->time()) / (bp->next()->time() - bp->time());
		double favg = (0.5 * alpha * bp->next()->frequency()) + 
						((1. - (0.5 * alpha)) * bp->frequency());
		double dp = TwoPi * (time - bp->time()) * favg;
		return fmod( bp->phase() + dp, TwoPi );
	}
}
// ---------------------------------------------------------------------------
//	bandwidthAtTime
// ---------------------------------------------------------------------------
//	
double
bandwidthAtTime( const Partial & p, double time )
{
	Assert( p.head() != Null );
	
	const Breakpoint * bp = p.find(time);
	if ( !bp ) {
	//	time is before the onset of the Partial:
		return p.head()->bandwidth();
	}
	else if (! bp->next() ){
	//	time is past the end of the Partial:
		return bp->bandwidth();
	}
	else {
	//	interpolate between bp and its successor:
		double alpha = (time - bp->time()) / (bp->next()->time() - bp->time());
		return (alpha * bp->next()->bandwidth()) + ((1. - alpha) * bp->bandwidth());
	}
}


#pragma mark -
#pragma mark stuff

// ---------------------------------------------------------------------------
//	select
// ---------------------------------------------------------------------------
//	From all, select Partials having the specified label, return a vector
//	of pointers to those Partials.
//	
list< Partial >
select( const list<Partial> & all, int label )
{
	list<Partial> ret;
	
	list<Partial>::const_iterator it;
	for ( it = all.begin(); it != all.end(); ++it ) {
		if ( (*it).label() == label )
			ret.push_back( *it );
	}
	
	return ret;
}


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
//	Morph constructor
// ---------------------------------------------------------------------------
//	Should clean up the pointers here, and use auto_ptrs, but as long as
//	this class is not subclassed (so no constructor further down the chain
//	of inheritence can throw and exception), then there won't be a problem
//	here, because the only thing that can generate an exception is the 
//	Distiller construction, and since the other pointers are Null, we won't
//	have anything else to clean up. FIX ANYWAY, ITS WRONG.
//
Morph::Morph( void ) :
	_freqFunction( Null ),
	_ampFunction( Null ),
	_bwFunction( Null ),
	_distiller( new Distiller() )
{
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
	delete _distiller;
}

// ---------------------------------------------------------------------------
//	setFreqFunction
// ---------------------------------------------------------------------------
//
void
Morph::setFreqFunction( const WeightFunction & f )
{
	delete _freqFunction;
	_freqFunction = new WeightFunction( f );
}

// ---------------------------------------------------------------------------
//	setAmpFunction
// ---------------------------------------------------------------------------
//
void
Morph::setAmpFunction( const WeightFunction & f )
{
	delete _ampFunction;
	_ampFunction = new WeightFunction( f );
}

// ---------------------------------------------------------------------------
//	setBwFunction
// ---------------------------------------------------------------------------
//
void
Morph::setBwFunction( const WeightFunction & f )
{
	delete _bwFunction;
	_bwFunction = new WeightFunction( f );
}

// ---------------------------------------------------------------------------
//	doit
// ---------------------------------------------------------------------------
//
void
Morph::doit( const list<Partial> & plist1, const list<Partial> & plist2 )
{
	//	loop over lots of labels:
	for ( int label = 1; label < 300; ++label ) {
		cout << "morphing partials with label " << label << endl;;
		
		// find partials with the correct label:
		list<Partial> sublist1 = select( plist1, label );
		list<Partial> sublist2 = select( plist2, label );
		
		cout << sublist1.size() << " and " << sublist2.size() << "partials" << endl;

		// sublist1.splice( sublist1.begin(), select( plist1, label ) );
		// sublist2.splice( sublist2.begin(), select( plist2, label ) );
		
		if ( sublist1.size() == 0 && sublist2.size() == 0 ) {
			continue;
		}
		
		//	distill the sublists into single partials:
		Partial p1 = _distiller->distill( sublist1 );
		Partial p2 = _distiller->distill( sublist2 );
		
		morphPartial( p1, p2 );
	}
}

// ---------------------------------------------------------------------------
//	freqWeight
// ---------------------------------------------------------------------------
//
inline const WeightFunction & 
Morph::freqWeight(void) const 
{ 
//	make sure the morphing function has been specified:
	Assert( _freqFunction != Null );
	
	return * _freqFunction; 
}

// ---------------------------------------------------------------------------
//	ampWeight
// ---------------------------------------------------------------------------
//
inline const WeightFunction & 
Morph::ampWeight(void) const
{ 
//	make sure the morphing function has been specified:
	Assert( _ampFunction != Null );
	
	return * _ampFunction; 
}

// ---------------------------------------------------------------------------
//	bwWeight
// ---------------------------------------------------------------------------
//
inline const WeightFunction & 
Morph::bwWeight(void) const
{ 
//	make sure the morphing function has been specified:
	Assert( _bwFunction != Null );
	
	return * _bwFunction; 
}

// ---------------------------------------------------------------------------
//	morphPartial
// ---------------------------------------------------------------------------
//	assume that the Morph object has a list of Partials that it is building
//	and access to three morphing functions, for frequency, amplitude, and
//	bandwidth.
//
void 
Morph::morphPartial( const Partial & p1, const Partial & p2 )
{
	//	make a new Partial:
	_partials.push_back( Partial() );
	Partial & newp = _partials.back();
	
	//	loop over Breakpoints in first partial:
	for ( const Breakpoint * bp1 = p1.head(); bp1 != Null; bp1 = bp1->next() ) {
		double alphaF = freqWeight().weightAtTime( bp1->time() );
		double alphaA = ampWeight().weightAtTime( bp1->time() );
		double alphaBW = bwWeight().weightAtTime( bp1->time() );
		
		double amp2 = ( p2.duration() > 0.) ? 
			amplitudeAtTime( p2, bp1->time() ) : 0.;
		double freq2 = ( p2.duration() > 0.) ? 
			frequencyAtTime( p2, bp1->time() ) : bp1->frequency();
		double bw2 = ( p2.duration() > 0.) ? 
			bandwidthAtTime( p2, bp1->time() ) : bp1->bandwidth();
		double theta2 = ( p2.duration() > 0.) ? 
			phaseAtTime( p2, bp1->time() ) : bp1->phase();
			
		Breakpoint newbp( (alphaF * freq2) + ((1.-alphaF) * bp1->frequency()),
						   (alphaA * amp2) + ((1.-alphaA) * bp1->amplitude()),
						   (alphaBW * bw2) + ((1.-alphaBW) * bp1->bandwidth()),
						   (alphaF * theta2) + ((1.-alphaF) * bp1->phase()) );
		
		newp.insert( bp1->time(), newbp );
	}
	
	//	now do it for the other Partial:
	for ( const Breakpoint * bp2 = p2.head(); bp2 != Null; bp2 = bp2->next() ) {
		double alphaF = 1. - freqWeight().weightAtTime( bp2->time() );
		double alphaA = 1. - ampWeight().weightAtTime( bp2->time() );
		double alphaBW = 1. - bwWeight().weightAtTime( bp2->time() );
		
		double amp1 = ( p1.duration() > 0.) ? 
			amplitudeAtTime( p1, bp2->time() ) : 0.;
		double freq1 = ( p1.duration() > 0.) ? 
			frequencyAtTime( p1, bp2->time() ) : bp2->frequency();
		double bw1 = ( p1.duration() > 0.) ? 
			bandwidthAtTime( p1, bp2->time() ) : bp2->bandwidth();
		double theta1 = ( p1.duration() > 0.) ? 
			phaseAtTime( p1, bp2->time() ) : bp2->phase();
			
		Breakpoint newbp( (alphaF * freq1) + ((1.-alphaF) * bp2->frequency()),
						   (alphaA * amp1) + ((1.-alphaA) * bp2->amplitude()),
						   (alphaBW * bw1) + ((1.-alphaBW) * bp2->bandwidth()),
						   (alphaF * theta1) + ((1.-alphaF) * bp2->phase()) );
		
		newp.insert( bp2->time(), newbp );
	}
	
	
}


End_Namespace( Loris )

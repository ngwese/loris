/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * Morpher.C
 *
 * Implementation of class Morpher.
 *
 * Kelly Fitz, 15 Oct 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include <Loris_prefix.h>
#include "Morpher.h"
#include "Exception.h"
#include "Partial.h"
#include "Breakpoint.h"
#include "Envelope.h"
#include "PartialUtils.h"
#include "notifier.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include <set>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class Morpher_imp
//
//	Implementation class for Morpher.
//
//	The Morpher object performs sound morphing (cite Lip's papers, and the book)
//	by interpolating Partial parmeter envelopes of corresponding Partials in
//	a pair of source sounds. The correspondences are established by labeling.
//	The Morpher object collects morphed Partials in a std::list< Partial >, that can
//	be accessed by clients.
//
class Morpher_imp
{
public:
//	-- instance variables --
	//	reference-counted smart pointers from Batov:
	//	(these Envelopes will never be modified by the Morpher_imp
	//	class, but Morpher should be able to grant non-const
	//	access to them (?), so they are not const)
	std::auto_ptr< Envelope > _freqFunction;	//	frequency morphing function
	std::auto_ptr< Envelope > _ampFunction;		//	amplitude morphing function
	std::auto_ptr< Envelope > _bwFunction;		//	bandwidth morphing function
	
	std::list< Partial > _partials;	//	collect Partials here
			
//	construction:
	Morpher_imp( const Envelope & ff, const Envelope & af, const Envelope & bwf );
	~Morpher_imp( void );
	
//	single Partial morph:
//	(core morphing operation, called by morph() and crossfade())
	void morphPartial( const Partial & p1, const Partial & p2, int assignLabel );
	
//	crossfade Partials with no correspondences:
//	(crossfaded Partials are unlabeled, or assigned the 
//	default label, 0)
	void crossfade( std::list< Partial >::const_iterator begin0, 
					std::list< Partial >::const_iterator end0,
					std::list< Partial >::const_iterator begin1, 
					std::list< Partial >::const_iterator end1 );

};	//	end of class Morpher_imp

// ---------------------------------------------------------------------------
//	Morpher_imp constructor 
// ---------------------------------------------------------------------------
//
Morpher_imp::Morpher_imp( const Envelope & ff, const Envelope & af, const Envelope & bwf ) :
	_freqFunction( ff.clone() ),
	_ampFunction( af.clone() ),
	_bwFunction( bwf.clone() )
{
}

// ---------------------------------------------------------------------------
//	Morpher_imp destructor
// ---------------------------------------------------------------------------
//
Morpher_imp::~Morpher_imp( void )
{
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
Morpher_imp::morphPartial( const Partial & p0, const Partial & p1, int assignLabel )
{
	//	make a new Partial:
	Partial newp;
	newp.setLabel( assignLabel );
	
	//	loop over Breakpoints in first partial:
	for ( PartialConstIterator iter = p0.begin(); iter != p0.end(); ++iter )
	{
		//	amplitude weight is always used:
		double alphaA = _ampFunction->valueAt( iter.time() );
		
		//	if p1 is a valid Partial, compute a weighted average 
		//	Breakpoint and insert it, otherwise just fade p0:
		if ( p1.duration() > 0. )
		{
			//	compute parameter weights:
			double alphaF = _freqFunction->valueAt( iter.time() );
			double alphaBW = _bwFunction->valueAt( iter.time() );
			
			//	compute interpolated values for p1:
			double amp1 = p1.amplitudeAt( iter.time() );
			double freq1 = p1.frequencyAt( iter.time() );
			double bw1 = p1.bandwidthAt( iter.time() );
			double theta1 = p1.phaseAt( iter.time() );
			
			//	create a new weighted average Breakpoint:	
			Breakpoint newbp( (alphaF * freq1) + ((1.-alphaF) * iter.breakpoint().frequency()),
							   (alphaA * amp1) + ((1.-alphaA) * iter.breakpoint().amplitude()),
							   (alphaBW * bw1) + ((1.-alphaBW) * iter.breakpoint().bandwidth()),
							   (alphaF * theta1) + ((1.-alphaF) * iter.breakpoint().phase()) );
			
			//	insert the new Breakpoint in the morphed Partial:
			newp.insert( iter.time(), newbp );
		}
		else
		{
			//	create a new scaled-amplitude Breakpoint:	
			Breakpoint newbp( iter.breakpoint().frequency(),
							  (1.-alphaA) * iter.breakpoint().amplitude(),
							  iter.breakpoint().bandwidth(),
							  iter.breakpoint().phase() );
		
			//	insert the new Breakpoint in the morphed Partial:
			newp.insert( iter.time(), newbp );
		}
	}
	
	//	now do it for the other Partial:
	for ( PartialConstIterator iter = p1.begin(); iter != p1.end(); ++iter )
	{
		//	amplitude weight is always used:
		double alphaA = 1. - _ampFunction->valueAt( iter.time() );
		
		//	if p0 is a valid Partial, compute a weighted average 
		//	Breakpoint and insert it, otherwise just fade p1:
		if ( p0.duration() > 0. )
		{
			//	compute parameter weights:
			double alphaF = 1. - _freqFunction->valueAt( iter.time() );
			double alphaBW = 1. - _bwFunction->valueAt( iter.time() );
			
			//	compute interpolated values for p1:
			double amp0 = p0.amplitudeAt( iter.time() );
			double freq0 = p0.frequencyAt( iter.time() );
			double bw0 = p0.bandwidthAt( iter.time() );
			double theta0 = p0.phaseAt( iter.time() );
			
			//	create a new weighted average Breakpoint:	
			Breakpoint newbp( (alphaF * freq0) + ((1.-alphaF) * iter.breakpoint().frequency()),
							   (alphaA * amp0) + ((1.-alphaA) * iter.breakpoint().amplitude()),
							   (alphaBW * bw0) + ((1.-alphaBW) * iter.breakpoint().bandwidth()),
							   (alphaF * theta0) + ((1.-alphaF) * iter.breakpoint().phase()) );
			
			//	insert the new Breakpoint in the morphed Partial:
			newp.insert( iter.time(), newbp );
		}
		else
		{
			//	create a new scaled-amplitude Breakpoint:	
			Breakpoint newbp( iter.breakpoint().frequency(),
							  (1.-alphaA) * iter.breakpoint().amplitude(),
							  iter.breakpoint().bandwidth(),
							  iter.breakpoint().phase() );
		
			//	insert the new Breakpoint in the morphed Partial:
			newp.insert( iter.time(), newbp );
		}
	}
	
		
	//	add the new partial to the collection,
	//	if it is valid:
	if ( newp.begin() != newp.end() ) 
	{
		_partials.push_back( newp );
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
Morpher_imp::crossfade( std::list< Partial >::const_iterator begin0, 
				  std::list< Partial >::const_iterator end0,
				  std::list< Partial >::const_iterator begin1, 
				  std::list< Partial >::const_iterator end1 )
{
	Partial nullPartial;
	debugger << "crossfading unlabeled (labeled 0) Partials" << endl;
	
	long debugCounter;

	//	crossfade Partials corresponding to a morph weight of 0:
	std::list< Partial >::const_iterator it;
	debugCounter = 0;
	for ( it = begin0; it != end0; ++it )
	{
		if ( it->label() == 0 )
		{
			morphPartial( *it, nullPartial, 0 );	
			++debugCounter;
		}
	}
	debugger << "there were " << debugCounter << " in sound 1" << endl;

	//	crossfade Partials corresponding to a morph weight of 1:
	debugCounter = 0;
	for ( it = begin1; it != end1; ++it )
	{
		if ( it->label() == 0 )
		{
			morphPartial( nullPartial, *it, 0 );
			++debugCounter;
		}
	}
	debugger << "there were " << debugCounter << " in sound 2" << endl;
}

// ---------------------------------------------------------------------------
//	Morpher constructor (single morph function)
// ---------------------------------------------------------------------------
//	In fully-insulating interface constructors, with no subclassing (fully-
//	insulating interfaces have no virtual members), can safely initialize the
//	imp pointer this way because only that constructor could generate an 
//	exception, and in that case the associated memory would be released 
//	automatically. So there's no risk of a memory leak associated with this
//	pointer initialization.
//
Morpher::Morpher( const Envelope & f ) :
	_imp( new Morpher_imp( f, f, f ) )
{
}

// ---------------------------------------------------------------------------
//	Morpher constructor (distinct morph functions)
// ---------------------------------------------------------------------------
//	In fully-insulating interface constructors, with no subclassing (fully-
//	insulating interfaces have no virtual members), can safely initialize the
//	imp pointer this way because only that constructor could generate an 
//	exception, and in that case the associated memory would be released 
//	automatically. So there's no risk of a memory leak associated with this
//	pointer initialization.
//
Morpher::Morpher( const Envelope & ff, const Envelope & af, const Envelope & bwf ) :
	_imp( new Morpher_imp( ff, af, bwf ) )
{
}

// ---------------------------------------------------------------------------
//	Morpher destructor
// ---------------------------------------------------------------------------
//
Morpher::~Morpher( void )
{
	delete _imp;
}

// ---------------------------------------------------------------------------
//	morph
// ---------------------------------------------------------------------------
//	Morph two sounds (collections of Partials labeled to indicate
//	correspondences) into a single labeled collection of Partials.
//
void 
Morpher::morph( std::list< Partial >::const_iterator begin0, 
			  std::list< Partial >::const_iterator end0,
			  std::list< Partial >::const_iterator begin1, 
			  std::list< Partial >::const_iterator end1 )
{
	//	collect the labels in the two Partial ranges, 
	//	object if the Partials have not been distilled,
	//	that is, if they contain multiple Partials having 
	//	the same non-zero label:
	std::set< int > labels, moreLabels;
	for ( std::list< Partial >::const_iterator it = begin0; it != end0; ++it ) 
	{
		//	don't add the crossfade label to the set:
		if ( it->label() != 0 )
		{
			//	set::insert() returns a pair, the second element
			//	of which is false if the insertion failed because
			//	the set already contained the insertee:
			if ( ! labels.insert(it->label()).second )
				Throw( InvalidObject, "Partials must be distilled before morphing." );
		}
	}
	for ( std::list< Partial >::const_iterator it = begin1; it != end1; ++it ) 
	{
		//	don't add the non-label, 0, to the set:
		if ( it->label() != 0 )
		{
			//	as above:
			if ( ! moreLabels.insert(it->label()).second )
				Throw( InvalidObject, "Partials must be distilled before morphing." );
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
		std::list< Partial >::const_iterator p0 = 
			std::find_if( begin0, end0, 
						  std::bind2nd(PartialUtils::label_equals(), *labelIter) );
				
		//	find source Partial 1:
		std::list< Partial >::const_iterator p1 = 
			std::find_if( begin1, end1, 
						  std::bind2nd(PartialUtils::label_equals(), *labelIter) );
		
		debugger << "morphing " << ((p0 != end0)?(1):(0)) 
				 << " and " << ((p1 != end1)?(1):(0)) 
				 <<	" partials with label " <<	*labelIter << endl;
				 
		if ( p0 == end0 )
		{
			Assert( p1 != end1 );
			_imp->morphPartial( Partial(), *p1, *labelIter );
		}
		else if ( p1 == end1 )
		{
			Assert( p0 != end0 );
			_imp->morphPartial( *p0, Partial(), *labelIter );
		}
		else
		{			 
			_imp->morphPartial( *p0, *p1, *labelIter );
		}
		
	}	//	end loop over labels
	
	//	crossfade the remaining unlabeled Partials:
	_imp->crossfade( begin0, end0, begin1, end1 );
}

// ---------------------------------------------------------------------------
//	setFrequencyFunction
// ---------------------------------------------------------------------------
//
void
Morpher::setFrequencyFunction( const Envelope & f )
{
	_imp->_freqFunction.reset( f.clone() );
}

// ---------------------------------------------------------------------------
//	setAmplitudeFunction
// ---------------------------------------------------------------------------
//
void
Morpher::setAmplitudeFunction( const Envelope & f )
{
	_imp->_ampFunction.reset( f.clone() );
}

// ---------------------------------------------------------------------------
//	setBandwidthFunction
// ---------------------------------------------------------------------------
//
void
Morpher::setBandwidthFunction( const Envelope & f )
{
	_imp->_bwFunction.reset( f.clone() );
}

// ---------------------------------------------------------------------------
//	frequencyFunction
// ---------------------------------------------------------------------------
//
const Envelope &
Morpher::frequencyFunction( void ) const 
{
	return * _imp->_freqFunction;
}

// ---------------------------------------------------------------------------
//	amplitudeFunction
// ---------------------------------------------------------------------------
//
const Envelope &
Morpher::amplitudeFunction( void ) const 
{
	return * _imp->_ampFunction;
}

// ---------------------------------------------------------------------------
//	bandwidthFunction
// ---------------------------------------------------------------------------
//
const Envelope &
Morpher::bandwidthFunction( void ) const 
{
	return * _imp->_bwFunction;
}

// ---------------------------------------------------------------------------
//	partials
// ---------------------------------------------------------------------------
//
std::list< Partial > & 
Morpher::partials( void )
{ 
	return _imp->_partials; 
}

// ---------------------------------------------------------------------------
//	partials
// ---------------------------------------------------------------------------
//
const std::list< Partial > & 
Morpher::partials( void ) const 
{ 
	return _imp->_partials; 
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#ifndef __Loris_morph__
#define __Loris_morph__
// ===========================================================================
//	Morph.h
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
#include "Partial.h"
#include "Map.h"
#include <list>
#include <memory>	//	for auto_ptr

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class Morph
//
//	auto_ptr is used to pass Map arguments to make explicit the
//	source/sink relationship between the caller and the Morph.
//	Morph assumes ownership, and the client's auto_ptr
//	will have no reference (or ownership).
//
//	This also frees the abstract class Map from requiring a clone()
//	member to be defined by subclasses. This is kind of inconvenient, 
//	though, because, it is not possible to have a single-argument 
//	constructor that assigns the same function to all three parameters,
//	nor can the caller create a single Map and pass it three times as
//	an argument. The client is responsible for making three dynamic
//	allocations. Finally, it is impossible to have a copy constructor
//	for Morph, because there is no way to copy the (abstract) Maps 
//	without a cloning operation. A solution is to leave the pure 
//	virtual clone()	member in the Map class, and take advantage of 
//	it in the single-argument and copy constructors.
//
//	update: this is WAY too much of a nuisance, need another solution.
//
class Morph
{
//	-- public interface --
public:
//	construction:
	Morph( std::auto_ptr< Map > f = std::auto_ptr< Map >() );
	Morph( std::auto_ptr< Map > ff, 
		   std::auto_ptr< Map > af, 
		   std::auto_ptr< Map > bwf );
	Morph( const Morph & other );
	
	//~Morph( void );	//	use compiler-generated destructor
	
	Morph & operator= ( const Morph & other );

//	morph two sounds (lists of Partials labeled to indicate
//	correspondences) into a single one:
	void morph( const std::list<Partial> & plist1, const std::list<Partial> & plist2 );

//	single Partial morph:
	void morphPartial( const Partial & p1, const Partial & p2 );
	
//	morphed Partial access:	
	std::list< Partial > & partials(void) { return _partials; }

//	crossfade Partials with no correspondences:
	void crossfadePartials( const std::list<Partial> & plist1, 
							const std::list<Partial> & plist2, 
							int label = 0);

//	morphing functions access/mutation:	
	void setFrequencyFunction( std::auto_ptr< Map > f = std::auto_ptr< Map >() );
	void setAmplitudeFunction( std::auto_ptr< Map > f = std::auto_ptr< Map >() );
	void setBandwidthFunction( std::auto_ptr< Map > f = std::auto_ptr< Map >() );

	const Map & frequencyFunction( void ) const { return * _freqFunction; }
	const Map & amplitudeFunction( void ) const { return * _ampFunction; }
	const Map & bandwidthFunction( void ) const { return * _bwFunction; }
	
	Map & frequencyFunction( void ) { return * _freqFunction; }
	Map & amplitudeFunction( void ) { return * _ampFunction; }
	Map & bandwidthFunction( void ) { return * _bwFunction; }
	
//	label range access:
	std::pair< int, int > range( void ) const { return std::make_pair( _minlabel, _maxlabel ); }
	void setRange( int min, int max );
	
	int crossfadeLabel( void ) const { return _crossfadelabel; }
	void setCrossfadeLabel( int l ) { _crossfadelabel = l; }
	
//	-- helpers --
protected:	
	int collectByLabel( const std::list<Partial>::const_iterator & start, 
						const std::list<Partial>::const_iterator & end, 
						std::list<Partial> & collector, int label) const;
						
	static std::auto_ptr< Map > defaultMap( void );

//	-- instance variables --
	//	morphed partials:
	std::list< Partial > _partials;
	
	//	morphing functions:
	std::auto_ptr< Map > _freqFunction;
	std::auto_ptr< Map > _ampFunction;
	std::auto_ptr< Map > _bwFunction;
	
	//	range of labels for morphing:
	int _minlabel, _maxlabel;
	
	//	label for partials that should be crossfaded,
	//	instead of morphing:
	int _crossfadelabel;

};	//	end of class Morph

/*
//	prototypes for things that aren't classes or
//	member functions yet:
#include <vector>
Partial dilate( const Partial & p, const std::vector< double > & current, 
				const std::vector< double > & desired );
*/

End_Namespace( Loris )

#endif	// ndef __Loris_morph__

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
//	This class does not own PartialIterators or implement the 
//	PartialIteratorOwner interface. There's a good reason for this:
//	the morphing process needs  to query the Partials directly using 
//	parameterAt() members, which are not available through the
//	PartialIterator interface. 
//
class Morph
{
//	-- public interface --
public:
//	construction:
	Morph( void );
	Morph( const Map & f );
	Morph( const Map & ff, const Map & af, const Map & bwf );
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
	void setFrequencyFunction( const Map & f );
	void setAmplitudeFunction( const Map & f );
	void setBandwidthFunction( const Map & f );

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

End_Namespace( Loris )

#endif	// ndef __Loris_morph__

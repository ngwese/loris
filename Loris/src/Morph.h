#ifndef __Loris_morph__
#define __Loris_morph__

// ===========================================================================
//	Morph.h
//	
//	Stuff related to morphing.
//
//	-kel 15 Oct 99
//
// ===========================================================================

#include "LorisLib.h"
#include "Partial.h"

#include <vector>
#include <list>

Begin_Namespace( Loris )

class Map;

// ---------------------------------------------------------------------------
//	class Morph
//
class Morph
{
//	-- public interface --
public:
//	construction:
	Morph( const Map & f );
	Morph( const Map & ff, 
		   const Map & af, 
		   const Map & bwf );
	Morph( const Morph & other );
	~Morph( void );

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

//	specify morphing functions:	
	void setFrequencyFunction( const Map & f );
	void setAmplitudeFunction( const Map & f );
	void setBandwidthFunction( const Map & f );

	inline const Map & frequencyFunction( void ) const;
	inline const Map & amplitudeFunction( void ) const;
	inline const Map & bandwidthFunction( void ) const;
	
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
	Map * _freqFunction;
	Map * _ampFunction;
	Map * _bwFunction;
	
	//	range of labels for morphing:
	int _minlabel, _maxlabel;
	
	//	label for partials that should be crossfaded,
	//	instead of morphing:
	int _crossfadelabel;

};	//	end of class Morph


// ---------------------------------------------------------------------------
//	class WeightFunction
//
//	Probably want to beautify this a little bit, and clean up
//	the stuff that is available in the standard library.
//
class WeightFunction
{
public:
//	construction:
	WeightFunction( void ) {}
	
	//	use compiler-generated versions of these:
	//WeightFunction( const WeightFunction & ) {}
	//~WeightFunction( void );

//	adding breakpoints:
	void insertBreakpoint( double time, double weight ); 
	
//	evaluating weights:
	double weightAtTime( double time ) const;
	
private:
//	-- instance variables --
	typedef std::vector< std::pair< double, double > > BreakpointsVector;
	 BreakpointsVector _breakpoints;

};	//	end of class WeightFunction

//	prototypes for things that aren't classes or
//	member functions yet:
Partial dilate( const Partial & p, const std::vector< double > & current, 
				const std::vector< double > & desired );

End_Namespace( Loris )

#endif	// ndef __Loris_morph__
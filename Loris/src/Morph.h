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

class WeightFunction;
class Distiller;

// ---------------------------------------------------------------------------
//	class Morph
//
class Morph
{
//	-- public interface --
public:
//	construction:
	Morph( void );
	~Morph( void );
	
	void setFreqFunction( const WeightFunction & f );
	void setAmpFunction( const WeightFunction & f );
	void setBwFunction( const WeightFunction & f );
	
	void morphPartial( const Partial & p1, const Partial & p2 );
	void doit( const std::list<Partial> & plist1, const std::list<Partial> & plist2 );
	
	std::list< Partial > & partials(void) { return _partials; }


//	-- private helpers --
private:	
	inline const WeightFunction & freqWeight(void) const;
	inline const WeightFunction & ampWeight(void) const;
	inline const WeightFunction & bwWeight(void) const;

//	-- instance variables --
	//	morphed partials:
	std::list< Partial > _partials;
	
	//	morphing functions:
	WeightFunction * _freqFunction;
	WeightFunction * _ampFunction;
	WeightFunction * _bwFunction;
	
	//	distillation (of many partials having the same label):
	Distiller * _distiller;

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
std::list< Partial > select( const std::list<Partial> & all, int label );
Partial dilate( const Partial & p, const std::vector< double > & current, 
				const std::vector< double > & desired );

End_Namespace( Loris )

#endif	// ndef __Loris_morph__
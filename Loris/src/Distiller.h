#ifndef __Loris_distiller__
#define __Loris_distiller__

// ===========================================================================
//	Distiller.h
//	
//	A group of Partials that logically represent a single component
//	can be distilled into a single Partial using a Distiller. 
//
//	-kel 20 Oct 99
//
// ===========================================================================
#include "LorisLib.h"
#include "PartialIterator.h"
#include "Partial.h"
#include <list>

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class Distiller
//
class Distiller : public PartialIteratorOwner
{
//	-- public interface --
public:
//	construction:
	Distiller( double x = 0.001 ) : _fadeTime( x ) {}
	
	//	let the compiler generate these:
	//Distiller( void );
	//~Distiller( void );
	
//	distillation:
	const Partial & distill( const std::list<Partial>::const_iterator & start,
							 const std::list<Partial>::const_iterator & end, 
							 int assignLabel = 0 );

//	access:
	std::list< Partial > & partials(void) { return _partials; }
	
	double fadeTime( void ) const { return _fadeTime; }
	void setFadeTime( double x ) { if ( x > 0. ) _fadeTime = x; } 
	
//	-- helpers --
protected:
	void distillOne( const Partial & src, Partial & dest, 
					 const std::list<Partial>::const_iterator & start,
					 const std::list<Partial>::const_iterator & end );
	boolean gapAt( double time, std::list<Partial>::const_iterator start,
				   std::list<Partial>::const_iterator end ) const;

//	-- instance variables --
private:
	std::list< Partial > _partials;
	double _fadeTime;	//	in seconds

};	//	end of class Distiller

End_Namespace( Loris )

#endif	// ndef __Loris_distiller__
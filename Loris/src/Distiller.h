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
#include "Partial.h"
#include <list>

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class Distiller
//
//	Make this interface more like other function objects in Loris.
//
class Distiller : public PartialCollector
{
//	-- public interface --
public:
//	construction:
	explicit Distiller( double x = 0.001 );

//	copying and assignment:	
	Distiller( const Distiller & other );
	Distiller & operator= ( const Distiller & other );
	
	//	let the compiler generate this:
	//~Distiller( void );
	
//	distillation:
	const Partial & distill( const std::list<Partial>::const_iterator start,
							 const std::list<Partial>::const_iterator end, 
							 int assignLabel = 0 );

//	access:
//	is this ever useful?
	double fadeTime( void ) const { return _fadeTime; }
	void setFadeTime( double x ) { if ( x > 0. ) _fadeTime = x; } 
	
//	-- helpers --
protected:
	void distillOne( const Partial & src, Partial & dest, 
					 const std::list<Partial>::const_iterator start,
					 const std::list<Partial>::const_iterator end );
	boolean gapAt( double time, std::list<Partial>::const_iterator start,
				   std::list<Partial>::const_iterator end ) const;

//	-- instance variables --
private:
	double _fadeTime;	//	in seconds

};	//	end of class Distiller

End_Namespace( Loris )

#endif	// ndef __Loris_distiller__
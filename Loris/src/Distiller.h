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
class Distiller
{
//	-- public interface --
public:
//	construction:
	Distiller( void ) {}
	
	//	let the compiler generate these:
	//Distiller( void );
	//~Distiller( void );
	
//	distillation:
	Partial distill( const std::list<Partial> & all ) const;

//	-- instance variables --
private:

};	//	end of class Distiller

End_Namespace( Loris )

#endif	// ndef __Loris_distiller__
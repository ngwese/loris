#ifndef INCLUDE_DISTILLER_H
#define INCLUDE_DISTILLER_H
// ===========================================================================
//	Distiller.h
//	
//	A group of Partials that logically represent a single component
//	can be distilled into a single Partial using a Distiller. 
//
//	UPDATE THIS COMMENT
//
//	-kel 20 Oct 99
//
// ===========================================================================
#include "Partial.h" 	//	only needed for PartialList definition, duh.

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

class Partial;

// ---------------------------------------------------------------------------
//	class Distiller
//
//
class Distiller
{
//	-- public interface --
public:
//	construction:	
	Distiller( void );
	~Distiller( void );
	
//	distillation:
	void distill( PartialList & l );

//	-- unimplemented --
private:
	Distiller( const Distiller & other );
	Distiller & operator= ( const Distiller & other );
	
};	//	end of class Distiller

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_DISTILLER_H

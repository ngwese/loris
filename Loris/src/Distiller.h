#ifndef INCLUDE_DISTILLER_H
#define INCLUDE_DISTILLER_H
// ===========================================================================
//	Distiller.h
//	
//	A group of Partials that logically represent a single component
//	can be distilled into a single Partial using a Distiller. 
//
//	-kel 20 Oct 99
//
// ===========================================================================
#include "Partial.h"	//	needed only for PartalList definition

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

class Distiller_imp;

// ---------------------------------------------------------------------------
//	class Distiller
//
//
class Distiller
{
//	-- insulating implementation --
	Distiller_imp * _imp;	
			
//	-- public interface --
public:
//	construction:	
	Distiller( void );
	~Distiller( void );
	
//	distillation:
	void distill( PartialList::const_iterator start,
				  PartialList::const_iterator end,
				  int assignLabel );
	void distillAll( PartialList::const_iterator start,
			 		 PartialList::const_iterator end );

//	PartialList access:
	PartialList & partials( void );
	const PartialList & partials( void ) const;
				   
//	-- unimplemented --
private:
	Distiller( const Distiller & other );
	Distiller & operator= ( const Distiller & other );
	
};	//	end of class Distiller

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_DISTILLER_H

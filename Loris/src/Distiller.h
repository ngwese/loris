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
#include "Partial.h"
#include <list>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif


// ---------------------------------------------------------------------------
//	class Distiller
//
//	Make this interface more like other function objects in Loris.
//
class Distiller
{
//	-- instance variables --
	PartialList _partials;	//	collect Partials here
			
//	-- public interface --
public:
//	construction:	
//	(allow compiler to generate destructor)
	Distiller( void ) {}
	
//	distillation:
	const Partial & distill( PartialList::const_iterator start,
							 PartialList::const_iterator end, 
							 int assignLabel = 0 );

//	PartialList access:
	PartialList & partials( void ) { return _partials; }
	const PartialList & partials( void ) const { return _partials; }
	
//	-- helpers --
protected:
	void distillOne( const Partial & src, 
					 Partial & dest, 
					 PartialList::const_iterator start,
					 PartialList::const_iterator end );
	bool gapAt( double time, 
				PartialList::const_iterator start,
				PartialList::const_iterator end ) const;
				   
//	-- unimplemented --
private:
	Distiller( const Distiller & other );
	Distiller & operator= ( const Distiller & other );
	
};	//	end of class Distiller

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef __Loris_distiller__
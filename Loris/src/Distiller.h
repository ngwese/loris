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
class Distiller : public PartialCollector
{
//	-- public interface --
public:
//	copying and assignment:	
//	(allow compiler to generate destructor)
	Distiller( void ) {}
	Distiller( const Distiller & other );
	Distiller & operator= ( const Distiller & other );
	
//	distillation:
	const Partial & distill( const std::list<Partial>::const_iterator start,
							 const std::list<Partial>::const_iterator end, 
							 int assignLabel = 0 );

//	-- helpers --
protected:
	void distillOne( const Partial & src, Partial & dest, 
					 const std::list<Partial>::const_iterator start,
					 const std::list<Partial>::const_iterator end );
	bool gapAt( double time, std::list<Partial>::const_iterator start,
				   std::list<Partial>::const_iterator end ) const;
};	//	end of class Distiller

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef __Loris_distiller__
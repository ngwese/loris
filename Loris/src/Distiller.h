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
#include "Partial.h"
#include "Exception.h"
#include <memory>	//	for auto_ptr

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

class Map;

// ---------------------------------------------------------------------------
//	class Distiller
//
//	Make this interface more like other function objects in Loris.
//
class Distiller
{
//	-- instance variables --
	PartialList _partials;	//	collect Partials here
	std::auto_ptr< Map > _referenceEnv;
	int _refLabel;
			
//	-- public interface --
public:
//	construction:	
	Distiller( const Map & env, int label );
	~Distiller( void );
	
//	distillation:
	void distill( PartialList::const_iterator start,
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
				
	void fixGaps( Partial & dest, 
				  PartialList::const_iterator start,
				  PartialList::const_iterator end,
				  double freqRatio );
				   
//	-- unimplemented --
private:
	Distiller( const Distiller & other );
	Distiller & operator= ( const Distiller & other );
	
};	//	end of class Distiller

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_DISTILLER_H
#ifndef INCLUDE_DILATOR_H
#define INCLUDE_DILATOR_H
// ===========================================================================
//	Dilator.h
//
//	Dilator is a class of objects for temporally dilating and compressing
//	Partials by specifying initial and target times of temporal features.
//
//	-kel 26 Oct 99
//
// ===========================================================================
#include "Partial.h"
#include <vector>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class Dilator
//
//
class Dilator
{
//	-- implementation --
	std::vector< double > _initial, _target;	//	time points
	
	PartialList _partials;	//	collect Partials here

//	-- public interface --
public:
//	construction from n time points:
//	(allow compiler to generate destructor)
	Dilator( const double * ibegin, const double * tbegin, int n );
	
//	dilation:
	void dilate( Partial & p );
	void dilate( PartialList::iterator begin, PartialList::iterator end );
	
//	PartialList access:
	PartialList & partials( void ) { return _partials; }
	const PartialList & partials( void ) const { return _partials; }

//	-- unimplemented until useful --
private:
	Dilator( const Dilator & );
	Dilator & operator= ( const Dilator & rhs );
	
};	//	end of class Dilator

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_DILATOR_H

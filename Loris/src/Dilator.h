#ifndef __INCLUDE_DILATOR_H__
#define __INCLUDE_DILATOR_H__
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
class Dilator : public PartialCollector
{
//	-- implementation --
	std::vector< double > _initial, _target;
	
//	-- public interface --
public:
//	construction from n time points:
//	(allow compiler to generate destructor)
	Dilator( const double * ibegin, const double * tbegin, int n );
	
//	dilation:
	void dilate( Partial & p );
	void dilate( PartialList::iterator begin, PartialList::iterator end );
	
//	not implemented:
private:
	Dilator( const Dilator & );
	Dilator & operator= ( const Dilator & rhs );
};	//	end of class Dilator


#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef __INCLUDE_DILATOR_H__

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
	Dilator( const double * ibegin, const double * tbegin, int n );
	
	//	use compiler-generated:
	// ~Dilator( void );
	
//	dilation:
	void dilate( Partial & p );
	
//	template dilation of an iterator range:
//	(only if template members are allowed)
#if !defined(No_template_members)
	template < class Iter >
	void dilate( Iter begin, Iter end ) const
	{
		while ( begin != end )
			dilate( *(begin++) );
	}
#endif
	
//	not implemented:
private:
	Dilator( void );
	Dilator( const Dilator & );
	Dilator & operator= ( const Dilator & other );
};	//	end of class Dilator


#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef __INCLUDE_DILATOR_H__

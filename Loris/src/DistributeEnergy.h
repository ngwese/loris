#ifndef INCLUDE_DISTRIBUTEENERGY_H
#define INCLUDE_DISTRIBUTEENERGY_H
// ===========================================================================
//	DistributeEnergy.h
//	
//	Class definition for Loris function object DistributeEnergy.
//
//	-kel 23 Jan 2000
//
// ===========================================================================
#include "Partial.h"
//	can't get around including Partial.h until make a 
//	class out of PartialList and its iterators that I can
//	forward-declare

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class DistributeEnergy
//
//	Function object, has no instance variables or state. Not very useful
//	with STL though, as it takes three arguments. Could give it state by
//	constructing it with the range of receiving partials, and then it 
//	would be invoked with only one argument. Is it worth the overhead
//	of transfering the partials?
//
class DistributeEnergy
{
	double _maxdist;//	the maximum frequency distance (in Hz) over
					//	which energy will be redistributed; energy from
					//	a discarded Partial will be distributed only
					//	to Partials nearer in frequency than _maxdist
public:
	//	construction:
	explicit DistributeEnergy( double distanceHz );
	~DistributeEnergy(void);
	
	//	distribution:
	void distribute( const Partial & p, PartialList::iterator begin, 
										PartialList::iterator end ) const;
	
private: //	unimplemented:
	DistributeEnergy(void);
	DistributeEnergy(const DistributeEnergy &);
	
};	// end of class DistributeEnergy

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif 	// ndef INCLUDE_DISTRIBUTEENERGY_H
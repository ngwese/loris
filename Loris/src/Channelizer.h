#ifndef INCLUDE_CHANNELIZER_H
#define INCLUDE_CHANNELIZER_H
// ===========================================================================
//	Channelizer.h
//	
//	Definition of a class for labeling Partials in a PartialList
//	according to a set of linearly-spaced, variable-frequency
//	channels (like a time-varying harmonic frequency relationship).
//
//	This class interface is fully insulating.
//	This class is a leaf class, not for subclassing (no virtual destructor). 
//
//	-kel 21 July 2000
//
// ===========================================================================
#include <memory>	//	auto_ptr<>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

class Channelizer_Imp;

// ---------------------------------------------------------------------------
//	class Channelizer
//
class Channelizer
{
//	-- insulating implementaion --
	std::auto_ptr<Channelizer_Imp> _imp;
	
//	-- public interface --
public:
	Channelizer( void );
	~Channelizer( void );
	
};	//	end of class Channelizer

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_CHANNELIZER_H
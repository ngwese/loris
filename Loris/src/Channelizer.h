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
#include "Partial.h"	//	needed only for PartialList definition

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

class Channelizer_imp;
template <class T> class Handle;	//	Batov's Handle<> tempalte, in Handle.h
class Envelope;

// ---------------------------------------------------------------------------
//	class Channelizer
//
class Channelizer
{
//	-- insulating implementaion --
	Channelizer_imp * _imp;
	
//	-- public interface --
public:
	Channelizer( Handle<Envelope> refChanFreq, int refChanLabel );
	~Channelizer( void );

	//	channelizing:
	void channelize( PartialList::iterator begin, PartialList::iterator end );
	
};	//	end of class Channelizer

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_CHANNELIZER_H

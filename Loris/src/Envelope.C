// ===========================================================================
//	Envelope.C
//	
//	Envelope is an abstract base class representing a generic real (double) 
//	function of one real (double) argument. 
//
//	Since Envelope is just an interface, there's nothing interesting in 
//	the implementation file.
//
//	-kel 21 July 2000
//
// ===========================================================================
#include "Envelope.h"

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	constructor
// ---------------------------------------------------------------------------
//
Envelope::Envelope(void)
{
}

// ---------------------------------------------------------------------------
//	copy constructor
// ---------------------------------------------------------------------------
//
Envelope::Envelope(const Envelope &)
{
}

// ---------------------------------------------------------------------------
//	destructor
// ---------------------------------------------------------------------------
//
Envelope::~Envelope(void)
{
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

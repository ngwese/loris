// ===========================================================================
//	Handle.C
//	
//	Adapted from Vladimir Batov's Handle class by Kelly Fitz for
//	use in the Loris class library. Only class Counted in file
//	counted.h was changed substatially.
//	
//	-kel 8 Aug 00
//
// ===========================================================================

// Copyright (c) 1996-2000 by Vladimir Batov. Permission to  use,
// copy, modify,  distribute  and  sell  this  software  and  its
// documentation for any purpose is hereby granted  without  fee,
// provided that the above copyright notice appear in all  copies
// and that both that copyright notice and this permission notice
// appear in supporting documentation. I make no  representations
// about the suitability of this software for any purpose. It  is
// provided "as is" without express or implied warranty.

#include "Handle.h"

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

HandleBase::Counter HandleBase::_null(1); // Never deleted.

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif


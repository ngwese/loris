// Copyright (c) 1996-2000 by Vladimir Batov. Permission to  use,
// copy, modify,  distribute  and  sell  this  software  and  its
// documentation for any purpose is hereby granted  without  fee,
// provided that the above copyright notice appear in all  copies
// and that both that copyright notice and this permission notice
// appear in supporting documentation. I make no  representations
// about the suitability of this software for any purpose. It  is
// provided "as is" without express or implied warranty.
//
//
//	Handle.C
//	
//	Adapted from Vladimir Batov's Handle class by Kelly Fitz for
//	use in the Loris class library. Only class Counted in file
//	counted.h was changed substatially.
//
//	As originally written, Handles of base classes constructed from
//	Handles of derived classes produced slices that were not properly
//	destroyed by the base Handles because Counted contained its template
//	argument type, instead of containing a pointer or reference. This
//	version, wherein Counted contains a pointer to its template argument
//	type, is less efficient, but manages memory as expected when the last
//	surviving Handle is a base Handle.
//
//	Changes are copyright (c) 1999-2000 Kelly Fitz and Lippold Haken, and
//	are distributed freely and without warranty under the terms of the GNU 
//	General Public License. See the COPYRIGHT and LICENSE documents.
//	
//	Kelly Fitz, 8 Aug 2000
//

#include "Handle.h"

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

HandleBase::Counter HandleBase::_null(1); // Never deleted.

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif


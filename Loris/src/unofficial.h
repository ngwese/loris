// ===========================================================================
//	unofficial.h
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

#ifndef HANDLE_UNOFFICIAL_H
#define HANDLE_UNOFFICIAL_H

// The following constructors pass incoming arguments down to Co-
// unted that in turn, passes the arguments to the internal  Data
// instance. Therefore, "Handle<Data> h(arg1, arg2);" creates co-
// rresponding internal Data instance with Data(Arg1, Arg2) cons-
// tructor. The friendly interface is "unofficial" as it  is  not
// totally general. Handle(Arg1) overlaps with Handle copy-const-
// ructors if Arg1 is of the Handle type.

// One argument.

template<class Arg1>
explicit Handle(const Arg1& arg1)
: _counted(new Counted(arg1)) { _counted->use(); }

template<class Arg1>
explicit Handle(Arg1& arg1)
: _counted(new Counted(arg1)) { _counted->use(); }

// Two args.

#define TEMPLATE template<class Arg1, class Arg2>
#define CONSTRUCTOR(Arg1, Arg2) \
   explicit Handle(Arg1& arg1, Arg2& arg2) \
   : _counted(new Counted(arg1, arg2)) { _counted->use(); }

TEMPLATE CONSTRUCTOR(      Arg1,       Arg2)
TEMPLATE CONSTRUCTOR(      Arg1, const Arg2)
TEMPLATE CONSTRUCTOR(const Arg1,       Arg2)
TEMPLATE CONSTRUCTOR(const Arg1, const Arg2)

#undef TEMPLATE
#undef CONSTRUCTOR

// Three args require  8 constructors.

#define TEMPLATE template<class Arg1, class Arg2, class Arg3>
#define CONSTRUCTOR(Arg1, Arg2, Arg3) \
   explicit Handle(Arg1& arg1, Arg2& arg2, Arg3& arg3) \
   : _counted(new Counted(arg1, arg2, arg3)) { _counted->use(); }

TEMPLATE CONSTRUCTOR(const Arg1, const Arg2, const Arg3)
TEMPLATE CONSTRUCTOR(const Arg1, const Arg2,       Arg3)
TEMPLATE CONSTRUCTOR(const Arg1,       Arg2, const Arg3)
TEMPLATE CONSTRUCTOR(const Arg1,       Arg2,       Arg3)
TEMPLATE CONSTRUCTOR(      Arg1, const Arg2, const Arg3)
TEMPLATE CONSTRUCTOR(      Arg1, const Arg2,       Arg3)
TEMPLATE CONSTRUCTOR(      Arg1,       Arg2, const Arg3)
TEMPLATE CONSTRUCTOR(      Arg1,       Arg2,       Arg3)

#undef TEMPLATE
#undef CONSTRUCTOR

// Four  args require 16 constructors.
// Five  args require 32 constructors.
// Implement when needed.

#endif // HANDLE_UNOFFICIAL_H

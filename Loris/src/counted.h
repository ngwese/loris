// ===========================================================================
//	counted.h
//	
//	Adapted from Vladimir Batov's Handle class by Kelly Fitz for
//	use in the Loris class library. Only class Counted in file
//	counted.h (this file) was changed substatially.
//
//	As originally written, Handles of base classes constructed from
//	Handles of derived classes produced slices that were not properly
//	destroyed by the base Handles because Counted contained its template
//	argument type, instead of containing a pointer or reference. This
//	version, wherein Counted contains a pointer to its template argument
//	type, is less efficient, but manages memory as expected when the last
//	surviving Handle is a base Handle.
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

class Counted
{
   public:

  ~Counted() { delete _instance; }
   Counted() : _counter(0), _instance( new Data() ) {}

// If error processing is enabled.
// void dismiss () { if (!--_counter) 
//                      if (_counter.is_error()) delete (Counter*) this;
//                      else                     delete this; 
//                 }

   void dismiss () { if (!--_counter) delete this; }
   void     use () { ++_counter; }

   operator   Data& () { return  *_instance; }
   operator   Data* () { return _instance; }
   Data* operator-> () { return _instance; }

   template<class Derived>
   void dyn_cast() const
   {
      dynamic_cast<Derived&>(*_instance);
   }

   template<class Derived>
   void dyn_cast()
   {
      dynamic_cast<Derived&>(*_instance);
   }

   private:

   mutable Counter _counter; // Reference counter.
   Data           *_instance;

   Counted(const Counted&); // Not implemented 

   public:

   // Template constructors passing incoming arguments
   // to the internal Data instance.
   
   // One argument.

   template<class Arg1>
   Counted(const Arg1& arg1) 
   : _counter(0), _instance( new Data(arg1) ) {}

   template<class Arg1>
   Counted(Arg1& arg1) 
   : _counter(0), _instance( new Data(arg1) ) {}

   // Two args.

#define TEMPLATE template<class Arg1, class Arg2>
#define CONSTRUCTOR(Arg1, Arg2)     \
   Counted(Arg1& arg1, Arg2& arg2)  \
   : _counter(0), _instance( new Data(arg1, arg2) ) {}

TEMPLATE CONSTRUCTOR(const Arg1, const Arg2)
TEMPLATE CONSTRUCTOR(const Arg1,       Arg2)
TEMPLATE CONSTRUCTOR(      Arg1, const Arg2)
TEMPLATE CONSTRUCTOR(      Arg1,       Arg2)

#undef TEMPLATE
#undef CONSTRUCTOR

   // Three args require  8 constructors.

#define TEMPLATE template<class Arg1, class Arg2, class Arg3>
#define CONSTRUCTOR(Arg1, Arg2, Arg3)           \
   Counted(Arg1& arg1, Arg2& arg2, Arg3& arg3)  \
   : _counter(0), _instance( new Data(arg1, arg2, arg3) ) {}

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

#define TEMPLATE template<class Arg1, class Arg2, class Arg3, class Arg4>
#define CONSTRUCTOR(Arg1, Arg2, Arg3, Arg4)                 \
   Counted(Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4)  \
   : _counter(0), _instance(new Data(arg1, arg2, arg3, arg4) ) {}

TEMPLATE CONSTRUCTOR(const Arg1, const Arg2, const Arg3, const Arg4)
TEMPLATE CONSTRUCTOR(const Arg1, const Arg2, const Arg3,       Arg4)
TEMPLATE CONSTRUCTOR(const Arg1, const Arg2,       Arg3, const Arg4)
TEMPLATE CONSTRUCTOR(const Arg1, const Arg2,       Arg3,       Arg4)
TEMPLATE CONSTRUCTOR(const Arg1,       Arg2, const Arg3, const Arg4)
TEMPLATE CONSTRUCTOR(const Arg1,       Arg2, const Arg3,       Arg4)
TEMPLATE CONSTRUCTOR(const Arg1,       Arg2,       Arg3, const Arg4)
TEMPLATE CONSTRUCTOR(const Arg1,       Arg2,       Arg3,       Arg4)
TEMPLATE CONSTRUCTOR(      Arg1, const Arg2, const Arg3, const Arg4)
TEMPLATE CONSTRUCTOR(      Arg1, const Arg2, const Arg3,       Arg4)
TEMPLATE CONSTRUCTOR(      Arg1, const Arg2,       Arg3, const Arg4)
TEMPLATE CONSTRUCTOR(      Arg1, const Arg2,       Arg3,       Arg4)
TEMPLATE CONSTRUCTOR(      Arg1,       Arg2, const Arg3, const Arg4)
TEMPLATE CONSTRUCTOR(      Arg1,       Arg2, const Arg3,       Arg4)
TEMPLATE CONSTRUCTOR(      Arg1,       Arg2,       Arg3, const Arg4)
TEMPLATE CONSTRUCTOR(      Arg1,       Arg2,       Arg3,       Arg4)

#undef TEMPLATE
#undef CONSTRUCTOR

   // Five  args require 32 constructors.
   // Implement when needed.
};

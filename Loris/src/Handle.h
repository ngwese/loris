// ===========================================================================
//	Handle.h
//	
//	Adapted from Vladimir Batov's Handle class by Kelly Fitz for
//	use in the Loris class library. Only class Counted in file
//	counted.h was changed substatially. Also, added Handle<> to 
//	the Loris namespace.
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

#ifndef BATOV_HANDLE_H
#define BATOV_HANDLE_H

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

class HandleBase
{
   protected:
   
   typedef int Counter;

   // All Handle::null() instances point at the counter. It is
   // initialized with the value of 1. Therefore, it is never 0
   // and, consequently, there are no attempts to delete it.

   static Counter _null;
};

template<class Data>
class Handle : public HandleBase
{
#include "counted.h"   

   public:

   // Direct access.

   operator Data& () const { return _counted->operator Data&(); }
   operator Data* () const { return _counted->operator Data*(); }
   Data* operator->() const { return _counted->operator->(); }

  ~Handle() { _counted->dismiss(); }
  
   explicit Handle()
   : _counted(new Counted()) { _counted->use(); }

   Handle(const Handle<Data>& ref) 
   : _counted(ref._counted)  { _counted->use(); }

#include "create.h"
#include "unofficial.h"

   // The following partial specializations are needed to
   // support polymorphic behavior like:
   //       Handle<Derived> dh;
   //       Handle<Base> bh(dh);
   //       Handle<Base> bh = dh;
   // and override default behavior of more general 
   // "unofficial" Handle(Arg1) constructor.

   template<class Other>
   Handle(const Handle<Other>& ref) 
   : _counted(ref.cast<Data>()._counted)
   {
      _counted->use();
   }
  
   template<class Other>
   Handle(Handle<Other>& ref) 
   : _counted(ref.cast<Data>()._counted)
   {
      _counted->use();
   }

   Handle(Handle<Data>& ref) 
   : _counted(ref._counted)
   {
      _counted->use();
   }

   // Assignments

   Handle<Data>& 
   operator=(const Handle<Data>& src)
   {
      if (this->_counted != src._counted)
      {
         _counted->dismiss();
         _counted = src._counted;
         _counted->use();
      }
      return *this;
   }

   template<class Other>
   Handle<Data>& 
   operator=(const Handle<Other>& src)
   {
      return operator=(src.cast<Data>());
   }

   // Static cast:
   // from Handle<Derived> to Handle<Base>
   // from Handle<Data> to Handle<const Data>
   // etc.

   template<class Other>
   Handle<Other>&
   cast()
   {
      return (_cast_test<Other>(), *(Handle<Other>*) this);
   }
   template<class Other>
   const Handle<Other>&
   cast() const
   {
      return (_cast_test<Other>(), *(Handle<Other>*) this);
   }

   // Dynamic downcast: from Handle<Base> to Handle<Derived>

   template<class Other>
   const Handle<Other>&
   dyn_cast() const
   {
      _counted->dyn_cast<Other>(); //test
      return *(Handle<Other>*) this;
   }

   // Special instance to represent unassigned pointer.
  
   static 
   Handle<Data>
   null()
   {
      return Handle<Data>((Counted*) &_null);
   }

   private:

   Counted* _counted;

   Handle(Counted* counted) : _counted(counted) { _counted->use(); }

   template<class Other>
   Other* 
   _cast_test() const { return (Data*) 0; }
};

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif // BATOV_HANDLE_H

// Copyright (c) 1996-2000 by Vladimir Batov. Permission to  use,
// copy, modify,  distribute  and  sell  this  software  and  its
// documentation for any purpose is hereby granted  without  fee,
// provided that the above copyright notice appear in all  copies
// and that both that copyright notice and this permission notice
// appear in supporting documentation. I make no  representations
// about the suitability of this software for any purpose. It  is
// provided "as is" without express or implied warranty.

#ifndef HANDLE_CREATE_H
#define HANDLE_CREATE_H

// No args: Handle<Data> = Handle<Data>::create();

static
Handle<Data>
create()
{
   return new Counted();
}

// One arg: Handle<Data> = Handle<Data>::create(arg1);

template<class Arg1>
static
Handle<Data>
create(Arg1& arg1)
{
   return new Counted(arg1);
}

template<class Arg1>
static
Handle<Data>
create(const Arg1& arg1)
{
   return new Counted(arg1);
}

// Two args: Handle<Data> = Handle<Data>::create(arg1, arg2);

#define TEMPLATE template<class Arg1, class Arg2>
#define CREATE(Arg1, Arg2)                   \
   static                                    \
   Handle<Data>                              \
   create(Arg1& arg1, Arg2& arg2)            \
   {                                         \
      return new Counted(arg1, arg2);  \
   }

TEMPLATE CREATE(const Arg1, const Arg2)
TEMPLATE CREATE(const Arg1,       Arg2)
TEMPLATE CREATE(      Arg1, const Arg2)
TEMPLATE CREATE(      Arg1,       Arg2)

#undef TEMPLATE
#undef CREATE

// Three args require 8 functions.

#define TEMPLATE template<class Arg1, class Arg2, class Arg3>
#define CREATE(Arg1, Arg2, Arg3)                   \
   static                                          \
   Handle<Data>                                    \
   create(Arg1& arg1, Arg2& arg2, Arg3& arg3)      \
   {                                               \
      return new Counted(arg1, arg2, arg3);  \
   }

TEMPLATE CREATE(const Arg1, const Arg2, const Arg3)
TEMPLATE CREATE(const Arg1, const Arg2,       Arg3)
TEMPLATE CREATE(const Arg1,       Arg2, const Arg3)
TEMPLATE CREATE(const Arg1,       Arg2,       Arg3)
TEMPLATE CREATE(      Arg1, const Arg2, const Arg3)
TEMPLATE CREATE(      Arg1, const Arg2,       Arg3)
TEMPLATE CREATE(      Arg1,       Arg2, const Arg3)
TEMPLATE CREATE(      Arg1,       Arg2,       Arg3)

#undef TEMPLATE
#undef CREATE

// Four  args require 16 functions.

#define TEMPLATE template<class Arg1, class Arg2, class Arg3, class Arg4>
#define CREATE(Arg1, Arg2, Arg3, Arg4)                      \
   static                                                   \
   Handle<Data>                                             \
   create(Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4)   \
   {                                                        \
      return new Counted(arg1, arg2, arg3, arg4);     \
   }

TEMPLATE CREATE(const Arg1, const Arg2, const Arg3, const Arg4)
TEMPLATE CREATE(const Arg1, const Arg2, const Arg3,       Arg4)
TEMPLATE CREATE(const Arg1, const Arg2,       Arg3, const Arg4)
TEMPLATE CREATE(const Arg1, const Arg2,       Arg3,       Arg4)
TEMPLATE CREATE(const Arg1,       Arg2, const Arg3, const Arg4)
TEMPLATE CREATE(const Arg1,       Arg2, const Arg3,       Arg4)
TEMPLATE CREATE(const Arg1,       Arg2,       Arg3, const Arg4)
TEMPLATE CREATE(const Arg1,       Arg2,       Arg3,       Arg4)
TEMPLATE CREATE(      Arg1, const Arg2, const Arg3, const Arg4)
TEMPLATE CREATE(      Arg1, const Arg2, const Arg3,       Arg4)
TEMPLATE CREATE(      Arg1, const Arg2,       Arg3, const Arg4)
TEMPLATE CREATE(      Arg1, const Arg2,       Arg3,       Arg4)
TEMPLATE CREATE(      Arg1,       Arg2, const Arg3, const Arg4)
TEMPLATE CREATE(      Arg1,       Arg2, const Arg3,       Arg4)
TEMPLATE CREATE(      Arg1,       Arg2,       Arg3, const Arg4)
TEMPLATE CREATE(      Arg1,       Arg2,       Arg3,       Arg4)

#undef TEMPLATE
#undef CREATE

// Five  args require 32 functions.
// Implement more args when needed.

#endif // HANDLE_CREATE_H

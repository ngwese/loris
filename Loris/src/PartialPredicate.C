// ===========================================================================
//	PartialPredicate.C
//	
//	Base class for function objects that are unary preidcates on Partials.
//	This class is used to build algorithms in the style of STL without 
//	gratuitous templatization when the algorithms are not really generic 
//	(like STL algorirthms) but are specific to Partials.
//
//	-kel 25 Oct 99
//
// ===========================================================================

#include "LorisLib.h"
#include "PartialPredicate.h"

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	True
// ---------------------------------------------------------------------------
//	Trivial predicate that always returns true.
//
const PartialPredicate & 
PartialPredicate::True( void )
{
	struct _alwaysTrue : public PartialPredicate 
	{
		virtual boolean operator() ( const Partial & p ) const 
		{
			return true;
		}
	};
	
	static const _alwaysTrue _t;
	
	return _t;
}

// ---------------------------------------------------------------------------
//	False
// ---------------------------------------------------------------------------
//	Trivial predicate that always returns false.
//
const PartialPredicate & 
PartialPredicate::False( void )
{
	struct _alwaysFalse : public PartialPredicate 
	{
		virtual boolean operator() ( const Partial & p ) const 
		{
			return false;
		}
	};
	
	static const _alwaysFalse _f;
	
	return _f;
}


End_Namespace( Loris )

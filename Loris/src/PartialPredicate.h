#ifndef __Loris_partial_predicate__
#define __Loris_partial_predicate__

// ===========================================================================
//	PartialPredicate.h
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
#include "Partial.h"

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class PartialPredicate
//
//	Abstract base class, derived classes must implement operator ().
//
class PartialPredicate
{
public:
	//	public interface:
	virtual boolean operator() ( const Partial & p ) const = 0;
	virtual ~PartialPredicate( void ) {}
	
	//	static members for trivial predicates:
	static const PartialPredicate & True( void );
	static const PartialPredicate & False( void );
	
};	//	end of class PartialPredicate

End_Namespace( Loris )

#endif	// ndef __Loris_partial_predicate__
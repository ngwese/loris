#ifndef __Loris_Init__
#define __Loris_Init__
// ===========================================================================
//  LorisInit.h
//
//  Definition of Loris::Init_.
//  This class does nothing, except initialize the Loris library in its
//  constructor (private, called only by instance) and finalize it in
//  its destructor.
//
//  A static const Init_ reference is created in every compilation that
//  includes LorisLib.h, and it is initialized with this static member.
//  No other instance of Init_ is possible, and Init_ has no functional
//  members.
//
//  -kel 4 Oct 99
//
// ===========================================================================

#include "LorisLib.h"

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//  class Init_
//
//	This should probably be usable as a base class
//	to provide for different initializations on 
//	difference systems, but its not there now.
//	Right now, the singleton is declared and constructed
//	in instance().
//
class Init_
{
//	construction and destruciton are protected:
protected:
	Init_( void );

public:
	~Init_( void );
	
//	Singleton access:
public:
	static const Init_ & instance( void );

};	//	end of class Init_	
		
End_Namespace( Loris )

#endif	// 	ndef __Loris_Init__

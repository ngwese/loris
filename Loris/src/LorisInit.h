#ifndef __Loris_Init__
#define __Loris_Init__
// ===========================================================================
//  LorisInit.h
//
//	Implementation of members of Loris::Init_.
//	This class does nothing, except initialize the Loris library in its
//	constructor and finalize it in its destructor. It is not thread safe,
//	perhaps, but it is re-entrant.
//
//	In particular, initLib() checks data sizes and reserves some memory
//	for recovery in low memory situations. 
//
//	Init__ can be a base class for systems that need more elaborate 
//	intialization or finalization. Derived classes can override initLib()
//	and/or finalLib() as necessary. 
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
//
//	
//
class Init_
{
public:
//	construction/initialization:
	Init_( void );

//	destruction/finalization:
//	(virtual to allow subclassing)
	virtual ~Init_( void );
		
private:
//	counter to prevent initialization and finalization
//	from happening more than once:
	static int _init_count;

//	library initialization/finalization:
	virtual void initLib( void ) throw();
	virtual void finalLib( void ) throw();
	
};	//	end of class Init_	
		
End_Namespace( Loris )

#endif	// 	ndef __Loris_Init__

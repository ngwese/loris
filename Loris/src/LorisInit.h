
#include "LorisLib.h"

Begin_Namespace( Loris )

struct Init_
{
	Init_( void );
	
	static const Init_ & instance( void );
};	
		
static const Init_ & yo = Init_::instance();
 
End_Namespace( Loris )
/* 
	FrequencyReference.h

	Author:			Kelly Fitz
	Description:	<describe the FrequencyReference class here>
*/

#ifndef FrequencyReference_H
#define FrequencyReference_H

#include <Partial.h>
#include <list>
#include <memory>

namespace Loris {

class BreakpointEnvelope;

class FrequencyReference
{
private:
	std::auto_ptr< BreakpointEnvelope > _env;
	
public:
	//	construction: 
	FrequencyReference( std::list<Partial>::const_iterator begin, 
						std::list<Partial>::const_iterator end, 
						double minFreq, double maxFreq );
	~FrequencyReference();

	//	BreakpointEnvelope access:
	BreakpointEnvelope & envelope( void ) { return *_env; }
	const BreakpointEnvelope & envelope( void ) const { return *_env; }
};

}	// namespace Loris

#endif	// FrequencyReference_H

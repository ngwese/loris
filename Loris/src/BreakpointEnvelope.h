#ifndef INCLUDE_BREAKPOINTENVELOPE_H
#define INCLUDE_BREAKPOINTENVELOPE_H
// ===========================================================================
//	BreakpointEnvelope.h
//	
//	BreakpointEnvelope specifyies a linear segment breakpoint function, 
//	and implements the Envelope interface. 
//
//	Should this class be almost fully-insulating (i.e. hide std::map<>)?
//
//	-kel 21 July 2000
//
// ===========================================================================
#include <map>

// ---------------------------------------------------------------------------
//	class BreakpointEnvelope
//
//	Linear-segment breakpoint function with infinite extension at each end.
//
class BreakpointEnvelope
{
//	-- instance variables --
	std::map< double, double > _breakpoints;

//	-- public interface --
public:
	virtual double valueAt( double x ) const;	
	
	BreakpointEnvelope( void );
	BreakpointEnvelope( const BreakpointEnvelope & );
	virtual ~BreakpointEnvelope( void );
	
	BreakpointEnvelope & operator= ( const BreakpointEnvelope & );

};	//	end of abstract class BreakpointEnvelope

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_BREAKPOINTENVELOPE_H
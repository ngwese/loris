#ifndef INCLUDE_BREAKPOINTENVELOPE_H
#define INCLUDE_BREAKPOINTENVELOPE_H
// ===========================================================================
//	BreakpointEnvelope.h
//	
//	BreakpointEnvelope specifyies a linear segment breakpoint function, 
//	and implements the Envelope interface. 
//
//	This class isn't insulating, clients that should be protected from 
//	the details should use the EnvelopeInterface.
//
//	-kel 21 July 2000
//
// ===========================================================================
#include <map>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

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
	//	Envelope interface:
	virtual double valueAt( double x ) const;	
	
	//	construction:
	BreakpointEnvelope( void );
	BreakpointEnvelope( const BreakpointEnvelope & );
	virtual ~BreakpointEnvelope( void );
	
	//	compiler-generated assignment is okay
	//	BreakpointEnvelope & operator= ( const BreakpointEnvelope & );

	//	envelope composition:
	void insertBreakpoint( double x, double y );

};	//	end of abstract class BreakpointEnvelope

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_BREAKPOINTENVELOPE_H

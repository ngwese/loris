#ifndef __Loris_analyzer__
#define __Loris_analyzer__
// ===========================================================================
//	Analyzer.h
//	
//	Class definition for Loris::Analyzer.
//
//	-kel 5 Dec 99
//
// ===========================================================================
#include "LorisLib.h"
#include "Partial.h"
#include "ReassignedSpectrum.h"

#include <vector>
#include <memory>

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class Analyzer
//	
class Analyzer : public PartialCollector
{
//	-- public interface --
public:
//	construction:
	Analyzer( void );
	~Analyzer( void );

//	analysis:
	void analyze( std::vector< double > & buf, double srate );
	
	void formPartials( double frameTime );
	void spawnPartial( double time, const Breakpoint & bp );
	
	double hopSize( void ) const;	//	return seconds
	
//	-- instance variables --
private:
	std::auto_ptr< ReassignedSpectrum > _spectrum;
};	//	end of class Analyzer

End_Namespace( Loris )

#endif	// ndef __Loris_analyzer__

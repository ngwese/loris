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
#include "Partial.h"	//	for base class
#include <vector>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif


class Breakpoint;
class AnalyzerState;

// ---------------------------------------------------------------------------
//	class Analyzer
//	
class Analyzer : public PartialCollector
{
//	-- analysis parameters --
	double _resolution;	//	in Hz, minimum instantaneous frequency distance;
						//	this is the core parameter, others are, by default,
						//	computed from this one
	double _floor;		//	dB, relative to full amplitude sine wave, absolute
						//	amplitude threshold
	double _windowWidth;//	in Hz, width of main lobe; this might be more
						//	conveniently presented as window length, but
						//	the main lobe width more explicitly highlights
						//	the critical interaction with _resolution
	double _minFrequency;	//	lowest frequency component extracted
							//	in spectral analysis
	double _hop;		//	in seconds, time between analysis windows in
						//	successive spectral analyses
	double _bwRegionWidth;	//	width in Hz of overlapping bandwidth 
							//	association regions
							
//	-- public interface --
public:
//	construction:
	explicit Analyzer( double resolutionHz );
	~Analyzer( void );

//	analysis:
	void analyze( const std::vector< double > & buf, double srate, double offset = 0. );
	
//	configuration:
	void configure( double resolutionHz );
	
//	parameter access:
	double freqResolution( void ) const { return _resolution; }
	double ampFloor( void ) const { return _floor; }
	double windowWidth( void ) const { return _windowWidth; }
	double freqFloor( void ) const { return _minFrequency; }
	double hopTime( void ) const { return _hop; }
	double bwRegionWidth( void ) const { return _bwRegionWidth; }
	
//	parameter mutation:
	void setFreqResolution( double x ) { _resolution = x; }
	void setAmpFloor( double x ) { _floor = x; }
	void setWindowWidth( double x ) { _windowWidth = x; }
	void setFreqFloor( double x ) { _minFrequency = x; }
	void setHopTime( double x ) { _hop = x; }
	void setBwRegionWidth( double x ) { _bwRegionWidth = x; }	

//	-- internal helpers --
//	Should these be completely hidden? They all only access public 
//	members of Analyzer, so they could be redefined static to 
//	the implementation file and accept the Analyzer as an argument.
private:
	void extractPeaks( std::list< Breakpoint > & frame, double frameTime, AnalyzerState & state );
	void formPartials( std::list< Breakpoint > & frame, double frameTime, AnalyzerState & state );
	void thinPeaks( std::list< Breakpoint > & frame );
	void pruneBogusPartials( AnalyzerState & state );
	void spawnPartial( double time, const Breakpoint & bp );
	
//	-- unimplemented --
	Analyzer( const Analyzer & other );
	Analyzer  & operator = ( const Analyzer & rhs );
};	//	end of class Analyzer

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef __Loris_analyzer__

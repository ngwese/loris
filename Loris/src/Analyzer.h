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
#include "AssociateBandwidth.h"
#include "notifier.h"
#include <list>
#include <vector>
#include <memory>
#include <map>

Begin_Namespace( Loris )

class Breakpoint;

// ---------------------------------------------------------------------------
//	class Analyzer
//	
class Analyzer : public PartialCollector
{
//	-- public interface --
public:
//	construction:
	explicit Analyzer( double resolutionHz );
	~Analyzer( void );

//	analysis:
	void analyze( const std::vector< double > & buf, double srate );
	
//	configuration:
	void configure( double resolutionHz );
	
//	parameter access:
	double resolution( void ) const { return _resolution; }
	double floor( void ) const { return _floor; }
	double windowWidth( void ) const { return _windowWidth; }
	double minFrequency( void ) const { return _minFrequency; }
	double frameLength( void ) const { return _frameLength; }
	double bwRegionWidth( void ) const { return _bwRegionWidth; }
	
//	parameter mutation:
	void setResolution( double x ) { _resolution = x; }
	void setFloor( double x ) { _floor = x; }
	void setWindowWidth( double x ) { _windowWidth = x; }
	void setMinFrequency( double x ) { _minFrequency = x; }
	void setFrameLength( double x ) { _frameLength = x; }
	void setBwRegionWidth( double x ) { _bwRegionWidth = x; }	
/*
//	Lemur parameter specfication:
	void setPartialSeparation( double x ) { _freqResolution = x; }
	void setNoiseFloor( double x ) { _noiseFloor = x; }
	void setWindowWidth( double x ) { _windowWidth = x; }
	void setWindowAttenuation( double x ) { _windowAtten = x; }

//	parameter access, internal:
	long hopSize( void ) const			//	samples
		{ return _hop; }
	double frameLength( void ) const	//	seconds
		{ return _hop / _srate; }
	double sampleRate( void ) const
		{ return _srate; }
	double partialSeparation( void ) const
		{ return _freqResolution; }
	double noiseFloor( void ) const	//	(negative) dB
		{ return _noiseFloor; }
	double captureRange( void ) const	//	Hz
		{ return 0.5 * _freqResolution; }
	double frameTime( void ) const  		//	the time in seconds corresponding to the 
		{ return _winMiddleIdx	/ _srate; }	//	center of the current analysis window
*/
//	-- internal helpers --
private:
	typedef std::list< Breakpoint > Frame;
	
	//	construct spectrum analyzer and analysis window:
	void createSpectrum( double srate );
	
	void extractPeaks( Frame & frame );
	void thinPeaks( Frame & frame );
	
	//	Partial construction:
	void formPartials( Frame & frame );
	void spawnPartial( double time, const Breakpoint & bp );
	void pruneBogusPartials( void );
	
//	-- instance variables --
private:
	std::auto_ptr< ReassignedSpectrum > _spectrum;
	std::auto_ptr< AssociateBandwidth > _bw;
	
	std::map< double, double > _peakTimeCache;	//	yuck
/*	
	//	parameters (from Lemur):
	double _freqResolution;		//	minimum frequency distance (Hz) between Partials
	double _noiseFloor;			//	magnitude-spectral peak detection threshold
	double _windowWidth, _windowAtten; 	//	window parameters
	
	//	state variables:
	double _srate;
	double _minfreq;	//	lowest frequency the fits two periods in the analysis window
	long _hop;			//	in samples
	long _winMiddleIdx;
*/	
	//	parameters:
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
							//	yucky name
	double _frameLength;//	in seconds, time between analysis windows in
						//	successive spectral analyses
	double _bwRegionWidth;	//	width in Hz of overlapping bandwidth 
							//	association regions
};	//	end of class Analyzer

End_Namespace( Loris )

#endif	// ndef __Loris_analyzer__

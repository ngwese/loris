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
#include <list>
#include <vector>
#include <memory>

class Breakpoint;

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
	void analyze( const std::vector< double > & buf, double srate );

//	Lemur parameter specfication:
	void setPartialSeparation( double x ) { _freqResolution = x; }
	void setNoiseFloor( double x ) { _noiseFloor = x; }
	void setWindowWidth( double x ) { _windowWidth = x; }
	void setWindowAttenuation( double x ) { _windowAtten = x; }

//	parameter access, internal:
	long hopSize( void ) const			//	samples
		{ return _hop; }
	double frameLength( void ) const	//	seconds
		{ return _hop * _srate; }
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

//	-- internal helpers --
private:
	//	auxiliary class representing spectral peaks, 
	//	extending the Breakpoint class to include some 
	//	extra data needed only for analysis:
	class Peak : public Breakpoint
	{
	public:
		//	construction:
		Peak( double f, double a, double b, double p, double t ) : 
			_time( t ), Breakpoint( f, a, b, p ) {}
		Peak( void ) : _time( 0. ), Breakpoint() {}
		//	access:
		double time( void ) const { return _time; }
		void setTime( double x ) { _time = x; }
	private:
		double _time; 	//	in seconds
	};	//	end of class Peak
	
	typedef std:: list< Peak > Frame;
	
	//	construct spectrum analyzer and analysis window:
	void createSpectrum( double srate );
	
	void extractPeaks( Frame & frame );
	void thinPeaks( Frame & frame );
	
	//	Partial construction:
	void formPartials( Frame & frame );
	void spawnPartial( double time, const Breakpoint & bp );
	
//	-- instance variables --
private:
	std::auto_ptr< ReassignedSpectrum > _spectrum;
	AssociateBandwidth _bw;
	
	//	parameters (from Lemur):
	double _freqResolution;		//	minimum frequency distance (Hz) between Partials
	double _noiseFloor;			//	magnitude-spectral peak detection threshold
	double _windowWidth, _windowAtten; 	//	window parameters
	
	//	state variables:
	double _srate;
	double _minfreq;	//	lowest frequency the fits two periods in the analysis window
	long _hop;			//	in samples
	long _winMiddleIdx;
};	//	end of class Analyzer

End_Namespace( Loris )

#endif	// ndef __Loris_analyzer__

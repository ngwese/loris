#ifndef INCLUDE_ANALYZER_H
#define INCLUDE_ANALYZER_H
/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * Analyzer.h
 *
 * Definition of class Loris::Analyzer.
 *
 * Kelly Fitz, 5 Dec 99
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "Partial.h"
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
class Analyzer
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
	double _minFrequency;	//	lowest frequency (Hz) component extracted
							//	in spectral analysis
	double _drift;		//	the maximum frequency (Hz) difference between two 
						//	consecutive Breakpoints that will be linked to
						//	form a Partial
	double _hop;		//	in seconds, time between analysis windows in
						//	successive spectral analyses
	double _cropTime;	//	in seconds, maximum time correction for a spectral
						//	component to be considered reliable, and to be eligible
						//	for extraction and for Breakpoint formation
	double _bwRegionWidth;	//	width in Hz of overlapping bandwidth 
							//	association regions
							
	PartialList _partials;	//	collect Partials here
			
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
	double freqResolution( void ) const { return _resolution; }
	double ampFloor( void ) const { return _floor; }
	double windowWidth( void ) const { return _windowWidth; }
	double freqFloor( void ) const { return _minFrequency; }
	double hopTime( void ) const { return _hop; }
	double freqDrift( void ) const { return _drift;}
	double cropTime( void ) const { return _cropTime; }
	double bwRegionWidth( void ) const { return _bwRegionWidth; }
	
//	parameter mutation:
	void setFreqResolution( double x ) { _resolution = x; }
	void setAmpFloor( double x ) { _floor = x; }
	void setWindowWidth( double x ) { _windowWidth = x; }
	void setFreqFloor( double x ) { _minFrequency = x; }
	void setFreqDrift( double x ) { _drift = x; }
	void setHopTime( double x ) { _hop = x; }
	void setCropTime( double x ) { _cropTime = x; }
	void setBwRegionWidth( double x ) { _bwRegionWidth = x; }	

//	PartialList access:
	PartialList & partials( void ) { return _partials; }
	const PartialList & partials( void ) const { return _partials; }

//	-- internal helpers --
//	Should these be completely hidden? They all only access public 
//	members of Analyzer, so they could be redefined static to 
//	the implementation file and accept the Analyzer as an argument.
private:
	void extractPeaks( std::list< Breakpoint > & frame, double frameTime, AnalyzerState & state );
	void formPartials( std::list< Breakpoint > & frame, double frameTime, AnalyzerState & state );
	void thinPeaks( std::list< Breakpoint > & frame, AnalyzerState & state );
	void pruneBogusPartials( AnalyzerState & state );
	void spawnPartial( double time, const Breakpoint & bp );
	
//	-- unimplemented --
	Analyzer( const Analyzer & other );
	Analyzer  & operator = ( const Analyzer & rhs );
};	//	end of class Analyzer

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_ANALYZER_H

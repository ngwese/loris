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
 *	lorisAnalyzer_pi.C
 *
 *	A component of the C-linkable procedural interface for Loris. 
 *
 *	Main components of the Loris procedural interface:
 *	- object interfaces - Analyzer, Synthesizer, Partial, PartialIterator, 
 *		PartialList, PartialListIterator, Breakpoint, BreakpointEnvelope,  
 *		and SampleVector need to be (opaque) objects in the interface, 
 * 		either because they hold state (e.g. Analyzer) or because they are 
 *		fundamental data types (e.g. Partial), so they need a procedural 
 *		interface to their member functions. All these things need to be 
 *		opaque pointers for the benefit of C.
 *	- non-object-based procedures - other classes in Loris are not so stateful,
 *		and have sufficiently narrow functionality that they need only 
 *		procedures, and no object representation.
 *	- utility functions - some procedures that are generally useful but are
 *		not yet part of the Loris core are also defined.
 *	- notification and exception handlers - all exceptions must be caught and
 *		handled internally, clients can specify an exception handler and 
 *		a notification function (the default one in Loris uses printf()).
 *
 *	This file contains the procedural interface for the Loris Analyzer class.
 *
 * Kelly Fitz, 10 Nov 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#define LORIS_OPAQUE_POINTERS 0
#include "loris.h"
#include "lorisException_pi.h"

#include "Analyzer.h"
#include "notifier.h"

using namespace Loris;

/* ---------------------------------------------------------------- */
/*		Analyzer object interface
/*
/*	An Analyzer represents a configuration of parameters for
	performing Reassigned Bandwidth-Enhanced Additive Analysis
	of sampled waveforms. This analysis process yields a collection 
	of Partials, each having a trio of synchronous, non-uniformly-
	sampled breakpoint envelopes representing the time-varying 
	frequency, amplitude, and noisiness of a single bandwidth-
	enhanced sinusoid. 

	For more information about Reassigned Bandwidth-Enhanced 
	Analysis and the Reassigned Bandwidth-Enhanced Additive Sound 
	Model, refer to the Loris website: www.cerlsoundgroup.org/Loris/.

	In C++, an Analyzer * is a Loris::Analyzer *.
 */

/* ---------------------------------------------------------------- */
/*        createAnalyzer
/*
/*	Construct and return a new Analyzer configured with the given	
	frequency resolution (minimum instantaneous frequency	
	difference between Partials). All other Analyzer parameters 	
	are computed from the specified frequency resolution. 			
 */
extern "C"
Analyzer * createAnalyzer( double resolution )
{
	try 
	{
		debugger << "creating Analyzer" << endl;
		return new Analyzer( resolution );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in createAnalyzer(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in createAnalyzer(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return NULL;
}

/* ---------------------------------------------------------------- */
/*        destroyAnalyzer        
/*
/*	Destroy this Analyzer. 								
 */
extern "C"
void destroyAnalyzer( Analyzer * ptr_this )
{
	try 
	{
		ThrowIfNull((Analyzer *) ptr_this);
		
		debugger << "deleting Analyzer" << endl;
		delete ptr_this;
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in destroyAnalyzer(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in destroyAnalyzer(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        Analyzer_copy                                             
/*
/*	Not allowed yet in Analyzer interface, could implement here 
	by copying parameters.
 */ 
/*
extern "C"
void Analyzer_copy( Analyzer * dst, const Analyzer * src )
{
	try 
	{
		debugger << "copying Analyzer" << endl;
		*dst = *src;
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in Analyzer_copy(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in Analyzer_copy(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}
*/
/* ---------------------------------------------------------------- */
/*        analyzer_analyze        
/*
/*	Analyze a vector of (mono) samples at the given sample rate 	  	
	(in Hz) and append the extracted Partials to the given 
	PartialList. 												
 */
extern "C"
void analyzer_analyze( Analyzer * ptr_this, 
					   const SampleVector * vec, double srate,
					   PartialList * partials )
{
	try 
	{
		ThrowIfNull((Analyzer *) ptr_this);
		ThrowIfNull((SampleVector *) vec);
		ThrowIfNull((PartialList *) partials);
		
		//	perform analysis:
		notifier << "analyzing " << vec->size() << " samples at " <<
					srate << " Hz with frequency resolution " <<
					ptr_this->freqResolution() << endl;
		ptr_this->analyze( vec->begin(), vec->end(), srate );
		
		//	splice the Partials into the destination list:
		partials->splice( partials->end(), ptr_this->partials() );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in analyzer_analyze(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in analyzer_analyze(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        analyzer_configure        
/*
/*	Configure this Analyzer with the given frequency resolution 
	(minimum instantaneous frequency difference between Partials). 
	All other Analyzer parameters are (re-)computed from the 
	frequency resolution. 		
 */
extern "C"
void analyzer_configure( Analyzer * ptr_this, double resolution )
{
	try  
	{
		ThrowIfNull((Analyzer *) ptr_this);
		ptr_this->configure( resolution );
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in analyzer_configure(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in analyzer_configure(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        analyzer_getFreqResolution        
/*
/*	Return the frequency resolution (minimum instantaneous frequency  		
	difference between Partials) for this Analyzer. 	
 */
extern "C"
double analyzer_getFreqResolution( const Analyzer * ptr_this )
{
	try  
	{
		ThrowIfNull((Analyzer *) ptr_this);
		return ptr_this->freqResolution();
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in analyzer_getFreqResolution(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in analyzer_getFreqResolution(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        analyzer_setFreqResolution        
/*
/*	Set the frequency resolution (minimum instantaneous frequency  		
	difference between Partials) for this Analyzer. (Does not cause 	
	other parameters to be recomputed.) 									
 */
extern "C"
void analyzer_setFreqResolution( Analyzer * ptr_this, double x )
{
	try  
	{
		ThrowIfNull((Analyzer *) ptr_this);
		ptr_this->setFreqResolution( x );
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in analyzer_setFreqResolution(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in analyzer_setFreqResolution(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        analyzer_getAmpFloor        
/*
/*	Return the amplitude floor (lowest detected spectral amplitude),  			
	in (negative) dB, for this Analyzer. 				
 */
extern "C"
double analyzer_getAmpFloor( const Analyzer * ptr_this )
{
	try  
	{
		ThrowIfNull((Analyzer *) ptr_this);
		return ptr_this->ampFloor();
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in analyzer_getAmpFloor(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in analyzer_getAmpFloor(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        analyzer_setAmpFloor        
/*
/*	Set the amplitude floor (lowest detected spectral amplitude), in  			
	(negative) dB, for this Analyzer. 				
 */
extern "C"
void analyzer_setAmpFloor( Analyzer * ptr_this, double x )
{
	try  
	{
		ThrowIfNull((Analyzer *) ptr_this);
		ptr_this->setAmpFloor( x );
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in analyzer_setAmpFloor(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in analyzer_setAmpFloor(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        analyzer_getWindowWidth        
/*
/*	Return the frequency-domain main lobe width (measured between 
	zero-crossings) of the analysis window used by this Analyzer. 				
 */
extern "C"
double analyzer_getWindowWidth( const Analyzer * ptr_this )
{
	try  
	{
		ThrowIfNull((Analyzer *) ptr_this);
		return ptr_this->windowWidth();
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in analyzer_getWindowWidth(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in analyzer_getWindowWidth(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        analyzer_setWindowWidth        
/*
/*	Set the frequency-domain main lobe width (measured between 
	zero-crossings) of the analysis window used by this Analyzer. 				
 */
extern "C"
void analyzer_setWindowWidth( Analyzer * ptr_this, double x )
{
	try  
	{
		ThrowIfNull((Analyzer *) ptr_this);
		ptr_this->setWindowWidth( x );
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in analyzer_setWindowWidth(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in analyzer_setWindowWidth(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        analyzer_getFreqFloor        
/*
/*	Return the frequency floor (minimum instantaneous Partial  				
	frequency), in Hz, for this Analyzer. 				
 */
extern "C"
double analyzer_getFreqFloor( const Analyzer * ptr_this )
{
	try  
	{
		ThrowIfNull((Analyzer *) ptr_this);
		return ptr_this->freqFloor();
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in analyzer_getFreqFloor(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in analyzer_getFreqFloor(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        analyzer_setFreqFloor        
/*
/*	Set the amplitude floor (minimum instantaneous Partial  				
	frequency), in Hz, for this Analyzer.
 */
extern "C"
void analyzer_setFreqFloor( Analyzer * ptr_this, double x )
{
	try  
	{
		ThrowIfNull((Analyzer *) ptr_this);
		ptr_this->setFreqFloor( x );
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in analyzer_setFreqFloor(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in analyzer_setFreqFloor(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        analyzer_getFreqDrift        
/*
/*	Return the maximum allowable frequency difference between 					
	consecutive Breakpoints in a Partial envelope for this Analyzer. 				
 */
extern "C"
double analyzer_getFreqDrift( const Analyzer * ptr_this )
{
	try  
	{
		ThrowIfNull((Analyzer *) ptr_this);
		return ptr_this->freqDrift();
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in analyzer_getFreqDrift(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in analyzer_getFreqDrift(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        analyzer_setFreqDrift        
/*
/*	Set the maximum allowable frequency difference between 					
	consecutive Breakpoints in a Partial envelope for this Analyzer. 				
 */
extern "C"
void analyzer_setFreqDrift( Analyzer * ptr_this, double x )
{
	try  
	{
		ThrowIfNull((Analyzer *) ptr_this);
		ptr_this->setFreqDrift( x );
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in analyzer_setFreqDrift(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in analyzer_setFreqDrift(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        analyzer_getHopTime        
/*
/*	Return the hop time (which corresponds approximately to the 
	average density of Partial envelope Breakpoint data) for this 
	Analyzer.
 */
extern "C"
double analyzer_getHopTime( const Analyzer * ptr_this )
{
	try  
	{
		ThrowIfNull((Analyzer *) ptr_this);
		return ptr_this->hopTime();
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in analyzer_getHopTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	 
	{
		std::string s("std C++ exception in analyzer_getHopTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        analyzer_setHopTime        
/*
/*	Set the hop time (which corresponds approximately to the average
	density of Partial envelope Breakpoint data) for this Analyzer.
 */
extern "C"
void analyzer_setHopTime( Analyzer * ptr_this, double x )
{
	try 
	{
		ThrowIfNull((Analyzer *) ptr_this);
		ptr_this->setHopTime( x );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in analyzer_setHopTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in analyzer_setHopTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}
/* ---------------------------------------------------------------- */
/*        analyzer_getCropTime        
/*
/*	Return the crop time (maximum temporal displacement of a time-
	frequency data point from the time-domain center of the analysis
	window, beyond which data points are considered "unreliable")
	for this Analyzer.
 */
extern "C"
double analyzer_getCropTime( const Analyzer * ptr_this )
{
	try 
	{
		ThrowIfNull((Analyzer *) ptr_this);
		return ptr_this->cropTime();
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in analyzer_getCropTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in analyzer_getCropTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        analyzer_setCropTime        
/*
/*	Set the crop time (maximum temporal displacement of a time-
	frequency data point from the time-domain center of the analysis
	window, beyond which data points are considered "unreliable")
	for this Analyzer.
 */
extern "C"
void analyzer_setCropTime( Analyzer * ptr_this, double x )
{
	try 
	{
		ThrowIfNull((Analyzer *) ptr_this);
		ptr_this->setCropTime( x );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in analyzer_setCropTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in analyzer_setCropTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}



/* ---------------------------------------------------------------- */
/*        analyzer_getBwRegionWidth        
/*
/*	Return the width (in Hz) of the Bandwidth Association regions
	used by this Analyzer.
 */
extern "C"
double analyzer_getBwRegionWidth( const Analyzer * ptr_this )
{
	try 
	{
		ThrowIfNull((Analyzer *) ptr_this);
		return ptr_this->bwRegionWidth();
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in analyzer_getBwRegionWidth(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in analyzer_getBwRegionWidth(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        analyzer_setBwRegionWidth        
/*
/*	Set the width (in Hz) of the Bandwidth Association regions
	used by this Analyzer.
 */
extern "C"
void analyzer_setBwRegionWidth( Analyzer * ptr_this, double x )
{
	try 
	{
		ThrowIfNull((Analyzer *) ptr_this);
		ptr_this->setBwRegionWidth( x );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in analyzer_setBwRegionWidth(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in analyzer_setBwRegionWidth(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}


#ifndef INCLUDE_LORIS_H
#define INCLUDE_LORIS_H
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
 *	loris.h
 *
 *	Header specifying C-linkable procedural interface for Loris. 
 *
 *	Main components of this interface:
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
 * Kelly Fitz, 2 Nov 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 
/* ---------------------------------------------------------------- */
/*		object type declarations
/*
/*	The following opaque pointer types are exposed procedurally 
	through this interface. Loris types and classes are all in the 
	C++ namespace Loris. Collections from the standard namespace 
	(namespace std) are also used.
	
	The C++ version is needed if this header is included in a 
	source file that also includes other Loris headers (where 
	those classes are defined). If only the procedural interface 
	is used, do not include the other Loris headers. If the Loris
	class interfaces are also used, then define the symbol 
	LORIS_OPAQUE_POINTERS to be false in the source file before
	including this header.
 */
#ifndef LORIS_OPAQUE_POINTERS
#define LORIS_OPAQUE_POINTERS 1
#endif
#if defined(__cplusplus) && ! LORIS_OPAQUE_POINTERS
	//	declare Loris classes:
	namespace Loris
	{
		class Analyzer;
		class BreakpointEnvelope;
		class Partial;
	}
	//	include std library headers, declaring template classes
	//	is too painful and fragile:
	#include <list>
	#include <vector>
	//	define types used in procedural interface:
	typedef Loris::Analyzer * Analyzer_Ptr;
	typedef Loris::BreakpointEnvelope * BreakpointEnvelope_Ptr;
	typedef std::list< Loris::Partial > * PartialList_Ptr;
	typedef std::vector< double > * SampleVector_Ptr;
#else /* no classes, just opaque C pointers */
	typedef struct Analyzer * Analyzer_Ptr;
	typedef struct BreakpointEnvelope * BreakpointEnvelope_Ptr;
	typedef struct PartialList * PartialList_Ptr;
	typedef struct SampleVector * SampleVector_Ptr;
#endif

#ifdef __cplusplus
	extern "C" {
#endif

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

	In C++, an Analyzer_Ptr is a Loris::Analyzer *.
 */
Analyzer_Ptr createAnalyzer( double resolution );
/*	Construct and return a new Analyzer configured with the given	
	frequency resolution (minimum instantaneous frequency	
	difference between Partials). All other Analyzer parameters 	
	are computed from the specified frequency resolution. 			
 */
void destroyAnalyzer( Analyzer_Ptr ptr_this );
/*	Destroy this Analyzer. 								
 */
void analyzer_analyze( Analyzer_Ptr ptr_this, 
					   const SampleVector_Ptr vec, double srate,
					   PartialList_Ptr partials );
/*	Analyze a vector of (mono) samples at the given sample rate 	  	
	(in Hz) and append the extracted Partials to the given 
	PartialList. 												
 */
	
void analyzer_configure( Analyzer_Ptr ptr_this, double resolution );
/*	Configure this Analyzer with the given frequency resolution 
	(minimum instantaneous frequency difference between Partials). 
	All other Analyzer parameters are (re-)computed from the 
	frequency resolution. 		
 */
double analyzer_getFreqResolution( const Analyzer_Ptr ptr_this );
/*	Return the frequency resolution (minimum instantaneous frequency  		
	difference between Partials) for this Analyzer. 	
 */
void analyzer_setFreqResolution( Analyzer_Ptr ptr_this, double x );
/*	Set the frequency resolution (minimum instantaneous frequency  		
	difference between Partials) for this Analyzer. (Does not cause 	
	other parameters to be recomputed.) 									
 */
double analyzer_getAmpFloor( const Analyzer_Ptr ptr_this );
/*	Return the amplitude floor (lowest detected spectral amplitude),  			
	in (negative) dB, for this Analyzer. 				
 */
void analyzer_setAmpFloor( Analyzer_Ptr ptr_this, double x );
/*	Set the amplitude floor (lowest detected spectral amplitude), in  			
	(negative) dB, for this Analyzer. 				
 */
double analyzer_getWindowWidth( const Analyzer_Ptr ptr_this );
/*	Return the frequency-domain main lobe width (measured between 
	zero-crossings) of the analysis window used by this Analyzer. 				
 */
void analyzer_setWindowWidth( Analyzer_Ptr ptr_this, double x );
/*	Set the frequency-domain main lobe width (measured between 
	zero-crossings) of the analysis window used by this Analyzer. 				
 */
double analyzer_getFreqFloor( const Analyzer_Ptr ptr_this );
/*	Return the frequency floor (minimum instantaneous Partial  				
	frequency), in Hz, for this Analyzer. 				
 */
void analyzer_setFreqFloor( Analyzer_Ptr ptr_this, double x );
/*	Set the amplitude floor (minimum instantaneous Partial  				
	frequency), in Hz, for this Analyzer.
 */
double analyzer_getFreqDrift( const Analyzer_Ptr ptr_this );
/*	Return the maximum allowable frequency difference between 					
	consecutive Breakpoints in a Partial envelope for this Analyzer. 				
 */
void analyzer_setFreqDrift( Analyzer_Ptr ptr_this, double x );
/*	Set the maximum allowable frequency difference between 					
	consecutive Breakpoints in a Partial envelope for this Analyzer. 				
 */
double analyzer_getHopTime( const Analyzer_Ptr ptr_this );
/*	Return the hop time (which corresponds approximately to the 
	average density of Partial envelope Breakpoint data) for this 
	Analyzer.
 */
void analyzer_setHopTime( Analyzer_Ptr ptr_this, double x );
/*	Set the hop time (which corresponds approximately to the average
	density of Partial envelope Breakpoint data) for this Analyzer.
 */
double analyzer_getCropTime( const Analyzer_Ptr ptr_this );
/*	Return the crop time (maximum temporal displacement of a time-
	frequency data point from the time-domain center of the analysis
	window, beyond which data points are considered "unreliable")
	for this Analyzer.
 */
void analyzer_setCropTime( Analyzer_Ptr ptr_this, double x );
/*	Set the crop time (maximum temporal displacement of a time-
	frequency data point from the time-domain center of the analysis
	window, beyond which data points are considered "unreliable")
	for this Analyzer.
 */
double analyzer_getBwRegionWidth( const Analyzer_Ptr ptr_this );
/*	Return the width (in Hz) of the Bandwidth Association regions
	used by this Analyzer.
 */
void analyzer_setBwRegionWidth( Analyzer_Ptr ptr_this, double x );
/*	Set the width (in Hz) of the Bandwidth Association regions
	used by this Analyzer.
 */

/* ---------------------------------------------------------------- */
/*		BreakpointEnvelope object interface								
/*
/*	A BreakpointEnvelope represents a linear segment breakpoint 
	function with infinite extension at each end (that is, the 
	values past either end of the breakpoint function have the 
	values at the nearest end).

	In C++, a BreakpointEnvelope_Ptr is a Loris::BreakpointEnvelope *.
 */
 
BreakpointEnvelope_Ptr createBreakpointEnvelope( void );
/*	Construct and return a new BreakpointEnvelope having no 
	breakpoints (and an implicit value of 0. everywhere).			
 */

void destroyBreakpointEnvelope( BreakpointEnvelope_Ptr ptr_this );
/*	Destroy this BreakpointEnvelope. 								
 */
 
void breakpointEnvelope_insertBreakpoint( BreakpointEnvelope_Ptr ptr_this,
					double time, double val );
/*	Insert a breakpoint representing the specified (time, value) 
	pair into this BreakpointEnvelope. If there is already a 
	breakpoint at the specified time, it will be replaced with 
	the new breakpoint.
 */

double breakpointEnvelope_valueAt( BreakpointEnvelope_Ptr ptr_this, 
								   double time );
/*	Return the interpolated value of this BreakpointEnvelope at the 
	specified time.							
 */

/* ---------------------------------------------------------------- */
/*		PartialList object interface
/*
/*	A PartialList represents a collection of Bandwidth-Enhanced 
	Partials, each having a trio of synchronous, non-uniformly-
	sampled breakpoint envelopes representing the time-varying 
	frequency, amplitude, and noisiness of a single bandwidth-
	enhanced sinusoid.

	For more information about Bandwidth-Enhanced Partials and the  
	Reassigned Bandwidth-Enhanced Additive Sound Model, refer to
	the Loris website: www.cerlsoundgroup.org/Loris/.

	In C++, a PartialList_Ptr is a std::list< Loris::Partial > *.
 */ 
PartialList_Ptr createPartialList( void );
/*	Return a new empty PartialList.
 */
void destroyPartialList( PartialList_Ptr ptr_this );
/*	Destroy this PartialList.
 */
void partialList_clear( const PartialList_Ptr ptr_this );
/*	Remove (and destroy) all the Partials from this PartialList,
	leaving it empty.
 */
void partialList_copy( PartialList_Ptr ptr_this, 
					   const PartialList_Ptr src );
/*	Make this PartialList a copy of the source PartialList by making
	copies of all of the Partials in the source and adding them to 
	this PartialList.
 */
unsigned long partialList_size( const PartialList_Ptr ptr_this );
/*	Return the number of Partials in this PartialList.
 */
void partialList_splice( PartialList_Ptr ptr_this, 
						 const PartialList_Ptr src );
/*	Splice all the Partials in the source PartialList onto the end of
	this PartialList, leaving the source empty.
 */

/* ---------------------------------------------------------------- */
/*		SampleVector object interface								
/*
/*	A SampleVector represents a vector of floating point samples of
	an audio waveform. In Loris, the samples are assumed to have 
	values on the range (-1., 1.) (though this is not enforced or 
	checked). 

	In C++, a SampleVector_Ptr is a std::vector< double > *.
 */
 
SampleVector_Ptr createSampleVector( unsigned long size );
/*	Return a new SampleVector having the specified number of samples,
	all of whose value is 0.
 */
void destroySampleVector( SampleVector_Ptr ptr_this );
/*	Destroy this SampleVector.
 */
void sampleVector_copy( SampleVector_Ptr ptr_this, 
						const SampleVector_Ptr src );
/*	Make this SampleVector a copy of the source SampleVector, having 
	the same number of samples, and samples at every position in this
	SampleVector having the same value as the corresponding sample in
	the source.
 */
double sampleVector_getAt( const SampleVector_Ptr ptr_this, unsigned long idx );
/*	Return the value of the sample at the given position (index) in
	this SampleVector.
 */
void sampleVector_setAt( SampleVector_Ptr ptr_this, unsigned long idx, double x );
/*	Set the value of the sample at the given position (index) in
	this SampleVector.
 */
unsigned long sampleVector_getLength( const SampleVector_Ptr ptr_this );
/*	Return the number of samples represented by this SampleVector.
 */
void sampleVector_setLength( SampleVector_Ptr ptr_this, unsigned long size );
/*	Change the number of samples represented by this SampleVector. If
	the given size is greater than the current size, all new samples 
	will have value 0. If the given size is less than the current 
	size, then samples in excess of the given size are removed.
 */

/* ---------------------------------------------------------------- */
/*		non-object-based procedures
/*
/*	Operations in Loris that need not be accessed though object
	interfaces are represented as simple functions.
 */

void channelize( PartialList_Ptr partials, 
				 BreakpointEnvelope_Ptr refFreqEnvelope, int refLabel );
/*	Label Partials in a PartialList with the integer nearest to
	the amplitude-weighted average ratio of their frequency envelope
	to a reference frequency envelope. The frequency spectrum is 
	partitioned into non-overlapping channels whose time-varying 
	center frequencies track the reference frequency envelope. 
	The reference label indicates which channel's center frequency
	is exactly equal to the reference envelope frequency, and other
	channels' center frequencies are multiples of the reference 
	envelope frequency divided by the reference label. Each Partial 
	in the PartialList is labeled with the number of the channel
	that best fits its frequency envelope. The quality of the fit
	is evaluated at the breakpoints in the Partial envelope and
	weighted by the amplitude at each breakpoint, so that high-
	amplitude breakpoints contribute more to the channel decision.
	Partials are labeled, but otherwise unmodified. In particular, 
	their frequencies are not modified in any way.
 */

void dilate( PartialList_Ptr partials, 
			 double * initial, double * target, int npts );
/*	Dilate Partials in a PartialList according to the given 
	initial and target time points. Partial envelopes are 
	stretched and compressed so that temporal features at
	the initial time points are aligned with the final time
	points. Time points are sorted, so Partial envelopes are 
	are only stretched and compressed, but breakpoints are not
	reordered. Duplicate time points are allowed. There must be
	the same number of initial and target time points.
 */

void distill( PartialList_Ptr partials );
/*	Distill labeled (channelized)  Partials in a PartialList into a 
	PartialList containing a single (labeled) Partial per label. 
	The distilled PartialList will contain as many Partials as
	there were non-zero labels in the original PartialList. Unlabeled 
	(label 0) Partials are eliminated.
 */

void exportAiff( const char * path, const SampleVector_Ptr vec, 
				 double samplerate, int nchannels, int bitsPerSamp );
/*	Export audio samples stored in a SampleVector to an AIFF file
	having the specified number of channels and sample rate at the 
	given file path (or name). The floating point samples in the 
	SampleVector are clamped to the range (-1.,1.) and converted 
	to integers having bitsPerSamp bits.
 */
				 
void exportSdif( const char * path, PartialList_Ptr partials, double hop );
/*	Export Partials in a PartialList to a SDIF file at the specified
	file path (or name). SDIF data is written in the 1TRC format.  
	For more information about SDIF, see the SDIF web site at:
		www.ircam.fr/equipes/analyse-synthese/sdif/  
		
	The hop parameter is currently used to specify a time-domain 
	resampling of the Partial envelopes. This parameter is 
	deprecated, and will be removed in future versions, which 
	will have explicit resampling functionality. If hop is 0., 
	Partial envelopes will not be resampled, and all Partial 
	envelope data will be stored.
 */

void importAiff( const char * path, SampleVector_Ptr vec, 
				 double * samplerate, int * nchannels );
/*	Import audio samples stored in an AIFF file at the given file
	path (or name). The samples are converted to floating point 
	values on the range (-1.,1.) and stored in the given 
	SampleVector, which is resized to (exactly) accomodate all the 
	samples from in the file. If samplerate is not a NULL pointer, 
	then, on return, it points to the value of the sample rate (in
	Hz) of the AIFF samples. Similarly, if nchannels is not a NULL
	pointer, then, on return, it points to the value of the number
	of channels of audio data represented by the AIFF samples.
 */

void importSdif( const char * path, PartialList_Ptr partials );
/*	Import Partials from an SDIF file at the given file path (or 
	name), and append them to a PartialList.
 */	

void morph( const PartialList_Ptr src0, const PartialList_Ptr src1, 
			const BreakpointEnvelope_Ptr ffreq, 
			const BreakpointEnvelope_Ptr famp, 
			const BreakpointEnvelope_Ptr fbw, 
			PartialList_Ptr dst );
/*	Morph labeled Partials in two PartialLists according to the
	given frequency, amplitude, and bandwidth (noisiness) morphing
	envelopes, and append the morphed Partials to the destination 
	PartialList. Loris morphs Partials by interpolating frequency,
	amplitude, and bandwidth envelopes of corresponding Partials in 
	the source PartialLists. For more information about the Loris
	morphing algorithm, see the Loris website: 
	www.cerlsoundgroup.org/Loris/
 */

void synthesize( const PartialList_Ptr partials,
				 SampleVector_Ptr samples,  
				 double srate );
/*	Synthesize Partials in a PartialList at the given sample
	rate, and store the (floating point) samples in a SampleVector.
	The SampleVector is resized, if necessary, to hold as many
	samples as are needed for the complete synthesis of all the
	Partials in the PartialList. The SampleVector is not 
	cleared before synthesis, so newly synthesized samples are
	added to any previously computed samples in the SampleVector.
 */

/* ---------------------------------------------------------------- */
/*		utility functions
/*
/*	These procedures are generally useful but are not yet  
	represented by classes in the Loris core.
 */
 
void copyByLabel( const PartialList_Ptr src, long label, PartialList_Ptr dst );
/*	Append copies of Partials in the source PartialList having the
	specified label to the destination PartialList. The source list
	is unmodified.
 */

BreakpointEnvelope_Ptr 
createFreqReference( PartialList_Ptr partials, int numSamples,
					 double minFreq, double maxFreq );
/*	Return a newly-constructed BreakpointEnvelope comprising the
	specified number of samples of the frequency envelope of the
	longest Partial in a PartialList. Only Partials whose frequency
	at the Partial's loudest (highest amplitude) breakpoint is
	within the given frequency range are considered. 
	
	For very simple sounds, this frequency reference may be a 
	good first approximation to a reference envelope for
	channelization (see channelize()).
	
	Clients are responsible for disposing of the newly-constructed 
	BreakpointEnvelope.
 */
 
void scaleAmp( PartialList_Ptr partials, BreakpointEnvelope_Ptr ampEnv );
/*	Scale the amplitude of the Partials in a PartialList according 
	to an envelope representing a time-varying amplitude scale value.
 */
				 
void scaleNoiseRatio( PartialList_Ptr partials, BreakpointEnvelope_Ptr noiseEnv );
/*	Scale the relative noise content of the Partials in a PartialList 
	according to an envelope representing a (time-varying) noise energy 
	scale value.
 */

void shiftPitch( PartialList_Ptr partials, BreakpointEnvelope_Ptr pitchEnv );
/*	Shift the pitch of all Partials in a PartialList according to 
	the given pitch envelope. The pitch envelope is assumed to have 
	units of cents (1/100 of a halfstep).
 */

/* ---------------------------------------------------------------- */
/*		notification and exception handlers							
/*
/*
	An exception handler and a notifier may be specified. Both 
	are functions taking a const char * argument and returning
	void.
 */

void setExceptionHandler( void(*f)(const char *) );
/*	Specify a function to call when reporting exceptions. The 
	function takes a const char * argument, and returns void.
 */

void setNotifier( void(*f)(const char *) );
/*	Specify a notification function. The function takes a 
	const char * argument, and returns void.
 */

#ifdef __cplusplus
}	/* extern "C" 	*/
#endif

#endif	/* ndef INCLUDE_LORIS_H */

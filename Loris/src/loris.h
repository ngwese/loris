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
		class ExportSpc;
		class Partial;
	}
	//	include std library headers, declaring template classes
	//	is too painful and fragile:
	#include <list>
	#include <vector>
	//	define types used in procedural interface:
	using Loris::Analyzer;
	using Loris::BreakpointEnvelope;
	using Loris::ExportSpc;
	typedef std::list< Loris::Partial > PartialList;
	typedef std::vector< double > SampleVector;
#else /* no classes, just opaque C pointers */
	typedef struct Analyzer Analyzer;
	typedef struct BreakpointEnvelope BreakpointEnvelope;
	typedef struct ExportSpc ExportSpc;
	typedef struct PartialList PartialList;
	typedef struct SampleVector SampleVector;
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
	
	In C++, Analyzer is Loris::Analyzer, defined in Analyzer.h.
 */
Analyzer * createAnalyzer( double resolution );
/*	Construct and return a new Analyzer configured with the given	
	frequency resolution (minimum instantaneous frequency	
	difference between Partials). All other Analyzer parameters 	
	are computed from the specified frequency resolution. 			
 */
void destroyAnalyzer( Analyzer * ptr_this );
/*	Destroy this Analyzer. 								
 */
void analyzer_analyze( Analyzer * ptr_this, 
					   const SampleVector * vec, double srate,
					   PartialList * partials );
/*	Analyze a vector of (mono) samples at the given sample rate 	  	
	(in Hz) and append the extracted Partials to the given 
	PartialList. 												
 */
	
void analyzer_configure( Analyzer * ptr_this, double resolution );
/*	Configure this Analyzer with the given frequency resolution 
	(minimum instantaneous frequency difference between Partials). 
	All other Analyzer parameters are (re-)computed from the 
	frequency resolution. 		
 */
double analyzer_getFreqResolution( const Analyzer * ptr_this );
/*	Return the frequency resolution (minimum instantaneous frequency  		
	difference between Partials) for this Analyzer. 	
 */
void analyzer_setFreqResolution( Analyzer * ptr_this, double x );
/*	Set the frequency resolution (minimum instantaneous frequency  		
	difference between Partials) for this Analyzer. (Does not cause 	
	other parameters to be recomputed.) 									
 */
double analyzer_getAmpFloor( const Analyzer * ptr_this );
/*	Return the amplitude floor (lowest detected spectral amplitude),  			
	in (negative) dB, for this Analyzer. 				
 */
void analyzer_setAmpFloor( Analyzer * ptr_this, double x );
/*	Set the amplitude floor (lowest detected spectral amplitude), in  			
	(negative) dB, for this Analyzer. 				
 */
double analyzer_getWindowWidth( const Analyzer * ptr_this );
/*	Return the frequency-domain main lobe width (measured between 
	zero-crossings) of the analysis window used by this Analyzer. 				
 */
void analyzer_setWindowWidth( Analyzer * ptr_this, double x );
/*	Set the frequency-domain main lobe width (measured between 
	zero-crossings) of the analysis window used by this Analyzer. 				
 */
double analyzer_getFreqFloor( const Analyzer * ptr_this );
/*	Return the frequency floor (minimum instantaneous Partial  				
	frequency), in Hz, for this Analyzer. 				
 */
void analyzer_setFreqFloor( Analyzer * ptr_this, double x );
/*	Set the amplitude floor (minimum instantaneous Partial  				
	frequency), in Hz, for this Analyzer.
 */
double analyzer_getFreqDrift( const Analyzer * ptr_this );
/*	Return the maximum allowable frequency difference between 					
	consecutive Breakpoints in a Partial envelope for this Analyzer. 				
 */
void analyzer_setFreqDrift( Analyzer * ptr_this, double x );
/*	Set the maximum allowable frequency difference between 					
	consecutive Breakpoints in a Partial envelope for this Analyzer. 				
 */
double analyzer_getHopTime( const Analyzer * ptr_this );
/*	Return the hop time (which corresponds approximately to the 
	average density of Partial envelope Breakpoint data) for this 
	Analyzer.
 */
void analyzer_setHopTime( Analyzer * ptr_this, double x );
/*	Set the hop time (which corresponds approximately to the average
	density of Partial envelope Breakpoint data) for this Analyzer.
 */
double analyzer_getCropTime( const Analyzer * ptr_this );
/*	Return the crop time (maximum temporal displacement of a time-
	frequency data point from the time-domain center of the analysis
	window, beyond which data points are considered "unreliable")
	for this Analyzer.
 */
void analyzer_setCropTime( Analyzer * ptr_this, double x );
/*	Set the crop time (maximum temporal displacement of a time-
	frequency data point from the time-domain center of the analysis
	window, beyond which data points are considered "unreliable")
	for this Analyzer.
 */
double analyzer_getBwRegionWidth( const Analyzer * ptr_this );
/*	Return the width (in Hz) of the Bandwidth Association regions
	used by this Analyzer.
 */
void analyzer_setBwRegionWidth( Analyzer * ptr_this, double x );
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

	In C++, a BreakpointEnvelope is a Loris::BreakpointEnvelope.
 */
 
BreakpointEnvelope * createBreakpointEnvelope( void );
/*	Construct and return a new BreakpointEnvelope having no 
	breakpoints and an implicit value of 0. everywhere, 
	until the first breakpoint is inserted.			
 */

BreakpointEnvelope * copyBreakpointEnvelope( const BreakpointEnvelope * ptr_this );
/*	Construct and return a new BreakpointEnvelope that is an
	exact copy of the specified BreakpointEnvelopes, having 
	an identical set of breakpoints.	
 */

void destroyBreakpointEnvelope( BreakpointEnvelope * ptr_this );
/*	Destroy this BreakpointEnvelope. 								
 */
 
void breakpointEnvelope_insertBreakpoint( BreakpointEnvelope * ptr_this,
					double time, double val );
/*	Insert a breakpoint representing the specified (time, value) 
	pair into this BreakpointEnvelope. If there is already a 
	breakpoint at the specified time, it will be replaced with 
	the new breakpoint.
 */

double breakpointEnvelope_valueAt( const BreakpointEnvelope * ptr_this, 
								   double time );
/*	Return the interpolated value of this BreakpointEnvelope at the 
	specified time.							
 */

/* ---------------------------------------------------------------- */
/*		ExportSpc object interface
/*
/*	An ExportSpc represents a configuration of parameters for
	exporting a collection of Bandwidth-Enhanced partials to 
	an spc-format file for use with the Symbolic Sound Kyma
	System.

	In C++, an ExportSpc * is a Loris::ExportSpc *.
 */
 
ExportSpc * createExportSpc( double midiPitch );
/*	Construct a new ExportSpc instance configured from the 
	given MIDI note number. All other ExportSpc parameters
	are computed fromthe specified note number.
 */

void destroyExportSpc( ExportSpc * ptr_this );
/*	Destroy this ExportSpc instance.
 */

void exportSpc_write( ExportSpc * ptr_this, const char * path,
					  PartialList * partials );
/*	Export the given list of Partials to an spc file having the
	specified path (or name) according to the current configuration 
	of this ExportSpc instance.
 */

void exportSpc_configure( ExportSpc * ptr_this, double midiPitch );
/*	Set the MIDI note number (69.00 = A440) for this spc file,
	and recompute all other parameters to default values.			
 */

double exportSpc_getMidiPitch( const ExportSpc * ptr_this );
/*	Return the MIDI note number (69.00 = A440) for this spc file.
 */

void exportSpc_setMidiPitch( ExportSpc * ptr_this, double x );
/*	Set the MIDI note number (69.00 = A440) for this 
	spc file. (Does not cause other parameters to be 
	recomputed.) 			
 */

int exportSpc_getNumPartials( const ExportSpc * ptr_this );
/*	Return the number of partials in spc file, may 
	be 32, 64, 128, or 256.
 */

void exportSpc_setNumPartials( ExportSpc * ptr_this, int x );
/*	Set the number of partials in spc file, may 
	be 32, 64, 128, or 256.
 */

int exportSpc_getRefLabel( const ExportSpc * ptr_this );
/*	Return the label of the reference partial for this ExportSpc
	instance. A reference label of 1 indicates the fundamental.
	The reference label is used for filling in frequencies during 
	time gaps in other partials. 
 */

void exportSpc_setRefLabel( ExportSpc * ptr_this, int x );
/*	Set the label of the reference partial for this ExportSpc
	instance. A reference label of 1 indicates the fundamental.
	The reference partial is used for filling in frequencies 
	during time gaps in other partials. 
 */

int exportSpc_getEnhanced( const ExportSpc * ptr_this );
/*	Return true if this spc file is in bandwidth-enhanced format,
	false if it is in pure sinusoidal format.
 */

void exportSpc_setEnhanced( ExportSpc * ptr_this, int boool );
/*	Set the type of spc file: true for bandwidth-enhanced format,
	false for pure sinusoidal format.
*/

double exportSpc_getHop( const ExportSpc * ptr_this );
/*	Return the frame duration (in seconds) for this spc file.
 */

void exportSpc_setHop( ExportSpc * ptr_this, double x );
/*	Set the frame duration (in seconds) for this spc file.
 */

double exportSpc_getAttackThreshold( const ExportSpc * ptr_this );
/*	Return the amplitude threshold for cropping the start of the 
	spc file. This is specified as a fraction of maximum amplitude 
	of the sound, with a value between 0.0 and 1.0. If the value is 
	0.0, this indicates no cropping at the start of the spc file.
 */

void exportSpc_setAttackThreshold( ExportSpc * ptr_this, double x );
/*	Set the amplitude threshold for cropping the start of the spc 
	file. This is specified as a fraction of maximum amplitude of 
	the sound, with a value between 0.0 and 1.0.  Specify 0.0 for 
	no cropping of the start of the spc file.
 */

double exportSpc_getStartFreqTime( const ExportSpc * ptr_this );
/*	Return the time (in seconds) at which frequency in attack is 
	considered stable.  Frequencies before this time are modified 
	in the spc file to avoid real-time morphing artifacts when the 
	spc file is used in Kyma. This returns 0.0 if the spc file has 
	no modified attack frequencies.
 */

void exportSpc_setStartFreqTime( ExportSpc * ptr_this, double x );
/*	Set the time (in seconds) at which frequency in attack is 
	considered stable.  Frequencies before this time are modified 
	in the spc file to avoid real-time morphing artifacts when the 
	spc file is used in Kyma. Specify 0.0 to avoid modified attack 
	frequencies.
 */

double exportSpc_getEndTime( const ExportSpc * ptr_this );
/*	Return the time (in seconds) at which the end of the spc file 
	is truncated. This returns 0.0 if the spc file is not truncate 
	at the end.
 */

void exportSpc_setEndTime( ExportSpc * ptr_this, double x );
/*	Set the time (in seconds) to truncate the end of the spc file.
	Set this to the 0.0 (or, equivalently, to the last end time of 
	any partial in the sound) to avoid truncating the end of the 
	spc file.
 */

double exportSpc_getEndApproachTime( const ExportSpc * ptr_this );
 /*	Return a value in seconds that indicates how long before the 
	end of the spc file the amplitude, frequency, and bandwidth 
	values are modified to make a gradual transition to the spectral 
	content at the end of the spc file.  This returns 0.0 if no such 
	modifications are done in the spc file.	
 */

void exportSpc_setEndApproachTime( ExportSpc * ptr_this, double x );
/*	Set how long (in seconds) before the end of the spc file the 
	amplitude, frequency, and bandwidth values are to be modified 
	to make a gradual transition to the spectral content at the 
	end of the spc file. Specify 0.0 to avoid these modifications 
	in the spc file.	
 */

double exportSpc_getMarkerTime( const ExportSpc * ptr_this );
/*	Return the time (in seconds) at which a marker is inserted in 
	the spc file. This returns 0.0 if no marker is inserted into 
	the spc file.
 */

void exportSpc_setMarkerTime( ExportSpc * ptr_this, double x );
/*	Set the time (in seconds) at which a marker is inserted in the 
	spc file. Only one marker is inserted into the spc file; it will
	be inserted at the time specified with setMarkerTime().  
	Specify 0.0 to avoid inserting a marker into the spc file.
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

	In C++, a PartialList is a std::list< Loris::Partial >.
 */ 
PartialList * createPartialList( void );
/*	Return a new empty PartialList.
 */
void destroyPartialList( PartialList * ptr_this );
/*	Destroy this PartialList.
 */
void partialList_clear( PartialList * ptr_this );
/*	Remove (and destroy) all the Partials from this PartialList,
	leaving it empty.
 */
void partialList_copy( PartialList * ptr_this, 
					   const PartialList * src );
/*	Make this PartialList a copy of the source PartialList by making
	copies of all of the Partials in the source and adding them to 
	this PartialList.
 */
unsigned long partialList_size( const PartialList * ptr_this );
/*	Return the number of Partials in this PartialList.
 */
void partialList_splice( PartialList * ptr_this, 
						 PartialList * src );
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

	In C++, a SampleVector is a std::vector< double >.
 */
 
SampleVector * createSampleVector( unsigned long size );
/*	Return a new SampleVector having the specified number of samples,
	all of whose value is 0.
 */
void destroySampleVector( SampleVector * ptr_this );
/*	Destroy this SampleVector.
 */
void sampleVector_copy( SampleVector * ptr_this, 
						const SampleVector * src );
/*	Make this SampleVector a copy of the source SampleVector, having 
	the same number of samples, and samples at every position in this
	SampleVector having the same value as the corresponding sample in
	the source.
 */
double sampleVector_getAt( const SampleVector * ptr_this, unsigned long idx );
/*	Return the value of the sample at the given position (index) in
	this SampleVector.
 */
void sampleVector_setAt( SampleVector * ptr_this, unsigned long idx, double x );
/*	Set the value of the sample at the given position (index) in
	this SampleVector.
 */
unsigned long sampleVector_getLength( const SampleVector * ptr_this );
/*	Return the number of samples represented by this SampleVector.
 */
void sampleVector_setLength( SampleVector * ptr_this, unsigned long size );
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

void channelize( PartialList * partials, 
				 BreakpointEnvelope * refFreqEnvelope, int refLabel );
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

void dilate( PartialList * partials, 
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

void distill( PartialList * partials );
/*	Distill labeled (channelized)  Partials in a PartialList into a 
	PartialList containing a single (labeled) Partial per label. 
	The distilled PartialList will contain as many Partials as
	there were non-zero labels in the original PartialList. Unlabeled 
	(label 0) Partials are eliminated.
 */

void exportAiff( const char * path, const SampleVector * vec, 
				 double samplerate, int nchannels, int bitsPerSamp );
/*	Export audio samples stored in a SampleVector to an AIFF file
	having the specified number of channels and sample rate at the 
	given file path (or name). The floating point samples in the 
	SampleVector are clamped to the range (-1.,1.) and converted 
	to integers having bitsPerSamp bits.
 */
				 
void exportSdif( const char * path, PartialList * partials );
/*	Export Partials in a PartialList to a SDIF file at the specified
	file path (or name). SDIF data is written in the 1TRC format.  
	For more information about SDIF, see the SDIF web site at:
		www.ircam.fr/equipes/analyse-synthese/sdif/  
 */

void exportSpc( const char * path, PartialList * partials,
				int numSpcPars, int refParNum, double frameRate, 
				double midiPitch, double thresh, double endtime, 
				double markertime, double approachtime );
/*	Export Partials in a PartialList to an SPC file, for use with
	the Kyma system. Waiting on a real doc string for this one.  
		
 */
				
void importAiff( const char * path, SampleVector * vec, 
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

void importSdif( const char * path, PartialList * partials );
/*	Import Partials from an SDIF file at the given file path (or 
	name), and append them to a PartialList.
 */	

void morph( const PartialList * src0, const PartialList * src1, 
			const BreakpointEnvelope * ffreq, 
			const BreakpointEnvelope * famp, 
			const BreakpointEnvelope * fbw, 
			PartialList * dst );
/*	Morph labeled Partials in two PartialLists according to the
	given frequency, amplitude, and bandwidth (noisiness) morphing
	envelopes, and append the morphed Partials to the destination 
	PartialList. Loris morphs Partials by interpolating frequency,
	amplitude, and bandwidth envelopes of corresponding Partials in 
	the source PartialLists. For more information about the Loris
	morphing algorithm, see the Loris website: 
	www.cerlsoundgroup.org/Loris/
 */

void synthesize( const PartialList * partials,
				 SampleVector * samples,  
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
 
void copyByLabel( const PartialList * src, long label, PartialList * dst );
/*	Append copies of Partials in the source PartialList having the
	specified label to the destination PartialList. The source list
	is unmodified.
 */

BreakpointEnvelope * 
createFreqReference( PartialList * partials, int numSamples,
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
 
void scaleAmp( PartialList * partials, BreakpointEnvelope * ampEnv );
/*	Scale the amplitude of the Partials in a PartialList according 
	to an envelope representing a time-varying amplitude scale value.
 */
				 
void scaleNoiseRatio( PartialList * partials, BreakpointEnvelope * noiseEnv );
/*	Scale the relative noise content of the Partials in a PartialList 
	according to an envelope representing a (time-varying) noise energy 
	scale value.
 */

void shiftPitch( PartialList * partials, BreakpointEnvelope * pitchEnv );
/*	Shift the pitch of all Partials in a PartialList according to 
	the given pitch envelope. The pitch envelope is assumed to have 
	units of cents (1/100 of a halfstep).
 */

/* ---------------------------------------------------------------- */
/*		Notification and exception handlers							
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

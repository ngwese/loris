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
 *	loris.i
 *
 *  SWIG interface file for building scripting language modules
 *  implementing Loris functionality. This interface has been 
 *  completely rewritten (23 Jan 2003) to support new versions
 *  of SWIG (current is 1.3.17) and take advantage of new features
 *  and bug fixes. This interface wraps many functions in the 
 *  Loris procedural interface, but also provides some Loris C++
 *  class wrappers, to provide enhanced functionality in the 
 *  context of laguages that perform garbage collection.
 *
 *	Also, several interface (.i) files were collapsed into one
 *	(not sure I did myself any favors).
 *
 * Kelly Fitz, 8 Nov 2000
 * rewrite: Kelly Fitz, 23 Jan 2003
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 

#if defined (SWIGPYTHON)
	%module loris
#elif defined (SWIGTCL)
	%module tcLoris
#elif defined (SWIGPERL)
	%module loris_perl
#else
	%module loris
#endif

//	perl defines list and screws us up,
//	undefine it so that we can use std::list
#if defined (SWIGPERL)
	%{
		#undef list
	%}
#endif

// ----------------------------------------------------------------
//		notification and exception handlers
//
%{
	#include<loris.h>
	
	//	import the entire Loris namespace, because
	//	SWIG does not seem to like to wrap functions
	//	with qualified names (like Loris::channelize),
	//	they simply get ignored.
	using namespace Loris;

	//	notification function for Loris debugging
	//	and notifications, installed in initialization
	//	block below:
	static void printf_notifier( const char * s )
	{
		printf("*\t%s\n", s);
	}	
	
	//	exception handling for the procedural interface
	//	(the pi catches all exceptions and handles them
	//	by passing their string descriptions to this 
	//	function):
	static char EXCEPTION_THROWN[256];
	static void exception_handler( const char * s )
	{
		sprintf(EXCEPTION_THROWN, "%255s\0", s);
	}
%}

//	Configure notification and debugging using a
//	in a SWIG initialization block. This code is
//	executed when the module is loaded by the 
//	host interpreter.
//
%init 
%{
	Loris::setNotifier( printf_notifier );
	Loris::setExceptionHandler( exception_handler );
%}


// ----------------------------------------------------------------
//		wrap procedural interface
//
//	Not all functions in the procedural interface are trivially
//	wrapped, some are wrapped to return newly-allocated objects,
//	which we wouldn't do in the procedural interface, but we
//	can do, because SWIG and the scripting langauges take care of 
//	the memory management ambiguities.
//

//	Wrap procedural interface calls with exception checks.
//	No exceptions are ever thrown out of the procedural interface,
//	of course, but they are reported using the exception
//	handler defined above, and installed when the module is
//	loaded.
//

%include exception.i 
%exception 
{
	*EXCEPTION_THROWN = '\0';
	$action
	if (*EXCEPTION_THROWN)
	{
		SWIG_exception( SWIG_UnknownError, EXCEPTION_THROWN );
	}
}

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


%newobject createFreqReference;
BreakpointEnvelope * 
createFreqReference( PartialList * partials, 
					 double minFreq, double maxFreq, long numSamps = 0 );
/*	Return a newly-constructed BreakpointEnvelope by sampling the 
	frequency envelope of the longest Partial in a PartialList. 
	Only Partials whose frequency at the Partial's loudest (highest 
	amplitude) breakpoint is within the given frequency range are 
	considered. 
	
	If the number of sample points is not specified, then the
	longest Partial's frequency envelope is sampled every 30 ms
	(No fewer than 10 samples are used, so the sampling maybe more
	dense for very short Partials.) 
	
	For very simple sounds, this frequency reference may be a 
	good first approximation to a reference envelope for
	channelization (see channelize()).
 */

//	dilate() needs to be wrapped by a function that
//	accepts the time points as strings until a language-
//	neutral method can be found for passing a sequence
//	of numbers. This wrapper uses the strtovec() converter
//	defined above to convert two strings into vectors
//	of time points:
//
//	HEY! how dumb is this? Just use the SampleVector (rename to 
//	just Vector or something) for this purpose!
//
%{
	#include <Exception.h>
	#include <Notifier.h>
	#include <string>
	#include <vector>
	
	//	Helper function for dilate: 
	//
	//	convert a string into a vector of doubles,
	//	ignore any extraneous characters:
	static std::vector<double> strtovec( const std::string & s )
	{
		std::vector<double> v;
		std::string::size_type beg, end;
		const std::string numparts("1234567890+-.");
		beg = s.find_first_of( numparts );
		while ( beg != std::string::npos )
		{
			end = s.find_first_not_of( numparts, beg );
			if ( end == std::string::npos )
				end = s.length();
	
			double x = atof( s.c_str() + beg );
			v.push_back(x);
	
			beg = s.find_first_of( numparts, end );
		}
		return v;
	}

	void dilate_str( PartialList * partials, 
					 char * initial_times, char * target_times )
	{
		std::vector<double> ivec = strtovec( initial_times );
		std::vector<double> tvec = strtovec( target_times );
		
		Loris::debugger << ivec.size() << " initial points, " 
						<< tvec.size() << " target points" << Loris::endl;
			
		if ( ivec.size() != tvec.size() )
			Throw( Loris::InvalidArgument, "Invalid arguments to dilate(): there must be as many target points as initial points" );
				
		double * initial = &(ivec[0]);
		double * target = &(tvec[0]);
		int npts = ivec.size();
		dilate( partials, initial, target, npts );
	}
%}

%rename(dilate) dilate_str;
void dilate_str( PartialList * partials, 
				 char * initial_times, char * target_times );
/*	Dilate Partials in a PartialList according to the given 
	initial and target time points. Partial envelopes are 
	stretched and compressed so that temporal features at
	the initial time points are aligned with the final time
	points. Time points are sorted, so Partial envelopes are 
	are only stretched and compressed, but breakpoints are not
	reordered. Duplicate time points are allowed. There must be
	the same number of initial and target time points.
	
	The time points are passed as strings; convert any native
	collection to a string representation, numerical elements
	will be extracted, other characters will be ignored.
 */



void distill( PartialList * partials );
/*	Distill labeled (channelized)  Partials in a PartialList into a 
	PartialList containing a single (labeled) Partial per label. 
	The distilled PartialList will contain as many Partials as
	there were non-zero labels in the original PartialList. Unlabeled 
	(label 0) Partials are eliminated.
 */
				 

void exportAiff( const char * path, SampleVector * samples,
 				 double samplerate = 44100.0, int nchannels = 1, int bitsPerSamp = 16 );
/*	Export audio samples stored in a SampleVector to an AIFF file
	having the specified number of channels and sample rate at the 
	given file path (or name). The floating point samples in the 
	SampleVector are clamped to the range (-1.,1.) and converted 
	to integers having bitsPerSamp bits. The default values for the
	sample rate, number of channels, and sample size, if unspecified,
	are 44100 Hz (CD quality), 1 channel, and 16 bits per sample, 
	respectively.
 */


void exportSdif( const char * path, PartialList * partials );
/*	Export Partials in a PartialList to a SDIF file at the specified
	file path (or name). SDIF data is written in the 1TRC format.  
	For more information about SDIF, see the SDIF website at:
		www.ircam.fr/equipes/analyse-synthese/sdif/  
 */



void exportSpc( const char * path, PartialList * partials, double midiPitch, 
				int enhanced = true, double endApproachTime = 0. );
/*	Export Partials in a PartialList to a Spc file at the specified file
	path (or name). The fractional MIDI pitch must be specified. The optional
	enhanced parameter defaults to true (for bandwidth-enhanced spc files), 
	but an be specified false for pure-sines spc files. The optional 
	endApproachTime parameter is in seconds; its default value is zero (and 
	has no effect). A nonzero endApproachTime indicates that the PartialList does 
	not include a release, but rather ends in a static spectrum corresponding 
	to the final breakpoint values of the partials. The endApproachTime
	specifies how long before the end of the sound the amplitude, frequency, 
	and bandwidth values are to be modified to make a gradual transition to 
	the static spectrum.
 */


%newobject importSdif;
%inline %{
	PartialList * importSdif( const char * path )
	{
		PartialList * dst = createPartialList();
		importSdif( path, dst );

		// check for exception:
		if (*EXCEPTION_THROWN)
		{
			destroyPartialList( dst );
			dst = NULL;
		}
		return dst;
	}
%}
/*  Import Partials from an SDIF file at the given file path (or
	name), and return them in a PartialList.
	For more information about SDIF, see the SDIF website at:
		www.ircam.fr/equipes/analyse-synthese/sdif/
 */


%newobject importSpc;
%inline %{
	PartialList * importSpc( const char * path )
	{
		PartialList * dst = createPartialList();
		importSpc( path, dst );

		// check for exception:
		if (*EXCEPTION_THROWN)
		{
			destroyPartialList( dst );
			dst = NULL;
		}
		return dst;
	}
%}
/*  Import Partials from an Spc file at the given file path (or
	name), and return them in a PartialList.
 */

%newobject morph;
%inline %{
	PartialList * morph( const PartialList * src0, const PartialList * src1, 
						 const BreakpointEnvelope * ffreq, 
						 const BreakpointEnvelope * famp, 
						 const BreakpointEnvelope * fbw )
	{
		PartialList * dst = createPartialList();
		morph( src0, src1, ffreq, famp, fbw, dst );
		
		// check for exception:
		if (*EXCEPTION_THROWN)
		{
			destroyPartialList( dst );
			dst = NULL;
		}
		return dst;
	}
%}
/*  Morph labeled Partials in two PartialLists according to the
	given frequency, amplitude, and bandwidth (noisiness) morphing
	envelopes, and return the morphed Partials in a PartialList.
	Loris morphs Partials by interpolating frequency, amplitude,
	and bandwidth envelopes of corresponding Partials in the
	source PartialLists. For more information about the Loris
	morphing algorithm, see the Loris website:
	www.cerlsoundgroup.org/Loris/
 */


%newobject synthesize;
%inline %{
	SampleVector * synthesize( const PartialList * partials, double srate = 44100.0 )
	{
		SampleVector * dst = createSampleVector(0);
		synthesize( partials, dst, srate );
				
		// check for exception:
		if (*EXCEPTION_THROWN)
		{
			destroySampleVector( dst );
			dst = NULL;
		}
		return dst;
	}
%}
/*  Synthesize Partials in a PartialList at the given sample
	rate, and return the (floating point) samples in a SampleVector.
	The SampleVector is sized to hold as many samples as are needed
	for the complete synthesis of all the Partials in the PartialList.
	If the sample rate is unspecified, the deault value of 44100 Hz
	(CD quality) is used.
 */

void sift( PartialList * partials );
/*  Eliminate overlapping Partials having the same label
	(except zero). If any two partials with same label
   	overlap in time, keep only the longer of the two.
   	Set the label of the shorter duration partial to zero.
 */


%newobject extract;
%inline %{
	PartialList * extract( PartialList * partials, long label )
	{
		PartialList * dst = createPartialList();
		spliceByLabel( partials, label, dst );
		
		// check for exception:
		if (*EXCEPTION_THROWN)
		{
			destroyPartialList( dst );
			dst = NULL;
		}
		return dst;
	}
%}
/*  Extract Partials in the source PartialList having the specified
    label and return them in a new PartialList.
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


%inline %{
	const char * version( void )
	{
		static const char * vstr = LORIS_VERSION_STR;
		return vstr;
	}
%}
 
// ----------------------------------------------------------------
//		wrap Loris classes

//	Wrap all calls into the Loris library with exception
//	handlers to prevent exceptions from leaking out of the
//	C++ code, wherein they can be handled, and into the
//	interpreter, where they will surely cause an immediate
//	halt. Only std::exceptions and Loris::Exceptions (and 
//	subclasses) can be thrown.
//
//	Don't use procedural interface calls here, because this 
//	exception handler doesn't check for exceptions raised in
//	the procedural interface!
//
%{
	#include <Exception.h>
	#include <stdexcept>
%}

//	These should probably not all report UnknownError, could
//	make an effort to raise the right kind of (SWIG) exception.
//
%exception {
	try
	{	
		$action
	}
	catch( Loris::Exception & ex ) 
	{
		//	catch Loris::Exceptions:
		std::string s("Loris exception: " );
		s.append( ex.what() );
		SWIG_exception( SWIG_UnknownError, (char *) s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		//	catch std::exceptions:
		std::string s("std C++ exception: " );
		s.append( ex.what() );
		SWIG_exception( SWIG_UnknownError, (char *) s.c_str() );
	}
}

// several classes define a copy member that
// returns a new object:
//
%newobject *::copy;


// ---------------------------------------------------------------------------
//	class AiffFile
//	
//	An AiffFile represents a sample file (on disk) in the Audio Interchange
//	File Format. The file is read from disk and the samples stored in memory
//	upon construction of an AiffFile instance. The samples are accessed by 
//	the samples() method, which converts them to double precision floats and
//	returns them in a SampleVector.
//
%{
	#include<AiffFile.h>
%}

%newobject AiffFile::samples;

class AiffFile
{
public:
	AiffFile( const char * filename );
	~AiffFile( void );
	
	int channels( void ) const;
	unsigned long sampleFrames( void ) const;
	double sampleRate( void ) const;
	int sampleSize( void ) const;
	 
	%extend 
	{
		SampleVector * samples( void )
		{
			SampleVector * vec = new SampleVector( self->sampleFrames() );
			if ( ! vec->empty() )
				self->getSamples( &((*vec)[0]), &((*vec)[vec->size()]) );
			return vec;
		}
		 
	}	
	/*	Return a SampleVector containing the AIFF samples from this AIFF 
		file as double precision floats on the range -1,1.
	 */
};

// ---------------------------------------------------------------------------
//	class Analyzer
//	
//	An Analyzer represents a configuration of parameters for
//	performing Reassigned Bandwidth-Enhanced Additive Analysis
//	of sampled waveforms. This analysis process yields a collection 
//	of Partials, each having a trio of synchronous, non-uniformly-
//	sampled breakpoint envelopes representing the time-varying 
//	frequency, amplitude, and noisiness of a single bandwidth-
//	enhanced sinusoid. 
//
//	For more information about Reassigned Bandwidth-Enhanced 
//	Analysis and the Reassigned Bandwidth-Enhanced Additive Sound 
//	Model, refer to the Loris website: www.cerlsoundgroup.org/Loris/
//

%{
	#include<Analyzer.h>
	#include<Partial.h>
%}

%newobject Analyzer::analyze;
			
class Analyzer
{
public:
	%extend 
	{
		//	construction:
		Analyzer( double resolutionHz, double windowWidthHz = 0. )
		{
			if ( windowWidthHz == 0. )
				windowWidthHz = resolutionHz;
			return new Analyzer( resolutionHz, windowWidthHz );
		}
		/*	Construct and return a new Analyzer configured with the given	
			frequency resolution (minimum instantaneous frequency	
			difference between Partials) and analysis window main 
			lobe width (between zeros). All other Analyzer parameters 	
			are computed from the specified resolution and window
			width. If the window width is not specified, or is 0,
			then it is assumed to be equal to the resolution. 			
		 */
	
		Analyzer * copy( void )
		{
			return new Analyzer( *self );
		}
		/*	Construct and return a new Analyzer having identical
			parameter configuration to another Analyzer.			
		 */
	
		//	analysis:
		PartialList * analyze( const SampleVector * vec, double srate )
		{
			PartialList * partials = new PartialList();
			if ( ! vec->empty() )
				self->analyze( &((*vec)[0]), &((*vec)[vec->size()]), srate );
			partials->splice( partials->end(), self->partials() );
			return partials;
		}
		/*	Analyze a SampleVector of (mono) samples at the given sample rate 	  	
			(in Hz) and return the resulting Partials in a PartialList. 												
		 */
	}
	
	//	parameter access:
	double freqResolution( void ) const;
	double ampFloor( void ) const;
 	double windowWidth( void ) const;
 	double freqFloor( void ) const;
	double hopTime( void ) const;
 	double freqDrift( void ) const;
 	double cropTime( void ) const;
	double bwRegionWidth( void ) const;
	
	//	parameter mutation:
	void setFreqResolution( double x );
	void setAmpFloor( double x );
	void setWindowWidth( double x );
	void setFreqFloor( double x );
	void setFreqDrift( double x );
 	void setHopTime( double x );
 	void setCropTime( double x );
 	void setBwRegionWidth( double x );

};	//	end of class Analyzer
			
// ---------------------------------------------------------------------------
//	class BreakpointEnvelope
//
//	A BreakpointEnvelope represents a linear segment breakpoint 
//	function with infinite extension at each end (that is, the 
//	values past either end of the breakpoint function have the 
//	values at the nearest end).
//
%{
	#include<BreakpointEnvelope.h>
%}

class BreakpointEnvelope
{
public:
	//	construction:
	BreakpointEnvelope( void );
	~BreakpointEnvelope( void );
	
	%name(BreakpointEnvelopeWithValue) BreakpointEnvelope( double initialValue );
	
	%extend 
	{
		BreakpointEnvelope * copy( void )
		{
			return new BreakpointEnvelope( *self );
		}
		/*	Construct and return a new BreakpointEnvelope that is
			a copy of this BreapointEnvelope (has the same value
			as this BreakpointEnvelope everywhere).			
		 */
	}

	//	envelope access and mutation:
	void insertBreakpoint( double time, double value );
	double valueAt( double x ) const;		
	 
};	//	end of class BreakpointEnvelope


// ---------------------------------------------------------------------------
//	class SampleVector
//	
//	A SampleVector represents a vector of floating point samples of
//	an audio waveform. In Loris, the samples are assumed to have 
//	values on the range (-1., 1.) (though this is not enforced or 
//	checked). 
//
class SampleVector
{
public:
	SampleVector( unsigned long size = 0 );	 
	 ~SampleVector( void );

	void clear( void );
	void resize( unsigned long size );
	unsigned long size( void );
	 
	%extend
	{
		SampleVector * copy( void )
		{
			return new SampleVector( *self );
		}
		/*	Return a new SampleVector that is a copy of this 
			SampleVector, having the same number of samples, 
			and samples at every position in the copy having 
			the same value as the corresponding sample in
			this SampleVector.
		 */
		 
		double getAt( unsigned long idx )
		{
			if ( idx >= self->size() )
				throw std::out_of_range("SampleVector::getAt index out of range");
			return (*self)[idx];
		}
		/*	Return the value of the sample at the given position (index) in
			this SampleVector.
		 */
		 
		void setAt( unsigned long idx, double x )
		{
			if ( idx >= self->size() )
				throw std::out_of_range("SampleVector::setAt index out of range");
			(*self)[idx] = x;
		}
		/*	Set the value of the sample at the given position (index) in
			this SampleVector.
		 */
	}

};	//	end of class SampleVector

// ----------------------------------------------------------------
//		wrap PartialList classes
//
//	(PartialList, PartialListIterator, Partial, PartialIterator, 
//	and Breakpoint)
//
//	This stuff is kind of big, so it lives in its own interface
//	file.
%include lorisPartialList.i


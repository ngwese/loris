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
 *	SWIG interface file supporting the Loris procedural inteface
 *	declared in loris.h. The non-object functions and utility functions
 *	are described in this file, object representations are described 
 *	in their own interface (.i) files.
 *
 *
 * Kelly Fitz, 8 Nov 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#if defined (SWIGPYTHON)
	%module loris
#elif defined (SWIGTCL8)
	%module tcLoris
#elif defined (SWIGPERL5)
	%module loris_perl
#else
	%module loris
#endif

//	perl defines list and screws us up,
//	undefine it so that we can use std::list
#if defined (SWIGPERL5)
	%{
		#undef list
	%}
#endif

//	Exceptions, notification, and debugging
//
%{
	#include<Notifier.h>
	#include<Exception.h>
	#include <string>
	
	//	notification function for Loris debugging
	//	and notifications, installed in initialization
	//	block below:
	static void printf_notifier( const char * s )
	{
		printf("*\t%s\n", s);
	}	
	
	//	class NullPointer
	//
	//	Define a Loris::Exception subclass for catching NULL pointers.
	//	This is copied from lorisException_pi.h. It could be in Loris,
	//	but probably was never needed because Loris doesn't make much 
	//	use of pointers.
	//
	class NullPointer : public Loris::Exception
	{
	public: 
		NullPointer( const std::string & str, const std::string & where = "" ) : 
			Exception( std::string("NULL pointer exception -- ").append( str ), where ) {}
	};	//	end of class NullPointer
	
	//	define a macro for testing and throwing:
	#define ThrowIfNull(ptr) if ((ptr)==NULL) Throw( NullPointer, #ptr );	
%}

%{
	#include<loris.h>
	using namespace Loris;
%}

//	Configure notification and debugging using a
//	in a SWIG initialization block. This code is
//	executed when the module is loaded by the 
//	host interpreter.
//
%init 
%{
	Loris::setNotifierHandler( printf_notifier );
	Loris::setDebuggerHandler( printf_notifier );
%}

//	Wrap all calls into the Loris library with exception
//	handlers to prevent exceptions from leaking out of the
//	C++ code, wherein they can be handled, and into the
//	interpreter, where they will surely cause an immediate
//	halt. Only std::exceptions and Loris::Exceptions (and 
//	subclasses) can be thrown.
//
%include exception.i 
%except {
	try
	{	
		$function
	}
	catch( Loris::Exception & ex ) 
	{
		//	catch Loris::Exceptions:
		std::string s("Loris exception: " );
		s.append( ex.what() );
		SWIG_exception( SWIG_RuntimeError, (char *) s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		//	catch std::exceptions:
		//	(these are very unlikely to come from the interface
		//	code, and cannot escape the procedural interface to
		//	Loris, which catches all exceptions.)
		std::string s("std C++ exception: " );
		s.append( ex.what() );
		SWIG_exception( SWIG_RuntimeError, (char *) s.c_str() );
	}
}

//	include the PartialList class interface:
%include lorisPartialList.i

//	include the SampleVector class interface:
%include lorisSampleVector.i

//	include the Analyzer class interface:
%include lorisAnalyzer.i

//	include the BreakpointEnvelope class interface:
%include lorisBpEnvelope.i

//	include the AiffFile class interface:
%include lorisAiffFile.i

/* ---------------------------------------------------------------- */
/*		non-object-based procedures
/*
 *	Operations in Loris that need not be accessed though object
 *	interfaces are represented as simple functions.
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
 
%new BreakpointEnvelope * 
createFreqReference( PartialList * partials, double minFreq, double maxFreq, long numSamps = 0 );
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
%{
	#include<Dilator.h>
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
				
		double * initial = ivec.begin();
		double * target = tvec.begin();
		int npts = ivec.size();
	
		//	USE THE PI INSTEAD!
			
		ThrowIfNull((PartialList *) partials);
		ThrowIfNull((double *) initial);
		ThrowIfNull((double *) target);
	
		Loris::notifier << "dilating " << partials->size() << " Partials" << Loris::endl;
		Loris::Dilator dil( initial, target, npts );
		dil.dilate( partials->begin(), partials->end() );
	}
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
%}

%rename(dilate) dilate_str;
void dilate_str( PartialList * partials, 
				 char * initial_times, char * target_times );



void distill( PartialList * partials );
/*	Distill labeled (channelized)  Partials in a PartialList into a 
	PartialList containing a single (labeled) Partial per label. 
	The distilled PartialList will contain as many Partials as
	there were non-zero labels in the original PartialList. Unlabeled 
	(label 0) Partials are eliminated.
 */
				 

void exportAiff( const char * path,
				 SampleVector * samples,
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


//	SWIG problem:
//	%new and %inline don't play nice together, so if I %inline
//	these next four functions, then their return objects don't
//	get ownership on the scripting side. So cannot use the 
//	%inline shortcut when the return value is %new.
//
%{
	#include<SdifFile.h>
	PartialList * importSdif_( const char * path )
	{
		Loris::notifier << "importing Partials from " << path << Loris::endl;
		Loris::SdifFile imp( path );

		PartialList * partials = new PartialList();
		//	splice() can't throw, can it???
		partials->splice( partials->end(), imp.partials() );

		return partials;
	}
%}
%rename(importSdif) importSdif_;
%new PartialList * importSdif_( const char * path );
/*	Import Partials from an SDIF file at the given file path (or 
	name), and return them in a PartialList.
	For more information about SDIF, see the SDIF website at:
		www.ircam.fr/equipes/analyse-synthese/sdif/  
 */	

%{
	#include<SpcFile.h>
	PartialList * importSpc_( const char * path )
	{
		Loris::notifier << "importing Partials from " << path << Loris::endl;
		Loris::SpcFile imp( path );

		PartialList * partials = new PartialList();
		//	splice() can't throw, can it???
		partials->splice( partials->end(), imp.partials() );

		return partials;
	}
%}

%rename(importSpc) importSpc_;
%new PartialList * importSpc_( const char * path );
/*	Import Partials from an Spc file at the given file path (or 
	name), and return them in a PartialList.
 */	

%{
	#include<Morpher.h>
	PartialList * morph_( const PartialList * src0, const PartialList * src1, 
						 const BreakpointEnvelope * ffreq, 
						 const BreakpointEnvelope * famp, 
						 const BreakpointEnvelope * fbw )
	{
		ThrowIfNull((PartialList *) src0);
		ThrowIfNull((PartialList *) src1);
		ThrowIfNull((BreakpointEnvelope *) ffreq);
		ThrowIfNull((BreakpointEnvelope *) famp);
		ThrowIfNull((BreakpointEnvelope *) fbw);

		Loris::notifier << "morphing " << src0->size() << " Partials with "
						<< src1->size() << " Partials" << Loris::endl;
					
		//	make a Morpher object and do it:
		Loris::Morpher m( *ffreq, *famp, *fbw );
		m.morph( src0->begin(), src0->end(), src1->begin(), src1->end() );
				
		//	splice the morphed Partials into a new PartialList:
		PartialList * dst = new PartialList();
		//	splice() can't throw, can it???
		dst->splice( dst->end(), m.partials() );
		return dst;
	}
%}

%rename(morph) morph_;
%new PartialList * morph_( const PartialList * src0, const PartialList * src1, 
						  const BreakpointEnvelope * ffreq, 
						  const BreakpointEnvelope * famp, 
						  const BreakpointEnvelope * fbw );
/*	Morph labeled Partials in two PartialLists according to the
	given frequency, amplitude, and bandwidth (noisiness) morphing
	envelopes, and return the morphed Partials in a PartialList. 
	Loris morphs Partials by interpolating frequency, amplitude, 
	and bandwidth envelopes of corresponding Partials in the
	source PartialLists. For more information about the Loris
	morphing algorithm, see the Loris website: 
	www.cerlsoundgroup.org/Loris/
 */

%{
	#include<Synthesizer.h>
	SampleVector * synthesize_( const PartialList * partials, double srate = 44100.0 )
	{
		ThrowIfNull((PartialList *) partials);

		Loris::notifier << "synthesizing " << partials->size() 
						<< " Partials at " << srate << " Hz" << Loris::endl;

		//	compute the duration:
		debugger << "computing duration..." << Loris::endl;
		double maxtime = 0.;
		PartialList::const_iterator it;
		for ( it = partials->begin(); it != partials->end(); ++it ) 
		{
			maxtime = std::max( maxtime, it->endTime() );
		}
		debugger << maxtime << " seconds" << Loris::endl;
		
		//	allocate a SampleVector to accomodate the fade-out at 
		//	the end of the latest Partial:
		const long nsamps = long( srate * ( maxtime + Partial::FadeTime() ) );	
		SampleVector * samples = new SampleVector( nsamps, 0. );
		
		//	synthesize:
		try
		{
			Loris::Synthesizer synth( srate, samples->begin(), samples->end() );
			for ( it = partials->begin(); it != partials->end(); ++it ) 
			{
				synth.synthesize( *it );
			}
		}
		catch(...)
		{
			delete samples;
			throw;
		}
		
		return samples;
	}
%}
%rename(synthesize) synthesize_;
%new SampleVector * synthesize_( const PartialList * partials, double srate = 44100.0 );
/*	Synthesize Partials in a PartialList at the given sample
	rate, and return the (floating point) samples in a SampleVector.
	The SampleVector is sized to hold as many samples as are needed 
	for the complete synthesis of all the Partials in the PartialList. 
	If the sample rate is unspecified, the deault value of 44100 Hz 
	(CD quality) is used.
 */



/* ---------------------------------------------------------------- */
/*		utility functions
/*
 *	These procedures are generally useful but are not yet  
 *	represented by classes in the Loris core.
 */
%{
	PartialList * extract_( PartialList * partials, long label )
	{
        ThrowIfNull((PartialList *) partials);

		PartialList * ret = new PartialList();
		try 
		{
			spliceByLabel( partials, label, ret );
		}
		catch(...)
		{
			delete ret;
			throw;
		}
		return ret;
	}
%}
%rename(extractLabeled) extract_;
%new PartialList * extract_( PartialList * partials, long label );
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
 

			

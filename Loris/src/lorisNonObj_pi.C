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
 *	lorisNonObj_pi.C
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
 *	This file defines the non-object-based component of the Loris
 *	procedural interface.
 *
 * Kelly Fitz, 10 Nov 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "loris.h"
#include "lorisException_pi.h"

#include <AiffFile.h>
#include <Analyzer.h>
#include <BreakpointEnvelope.h>
#include <Channelizer.h>
#include <Dilator.h>
#include <Distiller.h>
#include <Exception.h>
#include <FrequencyReference.h>
#include <ImportLemur.h>
#include <Morpher.h>
#include <Notifier.h>
#include <Partial.h>
#include <PartialUtils.h>
#include <SdifFile.h>
#include <SpcFile.h>
#include <Synthesizer.h>

#include <cmath>
#include <list>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <set>


using namespace Loris;

/* ---------------------------------------------------------------- */
/*		non-object-based procedures
/*
/*	Operations in Loris that need not be accessed though object
	interfaces are represented as simple functions.
 */

/* ---------------------------------------------------------------- */
/*        channelize        
/*
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
extern "C"
void channelize( PartialList * partials, 
				 BreakpointEnvelope * refFreqEnvelope, int refLabel )
{
	try 
	{
		ThrowIfNull((PartialList *) partials);
		ThrowIfNull((BreakpointEnvelope *) refFreqEnvelope);

		if ( refLabel <= 0 )
			Throw( InvalidArgument, "Channelization reference label must be positive." );
		
		notifier << "channelizing " << partials->size() << " Partials" << endl;

		Channelizer chan( *refFreqEnvelope, refLabel );
		chan.channelize( partials->begin(), partials->end() );		
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in channelize(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in channelize(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        dilate        
/*
/*	Dilate Partials in a PartialList according to the given 
	initial and target time points. Partial envelopes are 
	stretched and compressed so that temporal features at
	the initial time points are aligned with the final time
	points. Time points are sorted, so Partial envelopes are 
	are only stretched and compressed, but breakpoints are not
	reordered. Duplicate time points are allowed. There must be
	the same number of initial and target time points.
 */
extern "C"
void dilate( PartialList * partials, 
			 double * initial, double * target, int npts )
{
	try 
	{
		ThrowIfNull((PartialList *) partials);
		ThrowIfNull((double *) initial);
		ThrowIfNull((double *) target);

		notifier << "dilating " << partials->size() << " Partials" << endl;
		Dilator dil( initial, target, npts );
		PartialList::iterator it;
		for ( it = partials->begin(); it != partials->end(); ++it )
		{
			dil.dilate( *it );
		}
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in dilate(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in dilate(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        distill        
/*
/*	Distill labeled (channelized)  Partials in a PartialList into a 
	PartialList containing a single (labeled) Partial per label. 
	The distilled PartialList will contain as many Partials as
	there were non-zero labels in the original PartialList. Unlabeled 
	(label 0) Partials are eliminated.
 */
extern "C"
void distill( PartialList * partials )
{
	try 
	{
		ThrowIfNull((PartialList *) partials);

		notifier << "distilling " << partials->size() << " Partials" << endl;
		Distiller still;
		still.distill( *partials );
		
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in distill(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in distill(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        exportAiff        
/*
/*	Export audio samples stored in a SampleVector to an AIFF file
	having the specified number of channels and sample rate at the 
	given file path (or name). The floating point samples in the 
	SampleVector are clamped to the range (-1.,1.) and converted 
	to integers having bitsPerSamp bits.
 */
extern "C"
void exportAiff( const char * path, const SampleVector * vec, 
				 double samplerate, int nchannels, int bitsPerSamp )
{
	try 
	{
		ThrowIfNull((SampleVector *) vec);

		//	write out samples:
		notifier << "writing " << vec->size() << " samples to " << path << endl;
		AiffFile::Export( path, samplerate, nchannels, bitsPerSamp, 
						  vec->begin(), vec->end() );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in exportAiff(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in exportAiff(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}

}

/* ---------------------------------------------------------------- */
/*        exportSdif        
/*
/*	Export Partials in a PartialList to a SDIF file at the specified
	file path (or name). SDIF data is written in the 1TRC format.  
	For more information about SDIF, see the SDIF web site at:
		www.ircam.fr/equipes/analyse-synthese/sdif/  
 */
extern "C"
void exportSdif( const char * path, PartialList * partials )
{
	try 
	{
		ThrowIfNull((PartialList *) partials);

		if ( partials->size() == 0 ) 
			Throw( Loris::InvalidObject, "No Partials in PartialList to export to sdif file." );
	
		Loris::notifier << "exporting sdif partial data to " << path << Loris::endl;		
		Loris::SdifFile::Export( path, *partials );
		
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in exportSdif(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in exportSdif(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}

}

/* ---------------------------------------------------------------- */
/*        exportSpc        
/*
/*	Export Partials in a PartialList to a Spc file at the specified file
	path (or name). The fractional MIDI pitch must be specified. The 
	enhanced parameter defaults to true (for bandwidth-enhanced spc files), 
	but an be specified false for pure-sines spc files. The endApproachTime 
	parameter is in seconds. A nonzero endApproachTime indicates that the plist does 
	not include a release, but rather ends in a static spectrum corresponding 
	to the final breakpoint values of the partials. The endApproachTime
	specifies how long before the end of the sound the amplitude, frequency, 
	and bandwidth values are to be modified to make a gradual transition to 
	the static spectrum.
 */
extern "C"
void exportSpc( const char * path, PartialList * partials, double midiPitch, 
				int enhanced, double endApproachTime )
{
	try 
	{
		ThrowIfNull((PartialList *) partials);

		if ( partials->size() == 0 )
			Throw( Loris::InvalidObject, "No Partials in PartialList to export to Spc file." );

		Loris::notifier << "exporting Spc partial data to " << path << Loris::endl;
		Loris::SpcFile::Export( path, *partials, midiPitch, enhanced, endApproachTime );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in exportSpc(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in exportSdif(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}

}

/* ---------------------------------------------------------------- */
/*        createFreqReference        
/*
/*	Return a newly-constructed BreakpointEnvelope by sampling the 
	frequency envelope of the longest Partial in a PartialList. 
	Only Partials whose frequency at the Partial's loudest (highest 
	amplitude) breakpoint is within the given frequency range are 
	considered. The envelope will have the specified number of samples.
	If the specified number of samples is 0, then the
	longest Partial's frequency envelope is sampled every 30 ms
	(No fewer than 10 samples are used, so the sampling maybe more
	dense for very short Partials.) 
	
	For very simple sounds, this frequency reference may be a 
	good first approximation to a reference envelope for
	channelization (see channelize()).
	
	Clients are responsible for disposing of the newly-constructed 
	BreakpointEnvelope.
 */
extern "C"
BreakpointEnvelope * 
createFreqReference( PartialList * partials, double minFreq, double maxFreq, long numSamps )
{
	try 
	{
		ThrowIfNull((PartialList *) partials);
		
		//	use auto_ptr to manage memory in case 
		//	an exception is generated (hard to imagine):
		std::auto_ptr< BreakpointEnvelope > env_ptr;
		if ( numSamps != 0 )
		{
			env_ptr.reset( new BreakpointEnvelope( 
								FrequencyReference( partials->begin(), partials->end(), 
													minFreq, maxFreq, numSamps ).envelope() ) );
		}
		else
		{
			env_ptr.reset( new BreakpointEnvelope( 
								FrequencyReference( partials->begin(), partials->end(), 
													minFreq, maxFreq ).envelope() ) );
		}
		
		return env_ptr.release();
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in createFreqReference(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in createFreqReference(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return NULL;
}

/* ---------------------------------------------------------------- */
/*        importAiff        
/*
/*	Import audio samples stored in an AIFF file at the given file
	path (or name). The samples are converted to floating point 
	values on the range (-1.,1.) and stored in the given 
	SampleVector, which is resized to (exactly) accomodate all the 
	samples from the file. If samplerate is not a NULL pointer, 
	then, on return, it points to the value of the sample rate (in
	Hz) of the AIFF samples. Similarly, if nchannels is not a NULL
	pointer, then, on return, it points to the value of the number
	of channels of audio data represented by the AIFF samples.
 */
extern "C"
void importAiff( const char * path, SampleVector * ptr_this, 
				 double * samplerate, int * nchannels )
{
	try 
	{
		ThrowIfNull((SampleVector *) ptr_this);

		//	read samples:
		notifier << "reading samples from " << path << endl;
		AiffFile f( path );
		ptr_this->resize( f.sampleFrames(), 0. );
		f.getSamples( ptr_this->begin(), ptr_this->end() );
		notifier << "read " << f.sampleFrames() << " frames of " 
				 << f.channels() << " channel data at " 
				 << f.sampleRate() << " Hz" << endl;
		
		if ( samplerate )
			*samplerate = f.sampleRate();
		if ( nchannels )
			*nchannels = f.channels();
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in importAiff(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in importAiff(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        importSdif 
/*       
/*	Import Partials from an SDIF file at the given file path (or 
	name), and append them to a PartialList. Loris reads SDIF
	files in the 1TRC format. For more information about SDIF, 
	see the SDIF web site at:
		www.ircam.fr/equipes/analyse-synthese/sdif/ 
 */	
extern "C"
void importSdif( const char * path, PartialList * partials )
{
	try 
	{
		ThrowIfNull((PartialList *) partials);

		notifier << "importing Partials from " << path << endl;
		SdifFile imp( path );
		partials->splice( partials->end(), imp.partials() );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in importSdif(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in importSdif(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        importSpc 
/*       
/*	Import Partials from an Spc file at the given file path (or 
	name), and return them in a PartialList.
 */	
extern "C"
void importSpc( const char * path, PartialList * partials )
{
	try 
	{
		Loris::notifier << "importing Partials from " << path << Loris::endl;
		Loris::SpcFile imp( path );
		partials->splice( partials->end(), imp.partials() );

	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in importSpc(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in importSpc(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        morph        
/*
/*	Morph labeled Partials in two PartialLists according to the
	given frequency, amplitude, and bandwidth (noisiness) morphing
	envelopes, and append the morphed Partials to the destination 
	PartialList. Loris morphs Partials by interpolating frequency,
	amplitude, and bandwidth envelopes of corresponding Partials in 
	the source PartialLists. For more information about the Loris
	morphing algorithm, see the Loris website: 
	www.cerlsoundgroup.org/Loris/
 */
extern "C"
void morph( const PartialList * src0, const PartialList * src1, 
			const BreakpointEnvelope * ffreq, 
			const BreakpointEnvelope * famp, 
			const BreakpointEnvelope * fbw, 
			PartialList * dst )
{
	try 
	{
		ThrowIfNull((PartialList *) src0);
		ThrowIfNull((PartialList *) src1);
		ThrowIfNull((PartialList *) dst);
		ThrowIfNull((BreakpointEnvelope *) ffreq);
		ThrowIfNull((BreakpointEnvelope *) famp);
		ThrowIfNull((BreakpointEnvelope *) fbw);

		notifier << "morphing " << src0->size() << " Partials with " <<
					src1->size() << " Partials" << endl;
					
		//	make a Morpher object and do it:
		Morpher m( *ffreq, *famp, *fbw );
		m.morph( src0->begin(), src0->end(), src1->begin(), src1->end() );
				
		//	splice the morphed Partials into dst:
		dst->splice( dst->end(), m.partials() );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in morph(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in morph(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        synthesize        
/*
/*	Synthesize Partials in a PartialList at the given sample
	rate, and store the (floating point) samples in a SampleVector.
	The SampleVector is resized, if necessary, to hold as many
	samples as are needed for the complete synthesis of all the
	Partials in the PartialList. The SampleVector is not 
	cleared before synthesis, so newly synthesized samples are
	added to any previously computed samples in the SampleVector.
 */
extern "C"
void synthesize( const PartialList * partials, 
				 SampleVector * samples,  
				 double srate )
{
	try 
	{
		ThrowIfNull((PartialList *) partials);
		ThrowIfNull((SampleVector *) samples);

		notifier << "synthesizing " << partials->size() 
				 << " Partials at " << srate << " Hz" << endl;

		//	compute the duration:
		debugger << "computing duration..." << endl;
		double maxtime = 0.;
		PartialList::const_iterator it;
		for ( it = partials->begin(); it != partials->end(); ++it ) 
		{
			maxtime = std::max( maxtime, it->endTime() );
		}
		debugger << maxtime << " seconds" << endl;
		
		//	resize the Samplevector if necessary (pad the length
		//	to accomodate the fade-out at the end of the latest
		//	Partial):
		const long nsamps = long( srate * ( maxtime + Partial::FadeTime() ) );	
		if ( samples->size() < nsamps )
			samples->resize( nsamps, 0. );
		
		//	synthesize:
		Synthesizer synth( srate, samples->begin(), samples->end() );
		for ( it = partials->begin(); it != partials->end(); ++it ) 
		{
			synth.synthesize( *it );
		}
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in synthesize(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in synthesize(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

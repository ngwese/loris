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
 *	lorisExportSpc_pi.C
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
 *	This file contains the procedural interface for the Loris ExportSpc class.
 *
 * Kelly Fitz, 6 Dec 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#define LORIS_OPAQUE_POINTERS 0
#include "loris.h"
#include "lorisException_pi.h"

#include "ExportSpc.h"
#include "notifier.h"
#include "Partial.h"

using namespace Loris;

/* ---------------------------------------------------------------- */
/*		ExportSpc object interface
/*
/*	An ExportSpc represents a configuration of parameters for
	exporting a collection of Bandwidth-Enhanced partials to 
	an spc-format file for use with the Symbolic Sound Kyma
	System.

	In C++, an ExportSpc * is a Loris::ExportSpc *.
 */

/* ---------------------------------------------------------------- */
/*        createExportSpc
/*
/*	Construct a new ExportSpc instance configured from the 
	given MIDI note number. All other ExportSpc parameters
	are computed fromthe specified note number.
 */
extern "C"
ExportSpc * createExportSpc( double midiPitch )
{
	try 
	{
		debugger << "creating ExportSpc" << endl;
		return new ExportSpc( midiPitch );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in createExportSpc(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in createExportSpc(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return NULL;
}

/* ---------------------------------------------------------------- */
/*        destroyExportSpc        
/*
/*	Destroy this ExportSpc instance.
 */
extern "C"
void destroyExportSpc( ExportSpc * ptr_this )
{
	try 
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		
		debugger << "deleting ExportSpc" << endl;
		delete ptr_this;
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in destroyExportSpc(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in destroyExportSpc(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        exportSpc_write       
/*
/*	Export the given list of Partials to an spc file having the
	specified path (or name) according to the current configuration 
	of this ExportSpc instance.
 */
extern "C"
void exportSpc_write( ExportSpc * ptr_this, const char * path,
					  PartialList * partials )
{
	try 
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		ThrowIfNull((PartialList *) partials);
		notifier << "exporting spc partial data to " << path << endl;		
		ptr_this->write( path, *partials );
	}
	catch( Exception & ex ) 
	{
		std::string s("Loris exception in exportSpc_write(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		std::string s("std C++ exception in exportSpc_write(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        exportSpc_configure        
/*
/*	Set the MIDI note number (69.00 = A440) for this spc file,
	and recompute all other parameters to default values.			
 */
extern "C"
void exportSpc_configure( ExportSpc * ptr_this, double midiPitch )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		ptr_this->configure( midiPitch );
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_configure(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_configure(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        exportSpc_getMidiPitch       
/*
/*	Return the MIDI note number (69.00 = A440) for this spc file.
 */
extern "C"
double exportSpc_getMidiPitch( const ExportSpc * ptr_this )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		return ptr_this->midiPitch();
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_getMidiPitch(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_getMidiPitch(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        exportSpc_setMidiPitch        
/*
/*	Set the MIDI note number (69.00 = A440) for this 
	spc file. (Does not cause other parameters to be 
	recomputed.) 			
 */
extern "C"
void exportSpc_setMidiPitch( ExportSpc * ptr_this, double x )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		ptr_this->setMidiPitch( x );
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_setMidiPitch(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_setMidiPitch(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        exportSpc_getNumPartials       
/*
/*	Return the number of partials in spc file, may 
	be 32, 64, 128, or 256.
 */
extern "C"
int exportSpc_getNumPartials( const ExportSpc * ptr_this )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		return ptr_this->numPartials();
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_getNumPartials(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_getNumPartials(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        exportSpc_setNumPartials       
/*
/*	Set the number of partials in spc file, may 
	be 32, 64, 128, or 256.
 */
extern "C"
void exportSpc_setNumPartials( ExportSpc * ptr_this, int x )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		ptr_this->setNumPartials( x );
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_setNumPartials(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_setNumPartials(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        exportSpc_getRefLabel       
/*
/*	Return the label of the reference partial for this ExportSpc
	instance. A reference label of 1 indicates the fundamental.
	The reference label is used for filling in frequencies during 
	time gaps in other partials. 
 */
extern "C"
int exportSpc_getRefLabel( const ExportSpc * ptr_this )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		return ptr_this->refLabel();
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_getRefLabel(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_getRefLabel(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        exportSpc_setRefLabel      
/*
/*	Set the label of the reference partial for this ExportSpc
	instance. A reference label of 1 indicates the fundamental.
	The reference partial is used for filling in frequencies 
	during time gaps in other partials. 
 */
extern "C"
void exportSpc_setRefLabel( ExportSpc * ptr_this, int x )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		ptr_this->setRefLabel( x );
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_setRefLabel(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_setRefLabel(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        exportSpc_getEnhanced       
/*
/*	Return true if this spc file is in bandwidth-enhanced format,
	false if it is in pure sinusoidal format.
 */
extern "C"
int exportSpc_getEnhanced( const ExportSpc * ptr_this )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		return ptr_this->enhanced();
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_getEnhanced(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_getEnhanced(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        exportSpc_setEnhanced     
/*
/*	Set the type of spc file: true for bandwidth-enhanced format,
	false for pure sinusoidal format.
*/
extern "C"
void exportSpc_setEnhanced( ExportSpc * ptr_this, int boool )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		ptr_this->setEnhanced( boool );
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_setEnhanced(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_setEnhanced(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        exportSpc_getHop       
/*
/*	Return the frame duration (in seconds) for this spc file.
 */
extern "C"
double exportSpc_getHop( const ExportSpc * ptr_this )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		return ptr_this->hop();
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_getHop(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_getHop(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        exportSpc_setHop        
/*
/*	Set the frame duration (in seconds) for this spc file.
 */
extern "C"
void exportSpc_setHop( ExportSpc * ptr_this, double x )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		ptr_this->setHop( x );
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_setHop(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_setHop(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        exportSpc_getAttackThreshold       
/*
/*	Return the amplitude threshold for cropping the start of the 
	spc file. This is specified as a fraction of maximum amplitude 
	of the sound, with a value between 0.0 and 1.0. If the value is 
	0.0, this indicates no cropping at the start of the spc file.
 */
extern "C"
double exportSpc_getAttackThreshold( const ExportSpc * ptr_this )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		return ptr_this->attackThreshold();
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_getAttackThreshold(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_getAttackThreshold(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        exportSpc_setAttackThreshold        
/*
/*	Set the amplitude threshold for cropping the start of the spc 
	file. This is specified as a fraction of maximum amplitude of 
	the sound, with a value between 0.0 and 1.0.  Specify 0.0 for 
	no cropping of the start of the spc file.
 */
extern "C"
void exportSpc_setAttackThreshold( ExportSpc * ptr_this, double x )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		ptr_this->setAttackThreshold( x );
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_setAttackThreshold(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_setAttackThreshold(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        exportSpc_getStartFreqTime      
/*
/*	Return the time (in seconds) at which frequency in attack is 
	considered stable.  Frequencies before this time are modified 
	in the spc file to avoid real-time morphing artifacts when the 
	spc file is used in Kyma. This returns 0.0 if the spc file has 
	no modified attack frequencies.
 */
extern "C"
double exportSpc_getStartFreqTime( const ExportSpc * ptr_this )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		return ptr_this->startFreqTime();
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_getStartFreqTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_getStartFreqTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        exportSpc_setStartFreqTime       
/*
/*	Set the time (in seconds) at which frequency in attack is 
	considered stable.  Frequencies before this time are modified 
	in the spc file to avoid real-time morphing artifacts when the 
	spc file is used in Kyma. Specify 0.0 to avoid modified attack 
	frequencies.
 */
extern "C"
void exportSpc_setStartFreqTime( ExportSpc * ptr_this, double x )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		ptr_this->setStartFreqTime( x );
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_setStartFreqTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_setStartFreqTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        exportSpc_getEndTime      
/*
/*	Return the time (in seconds) at which the end of the spc file 
	is truncated. This returns 0.0 if the spc file is not truncate 
	at the end.
 */
extern "C"
double exportSpc_getEndTime( const ExportSpc * ptr_this )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		return ptr_this->endTime();
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_getEndTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_getEndTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        exportSpc_setEndTime       
/*
/*	Set the time (in seconds) to truncate the end of the spc file.
	Set this to the 0.0 (or, equivalently, to the last end time of 
	any partial in the sound) to avoid truncating the end of the 
	spc file.
 */
extern "C"
void exportSpc_setEndTime( ExportSpc * ptr_this, double x )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		ptr_this->setEndTime( x );
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_setEndTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_setEndTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        exportSpc_getEndApproachTime      
/*
/*	Return a value in seconds that indicates how long before the 
	end of the spc file the amplitude, frequency, and bandwidth 
	values are modified to make a gradual transition to the spectral 
	content at the end of the spc file.  This returns 0.0 if no such 
	modifications are done in the spc file.	
 */
extern "C"
double exportSpc_getEndApproachTime( const ExportSpc * ptr_this )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		return ptr_this->endApproachTime();
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_getEndApproachTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_getEndApproachTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        exportSpc_setEndApproachTime       
/*
/*	Set how long (in seconds) before the end of the spc file the 
	amplitude, frequency, and bandwidth values are to be modified 
	to make a gradual transition to the spectral content at the 
	end of the spc file. Specify 0.0 to avoid these modifications 
	in the spc file.	
 */
extern "C"
void exportSpc_setEndApproachTime( ExportSpc * ptr_this, double x )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		ptr_this->setEndApproachTime( x );
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_setEndApproachTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_setEndApproachTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

/* ---------------------------------------------------------------- */
/*        exportSpc_getMarkerTime      
/*
/*	Return the time (in seconds) at which a marker is inserted in 
	the spc file. This returns 0.0 if no marker is inserted into 
	the spc file.
 */
extern "C"
double exportSpc_getMarkerTime( const ExportSpc * ptr_this )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		return ptr_this->markerTime();
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_getMarkerTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_getMarkerTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/*        exportSpc_setMarkerTime       
/*
/*	Set the time (in seconds) at which a marker is inserted in the 
	spc file. Only one marker is inserted into the spc file; it will
	be inserted at the time specified with setMarkerTime().  
	Specify 0.0 to avoid inserting a marker into the spc file.
 */
extern "C"
void exportSpc_setMarkerTime( ExportSpc * ptr_this, double x )
{
	try  
	{
		ThrowIfNull((ExportSpc *) ptr_this);
		ptr_this->setMarkerTime( x );
	}
	catch( Exception & ex )  
	{
		std::string s("Loris exception in exportSpc_setMarkerTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
	catch( std::exception & ex )  
	{
		std::string s("std C++ exception in exportSpc_setMarkerTime(): " );
		s.append( ex.what() );
		handleException( s.c_str() );
	}
}

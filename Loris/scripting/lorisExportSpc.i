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
 *	lorisExportSpc.i
 *
 *	SWIG interface file describing the Loris::ExportSpc class.
 *	Include this file in loris.i to include the ExportSpc class
 *	interface in the scripting module. (Can be used with the 
 *	-shadow option to SWIG to build an ExportSpc class in the 
 *	Python interface.) This file does not support exactly the 
 *	public interface of the C++ class, but has been modified to
 *	better support SWIG and scripting languages.
 *
 *
 * Kelly Fitz, 6 Dec 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
%{
#include "ExportSpc.h"
using Loris::ExportSpc;

//	for procedural interface construction and 
//	destruction, see comment below:
#define LORIS_OPAQUE_POINTERS 0
#include "loris.h"
%}


// ---------------------------------------------------------------------------
//	class ExportSpc
//	
/*	An ExportSpc represents a configuration of parameters for
	exporting a collection of Bandwidth-Enhanced partials to 
	an spc-format file for use with the Symbolic Sound Kyma
	System.
 */
class ExportSpc
{
public:
//	construction:
//
//	Mac ONLY problem:
//	use the construction and destruction functions in the 
//	procedural interface until I can determine why deleting
//	objects with destructors defined out of line (in the Loris
//	DLL) cause the Macintosh to crash. Using the procedural 
//	interface causes the objects with out of line destructors
//	to be constructed and destructed in the DLL, instead of 
//	across DLL boundaries, which might make a difference on
//	the Mac.
//
%addmethods 
{
	ExportSpc( double midiPitch )
	{
		return createExportSpc( midiPitch );
	}
	/*	Construct a new ExportSpc instance configured from the 
		given MIDI note number. All other ExportSpc parameters
		are computed fromthe specified note number.
	 */
	~ExportSpc( void )
	{
		destroyExportSpc( self );
	}
	/*	Destroy this ExportSpc instance.
	 */
	 
//	writing:
	void write( const char * filename, const PartialList * partials )
	{
		self->write( filename, *partials );
	}
	/*	Export the given list of Partials to an spc file having the
		specified path (or name) according to the current configuration 
		of this ExportSpc instance.
	 */
		
}

//	configuration:
	void configure( double midiPitch );
	/*	Set the MIDI note number (69.00 = A440) for this spc file,
		and recompute all other parameters to default values.			
	 */
	
//	parameter access:
	double midiPitch( void ) const;
	/*	Return the MIDI note number (69.00 = A440) for this spc file.
	 */
	int numPartials( void ) const;
	/*	Return the number of partials in spc file, may 
		be 32, 64, 128, or 256.
	 */
	int refLabel( void ) const;
	/*	Return the label of the reference partial for this ExportSpc
		instance. A reference label of 1 indicates the fundamental.
		The reference label is used for filling in frequencies during 
		time gaps in other partials. 
	 */
	int enhanced( void ) const;
	/*	Return true if this spc file is in bandwidth-enhanced format,
		false if it is in pure sinusoidal format.
	 */
	double hop( void ) const;
	/*	Return the frame duration (in seconds) for this spc file.
	 */
	double attackThreshold( void ) const;
	/*	Return the amplitude threshold for cropping the start of the 
		spc file. This is specified as a fraction of maximum amplitude 
		of the sound, with a value between 0.0 and 1.0. If the value is 
		0.0, this indicates no cropping at the start of the spc file.
	 */
	double startFreqTime( void ) const;
	/*	Return the time (in seconds) at which frequency in attack is 
		considered stable.  Frequencies before this time are modified 
		in the spc file to avoid real-time morphing artifacts when the 
		spc file is used in Kyma. This returns 0.0 if the spc file has 
		no modified attack frequencies.
	 */
	double endTime( void ) const;
	/*	Return the time (in seconds) at which the end of the spc file 
		is truncated. This returns 0.0 if the spc file is not truncate 
		at the end.
	 */
	double endApproachTime( void ) const;
	/*	Return a value in seconds that indicates how long before the 
		end of the spc file the amplitude, frequency, and bandwidth 
		values are modified to make a gradual transition to the spectral 
		content at the end of the spc file.  This returns 0.0 if no such 
		modifications are done in the spc file.	
	 */
	double markerTime( void ) const;
	/*	Return the time (in seconds) at which a marker is inserted in 
		the spc file. This returns 0.0 if no marker is inserted into 
		the spc file.
	 */
							
//	parameter mutation:
	void setMidiPitch( double x );
	/*	Set the MIDI note number (69.00 = A440) for this 
		spc file. (Does not cause other parameters to be 
		recomputed.) 			
	 */
	void setNumPartials( int x );
	/*	Set the number of partials in spc file, may 
		be 32, 64, 128, or 256.
	 */
	void setRefLabel( int x );
	/*	Set the label of the reference partial for this ExportSpc
		instance. A reference label of 1 indicates the fundamental.
		The reference partial is used for filling in frequencies 
		during time gaps in other partials. 
	 */
	void setEnhanced( int x );
	/*	Set the type of spc file: true for bandwidth-enhanced format,
		false for pure sinusoidal format.
	*/
	void setHop( double x );
	/*	Set the frame duration (in seconds) for this spc file.
	 */

	void setAttackThreshold( double x );
	/*	Set the amplitude threshold for cropping the start of the spc 
		file. This is specified as a fraction of maximum amplitude of 
		the sound, with a value between 0.0 and 1.0.  Specify 0.0 for 
		no cropping of the start of the spc file.
	 */
	void setStartFreqTime( double x );
	/*	Set the time (in seconds) at which frequency in attack is 
		considered stable.  Frequencies before this time are modified 
		in the spc file to avoid real-time morphing artifacts when the 
		spc file is used in Kyma. Specify 0.0 to avoid modified attack 
		frequencies.
	 */
	void setEndTime( double x );
	/*	Set the time (in seconds) to truncate the end of the spc file.
		Set this to the 0.0 (or, equivalently, to the last end time of 
		any partial in the sound) to avoid truncating the end of the 
		spc file.
	 */
	void setEndApproachTime( double x );
	/*	Set how long (in seconds) before the end of the spc file the 
		amplitude, frequency, and bandwidth values are to be modified 
		to make a gradual transition to the spectral content at the 
		end of the spc file. Specify 0.0 to avoid these modifications 
		in the spc file.	
	 */
	void setMarkerTime( double x );
	/*	Set the time (in seconds) at which a marker is inserted in the 
		spc file. Only one marker is inserted into the spc file; it will
		be inserted at the time specified with setMarkerTime().  
		Specify 0.0 to avoid inserting a marker into the spc file.
	 */
};	//	end of class ExportSpc

#ifndef INCLUDE_EXPORTSPC_H
#define INCLUDE_EXPORTSPC_H
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
 * ExportSpc.h
 *
 * Definition of class ExportSpc, which exports spc files for
 * real-time synthesis in Kyma.
 *
 * Lippold Haken, 6 Nov 1999, 14 Nov 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include <Loris_prefix.h>
#include <list>
#include <string>
#include <iosfwd>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

class Partial;

// ---------------------------------------------------------------------------
//	class ExportSpc
//	
class ExportSpc
{
//	-- export parameters --
	double _midiPitch;		//	note number (69.00 = A440) for spc file;
							//	this is the core parameter, others are, by default,
							//	computed from this one
	int _numPartials;		//	number of partials in spc file, may be 32, 64, 128, or 256
	int _refLabel;			// 	label for reference partial (1 indicates fundamental),
							// 	used for filling in frequencies during time gaps in other partials
	int _enhanced;			//	true for bandwidth-enhanced spc file format,
							//	false for pure sinusoidal spc file format
	double _hop;			//	in seconds, frame duration for spc file
	double _attackThreshold;//	fraction of maximum amplitude of the sound (values 0.0 to 1.0), 
							//	this provides an amplitude threshold for cropping the start of 
							//	the spc file, 0.0 indicates no cropping
	double _startFreqTime;	//	in seconds, this indicates time at which frequency in attack is
							//	considered stable, frequencies before this time are modified in
							//	the spc file to avoid real-time morphing artifacts, 0.0 indicates
							//	the spc file is to have no attack frequencies modified
	double _endTime;		//	in seconds, this indicates the time at which to truncate the end
							//	of the spc file, 0.0 indicates no truncation
	double _endApproachTime;//	in seconds, this indicates how long before _endTime the
							//	amplitude, frequency, and bandwidth values are to be modified to
							//	make a gradual transition to the spectral content at _endTime,
							//	0.0 indicates no such modifications are to be done
	double _markerTime;		//	in seconds, this indicates time at which a marker is inserted in the
							//	spc file, 0.0 indicates no marker is desired

			
//	-- public interface --
public:
//	construction:
	explicit ExportSpc( double midiPitch );
	/*	Construct a new ExportSpc instance configured from the 
		given MIDI note number. All other ExportSpc parameters
		are computed fromthe specified note number.
	 */
	~ExportSpc( void );
	/*	Destroy this ExportSpc instance.
	 */

//	writing:
	void write( const std::string & filename, const std::list<Partial> & partials );
	/*	Export the given list of Partials to an spc file having the
		specified path (or name) according to the current configuration 
		of this ExportSpc instance.
	 */
	
	void write( std::ostream & file, const std::list<Partial> & partials );
	/*	Export the given list of Partials to an spc file using the
		specified ostream according to the current configuration 
		of this ExportSpc instance.
	 */
	
//	configuration:
	void configure( double midiPitch );
	/*	Set the MIDI note number (69.00 = A440) for this spc file,
		and recompute all other parameters to default values.			
	 */
	
//	parameter access:
	double midiPitch( void ) const { return _midiPitch; }
	/*	Return the MIDI note number (69.00 = A440) for this spc file.
	 */
	int numPartials( void ) const { return _numPartials; }
	/*	Return the number of partials in spc file, may 
		be 32, 64, 128, or 256.
	 */
	int refLabel( void ) const { return _refLabel; }
	/*	Return the label of the reference partial for this ExportSpc
		instance. A reference label of 1 indicates the fundamental.
		The reference label is used for filling in frequencies during 
		time gaps in other partials. 
	 */
	int enhanced( void ) const { return _enhanced; }
	/*	Return true if this spc file is in bandwidth-enhanced format,
		false if it is in pure sinusoidal format.
	 */
	double hop( void ) const { return _hop; }
	/*	Return the frame duration (in seconds) for this spc file.
	 */
	double attackThreshold( void ) const { return _attackThreshold; }
	/*	Return the amplitude threshold for cropping the start of the 
		spc file. This is specified as a fraction of maximum amplitude 
		of the sound, with a value between 0.0 and 1.0. If the value is 
		0.0, this indicates no cropping at the start of the spc file.
	 */
	double startFreqTime( void ) const { return _startFreqTime;}
	/*	Return the time (in seconds) at which frequency in attack is 
		considered stable.  Frequencies before this time are modified 
		in the spc file to avoid real-time morphing artifacts when the 
		spc file is used in Kyma. This returns 0.0 if the spc file has 
		no modified attack frequencies.
	 */
	double endTime( void ) const { return _endTime; }
	/*	Return the time (in seconds) at which the end of the spc file 
		is truncated. This returns 0.0 if the spc file is not truncate 
		at the end.
	 */
	double endApproachTime( void ) const { return _endApproachTime; }
	/*	Return a value in seconds that indicates how long before the 
		end of the spc file the amplitude, frequency, and bandwidth 
		values are modified to make a gradual transition to the spectral 
		content at the end of the spc file.  This returns 0.0 if no such 
		modifications are done in the spc file.	
	 */
	double markerTime( void ) const { return _markerTime; }
	/*	Return the time (in seconds) at which a marker is inserted in 
		the spc file. This returns 0.0 if no marker is inserted into 
		the spc file.
	 */
							
//	parameter mutation:
	void setMidiPitch( double x ) { _midiPitch = x; }
	/*	Set the MIDI note number (69.00 = A440) for this 
		spc file. (Does not cause other parameters to be 
		recomputed.) 			
	 */
	void setNumPartials( int x ) { _numPartials = x; }
	/*	Set the number of partials in spc file, may 
		be 32, 64, 128, or 256.
	 */
	void setRefLabel( int x ) { _refLabel = x; }
	/*	Set the label of the reference partial for this ExportSpc
		instance. A reference label of 1 indicates the fundamental.
		The reference partial is used for filling in frequencies 
		during time gaps in other partials. 
	 */
	void setEnhanced( int x ) { _enhanced = x; }
	/*	Set the type of spc file: true for bandwidth-enhanced format,
		false for pure sinusoidal format.
	*/
	void setHop( double x ) { _hop = x; }
	/*	Set the frame duration (in seconds) for this spc file.
	 */

	void setAttackThreshold( double x ) { _attackThreshold = x; }
	/*	Set the amplitude threshold for cropping the start of the spc 
		file. This is specified as a fraction of maximum amplitude of 
		the sound, with a value between 0.0 and 1.0.  Specify 0.0 for 
		no cropping of the start of the spc file.
	 */
	void setStartFreqTime( double x ) { _startFreqTime = x; }
	/*	Set the time (in seconds) at which frequency in attack is 
		considered stable.  Frequencies before this time are modified 
		in the spc file to avoid real-time morphing artifacts when the 
		spc file is used in Kyma. Specify 0.0 to avoid modified attack 
		frequencies.
	 */
	void setEndTime( double x ) { _endTime = x; }
	/*	Set the time (in seconds) to truncate the end of the spc file.
		Set this to the 0.0 (or, equivalently, to the last end time of 
		any partial in the sound) to avoid truncating the end of the 
		spc file.
	 */
	void setEndApproachTime( double x ) { _endApproachTime = x; }
	/*	Set how long (in seconds) before the end of the spc file the 
		amplitude, frequency, and bandwidth values are to be modified 
		to make a gradual transition to the spectral content at the 
		end of the spc file. Specify 0.0 to avoid these modifications 
		in the spc file.	
	 */
	void setMarkerTime( double x ) { _markerTime = x; }
	/*	Set the time (in seconds) at which a marker is inserted in the 
		spc file. Only one marker is inserted into the spc file; it will
		be inserted at the time specified with setMarkerTime().  
		Specify 0.0 to avoid inserting a marker into the spc file.
	 */
	
private:

//	-- unimplemented --
	ExportSpc( const ExportSpc & other );
	ExportSpc  & operator = ( const ExportSpc & rhs );
};	//	end of class ExportSpc


#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif /* ndef INCLUDE_EXPORTSPC_H */

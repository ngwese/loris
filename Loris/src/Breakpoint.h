#ifndef INCLUDE_BREAKPOINT_H
#define INCLUDE_BREAKPOINT_H
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
 * Breakpoint.h
 *
 * Definition of class Loris::Breakpoint.
 *
 * Kelly Fitz, 16 Aug 99
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

//	begin namespace
namespace Loris {


// ---------------------------------------------------------------------------
//	class Breakpoint
//
//	Class Breakpoint represents a single breakpoint in the
//	Partial parameter (frequency, amplitude, bandwidth) envelope.
//	Instantaneous phase is also stored, but is only used at the onset of 
//	a partial, or when it makes a transition from zero to nonzero amplitude.
//	
//	Loris Partials represent reassigned bandwidth-enhanced model components.
//	A Partial consists of a chain of Breakpoints describing the time-varying
//	frequency, amplitude, and bandwidth (noisiness) of the component.
//	For more information about Reassigned Bandwidth-Enhanced 
//	Analysis and the Reassigned Bandwidth-Enhanced Additive Sound 
//	Model, refer to the Loris website: 
//	www.cerlsoundgroup.org/Loris/.
//	
//	Breakpoint is a leaf class, do not subclass.
//
class Breakpoint
{
//	-- instance variables --
	double _frequency;	//	hertz
	double _amplitude;	//	absolute
	double _bandwidth;	//	fraction of total energy that is noise energy
	double _phase;		//	radians
	
//	-- public Breakpoint interface --
public:
//	-- construction --
//	(use compiler-generated destructor, copy, and assign)
	/*	Construct a new Breakpoint with all parameters initialized to 0.
	 */
 	Breakpoint( void );	//	needed for STL containability
	/*	Construct a new Breakpoint with frequency, amplitude, and bandwidth
		initialized to f, a, and b, respectively, and phase initialized to
		p, if specified, or 0 otherwise.
	 */
 	Breakpoint( double f, double a, double b, double p = 0. );
	
//	-- comparison --
	/*	Comparison operator: return true if the frequency, amplitude, 
		bandwidth, and phase of this Breakpoint are equal to those of
		the Breakpoint rhs. Otherwise, return false.
	 */
 	bool operator==( const Breakpoint & rhs ) const;

//	-- attribute access --
	/*	Return the amplitude of this Breakpoint.
	 */
 	double amplitude( void ) const { return _amplitude; }
	/*	Return the bandwidth (noisiness) coefficient of this Breakpoint.
	 */
 	double bandwidth( void ) const { return _bandwidth; }
	/*	Return the frequency of this Breakpoint.
	 */
 	double frequency( void ) const { return _frequency; }
	/*	Return the phase of this Breakpoint.
	 */
 	double phase( void ) const { return _phase; }
	
//	-- attribute mutation --
	/*	Set the amplitude of this Breakpoint.
	 */
 	void setAmplitude( double x ) { _amplitude = x; }
	/*	Set the bandwidth (noisiness) coefficient of this Breakpoint.
	 */
 	void setBandwidth( double x ) { _bandwidth = x; }
	/*	Set the frequency of this Breakpoint.
	 */
 	void setFrequency( double x ) { _frequency = x; }
	/*	Set the phase of this Breakpoint.
	 */
 	void setPhase( double x ) { _phase = x; }
	
};	//	end of class Breakpoint

}	//	end of namespace Loris

#endif /* ndef INCLUDE_BREAKPOINT_H */

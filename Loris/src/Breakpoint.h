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

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif


// ---------------------------------------------------------------------------
//	class Breakpoint
//
//	Definition of class of objects representing a single breakpoint in the
//	Partial parameter (frequency, amplitude, bandwidth) envelope.
//	Instantaneous phase is also stored, but is only used at the onset of 
//	a partial, or when it makes a transition from zero to nonzero amplitude.
//
//	Loris Partials represent reassigned bandwidth-enhanced model components.
//	A Partial consists of a chain of Breakpoints describing the time-varying
//	frequency, amplitude, and bandwidth (noisiness) of the component.
//
//	Leaf class, do not subclass.
//
class Breakpoint
{
//	-- envelope parameters --
	double _frequency;	//	hertz
	double _amplitude;	//	absolute
	double _bandwidth;	//	fraction of total energy that is noise energy
	double _phase;		//	radians
	
//	-- public Breakpoint interface --
public:
//	construction:
//	(use compiler-generated destructor, copy, and assign)
	Breakpoint( void );	//	needed for STL containability
	Breakpoint( double f, double a, double b, double p = 0. );
	
//	comparison:
	bool operator==( const Breakpoint & rhs ) const;

//	attribute access:
	double frequency( void ) const { return _frequency; }
	double amplitude( void ) const { return _amplitude; }
	double bandwidth( void ) const { return _bandwidth; }
	double phase( void ) const { return _phase; }
	
//	attribute mutation:
	void setFrequency( double x ) { _frequency = x; }
	void setAmplitude( double x ) { _amplitude = x; }
	void setBandwidth( double x ) { _bandwidth = x; }
	void setPhase( double x ) { _phase = x; }
	
//	add noise (bandwidth) energy:
//	should this really be part of the interface?
	void addNoise( double x );

};	//	end of class Breakpoint

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif /* ndef INCLUDE_BREAKPOINT_H */

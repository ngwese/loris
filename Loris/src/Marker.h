#ifndef INCLUDE_MARKER_H
#define INCLUDE_MARKER_H
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
 * Marker.h
 *
 * Definition of classes Marker and MarkerContainer representing labeled
 * time points or temporal features in imported and exported data. Used by 
 * file I/O classes AiffFile, SdifFile, and SpcFile.
 *
 * Kelly Fitz, 8 Jan 2003 
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <functional>
#include <string>
#include <vector>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	class Marker
//
//	Class Marker represents a labeled time point in a set of Partials
//	or a vector of samples. Collections of Markers (see the MarkerContainer
//	definition below) are held by the File I/O classes in Loris (AiffFile,
//	SdifFile, and SpcFile) to identify temporal features in imported
//	and exported data.
//
class Marker
{
//	-- implementation --
	double m_time;
	std::string m_name;
			
//	-- public interface --
public:
//	-- construction --
	Marker( void );
	/*	Default constructor - initialize a Marker at time zero with no label.
	 */
	 
	Marker( double t, const std::string & s );
	/*	Initialize a Marker with the specified time (in seconds) and name.
	 */
	 
	Marker( const Marker & other );
	/*	Initialize a Marker that is an exact copy of another Marker, that is,
		having the same time and name.
	 */
	
	Marker & operator=( const Marker & rhs );
	/*	Make this Marker an exact copy, having the same time and name, 
		as the Marker rhs.
	 */
	 
//	-- comparison --
	bool operator< ( const Marker & rhs ) const;
	/*	Return true if this Marker must appear earlier than rhs in a sorted
		collection of Markers, and false otherwise. (Markers are sorted by time.)
	 */
	 
//	-- access --
	std::string & name( void );
	const std::string & name( void ) const;
	/*	Return a reference (or const reference) to the name string
		for this Marker.
	 */
	 
	double time( void ) const;
	/*	Return the time (in seconds) associated with this Marker.
	 */
	 
//	-- mutation --
	void setName( const std::string & s );
	/*	Set the name of the Marker.
	 */
	 
	void setTime( double t );
	/* 	Set the time (in seconds) associated with this Marker.
	 */

//	-- comparitors --
	/*	Comparitor (binary) functor returning true if its first Marker
		argument should appear before the second in a range sorted
		by Marker name.
	 */
	struct sortByName : 
		public std::binary_function< const Marker, const Marker, bool >
	{
		bool operator()( const Marker & lhs, const Marker & rhs ) const 
			{ return lhs.name() < rhs.name(); }
	};
	
	
	/*	Comparitor (binary) functor returning true if its first Marker
		argument should appear before the second in a range sorted
		by Marker time.
	 */
	struct sortByTime : 
		public std::binary_function< const Marker, const Marker, bool >
	{
		bool operator()( const Marker & lhs, const Marker & rhs ) const 
			{ return lhs.time() < rhs.time(); }
	};
	
	
};	//	end of class Marker

}	//	end of namespace Loris

#endif /* ndef INCLUDE_MARKER_H */

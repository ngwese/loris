/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2004 by Kelly Fitz and Lippold Haken
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
 *	test_Sdif.C
 *
 *	Unit tests for SDIF import and export.
 *
 *
 * Kelly Fitz, 30 July 2003
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <Breakpoint.h>
#include <Partial.h>
#include <Exception.h>
#include <SdifFile.h>

#include <cmath>
#include <iostream>

using namespace Loris;
using namespace std;

const double Pi = 3.14159265358979324;


// --- macros ---

//	define this to see pages and pages of spew
// #define VERBOSE
#ifdef VERBOSE									
	#define TEST(invariant)									\
		do {													\
			std::cout << "TEST: " << #invariant << endl;		\
			Assert( invariant );								\
			std::cout << " PASS" << endl << endl;			\
		} while (false)
	
	#define TEST_VALUE( expr, val )									\
		do {															\
			std::cout << "TEST: " << #expr << "==" << (val) << endl;\
			Assert( (expr) == (val) );								\
			std::cout << "  PASS" << endl << endl;					\
		} while (false)
#else
	#define TEST(invariant)					\
		do {									\
			Assert( invariant );				\
		} while (false)
	
	#define TEST_VALUE( expr, val )			\
		do {									\
			Assert( (expr) == (val) );		\
		} while (false)
#endif	
	
static bool float_equal( double x, double y )
{
	#ifdef VERBOSE
	cout << "\t" << x << " == " << y << " ?" << endl;
	#endif
	#define EPSILON .0000001
	if ( std::fabs(x) > 0. )
		return std::fabs((x-y)/x) < EPSILON;
	else
		return std::fabs(x-y) < EPSILON;
}

#define SAME_PARAM_VALUES(x,y) TEST( float_equal((x),(y)) )

// ----------- test_simplePartial -----------
//
static void test_simplePartial( void )
{
	std::cout << "\t--- testing import/export identity using a simple Partial... ---\n\n";

	//	Fabricate a Partial:
	Partial p;
	double times[] = {0.001, 0.003, 0.005, 0.01, 0.21, 0.5};
	for ( int i = 0; i < 6; ++i )
	{
		double t = times[i];
		Breakpoint b( 100 + (10*t), t, t, t );
		p.insert( t, b );
	}
	PartialList l;
	l.push_back( p );

	// 	export and import:
	// SdifFile::Export( "tmp.sdif", l );
	SdifFile fout( l.begin(), l.end() );
	fout.write( "tmp.sdif" );
	SdifFile f( "tmp.sdif" );
	Partial p2 = f.partials().front();

	//	compare:
	TEST( p.numBreakpoints() == p2.numBreakpoints() );	
	Partial::iterator it1 = p.begin(), it2 = p2.begin();
	while ( it1 != p.end() )
	{
		Breakpoint bp1 = it1.breakpoint();
		Breakpoint bp2 = it2.breakpoint();

		SAME_PARAM_VALUES( it1.time(), it2.time() );
		SAME_PARAM_VALUES( bp1.frequency(), bp2.frequency() );
		SAME_PARAM_VALUES( bp1.amplitude(), bp2.amplitude() );
		SAME_PARAM_VALUES( bp1.phase(), bp2.phase() );
		SAME_PARAM_VALUES( bp1.bandwidth(), bp2.bandwidth() );

		++it1;
		++it2;
	}
}

// ----------- main -----------
//
int main( )
{
	std::cout << "Unit test for SdifFile class." << endl;
	std::cout << "Relies on Breakpoint, Partial, PartialList and Partial::iterator." << endl << endl;
	std::cout << "Built: " << __DATE__ << endl << endl;
	
	try 
	{
		test_simplePartial();
	}
	catch( Exception & ex ) 
	{
		cout << "Caught Loris exception: " << ex.what() << endl;
		return 1;
	}
	catch( std::exception & ex ) 
	{
		cout << "Caught std C++ exception: " << ex.what() << endl;
		return 1;
	}	
	
	//	return successfully
	cout << "SdifFile passed all tests." << endl;
	return 0;
}



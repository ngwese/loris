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
 *	test_Distiller.C
 *
 *	Unit tests for Distiller class. Relies on Breakpoint,
 *	Partial, PartialList and Loris Exceptions. Build with
 *	Partial.C, Breakpoint.C, Exception.C, Distiller.C 
 *	and Notifier.C.
 *
 * Kelly Fitz, 17 April 2003
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include <Distiller.h>
#include <Exception.h>
#include <Partial.h>
#include <PartialList.h>

#include <cmath>
#include <iostream>

using namespace Loris;
using namespace std;

// --- macros ---

#define TEST(invariant)									\
	do {												\
		std::cout << "TEST: " << #invariant << endl;	\
		Assert( invariant );							\
		std::cout << " PASS" << endl << endl;			\
	} while (false)

#define TEST_VALUE( expr, val )									\
	do {														\
		std::cout << "TEST: " << #expr << "==" << (val) << endl;\
		Assert( (expr) == (val) );								\
		std::cout << "  PASS" << endl << endl;					\
	} while (false)
	
	
static bool float_equal( double x, double y )
{
	cout << "\t" << x << " == " << y << " ?" << endl;
	#define EPSILON .0000001
	if ( std::fabs(x) > 0. )
		return std::fabs((x-y)/x) < EPSILON;
	else
		return std::fabs(x-y) < EPSILON;
}

#define SAME_PARAM_VALUES(x,y) TEST( float_equal((x),(y)) )

// ----------- test_distill_manylabels -----------
//
static void test_distill_manylabels( void )
{
	std::cout << "\t--- testing distill on "
				 "Partials having different labels... ---\n\n";

	//	Fabricate several Partials with different labels
	//	and distill them. Verify that Partials having
	//	different labels are not combined.
	Partial p1;
	p1.insert( 0.1, Breakpoint( 100, 0.1, 0, 0 ) );
	p1.insert( 1.1, Breakpoint( 110, 0.2, 0.2, .1 ) );
	p1.setLabel( 1 );
	
	Partial p2;
	p2.insert( 0.2, Breakpoint( 200, 0.1, 0, 0 ) );
	p2.insert( 1.2, Breakpoint( 210, 0.2, 0.2, .1 ) );
	p2.setLabel( 2 );
	
	Partial p3;
	p3.insert( 0.3, Breakpoint( 300, 0.1, 0, 0 ) );
	p3.insert( 1.3, Breakpoint( 310, 0.2, 0.2, .1 ) );
	p3.setLabel( 3 );
	
	PartialList l;
	l.push_back( p1 );
	l.push_back( p3 );
	l.push_back( p2 );
	
	Distiller d;
	d.distill( l );
	
	//	compare Partials (distilled Partials
	//	should be in label order):
	TEST( l.size() == 3 );
	PartialList::iterator it = l.begin();
	TEST( *it == p1 );
	++it;
	TEST( *it == p2 );
	++it;
	TEST( *it == p3 );
}

// ----------- test_distill_nonoverlapping -----------
//
static void test_distill_nonoverlapping( void )
{
	std::cout << "\t--- testing distill on "
				 "non-overlapping Partials... ---\n\n";
				 
	//	Fabricate three non-overlapping Partials, give
	//	them all the same label, and distill them. Also
	//	add a fourth Partial with a different label, verify
	//	that it remains unaffacted.
	
	
}

// ----------- test_distill_overlapping2 -----------
//
static void test_distill_nonoverlapping2( void )
{
	std::cout << "\t--- testing distill on two "
				 "temporally-overlapping Partials... ---\n\n";

	//	Fabricate two Partials, overlapping temporally, give
	//	them the same label, and distill them.
	
}

// ----------- test_distill_overlapping3 -----------
//
static void test_distill_nonoverlapping3( void )
{
	std::cout << "\t--- testing distill on three "
				 "temporally-overlapping Partials... ---\n\n";

	//	Fabricate three Partials, overlapping temporally, give
	//	them the same label, and distill them.
	
}

// ----------- test_collate -----------
//
static void test_collate( void )
{
	std::cout << "\t--- testing collate on three "
				 "temporally-overlapping Partials... ---\n\n";

	//	Fabricate three Partials, overlapping temporally, 
	//	leave them unlabeled, and distill (collate) them.
	
}


// ----------- main -----------
//
int main( )
{
	std::cout << "Unit test for Distiller class." << endl;
	std::cout << "Relies on Breakpoint and Partial." << endl << endl;
	std::cout << "Run: " << __DATE__ << endl << endl;
	
	try 
	{
		test_distill_manylabels();
		test_distill_nonoverlapping();
		test_distill_nonoverlapping2();
		test_distill_nonoverlapping3();
		test_collate();
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
	cout << "Distiller passed all tests." << endl;
	return 0;
}



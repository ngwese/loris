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
 *	test_Morpher.C
 *
 *	Unit test for Morpher class. Relies on Partial, Breakpoint, and BreakpointEnvelope,
 *	and Loris Exceptions.
 *
 * Kelly Fitz, 22 May 2002
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "Breakpoint.h"
#include "BreakpointEnvelope.h"
#include "Exception.h"
#include "Morpher.h"
#include "Partial.h"

#include <cmath>
#include <iostream>

using namespace Loris;
using namespace std;

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
	bool ret = false;
	if ( std::fabs(x) > 0. )
	{
		ret = std::fabs((x-y)/x) < EPSILON;
	}
	else
	{
		ret = std::fabs(x-y) < EPSILON;
	}
	if ( !ret )
	{
		cout << "\tFAILED: " << x << " != " << y << " !" << endl;
	}
	return ret;
}

//	function to do smarter phase interpolation, like the morpher does
static double 
interpolate_phases( double phi0, double phi1, double alpha )
{
	const double Pi = 3.14159265358979324;

	//	try to wrap the phase so that they are
	//	as similar as possible:
	while ( ( phi0 - phi1 ) > Pi )
	{
		phi0 -= 2 * Pi;
	}
	while ( ( phi1 - phi0 ) > Pi )
	{
		phi0 += 2 * Pi;
	}
	return std::fmod( (alpha * phi1) + ((1.-alpha) * phi0), 2 * Pi );
}

//	define a amplitude shaping parameter that gives nearly linear morphs
//	(the non-linear ones are too hard to predict and test)
const double ALMOSTLINEAR = 1E5;


int main( )
{
	std::cout << "Unit test for Morpher class." << endl;
	std::cout << "Relies on Partial, Breakpoint, and BreakpointEnvelope." << endl << endl;
	std::cout << "Built: " << __DATE__ << endl << endl;

	try 
	{
		//	construct Morphing envelopes:
		BreakpointEnvelope fenv, aenv, bwenv, otherenv;
		
		//	frequency envelope: (0,0), (.5, 1), (1,1)
		const int NUM_ENVPTS = 3;
		const double MENV_TIMES[] = {0, .5, 1};
		const double FENV_WEIGHTS[] = {0, 1, 1};
		for (int i = 0; i < NUM_ENVPTS; ++i )
			fenv.insertBreakpoint( MENV_TIMES[i], FENV_WEIGHTS[i] );
		
		//	amplitude envelope: (0,1), (.5, 0), (1,0)
		const double AENV_WEIGHTS[] = {1, 0, 0};
		for (int i = 0; i < NUM_ENVPTS; ++i )
			aenv.insertBreakpoint( MENV_TIMES[i], AENV_WEIGHTS[i] );
		
		//	bandwidth envelope: (0,0), (.5, 1), (1,0)
		const double BWENV_WEIGHTS[] = {0, 1, 0};
		for (int i = 0; i < NUM_ENVPTS; ++i )
			bwenv.insertBreakpoint( MENV_TIMES[i], BWENV_WEIGHTS[i] );
			
		//	construct Morpher:
		Morpher testM( fenv, aenv, bwenv );
		testM.setAmplitudeShape( ALMOSTLINEAR );
		
		
		/*                                                */
		/*************** envelope tests *******************/
		/*                                                */
		
		//	check the envelopes at several times to verify that the
		//	morphing envelopes got stored correctly:
		#define SAME_ENV_VALUES(x,y) TEST( float_equal((x),(y)) )
		SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.3), fenv.valueAt(.3) );
		SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.6), fenv.valueAt(.6) );
		SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.9), fenv.valueAt(.9) );
		
		SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.3), aenv.valueAt(.3) );
		SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.6), aenv.valueAt(.6) );
		SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.9), aenv.valueAt(.9) );
		
		SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.3), bwenv.valueAt(.3) );
		SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.6), bwenv.valueAt(.6) );
		SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.9), bwenv.valueAt(.9) );
		
		//	change each envelope, and verify that the changes are
		//	correctly registered:
		//	other envelope: (0,.4), (.5, .2), (1,.3)
		const double OTHERENV_WEIGHTS[] = {.4, .2, .3};
		for (int i = 0; i < NUM_ENVPTS; ++i )
			otherenv.insertBreakpoint( MENV_TIMES[i], OTHERENV_WEIGHTS[i] );
		
		//	change:	
		testM.setFrequencyFunction( otherenv );
		
		//	verify:
		SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.3), otherenv.valueAt(.3) );
		SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.6), otherenv.valueAt(.6) );
		SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.9), otherenv.valueAt(.9) );
		
		SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.3), aenv.valueAt(.3) );
		SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.6), aenv.valueAt(.6) );
		SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.9), aenv.valueAt(.9) );
		
		SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.3), bwenv.valueAt(.3) );
		SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.6), bwenv.valueAt(.6) );
		SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.9), bwenv.valueAt(.9) );
		
		//	restore
		testM.setFrequencyFunction( fenv );
		
		//	change:	
		testM.setAmplitudeFunction( otherenv );
		
		//	verify:
		SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.3), fenv.valueAt(.3) );
		SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.6), fenv.valueAt(.6) );
		SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.9), fenv.valueAt(.9) );
		
		SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.3), otherenv.valueAt(.3) );
		SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.6), otherenv.valueAt(.6) );
		SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.9), otherenv.valueAt(.9) );
		
		SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.3), bwenv.valueAt(.3) );
		SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.6), bwenv.valueAt(.6) );
		SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.9), bwenv.valueAt(.9) );
		
		//	restore
		testM.setAmplitudeFunction( aenv );
		
		
		//	change:	
		testM.setBandwidthFunction( otherenv );
		
		//	verify:
		SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.3), fenv.valueAt(.3) );
		SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.6), fenv.valueAt(.6) );
		SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.9), fenv.valueAt(.9) );
		
		SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.3), aenv.valueAt(.3) );
		SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.6), aenv.valueAt(.6) );
		SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.9), aenv.valueAt(.9) );
		
		SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.3), otherenv.valueAt(.3) );
		SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.6), otherenv.valueAt(.6) );
		SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.9), otherenv.valueAt(.9) );
		
		//	restore
		testM.setBandwidthFunction( bwenv );
		/*                                                */
		/*********** Partial morphing tests ***************/
		/*                                                */
		
		//	fabricate two Partials and the proper morphed Partial,
		//	and verify that the Morpher produces the correct morph:
		Partial p1, p2;
		
		const int NUM_BPTS = 2;
		const double P1_TIMES[] = {0, .8};
		const double P1_FREQS[] = {100, 180}; 	//	10 Hz per .1 s
		const double P1_AMPS[] = {.2, .2};
		const double P1_BWS[] = {.1, .9};		//	.1 per .1 s
		const double P1_PHS[] = {-.8, .8}; 		//	.2 per .1 s
		
		for (int i = 0; i < NUM_BPTS; ++i )
			p1.insert( P1_TIMES[i], Breakpoint( P1_FREQS[i], P1_AMPS[i], P1_BWS[i], P1_PHS[i] ) );
			
		const double P2_TIMES[] = {0.2, 1};
		const double P2_FREQS[] = {200, 200};
		const double P2_AMPS[] = {.1, .6};		//	.04 per .1 s
		const double P2_BWS[] = {.9, .1};		//	-.1 per .1 s
		const double P2_PHS[] = {0, 0}; 
		
		for (int i = 0; i < NUM_BPTS; ++i )
			p2.insert( P2_TIMES[i], Breakpoint( P2_FREQS[i], P2_AMPS[i], P2_BWS[i], P2_PHS[i] ) );
			
		//	the morphed Partial should have as many Breakpoints 
		//	as the combined Breakpoints of the constituent Partials:
        Partial pm_by_hand;
		const double PM_TIMES[] = {0, 0.2, .8, 1};
		for (int i = 0; i < 2*NUM_BPTS; ++i )
		{
			double t = PM_TIMES[i];
			double f = (1.-fenv.valueAt(t)) * p1.frequencyAt(t) + fenv.valueAt(t) * p2.frequencyAt(t);
			double a = (1.-aenv.valueAt(t)) * p1.amplitudeAt(t) + aenv.valueAt(t) * p2.amplitudeAt(t);
			double bw = (1.-bwenv.valueAt(t)) * p1.bandwidthAt(t) + bwenv.valueAt(t) * p2.bandwidthAt(t);
			double ph = interpolate_phases( p1.phaseAt(t), p2.phaseAt(t), fenv.valueAt(t) );
			pm_by_hand.insert( t, Breakpoint( f, a, bw, ph ) );
		}
		pm_by_hand.setLabel(2);
		
		//	morph p1 and p2 to obtain a morphed Partial, and check its 
		//	parameters against those of pm_by_hand at several times: 
		Partial pmorphed( testM.morphPartial( p1, p2, pm_by_hand.label() ) );
		
		//	check:
		TEST( pmorphed.label() == pm_by_hand.label() );
		TEST( pmorphed.numBreakpoints() == pm_by_hand.numBreakpoints() );

		#define SAME_PARAM_VALUES(x,y) TEST( float_equal((x),(y)) )
		SAME_PARAM_VALUES( pmorphed.startTime(), pm_by_hand.startTime() );
		SAME_PARAM_VALUES( pmorphed.endTime(), pm_by_hand.endTime() );
		SAME_PARAM_VALUES( pmorphed.duration(), pm_by_hand.duration() );
		
		SAME_PARAM_VALUES( pmorphed.frequencyAt(0), pm_by_hand.frequencyAt(0) );
		SAME_PARAM_VALUES( pmorphed.amplitudeAt(0), pm_by_hand.amplitudeAt(0) );
		SAME_PARAM_VALUES( pmorphed.bandwidthAt(0), pm_by_hand.bandwidthAt(0) );
		SAME_PARAM_VALUES( pmorphed.phaseAt(0), pm_by_hand.phaseAt(0) );
		
		SAME_PARAM_VALUES( pmorphed.frequencyAt(0.1), pm_by_hand.frequencyAt(0.1) );
		SAME_PARAM_VALUES( pmorphed.amplitudeAt(0.1), pm_by_hand.amplitudeAt(0.1) );
		SAME_PARAM_VALUES( pmorphed.bandwidthAt(0.1), pm_by_hand.bandwidthAt(0.1) );
		SAME_PARAM_VALUES( pmorphed.phaseAt(0.1), pm_by_hand.phaseAt(0.1) );
		
		SAME_PARAM_VALUES( pmorphed.frequencyAt(0.3), pm_by_hand.frequencyAt(0.3) );
		SAME_PARAM_VALUES( pmorphed.amplitudeAt(0.3), pm_by_hand.amplitudeAt(0.3) );
		SAME_PARAM_VALUES( pmorphed.bandwidthAt(0.3), pm_by_hand.bandwidthAt(0.3) );
		SAME_PARAM_VALUES( pmorphed.phaseAt(0.3), pm_by_hand.phaseAt(0.3) );
		
		SAME_PARAM_VALUES( pmorphed.frequencyAt(0.6), pm_by_hand.frequencyAt(0.6) );
		SAME_PARAM_VALUES( pmorphed.amplitudeAt(0.6), pm_by_hand.amplitudeAt(0.6) );
		SAME_PARAM_VALUES( pmorphed.bandwidthAt(0.6), pm_by_hand.bandwidthAt(0.6) );
		SAME_PARAM_VALUES( pmorphed.phaseAt(0.6), pm_by_hand.phaseAt(0.6) );
		
		SAME_PARAM_VALUES( pmorphed.frequencyAt(0.85), pm_by_hand.frequencyAt(0.85) );
		SAME_PARAM_VALUES( pmorphed.amplitudeAt(0.85), pm_by_hand.amplitudeAt(0.85) );
		SAME_PARAM_VALUES( pmorphed.bandwidthAt(0.85), pm_by_hand.bandwidthAt(0.85) );
		SAME_PARAM_VALUES( pmorphed.phaseAt(0.85), pm_by_hand.phaseAt(0.85) );
		
		SAME_PARAM_VALUES( pmorphed.frequencyAt(1), pm_by_hand.frequencyAt(1) );
		SAME_PARAM_VALUES( pmorphed.amplitudeAt(1), pm_by_hand.amplitudeAt(1) );
		SAME_PARAM_VALUES( pmorphed.bandwidthAt(1), pm_by_hand.bandwidthAt(1) );
		SAME_PARAM_VALUES( pmorphed.phaseAt(1), pm_by_hand.phaseAt(1) );
		
		/*                                                      */
		/*********** dummy Partial morphing tests ***************/
		/*                                                      */
		
		//	test morphing to a dummy Partial, should just fade the
		//	real Partial in (amp envelope starts at 1):
		Partial to_dummy_by_hand;
		for (int i = 0; i < NUM_BPTS; ++i )
		{
			double t = P1_TIMES[i];
			double f = p1.frequencyAt(t);
			double a = (1.-aenv.valueAt(t)) * p1.amplitudeAt(t);
			double bw = p1.bandwidthAt(t);
			double ph = p1.phaseAt(t);
			to_dummy_by_hand.insert( t, Breakpoint( f, a, bw, ph ) );
		}
		to_dummy_by_hand.setLabel(3);

		//	morph p1 and a dummy to obtain a morphed Partial, and check its 
		//	parameters against those of to_dummy_by_hand at several times: 
		Partial to_dummy = testM.morphPartial( p1, Partial(), to_dummy_by_hand.label() );
		
		//	check:
		TEST( to_dummy.label() == to_dummy_by_hand.label() );
		TEST( to_dummy.numBreakpoints() == to_dummy_by_hand.numBreakpoints() );

		SAME_PARAM_VALUES( to_dummy.startTime(), to_dummy_by_hand.startTime() );
		SAME_PARAM_VALUES( to_dummy.endTime(), to_dummy_by_hand.endTime() );
		SAME_PARAM_VALUES( to_dummy.duration(), to_dummy_by_hand.duration() );
		
		SAME_PARAM_VALUES( to_dummy.frequencyAt(0), to_dummy_by_hand.frequencyAt(0) );
		SAME_PARAM_VALUES( to_dummy.amplitudeAt(0), to_dummy_by_hand.amplitudeAt(0) );
		SAME_PARAM_VALUES( to_dummy.bandwidthAt(0), to_dummy_by_hand.bandwidthAt(0) );
		SAME_PARAM_VALUES( to_dummy.phaseAt(0), to_dummy_by_hand.phaseAt(0) );
		
		SAME_PARAM_VALUES( to_dummy.frequencyAt(0.1), to_dummy_by_hand.frequencyAt(0.1) );
		SAME_PARAM_VALUES( to_dummy.amplitudeAt(0.1), to_dummy_by_hand.amplitudeAt(0.1) );
		SAME_PARAM_VALUES( to_dummy.bandwidthAt(0.1), to_dummy_by_hand.bandwidthAt(0.1) );
		SAME_PARAM_VALUES( to_dummy.phaseAt(0.1), to_dummy_by_hand.phaseAt(0.1) );
		
		SAME_PARAM_VALUES( to_dummy.frequencyAt(0.3), to_dummy_by_hand.frequencyAt(0.3) );
		SAME_PARAM_VALUES( to_dummy.amplitudeAt(0.3), to_dummy_by_hand.amplitudeAt(0.3) );
		SAME_PARAM_VALUES( to_dummy.bandwidthAt(0.3), to_dummy_by_hand.bandwidthAt(0.3) );
		SAME_PARAM_VALUES( to_dummy.phaseAt(0.3), to_dummy_by_hand.phaseAt(0.3) );
		
		SAME_PARAM_VALUES( to_dummy.frequencyAt(0.6), to_dummy_by_hand.frequencyAt(0.6) );
		SAME_PARAM_VALUES( to_dummy.amplitudeAt(0.6), to_dummy_by_hand.amplitudeAt(0.6) );
		SAME_PARAM_VALUES( to_dummy.bandwidthAt(0.6), to_dummy_by_hand.bandwidthAt(0.6) );
		SAME_PARAM_VALUES( to_dummy.phaseAt(0.6), to_dummy_by_hand.phaseAt(0.6) );
		
		SAME_PARAM_VALUES( to_dummy.frequencyAt(0.85), to_dummy_by_hand.frequencyAt(0.85) );
		SAME_PARAM_VALUES( to_dummy.amplitudeAt(0.85), to_dummy_by_hand.amplitudeAt(0.85) );
		SAME_PARAM_VALUES( to_dummy.bandwidthAt(0.85), to_dummy_by_hand.bandwidthAt(0.85) );
		SAME_PARAM_VALUES( to_dummy.phaseAt(0.85), to_dummy_by_hand.phaseAt(0.85) );
		
		SAME_PARAM_VALUES( to_dummy.frequencyAt(1), to_dummy_by_hand.frequencyAt(1) );
		SAME_PARAM_VALUES( to_dummy.amplitudeAt(1), to_dummy_by_hand.amplitudeAt(1) );
		SAME_PARAM_VALUES( to_dummy.bandwidthAt(1), to_dummy_by_hand.bandwidthAt(1) );
		SAME_PARAM_VALUES( to_dummy.phaseAt(1), to_dummy_by_hand.phaseAt(1) );
		
		//	test morphing from a dummy Partial, should just fade the
		//	real Partial out (amp envelope starts at 1):
		Partial from_dummy_by_hand;
		for (int i = 0; i < NUM_BPTS; ++i )
		{
			double t = P1_TIMES[i];
			double f = p1.frequencyAt(t);
			double a = aenv.valueAt(t) * p1.amplitudeAt(t);
			double bw = p1.bandwidthAt(t);
			double ph = p1.phaseAt(t);
			from_dummy_by_hand.insert( t, Breakpoint( f, a, bw, ph ) );
		}
		from_dummy_by_hand.setLabel(4);

		//	morph p1 and a dummy to obtain a morphed Partial, and check its 
		//	parameters against those of from_dummy_by_hand at several times: 
		Partial from_dummy = testM.morphPartial( Partial(), p1, from_dummy_by_hand.label() );
		
		//	check:
		TEST( from_dummy.label() == from_dummy_by_hand.label() );
		TEST( from_dummy.numBreakpoints() == from_dummy_by_hand.numBreakpoints() );

		SAME_PARAM_VALUES( from_dummy.startTime(), from_dummy_by_hand.startTime() );
		SAME_PARAM_VALUES( from_dummy.endTime(), from_dummy_by_hand.endTime() );
		SAME_PARAM_VALUES( from_dummy.duration(), from_dummy_by_hand.duration() );
		
		SAME_PARAM_VALUES( from_dummy.frequencyAt(0), from_dummy_by_hand.frequencyAt(0) );
		SAME_PARAM_VALUES( from_dummy.amplitudeAt(0), from_dummy_by_hand.amplitudeAt(0) );
		SAME_PARAM_VALUES( from_dummy.bandwidthAt(0), from_dummy_by_hand.bandwidthAt(0) );
		SAME_PARAM_VALUES( from_dummy.phaseAt(0), from_dummy_by_hand.phaseAt(0) );
		
		SAME_PARAM_VALUES( from_dummy.frequencyAt(0.1), from_dummy_by_hand.frequencyAt(0.1) );
		SAME_PARAM_VALUES( from_dummy.amplitudeAt(0.1), from_dummy_by_hand.amplitudeAt(0.1) );
		SAME_PARAM_VALUES( from_dummy.bandwidthAt(0.1), from_dummy_by_hand.bandwidthAt(0.1) );
		SAME_PARAM_VALUES( from_dummy.phaseAt(0.1), from_dummy_by_hand.phaseAt(0.1) );
		
		SAME_PARAM_VALUES( from_dummy.frequencyAt(0.3), from_dummy_by_hand.frequencyAt(0.3) );
		SAME_PARAM_VALUES( from_dummy.amplitudeAt(0.3), from_dummy_by_hand.amplitudeAt(0.3) );
		SAME_PARAM_VALUES( from_dummy.bandwidthAt(0.3), from_dummy_by_hand.bandwidthAt(0.3) );
		SAME_PARAM_VALUES( from_dummy.phaseAt(0.3), from_dummy_by_hand.phaseAt(0.3) );
		
		SAME_PARAM_VALUES( from_dummy.frequencyAt(0.6), from_dummy_by_hand.frequencyAt(0.6) );
		SAME_PARAM_VALUES( from_dummy.amplitudeAt(0.6), from_dummy_by_hand.amplitudeAt(0.6) );
		SAME_PARAM_VALUES( from_dummy.bandwidthAt(0.6), from_dummy_by_hand.bandwidthAt(0.6) );
		SAME_PARAM_VALUES( from_dummy.phaseAt(0.6), from_dummy_by_hand.phaseAt(0.6) );
		
		SAME_PARAM_VALUES( from_dummy.frequencyAt(0.85), from_dummy_by_hand.frequencyAt(0.85) );
		SAME_PARAM_VALUES( from_dummy.amplitudeAt(0.85), from_dummy_by_hand.amplitudeAt(0.85) );
		SAME_PARAM_VALUES( from_dummy.bandwidthAt(0.85), from_dummy_by_hand.bandwidthAt(0.85) );
		SAME_PARAM_VALUES( from_dummy.phaseAt(0.85), from_dummy_by_hand.phaseAt(0.85) );
		
		SAME_PARAM_VALUES( from_dummy.frequencyAt(1), from_dummy_by_hand.frequencyAt(1) );
		SAME_PARAM_VALUES( from_dummy.amplitudeAt(1), from_dummy_by_hand.amplitudeAt(1) );
		SAME_PARAM_VALUES( from_dummy.bandwidthAt(1), from_dummy_by_hand.bandwidthAt(1) );
		SAME_PARAM_VALUES( from_dummy.phaseAt(1), from_dummy_by_hand.phaseAt(1) );
		
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
	cout << "Morpher passed all tests." << endl;
	return 0;
}


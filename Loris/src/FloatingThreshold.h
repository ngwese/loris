#ifndef __floating_threshold__
#define __floating_threshold__
// ===========================================================================
//	FloatingThreshold.h
//	
//	Class definition for Loris function object FloatingThreshold, a predicate
//	that evaluates Partial amplitude envelopes against a floating threshold.
//
//	-kel 23 Jan 2000
//
// ===========================================================================
#include "LorisLib.h"
#include "Partial.h"
#include <algorithm>

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class FloatingThreshold
//
class FloatingThreshold
{
//	-- public interface --
public:
	//	construction:
	FloatingThreshold( double range_dB );
		// : _ratio( pow( 10., - 0.05 * range_dB ) ) {}
	
	//	evaluate:
	//	(minimum clearance)
	template< class Iter >
	boolean evaluate( Partial & p, Iter begin, Iter end ) const
	{
		//	loop over Breakpoints in p's envelope, 
		//	remember the performance of the best Breakpoint:
		//	(since it only matters whether the best performance
		//	is positive, bestbp can be initialized to anything
		//	negative)
		double bestbp = -1.;
		for ( Partial::iterator env = p.begin(); env != p.end(); ++env ) {
			double time = env->first;
			Breakpoint & bp = env->second;
			
			//	loop over Partials in (begin,end), find the 
			//	loudest Partial at time:
			double loudest = 0.;
			for ( Iter it = begin; it != end; ++it ) {
				loudest = std::max( loudest, it->amplitudeAt( time ) );
			}
			
			//	this Breakpoint's performance is the difference
			//	between its amplitude and the loudest amplitude
			//	less the range (range is specified in dB, converted
			//	to a ratio); remember the best Breakpoint performance:
			bestbp = std::max( bestbp, bp.amplitude() - ( loudest * _ratio ) );
		}
		
		//	return true if p ever cleared the floating threshold:
		return bestbp >= 0.;
	}

	template< class Iter >
	boolean operator()( Partial & p, Iter begin, Iter end ) const
	{
		return evaluate( p, begin, end );
	}
	
//	-- instance variables --
private:
	const double _ratio;

};	// end of class FloatingThreshold

End_Namespace( Loris )

#endif 	// ndef __floating_threshold__
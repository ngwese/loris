#ifndef __energy_distribution_header__
#define __energy_distribution_header__
// ===========================================================================
//	DistributeEnergy.h
//	
//	Class definition for Loris function object DistributeEnergy.
//
//	-kel 23 Jan 2000
//
// ===========================================================================
#include "LorisLib.h"
#include "Partial.h"
#include "Breakpoint.h"
#include "notifier.h"
#include "bark.h"

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class DistributeEnergy
//
//	Function object, has no instance variables or state. Not very useful
//	with STL though, as it takes three arguments. Could give it state by
//	constructing it with the range of receiving partials, and then it 
//	would be invoked with only one argument. Is it worth the overhead
//	of transfering the partials?
//
class DistributeEnergy
{
//	-- public interface --
public:
	//	distribute
	template< class Iter >
	void distribute( const Partial & p, Iter begin, Iter end ) const
	{
		//	loop over Breakpoints in p:
		Partial::const_iterator envIter;
		double tUpperBound = p.startTime();	// initialize:
		for ( envIter = p.begin(); envIter != p.end(); ++envIter ) {
			double time = envIter->first;
			const Breakpoint & bp = envIter->second;
			
			//	find nearest Partial in (begin,end) above
			//	and below (in frequency) to bp at time:
			Iter above = end, below = end;
			double freqAbove = 0., freqBelow = 0.;
			for ( Iter it = begin; it != end; ++it ) {
				//	cannot distribute energy to a Partial
				//	that does not exist at time:
				if ( it->startTime() > time || it->endTime() < time ) {
					continue;
				}
				
				//	could also check to make sure that p isn't in
				//	(begin, end), so as not to distribute p's
				//	energy to itself:
				if ( &(*it) == &p ) {
					continue;
				}

				//	compare to other candidates:
				double f = it->frequencyAt( time );
				if ( f < bp.frequency() ) {
					//	candidate below:
					if ( below == end || f > freqBelow ) {
						//	better than previous candidate below:
						below = it;
						freqBelow = f;
					}
				}
				else {
					//	candidate above:
					if ( above == end || f < freqAbove ) {
						//	better than previous candidate above:
						above = it;
						freqAbove = f;
					}
				}
			}	//	end of loop over partials in (begin,end)
			
			//	compute time bounds for energy distribution:
			//	(halfway between this breakpoint and its neighbors)
			double tLowerBound = tUpperBound;
			Partial::const_iterator next( envIter );
			if ( ++next != p.end() ) {
				tUpperBound = ( next->first + time ) * 0.5;
			}
			else {
				tUpperBound = time;
			}
			
			/*
			if ( bp.frequency() < 2000. ) {
				debugger << "distributing energy, freq: " << bp.frequency() <<
							" time: " << time;
				if ( above != end ) {
					debugger << " candidate above freq " << freqAbove <<
								" duration " << above->duration();
				}
				if ( below != end ) {
					debugger << " candidate below freq " << freqBelow <<
								" duration " << below->duration();
				}
				debugger << endl;
			}
			*/
			//	make sure the candidates aren't too far
			//	(farther than one bark frequency unit)
			//	in frequency from bp:
			double b = bark( bp.frequency() );
			const double MAX = 1.;	// ???
			if ( bark( freqAbove ) - b > MAX ) {
				above = end;
			}
			if ( b - bark( freqBelow ) > MAX ) {
				below = end;
			} 
			
			
			//	four cases: either, neither, or both could
			//	be end (i.e. no candidate exists):
			double e = bp.amplitude() * bp.amplitude();
			if ( above != end && below != end ) {
				double alpha = computeAlpha( bp.frequency(), freqBelow, freqAbove );
				addNoiseEnergy( e * (1. - alpha), *below, tLowerBound, tUpperBound );
				addNoiseEnergy( e * alpha, *above, tLowerBound, tUpperBound );
			}
			else if ( above != end ) {
				addNoiseEnergy( e, *above, tLowerBound, tUpperBound );
			}
			else if ( below != end ) {
				addNoiseEnergy( e, *below, tLowerBound, tUpperBound );
			}
			//	else no Partials in (begin,end) are eligible
	
		}	//	end loop over partial envelope
	};

	template< class Iter >
	void operator() ( const Partial & p, Iter begin, Iter end ) const
	{ 
		distribute( p, begin, end ); 
	}
	
private:	
//	-- helpers --
	void addNoiseEnergy( double energy, Partial & p, double lower, double upper ) const;
	double computeAlpha( double x, double below, double above ) const;
	
};	// end of class DistributeEnergy

// ---------------------------------------------------------------------------
//	distributeEnergy
// ---------------------------------------------------------------------------
//
template< class Iter >
void distributeEnergy( const Partial & p, const Iter & begin, const Iter & end ) 
{
	DistributeEnergy d;
	d( p, begin, end );
}

End_Namespace( Loris )

#endif 	// ndef __energy_distribution_header__
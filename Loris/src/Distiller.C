// ===========================================================================
//	Distiller.C
//	
//	A group of Partials that logically represent a single component
//	can be distilled into a single Partial using a Distiller. 
//
//	-kel 20 Oct 99
//
// ===========================================================================

#include "LorisLib.h"

#include "Distiller.h"
#include "Partial.h"
#include "Breakpoint.h"

#include "Morph.h"	//	for amplitudeAtTime prototype, FIX

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
#else
	#include <math.h>
#endif

using namespace std;

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	distill
// ---------------------------------------------------------------------------
//	
Partial 
Distiller::distill( const std::list<Partial> & all ) const
{
	//	create the resulting distilled partial:
	Partial newp;
	
	//	loop over all Partials:
	for (list<Partial>::const_iterator it = all.begin(); it != all.end(); ++it ) {
		const Partial & p = *it;
		//	loop over all Breakpoints in p:
		for ( const Breakpoint * bp = p.head(); bp != Null; bp = bp->next() ) {
			double xse = 0.;
			//	loop over all Partials again:
			list<Partial>::const_iterator innerIt;
			for ( innerIt = all.begin(); 
				  innerIt != all.end(); 
				  ++innerIt ) {
				 //	skip the current Partial p:
				 if ( innerIt == it ) {
				 	continue;
				 }
				 
				 const Partial & innerP = *innerIt;
				 
				 double a = amplitudeAtTime( innerP, bp->time() );
				 if ( a > bp->amplitude() ) {
					 //	if the other dude is louder, 
					 //	stop looking at partials:
					break;	
				 }
				 else {
				 	//	accumulate energy to be added as bandwidth:
				 	//	(this should use the same algorithm as the energy
				 	//	redistribution in the analysis)
				 	xse += a*a;
				 }
			}
			
			//	if the innermost loop did not exit early,
			//	i.e., all Partials in were examined, then
			//	create a new Breakpoint and add it to newp:
			if ( innerIt == all.end() ) {
				//	add some bandwith energy:
				double etot = bp->amplitude() * bp->amplitude();
				double ebw = etot * bp->bandwidth();
				
				etot += xse;
				ebw += xse;
				
				//	create the new Breakpoint:
				Breakpoint newBp( *bp );
				newBp.setAmplitude( sqrt( etot ) );
				newBp.setBandwidth( ebw / etot );
				
				//	add it to the Partial:
				newp.insert( bp->time(), newBp );
				
				//	we may need to add a zero-amplitude
				//	Breakpoint if there is a gap:
				if ( ! bp->next() ) {
					//	loop over all the partials and make sure
					//	that no other partial exists one ms later:
					list<Partial>::const_iterator doober;
					for ( doober = all.begin(); 
						  doober != all.end(); 
						  ++doober ) {
						if ( amplitudeAtTime( *doober, bp->time() + 0.001 ) > 0. ) {
							break;
						}
					}
					if ( doober == all.end() ) {
						Breakpoint zeroPt( *bp );
						zeroPt.setAmplitude( 0. );
						zeroPt.setPhase( phaseAtTime( p, bp->time() + 0.001 ) );
						newp.insert( bp->time() + 0.001, zeroPt );
					}
				}
				if ( ! bp->prev() && bp->time() > 0.001 ) {
					//	loop over all the partials and make sure
					//	that no other partial exists one ms earlier:
					list<Partial>::const_iterator doober;
					for ( doober = all.begin(); 
						  doober != all.end(); 
						  ++doober ) {
						if ( amplitudeAtTime( *doober, bp->time() - 0.001 ) > 0. ) {
							break;
						}
					}
					if ( doober == all.end() ) {
						Breakpoint zeroPt( *bp );
						zeroPt.setAmplitude( 0. );
						zeroPt.setPhase( phaseAtTime( p, bp->time() - 0.001 ) );
						newp.insert( bp->time() - 0.001, zeroPt );
					}
				}
				
			
			}
		}	
	}
	
	//	return the newly-distilled partial:
	return newp;
}


End_Namespace( Loris )

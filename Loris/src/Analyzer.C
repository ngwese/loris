// ===========================================================================
//	Analyzer.C
//	
//	Implementation of class Loris::Analyzer.
//
//	-kel 5 Dec 99
//
// ===========================================================================
#include "Analyzer.h"

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	Analyzer constructor
// ---------------------------------------------------------------------------
//
Analyzer::Analyzer( void )
{
}

// ---------------------------------------------------------------------------
//	Analyzer destructor
// ---------------------------------------------------------------------------
//
Analyzer::~Analyzer( void )
{
}

// ---------------------------------------------------------------------------
//	analyze
// ---------------------------------------------------------------------------
//
void 
Analyzer::analyze( vector< double > & buf, double srate )
{
//	construct _spectrum if necessary:

//	loop over short-time analysis frames:

	//	compute reassigned spectrum,
	//	identify ridges:
	//	actually, need to call this with a 
	//	sub range of the sample buffer.
	_spectrum->transform( buf );

	//	perform bandwidth association:

	//	apply amplitude thresholds:

	//	match ridges identified in adjacent
	//	short-time frames:
	
	//	spawn Partials:
	
//	end of loop over short-time frames
}

// ---------------------------------------------------------------------------
//	formPartials
// ---------------------------------------------------------------------------
//	The reassigned spectrum has been computed, and short-time peaks
//	identified. From those peaks, construct Breakpoints and append
//	them to existing Partials, if appropriate, or else give birth to
//	new Partials.
//
void 
Analyzer::formPartials( double frameTime, double sampleRate )
{
	//	loop over short-time peaks:
	ReassignedSpectrum::ridge_iterator it;
	for ( it = _spectrum.ridgesBegin(); it != _spectrum.ridgesEnd(); ++it ) {
		//	if the time correction for this peak is large,
		//	forget it, go on to the next one:
		double peakTimeOffset = _spectrum.reassignedTime( *it );
		if ( abs(peakTimeOffset) > hopSize() ) {
			continue;	//	loop over short-time peaks
		} 
		
		//	create a Breakpoint corresponding to the
		//	short-time reassigned spectral peak:
		Breakpoint bp( _spectrum.reassignedFrequency( *it ) * sampleRate,
					   _spectrum.reassignedMagnitude( *it ),
					   0.,	//	bandwidth
					   _spectrum.reassignedPhase( *it, peakTimeOffset ) );
		
		//	compute the time after which a Partial
		//	must have Breakpoints in order to be 
		//	eligible to receive this Breakpoint:
		double tooEarly = frameTime - hopSize();	//	???
		
		//	loop over all Partials:
		
			//	if Partial ends before capture
			//	time (peak time - hop?), forget it
			
			//	otherwise, if Partial is nearer in 
			//	frequency to the peak than every other 	
			//	Partial we've seen, remember it
			
			//	(now have nearest Partial)
			//	if this Partial is outside of
			//	frequency capture range, or if
			//	no Partial was found that falls
			//	in the temporal capture range,
			//	no match for this peak, create
			//	a new Partial with the Breakpoint
			//	constructed from this peak
			
			//	otherwise, if no other peak is
			//	closer in frequency to this Partial,
			//	add the Breakpoint to this Partial
			
			//	otherwise, no match for this peak, create
			//	a new Partial with the Breakpoint
			//	constructed from this peak
	}			 
}

// ---------------------------------------------------------------------------
//	spawnPartial
// ---------------------------------------------------------------------------
//	Create a new Partial, beginning with the specified Breakpoint at
//	the specified time, and add it to the collection.
//
void 
Analyzer::spawnPartial( double time, const Breakpoint & bp )
{
	Partial p;
	p.insert( time, bp );
	partials().push_back( p );
}


End_Namespace( Loris )

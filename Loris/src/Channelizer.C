// ===========================================================================
//	Channelizer.C
//	
//	Definition of a class for labeling Partials in a PartialList
//	according to a set of linearly-spaced, variable-frequency
//	channels (like a time-varying harmonic frequency relationship).
//
//	This class interface is fully insulating.
//	This class is a leaf class, not for subclassing (no virtual destructor). 
//
//	-kel 21 July 2000
//
// ===========================================================================
#include "Channelizer.h"
#include "Handle.h"
#include "Envelope.h"
#include "Partial.h"

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class Channelizer_imp
//
//	Implementation class for Channelizer insulating Channelizer interface.
//
struct Channelizer_imp
{
	Handle<Envelope> _refChannelFreq;
	int _refChannelLabel;

	Channelizer_imp( Handle<Envelope> env, int label );
	~Channelizer_imp( void );
	
	void channelize( PartialList::iterator begin, PartialList::iterator end );

};	//	end of class Channelizer_imp

// ---------------------------------------------------------------------------
//	Channelizer_imp constructor 
// ---------------------------------------------------------------------------
//	Can verify in Meyer that when the exception is thrown, the Handle<>
//	destructor is guaranteed to be called.
//
Channelizer_imp::Channelizer_imp( Handle< Envelope > env, int label ) :
	_refChannelFreq( env ),
	_refChannelLabel( label )
{
	if ( label <= 0 )
		Throw( InvalidArgument, "Channelizer reference label must be positive." );
}

// ---------------------------------------------------------------------------
//	Channelizer_imp destructor
// ---------------------------------------------------------------------------
//
Channelizer_imp::~Channelizer_imp( void )
{
}

// ---------------------------------------------------------------------------
//	loudestAt (STATIC)
// ---------------------------------------------------------------------------
//	Helper for finding the time at which a Partial
//	attains its maximum amplitude.
//
//	Use sinusoidal amplitude, so that repeated channelizations and
//	distillations yield identical results.
//
//	This may not be used at all, we may instead use a weighted
//	average for determining channel number of a Partial, instead
//	of evaluating it at its loudest Breakpoint. Unresolved at this
//	time.
//
static double loudestAt( const Partial & p )
{
	PartialConstIterator env = p.begin();
	double maxAmp = env->amplitude() * std::sqrt( 1. - env->bandwidth() );
	double time = env.time();
	
	for ( ++env; env != p.end(); ++env ) {
		double a = env->amplitude() * std::sqrt( 1. - env->bandwidth() );
		if ( a > maxAmp ) {
			maxAmp = a;
			time = env.time();
		}
	}
	return time;
}

// ---------------------------------------------------------------------------
//	Channelizer_imp channelize
// ---------------------------------------------------------------------------
//	Two algorithms are defined here, the fancier one computes the best channel
//	label for each Partial by computing the amplitude-weighted average 
//	frequency channel for the Partial. The simple algorithm just evaluates the
//	frequency channel at the time of peak sinusoidal amplitude for each 
//	Partial. Still resolving which algorithm to use. Only matter for reference
//	envelopes which badly match the Partials to be channelized.
//	
void
Channelizer_imp::channelize( PartialList::iterator begin, PartialList::iterator end )
{
	for ( PartialList::iterator it = begin; it != end; ++it ) 
	{
		#define FANCY
		#ifdef FANCY

		//	compute an amplitude-weighted average channel
		//	label for each Partial:
		double ampsum = 0.;
		double weightedlabel = 0.;
		PartialConstIterator bp;
		for ( bp = it->begin(); bp != it->end(); ++bp )
		{
			//	use sinusoidal amplitude:
			double a = bp->amplitude() * std::sqrt( 1. - bp->bandwidth() );
			double f = bp->frequency();
			double t = bp.time();
			
			double refFreq = _refChannelFreq->valueAt( t ) / _refChannelLabel;
			weightedlabel += a * (f / refFreq);
			ampsum += a;
		}
		
		int label;
		if ( ampsum > 0. )	
			label = (int)((weightedlabel / ampsum) + 0.5);
		else	//	this should never happen, but just in case:
			label = 0;
		Assert( label >= 0 );
		
		#else	//	not def FANCY

		//	less fancy, just use label calculated at 
		//	time of peak sinusoidal amplitude.
		//	
		//	calculate time of peak sunusoidal amplitude:
		double time = loudestAt( *it );
					
		//	get reference frequency at time:
		double refFreq = refFreqEnvelope->valueAt( time ) / refLabel;
		
		//	compute the label for this partial as 
		//	nearest integer multiple of reference 
		//	frequency at time:
		int label = (int)((it->frequencyAt( time ) / refFreq) + 0.5);
	
		#endif	// def FANCY
			
		//	assign label, and remember it, but
		//	only if it is a valid (positive) 
		//	distillation label:
		it->setLabel( label );
	}
}

// ---------------------------------------------------------------------------
//	Channelizer constructor 
// ---------------------------------------------------------------------------
//	In fully-insulating interface constructors, with no subclassing (fully-
//	insulating interfaces have no virtual members), can safely initialize the
//	imp pointer this way because only that constructor could generate an 
//	exception, and in that case the associated memory would be released 
//	automatically. So there's no risk of a memory leak associated with this
//	pointer initialization.
//
Channelizer::Channelizer( Handle< Envelope > env, int label ) :
	_imp( new Channelizer_imp( env, label ) )
{
}

// ---------------------------------------------------------------------------
//	Channelizer destructor
// ---------------------------------------------------------------------------
//
Channelizer::~Channelizer( void )
{
	delete _imp;
}

// ---------------------------------------------------------------------------
//	Channelizer channelize
// ---------------------------------------------------------------------------
//	Delegate to implementation.
//	
void
Channelizer::channelize( PartialList::iterator begin, PartialList::iterator end )
{
	_imp->channelize( begin, end );
}

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

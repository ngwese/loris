#ifndef __bandwidth_association_header__
#define __bandwidth_association_header__
// ===========================================================================
//	AssociateBandwidth.h
//	
//	Class definition for Loris analysis strategy class AssociateBandwidth.
//
//	-kel 20 Jan 2000
//
// ===========================================================================
#include "LorisLib.h"
#include <numeric>
#include <complex>

//#include "Notifier.h"

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class AssociateBandwidth
//
//	Region center frequencies should be a function of the number of regions, 
//	but for now, use the Lemur algorithm which centers them all on integer
//	bark frequencies.
//
//	Use:
//		accumulateSpectrum( spectrum, srate );
//		accumulateSinusoids( iter, iter );
//		(both any number of times, any sequence)
//		associate( iter, iter ); (calls reset at end)
//
//	For now, accumulating spectral energy is different from accumulating
//	sinusoidal energy, because in the latter case we have to compute weights
//	too. The implicit assumption is that all the same breakpoints will be
//	submitted to accumulateSinusoids() and to associateBandwidth(), otherwise the 
//	weights make no sense. The rest of the algorithm doesn't require that
//	the same breakpoints be used. I can remove this assumption and get rid of
//	the weights by accumulating non-tapered energy for each region (i.e. don't 
//	compute alpha), and then only using alpha to determine the distribution of
//	bandwidth. That's a somewhat different algorithm though. Do the Lemur 
//	algorithm first.
//
//	The design strategy proliferation begins! This class uses mostly
//	template members, to avoid reference to other concrete types, particularly
//	container types. Do I like this?
//
class AssociateBandwidth
{
//	-- public interface --
public:
	//	construction:
	AssociateBandwidth( void );
	~AssociateBandwidth( void );
	
	//	spectral energy accumulation:
	//
	//	Spectrum is any class implementing size() and
	//	indexed access returning a value that can be normed.
	template< class Spectrum >
	void accumulateSpectrum( const Spectrum & spectrum, double srate )
	{
		//	spectral energy has to be scaled to account
		//	for windowing and transform length:
		const double energyScale = _windowFactor / spectrum.size();
		
		const int max_idx = spectrum.size() / 2;
		const double HzPerSample = srate / spectrum.size();
		
		for ( int i = 0; i < max_idx; ++i ) {
			double e = std::norm( spectrum[i] ) * energyScale;
			distribute( i * HzPerSample, e, _spectralEnergy );
		}
	}
	
	//	sinusoidal energy accumulation:
	template< class Iter >
	void accumulateSinusoids( Iter b, Iter e )
	{
		while ( b != e ) {
			accumulateSinusoid( *b );
			++b;
		}
	}
		
	template< class Sinusoid >
	void accumulateSinusoid( const Sinusoid & s )
	{
		double freq = s.frequency();
		double amp = s.amplitude();
		distribute( freq, amp * amp, _sinusoidalEnergy  );
		distribute( freq, 1., _weights );
	}
	
	//	bandwidth assocation:
	template< class Iter >
	void associate( Iter b, Iter e )
	{
		//	compute surplus spectral energy:
		std::vector< double > surplus( _spectralEnergy.size() );
		computeSurplusEnergy( surplus );
		
		while ( b != e ) {
			double freq = b->frequency();
			double amp = b->amplitude();
			double e = amp * amp;
			
			//	compute noise to add:
			double noise = computeNoiseEnergy( freq, surplus );
			
			//	compute new amplitude and bandwidth values:
			b->setBandwidth( noise / ( e + noise ) );
			b->setAmplitude( sqrt( e + noise ) );
			
			//	next:
			++b;
		}
		
		//	clear vectors:
		reset();
	}
	
	//	window specification:
	//	Spectral energy computation needs to be adjusted for the
	//	effects of the transform window. It _also_ needs to be 
	//	scaled (down) by the length of the transform, not included
	//	here.
	//
	//	(include a non-template version)
	template< class Iter >
	void setWindow( Iter b, Iter e ) 
	{
		//	Iters reference real numbers:
		double sqrsum = std::inner_product( b, e, b, 0. );
		_windowFactor = 1. / sqrsum;
		//	double because we only search half the spectrum:
		_windowFactor *= 2.;
		//	double again for no apparent reason:
		//	(never figured out why I need this extra factor of 2)
		_windowFactor *= 2.;
	}
		
private:	
//	-- helpers --
	void computeSurplusEnergy( std::vector<double> & surplus );
	double computeNoiseEnergy( double freqHz, const std::vector<double> & surplus );
	
	int findRegionBelow( double barks );
	double computeAlpha( double barks );
	
	void distribute( double freqHz, double x, std::vector<double> & regions );
	
	void reset( void );
	
//	-- instance variables --
	std::vector< double > _spectralEnergy;
	std::vector< double > _sinusoidalEnergy;
	std::vector< double > _weights;
	
	double _windowFactor;
	
};	// end of class AssociateBandwidth

End_Namespace( Loris )

#endif 	// ndef __bandwidth_association_header__
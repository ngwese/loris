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

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
	using std::sqrt;
#else
	#include <math.h>
#endif

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
//	One necessary improvement to this is tighter coupling with the rest of
//	the analysis. It really is essential that the same breakpoints be 
//	accumulated as are associated. And its safe to assume that we have the
//	same spectrum all the time. This thing should be constructed with the
//	spectrum and sample rate (store a reference to the spectrum, get the 
//	window information too somehow, and compute the energy scale) and invoked
//	on a range of breakpoints, calling accumulateSiusoids and associate
//	at that time.
//
//	To keep this generic, if desired, the whole class could be templatized
//	on the spectrum type and possibly (not nec.) the breakpoint type.
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
			//double m = abs( spectrum[i] );
			//double m = spectrum.magnitude(i);
			double m = 2. * abs( spectrum[i] ) * _moreshit;
			double e = m * m;// * energyScale;
			distribute( i * HzPerSample, e, _spectralEnergy );
		}
		
		_specCopy = std::vector<double>( spectrum.size(), 0. );
		_sinSpec = std::vector<double>( spectrum.size(), 0. );
		for ( int j = 0; j < spectrum.size(); ++j ) {
			_specCopy[j] = 2. * abs( spectrum[j] ) * _moreshit;
		}
		_residue = _specCopy;
	}
	
	//	sinusoidal energy accumulation:
	//	Iters are iterators over Analyzer::Peaks
	template< class Iter >
	void accumulateSinusoids( Iter b, Iter e )
	{
		while ( b != e ) {
			accumulateSinusoid( *b );
			++b;
		}
	}
		
	//	Sinusoid is Analyzer::Peak
	template< class Sinusoid >
	void accumulateSinusoid( const Sinusoid & s )
	{
		double freq = s.frequency();
		double amp = s.amplitude();
		if ( freq > 0. ) {
			ohBaby( freq, amp );
		}
	}
	
	//	bandwidth assocation:
	//	Iters are iterators over Analyzer::Peaks
	template< class Iter >
	void associate( Iter b, Iter e )
	{
		//	compute surplus spectral energy:
		std::vector< double > surplus( _spectralEnergy.size() );
		computeSurplusEnergy( surplus );
		
		while ( b != e ) {
			double freq = b->frequency();
			//double amp = b->amplitude();
			//double e = amp * amp;
			
			if ( freq > 0 ) {
				//	compute noise to add:
				double noise = computeNoiseEnergy( freq, surplus );
				b->addNoise( noise );
			}
			
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
	//
	//	Iters are a vector<double> range.
	template< class Iter >
	void setWindow( Iter b, Iter e, long speclen ) 
	{
		std::vector< double > duh( b, e );
		computeWindowSpectrum( duh, speclen );
		/*
		//	Iters reference real numbers:
		double sqrsum = std::inner_product( b, e, b, 0. );
		double sum = std::accumulate( b, e, 0. );
		_windowFactor = 1. / sqrsum;
		//_windowFactor = 0.25 * sum * sum / sqrsum;
		//	double because we only search half the spectrum:
		_windowFactor *= 2.;
		//	double again for no apparent reason:
		//	(never figured out why I need this extra factor of 2)
		//
		//	now I need _another_ factor of two to make it sound decent.
		//
		//_windowFactor *= 2.;
		*/
	}
	
	void computeWindowSpectrum( std::vector< double > & v, long len );
	void ohBaby( double f, double a );
	inline double binFreq( double freq );
	
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
	
	std::vector< double > _winspec;
	double _hzPerSamp, _moreshit;
	
	std::vector< double > _specCopy, _sinSpec, _residue;

	
	double _windowFactor;
	
};	// end of class AssociateBandwidth

End_Namespace( Loris )

#endif 	// ndef __bandwidth_association_header__
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
//#include <numeric>
//#include <complex>

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
	//using std::sqrt;
#else
	#include <math.h>
#endif

Begin_Namespace( Loris )

class ReassignedSpectrum;

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
//	The design strategy proliferation begins! This class uses mostly
//	template members, to avoid reference to other concrete types, particularly
//	container types. Do I like this?
//
class AssociateBandwidth
{
//	-- public interface --
public:
	//	construction:
	AssociateBandwidth( const ReassignedSpectrum & spec, 
						double srate, double resolution = 1000. );
	~AssociateBandwidth( void );

	//	bandwidth assocation:
	//	Iters are iterators over Analyzer::Peaks
	template< class Iter >
	void associate( Iter b, Iter e )
	{		
		//	accumulate spectral energy:
		accumulateSpectrum();
		
		//	accumulate sinusoidal energy:
		for ( Iter it = b; it != e; ++it ) {
			double a = accumulateSinusoid( it->frequency(), it->amplitude() );
			it->setAmplitude( a );
		}
		
		//	compute surplus spectral energy:
		computeSurplusEnergy();
		
		//	distribute surplus noise energy:
		//	(ignore negative frequencies)
		for ( Iter it = b; it != e; ++it ) {
			if ( it->frequency() > 0 ) {
				it->addNoise( computeNoiseEnergy( it->frequency() ) );
			}
		}
		
		//	clear vectors:
		reset();
	}
	
private:	
//	-- helpers --	
	//	initialization:
	void computeWindowSpectrum( const std::vector< double > & v );
	
	//	called in associate():	
	double accumulateSinusoid( double f, double a  );
	void accumulateSpectrum( void );
	double computeNoiseEnergy( double freqHz );
	void computeSurplusEnergy( void );
	
	inline double binFrequency( double freq );
	double computeAlpha( double binfreq );
	void distribute( double freqHz, double x, std::vector<double> & regions );
	int findRegionBelow( double binfreq );	
	void reset( void );
	
//	-- instance variables --
	const ReassignedSpectrum & _spectrum;
	
	//	energy vectors, reused each associate() call:
	std::vector< double > _spectralEnergy, _sinusoidalEnergy, _weights, _surplus;
	
	//	oversampled window spectrum for correcting magnitudes:
	std::vector< double > _mainlobe;
	
	double _regionRate;
	double _hzPerSamp;
	double _windowFactor;
	
	//	debugging:
#ifdef Debug_Loris
	std::vector< double > _specCopy, _sinSpec, _residue;
#endif
};	// end of class AssociateBandwidth

End_Namespace( Loris )

#endif 	// ndef __bandwidth_association_header__
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
#include <vector>

#if !defined( Deprecated_cstd_headers )
	#include <cmath>
#else
	#include <math.h>
#endif

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif


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
//	container types. Do I like this? No I do not.
//
class AssociateBandwidth
{
//	-- instance variables --
	const ReassignedSpectrum & _spectrum;
	
	//	energy vectors, reused each associate() call:
	std::vector< double > _spectralEnergy, _sinusoidalEnergy, _weights, _surplus;
	
	double _regionRate;
	double _hzPerSamp;
	double _cropSamps;	//	analysis cropTime in samples, see accumulateSpectrum
	
//	-- public interface --
public:
	//	construction:
	AssociateBandwidth( const ReassignedSpectrum & spec, 
						double srate, double regionWidth, double crop );
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
			accumulateSinusoid( it->frequency(), it->amplitude() );
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
	//	called in associate():	
	void accumulateSinusoid( double f, double a  );
	void accumulateSpectrum( void );
	double computeNoiseEnergy( double freqHz );
	void computeSurplusEnergy( void );
	
	inline double binFrequency( double freq );
	double computeAlpha( double binfreq );
	void distribute( double freqHz, double x, std::vector<double> & regions );
	int findRegionBelow( double binfreq );	
	void reset( void );
	
};	// end of class AssociateBandwidth

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif 	// ndef __bandwidth_association_header__
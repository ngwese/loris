#ifndef INCLUDE_ASSOCIATE_BANDWIDTH_H
#define INCLUDE_ASSOCIATE_BANDWIDTH_H
// ===========================================================================
//	AssociateBandwidth.h
//	
//	Class definition for Loris analysis strategy class AssociateBandwidth.
//
//	-kel 20 Jan 2000
//
// ===========================================================================
#include <vector>
#include <cmath>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

class Breakpoint;
class ReassignedSpectrum;

// ---------------------------------------------------------------------------
//	class AssociateBandwidth
//
//	Two strategies of bandwidth association are represented here:
//
// 	In the old strategy, Breakpoints are extracted and thinned and
//	the survivors are accumulated as sinusoids. Then the spectral
//	spectral energy is accumulated, the surplus is computed as the
//	difference, and the results is distributed as bandwidth. 
//
//	In the new strategy, Breakpoints are extracted and accumulated
//	as sinusoids. Spectral peaks that are not extracted (don't exceed
//	the amplitude floor) are accumulated diectly as noise (surplus). 
//	Breakpoints which are subsequently thinned are just lost, not
//	represented as noise. After all spectral peaks have been accumulated
//	as noise or sinusoids, the noise is distributed as bandwidth.
//
class AssociateBandwidth
{
//	-- instance variables --
	//	energy vectors, reused each associate() call:
	std::vector< double > _spectralEnergy, _sinusoidalEnergy, _weights, _surplus;
	
	double _regionRate;
	double _hzPerSamp;		//	this is needed only by the old strategy
	
//	-- public interface --
public:
	//	construction:
	//	(first two args needed only under the old strategy)
	AssociateBandwidth( const ReassignedSpectrum & spec, 
						double srate, double regionWidth );
	~AssociateBandwidth( void );
	
	//	energy accumulation:
	void accumulateNoise( double freq, double amp );				//	new strategy only
	void accumulateSinusoid( double f, double a  );					//	both new and old strategies
	void accumulateSpectrum( const ReassignedSpectrum & spectrum );	//	old strategy only
	void computeSurplusEnergy( void );								//	old strategy only
	
	//	bandwidth assocation:
	void associate( Breakpoint & bp );
	
	//	call this to wipe out the accumulated energy to 
	//	prepare for the next frame (yuk):
	void reset( void );
		
private:	
//	-- helpers --	
	//	called in associate():	
	double computeNoiseEnergy( double freqHz );
	
	inline double binFrequency( double freq );
	double computeAlpha( double binfreq );
	void distribute( double freqHz, double x, std::vector<double> & regions );
	int findRegionBelow( double binfreq );	
	
};	// end of class AssociateBandwidth

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif 	// ndef INCLUDE_ASSOCIATE_BANDWIDTH_H
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

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

class Breakpoint;

// ---------------------------------------------------------------------------
//	class AssociateBandwidth
//
//	In the new strategy, Breakpoints are extracted and accumulated
//	as sinusoids. Spectral peaks that are not extracted (don't exceed
//	the amplitude floor) or are rejected for certain reasons, are 
//	accumulated diectly as noise (surplus). After all spectral peaks 
//	have been accumulated as noise or sinusoids, the noise is distributed 
//	as bandwidth.
//
class AssociateBandwidth
{
//	-- instance variables --
	std::vector< double > _weights;	//	weights vector for recording 
									//	frequency distribution of retained
									//	sinusoids
	 std::vector< double > _surplus;//	surplus (noise) energy vector for
	 								//	accumulating the distribution of
	 								//	spectral energy to be distributed 
	 								//	as noise
	
	double _regionRate;				//	inverse of region center spacing
	
//	-- public interface --
public:
	//	construction:
	AssociateBandwidth( double regionWidth, double srate );
	~AssociateBandwidth( void );
	
	//	energy accumulation:
	void accumulateNoise( double freq, double amp );	
	void accumulateSinusoid( double freq, double amp  );	
	
	//	bandwidth assocation:
	void associate( Breakpoint & bp );
	
	//	call this to wipe out the accumulated energy to 
	//	prepare for the next frame (yuk):
	void reset( void );
		
private:	
//	-- helpers --	
	//	called in associate():	
	double computeNoiseEnergy( double freq, double amp );
	
	inline double binFrequency( double freq );
	double computeAlpha( double binfreq );
	void distribute( double freqHz, double x, std::vector<double> & regions );
	int findRegionBelow( double binfreq );	
	
};	// end of class AssociateBandwidth

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif 	// ndef INCLUDE_ASSOCIATE_BANDWIDTH_H
#ifndef INCLUDE_ASSOCIATEBANDWIDTH_H
#define INCLUDE_ASSOCIATEBANDWIDTH_H
/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * AssociateBandwidth.h
 *
 * Definition of Loris analysis strategy class AssociateBandwidth.
 *
 * Kelly Fitz, 20 Jan 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include <Loris_prefix.h>
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

#endif /* ndef INCLUDE_ASSOCIATEBANDWIDTH_H */

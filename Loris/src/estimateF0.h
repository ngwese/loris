/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2006 by Kelly Fitz and Lippold Haken
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
 * estimateF0.h
 *
 * Implementation of an iterative alrogithm for computing an 
 * estimate of fundamental frequency from a sequence of sinusoidal
 * frequencies and amplitudes using a maximum likelihood algorithm
 * adapted from Quatieri's Speech Signal Processing text. The 
 * algorithm here takes advantage of the fact that spectral peaks
 * have already been identified and extracted in the analysis/modeling
 * process.
 *
 * Kelly Fitz, 28 March 2006
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <vector>

//	begin namespace
namespace Loris {

double 
iterative_estimate( const std::vector<double> & amps, 
					const std::vector<double> & freqs, 
					double fmin, double fmax,
					double resolution );

}	//	end of namespace Loris

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
 * estimateF0.C
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

#if HAVE_CONFIG_H
	#include "config.h"
#endif

#include "estimateF0.h"

#include "LorisExceptions.h"	// for Assert

#include <algorithm>
#include <cmath>
#include <numeric>

#include <vector>
using std::vector;

#if defined(HAVE_M_PI) && (HAVE_M_PI)
	const double Pi = M_PI;
#else
	const double Pi = 3.14159265358979324;
#endif

//	begin namespace
namespace Loris {

//	forward declarations for helpers, defined below:
static void
compute_eval_freqs( double fmin, double fmax, 
					vector<double> & eval_freqs );				
static void
evaluate_Q( const vector<double> & amps, 
			const vector<double> & freqs, 
			const vector<double> & eval_freqs, 
			vector<double> & Q );
static vector<double>::const_iterator
choose_peak( const vector<double> & Q );


// ---------------------------------------------------------------------------
//	Iteratively compute the value of the likelihood function
//	at a range of frequencies around the peak likelihood.
//	Return the maximum value when the range of likelihood
//	values computed is less than the specified resolution.
//  Return the frequency and the normalized value of the 
//  likelihood function at that frequency (1.0 indicates that
//  all the peaks are perfect harmonics of the estimated
//  frequency).
F0estimate 
iterative_estimate( const vector<double> & amps, 
					const vector<double> & freqs, 
			        double fmin, double fmax,
			        double resolution )
{
	//	never consider DC (0 Hz) to be a valid fundamental
	fmin = std::max( 1., fmin );
	
    //	when the frequency range is small, few samples are
    //	needed, but initially make sure to sample at least
    //	every 20 Hz. 
    // Scratch that, 20 Hz isn't fine enough, could miss a 
    // peak that way, try 2 Hz. There might be some room to
    // adjust this parameter to trade off speed for robustness.
	int Nsamps = std::max( 8, (int)std::ceil((fmax-fmin)*0.5) );
	vector<double> eval_freqs, Q;
	double peak_freq = fmin, peak_Q = 1;
	
	//	invariant:
	//	the likelihood function for the estimate of the fundamental
	//	frequency is maximized at some frequency between
	//	fmin and fmax (stop when that range is smaller
	//	than the resolution)
	do
	{
		//	determine the frequencies at which to evaluate
		//	the likelihood function
		eval_freqs.resize( Nsamps );
		compute_eval_freqs( fmin, fmax, eval_freqs );
		
		//	evaluate the likelihood function at those 
		//	frequencies:
		Q.resize( Nsamps );
		evaluate_Q( amps, freqs, eval_freqs, Q );
		
		//	find the highest frequency at which the likelihood
		//	function peaks:
		vector<double>::const_iterator peak = choose_peak( Q );
		int peak_idx = peak - Q.begin();
		peak_Q = *peak;
		peak_freq = eval_freqs[ peak_idx ];
		
		//	update search range:
		fmin = eval_freqs[ std::max(peak_idx - 1, 0) ];
		fmax = eval_freqs[ std::min(peak_idx + 1, Nsamps - 1) ];
		Nsamps = std::max( 8, (int)std::ceil((fmax-fmin)*0.05) );
		
	} while ( (fmax - fmin) > resolution );

	return F0estimate( peak_freq, peak_Q );
}

//	compute_eval_freqs
//
//	Fill the frequency vector with a sampling
//	of the range [fmin,fmax].
//
static void
compute_eval_freqs( double fmin, double fmax, 
					vector<double> & eval_freqs )
{
	Assert( fmax > fmin );
	
	double delta = (fmax-fmin)/(eval_freqs.size()-1);
	double f = fmin;
	vector<double>::iterator it = eval_freqs.begin();
	while( it != eval_freqs.end() )
	{
		*it++ = f;
		f += delta;
	}
	eval_freqs.back() = fmax;
}

//	evaluate_Q
//
//	Evaluate the likelihood function at a range of 
//	frequencies, return the results in the vector Q.
//
//	Qterm is a functor to help compute terms
//	in the likelihood function sum.
//
struct Qterm
{
	double f0;
	Qterm( double f ) : f0(f) {}
	
	double operator()( double amp, double freq ) const
	{
		double arg = 2*Pi*freq/f0;
		return amp*amp*std::cos(arg);
	}
};

static void
evaluate_Q( const vector<double> & amps, 
			  const vector<double> & freqs, 
			  const vector<double> & eval_freqs, 
			  vector<double> & Q )
{
	Assert( eval_freqs.size() == Q.size() );
	Assert( amps.size() == freqs.size() );
	
    //  compute a normalization factor equal to the total
    //  energy represented by all the peaks passed in
    //  amps and freqs, so that the value of the likelihood
    //  function does not depend on the overall signal 
    //  amplitude, but instead depends only on the quality
    //  of the estimate, or the confidence in the result, 
    //  and the quality of the final estimate can be evaluated
    //  by the value of the likelihood function (should be greater
    //  than 0.9 or more)
    double etotal = 0;
    vector<double>::const_iterator it;
    for ( it = amps.begin(); it != amps.end(); ++it )
    {
        etotal += *it * *it;
    }
	double norm = 1 / etotal;
    
	//	iterate over the frequencies at which to 
	//	evaluate the likelihood function:
	vector<double>::const_iterator freq_it = eval_freqs.begin();
	vector<double>::iterator Q_it = Q.begin();
	while ( freq_it != eval_freqs.end() )
	{
		double result = 
			std::inner_product( amps.begin(), amps.end(),
								    freqs.begin(),
								    0.,
								    std::plus< double >(),
								    Qterm( *freq_it ) );
                                    
//cout << *freq_it << ", " << result * norm << ";\n";
                                    
		*Q_it++ = result * norm;
		++freq_it;
	}
}

//	choose_peak
//
//	Return the position of last peak that 
//	in the vector Q.
//
static vector<double>::const_iterator
choose_peak( const vector<double> & Q )
{
	Assert( !Q.empty() );
	
	double Qmax = *std::max_element( Q.begin(), Q.end() );
	vector<double>::const_iterator it = (Q.end()) - 1;
	double tmp = *it;
	
   // this threshold determines how strong the 
   // highest-frequency peak in the likelihood 
   // function needs to be relative to the overall
   // peak. For strongly periodic signals, this can
   // be quite near to 1, but for things that are 
   // somewhat non-harmonic, setting it too high
   // gives octave errors. Cannot tell whether errors
   // willbe introduced by having it too low.
	const double threshold = 0.85 * Qmax;
	while( (it != Q.begin()) && ((*it < threshold) || (*it < *(it-1))) )
	{
		--it;
		tmp = *it;
	}
	
	return it;
}

}	//	end of namespace Loris

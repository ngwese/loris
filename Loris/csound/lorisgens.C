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
 *	lorisgens.C
 *
 *	Implementation of Csound unit generators supporting bandwidth-enhanced 
 *	synthesis using the Loris library.
 *
 *	This lorisplay module was originally written by Corbin Champion, 2002.
 *
 * Kelly Fitz, 9 May 2002
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "cs.h"
#include "lorisgens.h"
#include "string.h"

#include <Exception.h>
#include <Oscillator.h>
#include <Partial.h>
#include <SdifFile.h>

#include <algorithm>
#include <exception>
#include <map>
#include <string>
#include <vector>

using namespace Loris;
using namespace std;

typedef std::vector< Partial > PARTIALS;
typedef std::vector< Oscillator > OSCILS;

// ---------------------------------------------------------------------------
//	sdif_openfile
// ---------------------------------------------------------------------------
//	Import Partials from the named SDIF file if they have not already 
//	been imported. Return a pointer to the SdifFile object containing the
//	Partials. 
//
//	This function could be used by several generators, so that they could
//	all share the pool of reassigned bandwidth-enhanced partials imported
//	from SDIF files.
//
static SdifFile * sdif_openfile( const std::string & filename )
{
	static std::map< std::string, SdifFile * > filenamemap;
	
	std::map< std::string, SdifFile * >::iterator it = filenamemap.find( filename );
		
	if ( it != filenamemap.end() )
	{
     	// fprintf(stderr, "** found SDIF file %s\n", filename.c_str());
		return it->second;
	}
	else
	{	
		try 
		{
			// fprintf(stderr, "** importing SDIF file %s\n", filename.c_str());
			SdifFile * f = new SdifFile(filename);
			filenamemap[filename] = f;
			return f;
		}
		catch(Exception ex)
		{
		  fprintf(stderr, "\nERROR importing SDIF file: %s", ex.what());
		  return NULL;
		}
		catch(std::exception ex)
		{
		  fprintf(stderr, "\nERROR importing SDIF file: %s", ex.what());
		  return NULL;
		}
	}
}

// ---------------------------------------------------------------------------
//	import_partials
// ---------------------------------------------------------------------------
//	Import the Partials, if necessary, and make a private copy of them,
//	and allocate Oscillators for each Partial.
//
static void import_partials( const std::string & sdiffilname, PARTIALS & part )
{
	SdifFile * f = sdif_openfile( sdiffilname );
	if ( f == NULL )
		return;
		
	//	copy the Partials into the vector in bwestore:
	//	NOTE - cannot (trivially) share Partials between generators, because
	//	fadetimes might be different!
	part.reserve( f->partials().size() );
	part.insert( part.begin(), f->partials().begin(), f->partials().end() );
}

// ---------------------------------------------------------------------------
//	apply_fadetime
// ---------------------------------------------------------------------------
//	Fade Partials in and out, if fadetime > 0.
//
static void apply_fadetime( PARTIALS & part, double fadetime )
{
	//	nothing to do if fadetime is zero:
	if (fadetime <= 0.)
		return;
		
	//	iterator over all Partials, adding Breakpoints at both ends:
	PARTIALS::iterator iter;
	for ( iter = part.begin(); iter != part.end(); ++iter )  
	{
		Partial & partial = *iter;
		
		double btime = partial.startTime();	// get start time of partial
	    double etime = partial.endTime();	// get end time of partial

		//	if a fadetime has been specified, introduce zero-amplitude
		//	Breakpoints to fade in and out over fadetime seconds:
		if( fadetime != 0 )
		{
			if ( partial.amplitudeAt(btime) > 0. )
			{
				//	only fade in if starting amplitude is non-zero:
				if( btime > 0. ) 
				{
					//	if the Partial begins after time 0, insert a Breakpoint
					//	of zero amplitude at a time fadetime before the beginning, 
					//	of the Partial, or at zero, whichever is later:
					double t = std::max(btime - fadetime, 0.);
					partial.insert( t, Breakpoint( partial.frequencyAt(t), partial.amplitudeAt(t), 
												   partial.bandwidthAt(t), partial.phaseAt(t)));
				}
				else 
				{
					//	if the Partial begins at time zero, insert the zero-amplitude
					//	Breakpoint at time zero, and make sure that the next Breakpoint
					//	in the Partial is no more than fadetime away from the beginning
					//	of the Partial:
					
					//	find the first Breakpoint later than time 0:
					Partial::iterator pit = partial.begin();
					while (pit.time() < 0.)
						++pit;
					if ( pit.time() > fadetime )
					{
						//	if first Breakpoint afer 0 is later than fadetime, 
						//	insert a Breakpoint at fadetime:
						double t = fadetime;
						partial.insert( t, Breakpoint( partial.frequencyAt(t), partial.amplitudeAt(t), 
													   partial.bandwidthAt(t), partial.phaseAt(t)));
					}
					
					//	insert the zero-amplitude Breakpoint at 0:
					partial.insert( 0, Breakpoint( partial.frequencyAt(0), 0, 
												   partial.bandwidthAt(0), partial.phaseAt(0)));

				}
			}
			
			//	add fadeout Breakpoint at end:
			double t = etime + fadetime;
			partial.insert( t, Breakpoint( partial.frequencyAt(t), 0, 
										   partial.bandwidthAt(t), partial.phaseAt(t)));
		}
	}
	
}

// ---------------------------------------------------------------------------
//	helpers
// ---------------------------------------------------------------------------

//function gets the amplitude of a partial at a specific time
static
float getAmp(float time, Partial *part)
{
return part->amplitudeAt(time);
}

//function gets the frequency of a partial at a specific time
static
float getFreq(float time, Partial *part)
{
return (part->frequencyAt(time))*TWOPI/esr;
}

//function gets the bandwidth of a partial at a specific time
static
float getBw(float time, Partial *part)
{
return part->bandwidthAt(time);
}

//function gets the phase of a partial at a specific time
static
float getPhase(float time, Partial *part)
{
return part->phaseAt(time);
}

//function initiallizes an oscillator according to a partial at a specific time
static
void init_oscillator(Partial *part, Oscillator *oscp, float time)
{
	//give the oscillator values for initialization
	oscp->setBandwidth(getBw(time, part));
	oscp->setAmplitude(getAmp(time, part));
	oscp->setRadianFreq(getFreq(time, part));
	oscp->setPhase(getPhase(time, part));

}

// ---------------------------------------------------------------------------
//	Lorisplay_priv definition
// ---------------------------------------------------------------------------
// 	Define a structure holding private internal data.
//
struct Lorisplay_priv
{
	OSCILS oscp;
	PARTIALS part;
	
	std::vector< double > dblbuffer;
	
	Lorisplay_priv( LORISPLAY * params );
	~Lorisplay_priv( void ) {}
}; 

// ---------------------------------------------------------------------------
//	Lorisplay_priv contructor
// ---------------------------------------------------------------------------
//
Lorisplay_priv::Lorisplay_priv( LORISPLAY * params ) :
	dblbuffer( ksmps, 0. )
{
	std::string sdiffilname;

	//	determine the name of the SDIF file to use:
	//	this code adapted from ugens8.c pvset()
	if ( *params->ifilnam == sstrcod )
	{
		//	use strg name, if given:
		sdiffilname = unquote(params->STRARG);
	}
    /* unclear what this does, not described in pvoc docs
    else if ((long)*p->ifilnam <= strsmax && strsets != NULL && strsets[(long)*p->ifilnam])
      strcpy(sdiffilname, strsets[(long)*p->ifilnam]);
     */
    else 
    {
    	//	else use loris.filnum
    	char tmp[32];
    	sprintf(tmp,"loris.%d", (int)*params->ifilnam);
		sdiffilname = tmp;
	}
	
	//	allocate Partials and Oscillators:
	import_partials( sdiffilname, part );
	oscp.resize( part.size() );	
	apply_fadetime( part, *params->fadetime );
}

//function runs at initialization time for lorisplay
static void lorisplay_cleanup(void * p);

extern "C"
void lorisplay_setup(LORISPLAY *p)
{
	p->bwestore = new Lorisplay_priv( p );
	p->h.dopadr = lorisplay_cleanup;  // set lorisplay_cleanup as cleanup routine
}

//function runs through on the k-rate for lorisplay
extern "C"
void lorisplay(LORISPLAY *p)
{
	//temp variables
	float *answer = p->result;
    int nn = ksmps;
	int i;
	float amp;
	float prevtime = *(p->time) - ksmps/esr;   //calculation of previous time
	int numOscils = p->bwestore->oscp.size();

	//	zero the buffer first!
	std::fill( p->bwestore->dblbuffer.begin(), p->bwestore->dblbuffer.end(), 0. );
	double * bufbegin =  p->bwestore->dblbuffer.begin();
	for(i=0; i < numOscils; i++)  //for each oscillator
	{
	amp = getAmp(*(p->time), &(p->bwestore->part[i]));  //get amp at current time
	if(amp > 0 || p->bwestore->oscp[i].amplitude() > 0)  //if current or last amplitude greater than zero
	{
		if (p->bwestore->oscp[i].amplitude() == 0){ //if last amplitude was zero reinitialize the oscillator
			// fprintf(stderr, "initializing oscillator %d at time %f for Partial beginning at time %f\n",
			//				i, prevtime, p->bwestore->part[i].startTime() );
			init_oscillator(&(p->bwestore->part[i]), &(p->bwestore->oscp[i]), prevtime);
		}	
		p->bwestore->oscp[i].generateSamples( bufbegin, bufbegin + nn, 
											  *(p->freqenv)*getFreq(*(p->time), &(p->bwestore->part[i])), 
											  *(p->ampenv)*getAmp(*(p->time), &(p->bwestore->part[i])), 
											  *(p->bwenv)*getBw(*(p->time), &(p->bwestore->part[i])));  
											  //generate samples from oscillators
	}
	} 
	
	std::vector<double> & buf = p->bwestore->dblbuffer;
	do{
		*answer++ = buf[ksmps-nn] * 32767;  // make amplitudes so they correspond to csound amplitudes
	}while(--nn);
	 
}

//	function cleans up after lorisplay
static
void lorisplay_cleanup(void * p)
{
	LORISPLAY * tp = (LORISPLAY *)p;
	delete tp->bwestore;
}

// ---------------------------------------------------------------------------
//	Lorismorph_priv definition
// ---------------------------------------------------------------------------
// 	Define a structure holding private internal data for lorismorph
//
struct Lorismorph_priv
{
	OSCILS oscp;
	
	PARTIALS src_part_labeled, tgt_part_labeled;
	PARTIALS src_part_unlabeled, tgt_part_unlabeled;
	
	std::vector< double > dblbuffer;
	
	Lorismorph_priv( LORISMORPH * params );
	~Lorismorph_priv( void );
}; 

// ---------------------------------------------------------------------------
//	Lorismorph_priv contructor
// ---------------------------------------------------------------------------
//
Lorismorph_priv::Lorismorph_priv( LORISMORPH * params ) :
	dblbuffer( ksmps, 0. )
{
}

/*
	Yuck. It appears that there's NO WAY to specify two string arguments at all.
	
	I think that the way to do this is going to be: implement a reader in the
	fashion of lpread, and give it a way to specify an index number, something 
	like lpslot, that can be used to refer to that data later. Then, use this
	preloaded data in lorismorph, and maybe optionally lorisplay too.
	
	The nice thing about a reader is that it could allow is to do temporal
	feature alignment in Csound too, as in the lp case, where the reader
	has a time index.
*/
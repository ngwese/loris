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
#include <string>
#include <vector>

using namespace Loris;
using namespace std;

static void createPartials(Lorisplay_priv *bwestore);
static void destroy_partials(Partial *part);
static void destroy_oscillators(Oscillator *oscp);
static void lorisplay_cleanup(void * p);

// ---------------------------------------------------------------------------
//	Lorisplay_priv definition
// ---------------------------------------------------------------------------
// 	Define a structure holding private internal data.
//
struct Lorisplay_priv
{
	Oscillator *oscp;
	Partial *part;
	
	// double *dblbuffer;
	std::vector< double > dblbuffer;
	std::string sdiffilnam;
	
	int n;
	float fadetime;
	
	Lorisplay_priv( LORISPLAY * params );
	~Lorisplay_priv( void );
}; 

// ---------------------------------------------------------------------------
//	Lorisplay_priv contructor
// ---------------------------------------------------------------------------
//
Lorisplay_priv::Lorisplay_priv( LORISPLAY * params ) :
	dblbuffer( ksmps, 0. ),
	fadetime( *params->fadetime )
{
	//	determine the name of the SDIF file to use:
	//	this code adapted from ugens8.c pvset()
	if ( *params->ifilnam == sstrcod )
	{
		//	use strg name, if given:
		sdiffilnam = unquote(params->STRARG);
	}
    /* unclear what this does, not described in pvoc docs
    else if ((long)*p->ifilnam <= strsmax && strsets != NULL && strsets[(long)*p->ifilnam])
      strcpy(sdiffilnam, strsets[(long)*p->ifilnam]);
     */
    else 
    {
    	//	else use loris.filnum
    	char tmp[32];
    	sprintf(tmp,"loris.%d", (int)*params->ifilnam);
		sdiffilnam = tmp;
	}
	
	//	allocate Partials and Oscillators:
	createPartials(this);
}

// ---------------------------------------------------------------------------
//	Lorisplay_priv contructor
// ---------------------------------------------------------------------------
//
Lorisplay_priv::~Lorisplay_priv( void )
{
	destroy_oscillators(oscp);
	destroy_partials(part);
}

/* I made a small step toward making this more efficient, by only
	loading each SDIF file once. I am still unnecessarily making
	copies of all the partials. There's lots of work to be done here.
	-kel
	7 May 2002
 */
 
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

//function creates an array of n oscillators
static
Oscillator * createOscillators(int n)
{
Oscillator *oscp = new Oscillator[n];
return oscp;
}

#define MAXFILES (32)
static SdifFile * files[MAXFILES];

//	access a named SDIF file
static int sdif_openfile(const std::string & filename)
{
	static std::vector< std::string > filenames(MAXFILES);
	int i;
    char *pname;
    SdifFile *pfile = NULL;

    for (i=0; i < MAXFILES; ++i) {
      if (files[i]==NULL)	{				//	if we find an empty space, load 
      										//	the file into that space
        break;
      }
      else if ( filenames[i] == filename ) {//	if we find the one we're looking
      										//	for, return its index
      	// fprintf(stderr, "** found SDIF file %s at index %d\n", filename.c_str(), i);
      	return i;
      }
    }
    
    if (i==MAXFILES) 
    {
      fprintf(stderr, "\nERROR importing SDIF file: too many files open");
      return -1;
    }

	try 
	{
    	files[i] = new SdifFile(filename);
    	filenames[i] = filename;
     	// fprintf(stderr, "** importing SDIF file %s at index %d\n", filename.c_str(), i);
    }
    catch(Exception ex)
    {
      fprintf(stderr, "\nERROR importing SDIF file: %s", ex.what());
      return -1;
    }
    catch(std::exception ex)
    {
      fprintf(stderr, "\nERROR importing SDIF file: %s", ex.what());
      return -1;
    }

    return i;
}

//function creates all the partials specified by the input file
static
void createPartials(Lorisplay_priv *bwestore)
{
int count = 0;

	int idx = sdif_openfile(bwestore->sdiffilnam);
	if (idx < 0) {
		bwestore->n = 0;
		return;
	}
	
	SdifFile & f = *(files[idx]);

	bwestore->n = f.partials().size();  //the number of partials total
	bwestore->oscp = createOscillators(bwestore->n);  //create n oscillators
	list< Partial >::iterator iter;  //make a list of partials to iterate through
	bwestore->part = new Partial[bwestore->n];  //create array of partials
	for ( iter = f.partials().begin(); iter != f.partials().end(); ++iter )  //for all of the partials
	{

		bwestore->part[count] = *iter;  //set the partial to one of the partials read in

		double btime = bwestore->part[count].startTime();  //get start time of partial
	        double etime = bwestore->part[count].endTime();  //get end time of partial

		//get parameters at begining and end of partial
		double bfreq = bwestore->part[count].frequencyAt(btime);
		double efreq = bwestore->part[count].frequencyAt(etime);
		double bamp = bwestore->part[count].amplitudeAt(btime);
		double eamp = bwestore->part[count].amplitudeAt(etime);
		double bbw = bwestore->part[count].bandwidthAt(btime);
		double ebw = bwestore->part[count].bandwidthAt(etime);
		double bph = bwestore->part[count].phaseAt(btime);
		double eph = bwestore->part[count].phaseAt(etime);
		if( (bwestore->fadetime) != 0 )  //if a fadetime is specified
			{
			if( btime != 0 ) //if the partial doesn't begin at 0
			{
				if( (btime - (bwestore->fadetime)) < 0 )  //if the partial start time - fadetime is less that zero
				{
					bwestore->part[count].insert(0.0, Breakpoint(bfreq, bamp, bbw, bph)); //start fade in at 0
				}
				else  //otherwise
				{
					bwestore->part[count].insert(btime - (bwestore->fadetime), Breakpoint(bfreq, bamp, bbw, bph));  //start fade in at start time -fadetime
				} 
			}
			bwestore->part[count].insert(etime + (bwestore->fadetime), Breakpoint(efreq, eamp, ebw, eph));  //add fadeout break points
			}

		init_oscillator(&(bwestore->part[count]), &(bwestore->oscp[count]),btime);  //initialize an oscillator with the partial
		++count;

	}
}

//function cleans up the array of partials created
static
void destroy_partials(Partial *part)
{
	
	delete [] part;

}

// function cleans up array of oscillators created
static
void destroy_oscillators(Oscillator *oscp)
{

	delete [] oscp;

}

//function runs at initialization time for bweoscil
extern "C"
void lorisplay_setup(LORISPLAY *p)
{
	int i;

	p->bwestore = new Lorisplay_priv( p );
	//p->bwestore = (Lorisplay_priv *)malloc(sizeof(Lorisplay_priv));  //allocate storage structure
	//p->bwestore->fadetime = *(p->fadetime);  //store fadetime in bwestore
	#if 0
	/* import the SDIF file, this code adapted from ugens8.c pvset() */
	if (*p->ifilnam == sstrcod)                         /* if strg name given */
      strcpy(p->bwestore->sdiffilnam, unquote(p->STRARG));           /*   use that         */
    /* unclear what this does, not described in pvoc docs
    else if ((long)*p->ifilnam <= strsmax && strsets != NULL && strsets[(long)*p->ifilnam])
      strcpy(sdiffilnam, strsets[(long)*p->ifilnam]);
     */
    else 
    	sprintf(p->bwestore->sdiffilnam,"loris.%d", (int)*p->ifilnam); /* else loris.filnum   */
	#endif
	
    //createPartials(p->bwestore);
	p->h.dopadr = lorisplay_cleanup;  //set lorisplay_cleanup as cleanup routine
	//p->bwestore->dblbuffer = (double *)malloc(ksmps*sizeof(double));  //allocate memory for buffer to be used for generating samples into
}

//function runs through on the k-rate for bweoscil
extern "C"
void lorisplay(LORISPLAY *p)
{
	//temp variables
	float *answer = p->result;
        int nn = ksmps;
	int i;
	float amp;
	float prevtime = *(p->time) - ksmps/esr;   //calculation of previous time
	//	zero the buffer first!
	//bzero(p->bwestore->dblbuffer->begin(), ksmps*sizeof(double));  //zero buffer that result is stored in
	std::fill( p->bwestore->dblbuffer.begin(), p->bwestore->dblbuffer.end(), 0. );
	double * bufbegin =  p->bwestore->dblbuffer.begin();
	for(i=0; i < (p->bwestore->n); i++)  //for each oscillator
	{
	amp = getAmp(*(p->time), &(p->bwestore->part[i]));  //get amp at currect time
	if(amp > 0 || p->bwestore->oscp[i].amplitude() > 0)  //if current or last amplitude greater than zero
	{
		if (p->bwestore->oscp[i].amplitude() == 0){ //if last amplitude was zero reinitialize the oscillator
			init_oscillator(&(p->bwestore->part[i]), &(p->bwestore->oscp[i]), prevtime);
		}	
		p->bwestore->oscp[i].generateSamples(bufbegin, bufbegin + nn, *(p->freqenv)*getFreq(*(p->time), &(p->bwestore->part[i])), *(p->ampenv)*getAmp(*(p->time), &(p->bwestore->part[i])), *(p->bwenv)*getBw(*(p->time), &(p->bwestore->part[i])));  //generate samples from oscillators
	}
	} 
	
	std::vector<double> & buf = p->bwestore->dblbuffer;
	do{
		*answer++ = buf[ksmps-nn] * 32767;  // make amplitudes so they correspond to csound amplitudes
	}while(--nn);
	 
}

//function cleans up after bweoscil is ran
static
void lorisplay_cleanup(void * p)
{
	LORISPLAY * tp = (LORISPLAY *)p;
	//destroy or free all memory allocation created
	delete tp->bwestore;
	// destroy_oscillators(tp->bwestore->oscp);
	// destroy_partials(tp->bwestore->part);
	//free(tp->bwestore->dblbuffer);
}


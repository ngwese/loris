//include appropriate libraries
#include "cs.h"
#include "bwe2.h"
#include "string.h"
#include <Exception.h>
#include <Oscillator.h>
#include <Partial.h>
#include <SdifFile.h>
#include <exception>
#include <string>
#include <vector>

using namespace Loris;
using namespace std;

//structure to hold values
struct PRIVSTORE{
double *store;
Oscillator *oscp;
Partial *part;
char strarg[64];
int n;
float fadetime;
}; 

/* GET RID OF ALL THESE EXTERN C'S, THESE THINGS ARE ALL LOCAL */	

/* I made a small step toward making this more efficient, by only
	loading each SDIF file once. I am still unnecessarily making
	copies of all the partials. There's lots of work to be done here.
	-kel
	7 May 2002
 */
 
//function gets the amplitude of a partial at a specific time
extern "C"
float getAmp(float time, Partial *part)
{
return part->amplitudeAt(time);
}

//function gets the frequency of a partial at a specific time
extern "C"
float getFreq(float time, Partial *part)
{
return (part->frequencyAt(time))*TWOPI/esr;
}

//function gets the bandwidth of a partial at a specific time
extern "C"
float getBw(float time, Partial *part)
{
return part->bandwidthAt(time);
}

//function gets the phase of a partial at a specific time
extern "C"
float getPhase(float time, Partial *part)
{
return part->phaseAt(time);
}

//function initiallizes an oscillator according to a partial at a specific time
extern "C"
void init_oscillator(Partial *part, Oscillator *oscp, float time)
{
	//give the oscillator values for initialization
	oscp->setBandwidth(getBw(time, part));
	oscp->setAmplitude(getAmp(time, part));
	oscp->setRadianFreq(getFreq(time, part));
	oscp->setPhase(getPhase(time, part));

}

//function creates an array of n oscillators
extern "C"
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
      	fprintf(stderr, "** found SDIF file %s at index %d\n", filename.c_str(), i);
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
     	fprintf(stderr, "** importing SDIF file %s at index %d\n", filename.c_str(), i);
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
extern "C"
void createPartials(PRIVSTORE *bwestore)
{
int count = 0;

	int idx = sdif_openfile(bwestore->strarg);
	if (idx < 0) {
		bwestore->n = 0;
		return;
	}
	
	SdifFile & f = *(files[idx]);
	//SdifFile f(bwestore->strarg); //read in partials

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
extern "C"
void destroy_partials(Partial *part)
{
	
	delete [] part;

}

// function cleans up array of oscillators created
extern "C"
void destroy_oscillators(Oscillator *oscp)
{

	delete [] oscp;

}

//function runs through on the k-rate for bweoscil
extern "C"
void bwes(BWE *p)
{
	//temp variables
	float *answer = p->result;
        int nn = ksmps;
	int i;
	float amp;
	float prevtime = *(p->time) - ksmps/esr;   //calculation of previous time
	//	zero the buffer first!
	bzero(p->bwestore->store, ksmps*sizeof(double));  //zero buffer that result is stored in
	for(i=0; i < (p->bwestore->n); i++)  //for each oscillator
	{
	amp = getAmp(*(p->time), &(p->bwestore->part[i]));  //get amp at currect time
	if(amp > 0 || p->bwestore->oscp[i].amplitude() > 0)  //if current or last amplitude greater than zero
	{
		if (p->bwestore->oscp[i].amplitude() == 0){ //if last amplitude was zero reinitialize the oscillator
			init_oscillator(&(p->bwestore->part[i]), &(p->bwestore->oscp[i]), prevtime);
		}	
		p->bwestore->oscp[i].generateSamples(p->bwestore->store, p->bwestore->store + nn, *(p->freqenv)*getFreq(*(p->time), &(p->bwestore->part[i])), *(p->ampenv)*getAmp(*(p->time), &(p->bwestore->part[i])), *(p->bwenv)*getBw(*(p->time), &(p->bwestore->part[i])));  //generate samples from oscillators
	}
	} 
	do{
		*answer++ = *(p->bwestore->store + (ksmps-nn))*32767;  //make amplitudes so they correspond to csound amplitudes
	}while(--nn);
	 
}

//function cleans up after bweoscil is ran
extern "C"
void trycleanup(void * p)
{
	BWE * tp = (BWE *)p;
	//destroy or free all memory allocation created
	destroy_oscillators(tp->bwestore->oscp);
	destroy_partials(tp->bwestore->part);
	free(tp->bwestore->store);
}

//function runs at initialization time for bweoscil
extern "C"
void bwei(BWE *p)
{
	int i;

	p->bwestore = (PRIVSTORE *)malloc(sizeof(PRIVSTORE));  //allocate storage structure
	p->bwestore->fadetime = *(p->fadetime);  //store fadetime in bwestore
	
	/* import the SDIF file, this code copied from ugens8.c pvset() */
	if (*p->ifilnam == sstrcod)                         /* if strg name given */
      strcpy(p->bwestore->strarg, unquote(p->STRARG));           /*   use that         */
    /* unclear what this does, not described in pvoc docs
    else if ((long)*p->ifilnam <= strsmax && strsets != NULL && strsets[(long)*p->ifilnam])
      strcpy(sdiffilnam, strsets[(long)*p->ifilnam]);
     */
    else 
    	sprintf(p->bwestore->strarg,"loris.%d", (int)*p->ifilnam); /* else loris.filnum   */

	
	/* Corbin's code
	if (*p->ifilnam == sstrcod)  //read in filename
		strcpy(p->bwestore->strarg, unquote(p->STRARG)); 
     */    
    createPartials(p->bwestore);
	p->h.dopadr = trycleanup;  //set trycleanup as cleanup routine
	p->bwestore->store = (double *)malloc(ksmps*sizeof(double));  //allocate memory for buffer to be used for generating samples into
}

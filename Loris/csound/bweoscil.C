//include appropriate libraries
#include "cs.h"
#include "bwe2.h"
#include "string.h"
#include <Oscillator.h>
#include <Partial.h>
#include <SdifFile.h>
namespace foo {
#include <math.h>  //have to add math.h this way, will get conflict otherwise
}

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
return (part->frequencyAt(time))*2*M_PI/esr;
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

//function creates all the partials specified by the input file
extern "C"
void createPartials(PRIVSTORE *bwestore)
{
int count = 0;
SdifFile f(bwestore->strarg); //read in partials
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
	if (*p->ifilnam == sstrcod)  //read in filename
		strcpy(p->bwestore->strarg, unquote(p->STRARG)); 
        createPartials(p->bwestore);
	p->h.dopadr = trycleanup;  //set trycleanup as cleanup routine
	p->bwestore->store = (double *)malloc(ksmps*sizeof(double));  //allocate memory for buffer to be used for generating samples into
}


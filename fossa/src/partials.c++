/*
 * This is Fossa, a grapical control application for analysis, synthesis, 
 * and manipulations of digitized sounds using Loris (Fitz and Haken). 
 *
 * Fossa is Copyright (c) 2001 - 2002 by Susanne Lefvert
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
 * partials.c++
 *
 * Implementation of the Partials class. 
 * A Partial has a name, and a list of actual Loris partials representing sound.
 * It contains state information and has plots over its frequency, amplitude, and 
 * noise envelopes over time. Partials communicates with lorisInterface in order
 * to preform modifications of the list of Loris::Partial.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#if HAVE_CONFIG_H    // #define directives are placed in config.h by autoconf
#include <config.h>  
#endif

#include "partials.h"

using std::cout;


// ---------------------------------------------------------------------------
//	Partials constructor
// ---------------------------------------------------------------------------
// A Partial has a name, and a list of actual Loris partials representing sound.
// It contains state information and has plots over its frequency, amplitude, and 
// noise envelopes over time. Partials communicates with lorisInterface in order
// to preform modifications of the list of Loris::Partial.
Partials::Partials(
	list<Loris::Partial>	p,
	QString			n,
	LorisInterface*		inter
){ 
  name        = n;
  interface   = inter;
  partialList = new list<Loris::Partial>(p);  // a list of Loris::Partial, the
  setValues();                                // actual sound partials

  distilled   = false;         
  channelized = false;
  state       = amplitude;

  // Plots over amplitude, frequency, and noise envelopes in partialList
  amplitudePixmap = new AmplitudePixmap(getPartials(), maxTime, maxAmplitude);
  frequencyPixmap = new FrequencyPixmap(getPartials(), maxTime, maxFrequency);
  noisePixmap     = new NoisePixmap(getPartials(), maxTime, maxNoise);
}

// ---------------------------------------------------------------------------
//	Partials destructor
// ---------------------------------------------------------------------------
Partials::~Partials(){
  delete partialList; 
}

// ---------------------------------------------------------------------------
//	setState
// ---------------------------------------------------------------------------
// state can be amplitude, frequency, and noise.
void Partials::setState(State newState){
  state = newState;
}

// ---------------------------------------------------------------------------
//	getName
// ---------------------------------------------------------------------------
// Gives the name of the partials
QString Partials::getName() const  { 
  return name; 
}

// ---------------------------------------------------------------------------
//	getName
// ---------------------------------------------------------------------------
// Returns number of partials
int Partials::getNumberOfPartials() const{
  return nrOfPartials;
}

// ---------------------------------------------------------------------------
//	getAmplitudePixmap
// ---------------------------------------------------------------------------
// Returns the amplitued envelope plotted against time in form of a pixmap
QPixmap Partials::getAmplitudePixmap() const{
  return *amplitudePixmap;
}

// ---------------------------------------------------------------------------
//	getFrequencyPixmap
// ---------------------------------------------------------------------------
// Returns the frequency envelope plotted against time in form of a pixmap
QPixmap Partials::getFrequencyPixmap() const{
  return *frequencyPixmap;
}

// ---------------------------------------------------------------------------
//	getNoisePixmap
// ---------------------------------------------------------------------------
// Returns the noise envelope plotted against time in form of a pixmap
QPixmap Partials::getNoisePixmap() const{
  return *noisePixmap;
}

// ---------------------------------------------------------------------------
//	getNoisePixmap
// ---------------------------------------------------------------------------
// Returns a list of Loris::Partial, the actual result of a sound analysis
list<Loris::Partial>* Partials::getPartials() const{
  return partialList;
}

// ---------------------------------------------------------------------------
//	getState
// ---------------------------------------------------------------------------
// State can be amplitude, frequency, and noise
Partials::State Partials::getState() const{
  return state;
}

// ---------------------------------------------------------------------------
//	getDuration
// ---------------------------------------------------------------------------
// Returns the time in seconds of the longest partial in the list
double Partials::getDuration() const{
  return maxTime;
}

// ---------------------------------------------------------------------------
//	getMaxAmplitude
// ---------------------------------------------------------------------------
// Returns the maximum amplitude of partials in the list
double Partials::getMaxAmplitude() const{
  return maxAmplitude;
}

// ---------------------------------------------------------------------------
//	getMaxFrequency
// ---------------------------------------------------------------------------
// Returns the maximum frequency of partials in the list
double Partials::getMaxFrequency() const{
  return maxFrequency;
}

// ---------------------------------------------------------------------------
//	getMaxNoise
// ---------------------------------------------------------------------------
// Returns the maximum noise of partials in the list
double Partials::getMaxNoise() const{
  return maxNoise;
}

// ---------------------------------------------------------------------------
//      isChannelized
// ---------------------------------------------------------------------------
// Returns true if partials are channelized
bool Partials::isChannelized() const{
  return channelized;
}

// ---------------------------------------------------------------------------
//      isDistilled
// ---------------------------------------------------------------------------
// Returns true if partials are distilled
bool Partials::isDistilled() const{
  return distilled;
}

// ---------------------------------------------------------------------------
//      play
// ---------------------------------------------------------------------------
// Plays partials 
// This operation produces delays in the gui and should be solved in 
// another way.  Ideally, partials should be synthsized everytime an
// modification action is performed (for example channelize, distill)
// and the result saved. Audition of the result would then be much quicker.
void Partials::play(){
  try{
    // do a synthesise instead and save the result in a temporary file 
    //ready to play
    interface->exportAiff(44100, 16, "temp", *partialList, getDuration()); 

    const char *cmd = "play temp";    // play any sound file to audio device
    if ((popen(cmd, "w")) != NULL){   // The  popen() function opens a process 
                                     // by creating a pipe, forking, and 
                                     // invoking the shell.
    }
    // temp should be deleted!!!!!
  }
  catch(...){
    throw;
  }
}

// ---------------------------------------------------------------------------
//      rename
// ---------------------------------------------------------------------------
// Change the name
void Partials::rename(QString newName){
  name = newName;
}


// ---------------------------------------------------------------------------
//      shiftAmplitude
// ---------------------------------------------------------------------------
// scales partials amplitude with the given argument value 
void Partials::shiftAmplitude(double val){
  list<Partial>::iterator it;
  Partial_Iterator pIt;

  try{
    for(it = partialList->begin(); it != partialList->end(); it++){
      for(pIt = it->begin(); pIt!=it->end(); pIt++){
	pIt->setAmplitude(pIt->amplitude() * val);
      }
    }
    if((maxAmplitude * val) > maxAmplitude){
      maxAmplitude = maxAmplitude * val;
    }
  }
  catch(...){
     throw;
  }

  setValues();
  updatePixmap();
}

// ---------------------------------------------------------------------------
//      shiftNoise
// ---------------------------------------------------------------------------
// scales partials noise with the given argument value 
void Partials::shiftNoise(double val){
  list<Partial>::iterator it;
  Partial_Iterator pIt;

  try{
    for(it = partialList->begin(); it != partialList->end(); it++){
      for(pIt = it->begin(); pIt!=it->end(); pIt++){
	pIt->setBandwidth(pIt->bandwidth() * val);
      }
    }
    if((maxNoise * val) > maxNoise){
      maxNoise = maxNoise * val;
    }
  }
  catch(...){
    throw;
  }

  setValues();
  updatePixmap();
}

// ---------------------------------------------------------------------------
//      shiftFrequency (shift pitch)
// ---------------------------------------------------------------------------
// scales partials frequency with the given argument value 
void Partials::shiftFrequency(double cents){
  list<Partial>::iterator it;
  Partial_Iterator pIt;

  double pscale = pow(2., (0.01* cents) / 12.);   
  
  //cout<<"Partials::shiftFrequency: shift frequency with scale of: "<<pscale<<endl;
  try{
    for(it = partialList->begin(); it != partialList->end(); ++it){
      for(pIt = it->begin(); pIt!=it->end(); ++pIt){
	pIt->setFrequency(pIt->frequency() * pscale);
      }
    }
    if(maxFrequency * pscale > maxFrequency){
      maxFrequency = maxFrequency * pscale;
    }
  }
  catch(...){
    cout<<"Partials::shiftFrequency(): caught any exception"<<endl;
    throw;
  }

  setValues();
  updatePixmap();
}

// ---------------------------------------------------------------------------
//      channelize
// ---------------------------------------------------------------------------
// Channelize Loris::Partials by communicating with lorisInterface.
// After channelization, max values have to be recalculated and the plots
// need to be updated
void Partials::channelize(
	int	refLabel,
	double	minFreq,
	double	maxFreq
){
  try{
    interface -> channelize(
	refLabel,
	minFreq,
	maxFreq,
	*partialList
    );
    channelized = true;
    setValues();
    updatePixmap();
  }
  catch(...){
    throw;
  }
}

// ---------------------------------------------------------------------------
//      distill
// ---------------------------------------------------------------------------
// Distill Loris::Partials by communicating with lorisInterface.
// After distillation, max values have to be recalculated and the plots
// need to be updated
void Partials::distill(){
  try{
    interface -> distill(*partialList);
    distilled = true;
    setValues();
    updatePixmap();
  }
  catch(...){
    throw;
  }
}

// ---------------------------------------------------------------------------
//      exportToAiff
// ---------------------------------------------------------------------------
// Export Loris::Partials to aiff file format by communicating with lorisInterface
void Partials::exportToAiff(
	double	sampleRate,
	int	sampleBits,
	const	char* name
){
  try{
    interface->exportAiff(
	sampleRate,
	sampleBits,
	name,
	*partialList,
	maxTime
    );
  }
  catch(...){
    throw;
  }
}

// ---------------------------------------------------------------------------
//      exportToSdif
// ---------------------------------------------------------------------------
// Export Loris::Partials to sdif file format by communicating with lorisInterface
void Partials::exportToSdif(const char* name){
  try{
    interface->exportSdif(name, *partialList);
  }
  catch(...){
    throw;
  }
}

/************************ helpers *****************************/

// ---------------------------------------------------------------------------
//      setValues
// ---------------------------------------------------------------------------
// The variables containing maximum amplitude, frequency, and noise, as well as
// longest partial and number of partials are recalculated. This is done when the
// Loris::Partials changes and not when they are requested to avoid unneccessary 
// delays in the GUI.
void Partials::setValues(){
  list<Partial>::iterator it;
  Partial_Iterator pIt;

  double amplitude;
  double frequency;
  double noise;
  
  maxAmplitude = 0;
  maxFrequency = 0;
  maxNoise     = 0;
  nrOfPartials = 0;
  maxTime      = 0;
  
  try{
    // loop through all partials in the list
    for(it = partialList->begin(); it != partialList->end(); it++){
      nrOfPartials ++;
      maxTime = std::max(maxTime, it->endTime());
      
      // loop through all breakpoints in a partial
      for(pIt = it->begin(); pIt != it->end(); pIt++){
	amplitude = pIt->amplitude();
	frequency = pIt->frequency();
	noise     = pIt->bandwidth();
      
	if(amplitude > maxAmplitude){
	  maxAmplitude = amplitude;
	}
	
	if(frequency > maxFrequency){
	  maxFrequency = frequency;
	}
      
	if(noise > maxNoise){
	  maxNoise = noise;
	}
      }
    }
  }
  catch(...){
    throw;
  }
}

// ---------------------------------------------------------------------------
//      updatePixmap
// ---------------------------------------------------------------------------
// Maybe the delete operations are unneccessary but it solves the problem. 
// updatePixmap is called whenever a modification has occured in the partials
// which will change the plots. This is done when Loris::Partials changes 
// and not when they are requested to avoid unneccessary delays in the GUI.
void Partials::updatePixmap(){
  if(amplitudePixmap){
    delete amplitudePixmap;
    amplitudePixmap = new AmplitudePixmap(partialList, maxTime, maxAmplitude);
  }
  
  if(frequencyPixmap){
    delete frequencyPixmap;
    frequencyPixmap = new FrequencyPixmap(partialList, maxTime, maxFrequency);
  }
  
  if(noisePixmap){
    delete noisePixmap;
    noisePixmap = new NoisePixmap(partialList, maxTime, maxNoise);
  }
}

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
 * sound.c++
 *
 * Implementation of the Sound class. 
 * A Partial has a name, and a list of actual Loris partials representing sound.
 * Sound communicates with lorisInterface in order to preform modifications of 
 * the list of Loris::Partial.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 */

#if HAVE_CONFIG_H    // #define directives are placed in config.h by autoconf
#include <config.h>  
#endif

#include "sound.h"

using std::cout;


/*
---------------------------------------------------------------------------
	Sound constructor
---------------------------------------------------------------------------
A Sound has a name, and a list of actual Loris partials.
Sound communicates with lorisInterface in
order to preform modifications of the list of Loris::Partial.
*/
Sound::Sound(
	list<Loris::Partial>	p,
	QString			n,
	LorisInterface*		inter
){
  name        = n;
  interface   = inter;
  partialList = new list<Loris::Partial>(p);
  setValues();

  distilled   = false;         
  channelized = false;
}

/*
---------------------------------------------------------------------------
	Sound destructor
---------------------------------------------------------------------------
*/
Sound::~Sound(){
  delete partialList; 
}

/*
---------------------------------------------------------------------------
	getName
---------------------------------------------------------------------------
Gives the name of the sound
*/
QString Sound::getName() const  {
  return name; 
}

/*
---------------------------------------------------------------------------
	getNumberOfPartials
---------------------------------------------------------------------------
Returns number of sound
*/
int Sound::getNumberOfPartials() const{
  return nrOfPartials;
}

/*
---------------------------------------------------------------------------
	getPartials
---------------------------------------------------------------------------
Returns a list of Loris::Partial, the actual result of a sound analysis
*/
list<Loris::Partial>* Sound::getPartials() const{
  return partialList;
}

/*
---------------------------------------------------------------------------
	getDuration
---------------------------------------------------------------------------
Returns the time in seconds of the longest partial in the list
*/
double Sound::getDuration() const{
  return maxTime;
}

/*
---------------------------------------------------------------------------
	getMax
---------------------------------------------------------------------------
Returns the max of either amplitude, frequency or noise depending on t.
*/
double Sound::getMax(ValType t) const{
  switch(t){
    case amplitude:
      return maxAmplitude;
      break;
    case frequency:
      return maxFrequency;
      break;
    case noise:
      return maxNoise;
      break;
  }
}
  
/*
---------------------------------------------------------------------------
	isChannelized
---------------------------------------------------------------------------
Returns true if sound are channelized
*/
bool Sound::isChannelized() const{
  return channelized;
}

/*
---------------------------------------------------------------------------
	isDistilled
---------------------------------------------------------------------------
Returns true if sound are distilled
*/
bool Sound::isDistilled() const{
  return distilled;
}

/* 
--------------------------------------------------------------------------
	play
---------------------------------------------------------------------------
Plays sound 
This operation produces delays in the gui and should be solved in 
another way. Ideally, sound should be synthsized every time an
modification action is performed (for example channelize, distill)
and the result saved. Audition of the result would then be much quicker.
*/
void Sound::play(){
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

/*
---------------------------------------------------------------------------
	rename
---------------------------------------------------------------------------
Change the name
*/
void Sound::rename(QString newName){
  name = newName;
}


/*
---------------------------------------------------------------------------
	shiftAmplitude
---------------------------------------------------------------------------
scales sound amplitude with the given argument value 
*/
void Sound::shiftAmplitude(double val){
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
}

/*
---------------------------------------------------------------------------
	shiftNoise
---------------------------------------------------------------------------
scales sound noise with the given argument value 
*/
void Sound::shiftNoise(double val){
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
}

/*
---------------------------------------------------------------------------
	shiftFrequency (shift pitch)
---------------------------------------------------------------------------
scales sound frequency with the given argument value 
*/
void Sound::shiftFrequency(double cents){
  list<Partial>::iterator it;
  Partial_Iterator pIt;

  double pscale = pow(2., (0.01* cents) / 12.);   
  
  //cout<<"Sound::shiftFrequency: shift frequency with scale of: "<<pscale<<endl;
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
    cout<<"Sound::shiftFrequency(): caught any exception"<<endl;
    throw;
  }

  setValues();
}

/*
---------------------------------------------------------------------------
	channelize
---------------------------------------------------------------------------
Channelize a list of Loris::Partials by communicating with lorisInterface.
After channelization, max values have to be recalculated and the plots
need to be updated
*/
void Sound::channelize(
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
  }
  catch(...){
    throw;
  }
}

/*
---------------------------------------------------------------------------
	dilate
---------------------------------------------------------------------------
*/
void Sound::dilate(
        list<double>* source,
        list<double>* target
){
  try{
    interface->dilate(
        getPartials(),
        source,
        target
    );
  }catch(...){ throw; }

  setValues();
}

/*
---------------------------------------------------------------------------
	distill
---------------------------------------------------------------------------
Distill Loris::Partials by communicating with lorisInterface.
After distillation, max values have to be recalculated and the plots
need to be updated.
*/
void Sound::distill(){
  try{
    interface -> distill(*partialList);
    distilled = true;
    setValues();
  }
  catch(...){
    throw;
  }
}

/*
---------------------------------------------------------------------------
	exportToAiff
---------------------------------------------------------------------------
Export Loris::Sound to aiff file format by communicating with lorisInterface
*/
void Sound::exportToAiff(
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

/*
---------------------------------------------------------------------------
	exportToSdif
---------------------------------------------------------------------------
Export Loris::Sound to sdif file format by communicating with lorisInterface
*/
void Sound::exportToSdif(const char* name){
  try{
    interface->exportSdif(name, *partialList);
  }
  catch(...){
    throw;
  }
}

/************************ helpers *****************************/

/*
---------------------------------------------------------------------------
	setValues
---------------------------------------------------------------------------
The variables containing maximum amplitude, frequency, and noise, as well as
longest partial and number of sound are recalculated. This is done when the
Loris::Sound changes and not when they are requested to avoid unneccessary 
delays in the GUI.
*/
void Sound::setValues(){
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
    // loop through all sound in the list
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
